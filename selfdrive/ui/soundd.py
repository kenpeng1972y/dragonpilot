import math
import numpy as np
import time
import wave

from typing import Dict, Optional, Tuple

from cereal import car, messaging
from openpilot.common.basedir import BASEDIR
from openpilot.common.filter_simple import FirstOrderFilter
from openpilot.common.realtime import Ratekeeper
from openpilot.common.retry import retry
from openpilot.common.swaglog import cloudlog
from openpilot.common.params import Params

from openpilot.system import micd

import os

SAMPLE_RATE = 24000
SAMPLE_BUFFER = 4096 # (approx 100ms)
MAX_VOLUME = 1.0
MIN_VOLUME = 0.1
CONTROLS_TIMEOUT = 5 # 5 seconds
FILTER_DT = 1. / (micd.SAMPLE_RATE / micd.FFT_SAMPLES)

AMBIENT_DB = 30 # DB where MIN_VOLUME is applied
DB_SCALE = 30 # AMBIENT_DB + DB_SCALE is where MAX_VOLUME is applied

AudibleAlert = car.CarControl.HUDControl.AudibleAlert


sound_list: Dict[int, Tuple[str, Optional[int], float]] = {
  # AudibleAlert, file name, play count (none for infinite)
  AudibleAlert.engage: ("engage.wav", 1, MAX_VOLUME),
  AudibleAlert.disengage: ("disengage.wav", 1, MAX_VOLUME),
  AudibleAlert.refuse: ("refuse.wav", 1, MAX_VOLUME),

  AudibleAlert.prompt: ("prompt.wav", 1, MAX_VOLUME),
  AudibleAlert.promptRepeat: ("prompt.wav", None, MAX_VOLUME),
  AudibleAlert.promptDistracted: ("prompt_distracted.wav", None, MAX_VOLUME),

  AudibleAlert.warningSoft: ("warning_soft.wav", None, MAX_VOLUME),
  AudibleAlert.warningImmediate: ("warning_immediate.wav", None, MAX_VOLUME),
}

def check_controls_timeout_alert(sm):
  controls_missing = time.monotonic() - sm.rcv_time['controlsState']

  if controls_missing > CONTROLS_TIMEOUT:
    if sm['controlsState'].enabled and (controls_missing - CONTROLS_TIMEOUT) < 10:
      return True

  return False


class Soundd:
  def __init__(self):
    self.load_sounds()

    self.current_alert = AudibleAlert.none
    self.current_volume = MIN_VOLUME
    self.current_sound_frame = 0

    self.controls_timeout_alert = False

    self.spl_filter_weighted = FirstOrderFilter(0, 2.5, FILTER_DT, initialized=False)

    params = Params()
    self._dp_device_audible_alert_mode = int(params.get("dp_device_audible_alert_mode"))
    self._dp_nav_voice_guidance = params.get_bool("dp_nav_voice_guidance")
    self._dp_nav_sound_distance_prev = None
    self._dp_nav_sound_direction_prev = None
    self._dp_nav_sound_queue = []
    self.current_alert_is_nav_voice = False

  def load_sounds(self):
    self.loaded_sounds: Dict[Any, np.ndarray] = {}

    # Load all sounds
    for sound in sound_list:
      filename, play_count, volume = sound_list[sound]

      wavefile = wave.open(BASEDIR + "/selfdrive/assets/sounds/" + filename, 'r')

      assert wavefile.getnchannels() == 1
      assert wavefile.getsampwidth() == 2
      # rick - nav voice uses 24000 and warning is 48000
      # assert wavefile.getframerate() == SAMPLE_RATE

      length = wavefile.getnframes()
      self.loaded_sounds[sound] = np.frombuffer(wavefile.readframes(length), dtype=np.int16).astype(np.float32) / (2**16/2)

  # rick - load nav sound dynamically
  def load_nav_sound(self, filename):
    full_filename = BASEDIR + "/selfdrive/assets/navigation/voices/" + filename + ".wav"
    if filename in self.loaded_sounds:
      return True
    if os.path.exists(full_filename):
      wavefile = wave.open(full_filename, 'r')
      length = wavefile.getnframes()
      self.loaded_sounds[filename] = np.frombuffer(wavefile.readframes(length), dtype=np.int16).astype(np.float32) / (2**16/2)
      return True
    return False

  def get_sound_data(self, frames): # get "frames" worth of data from the current alert sound, looping when required

    ret = np.zeros(frames, dtype=np.float32)

    # dp - set vol to 0 instead
    mute = False
    if self._dp_device_audible_alert_mode == 2:
      mute = True
    elif self._dp_device_audible_alert_mode == 1 and self.current_alert in [AudibleAlert.engage, AudibleAlert.disengage]:
      mute = True

    if self.current_alert != AudibleAlert.none:
      # rick - nav voice only play once
      num_loops = 1 if self.current_alert_is_nav_voice else sound_list[self.current_alert][1]
      sound_data = self.loaded_sounds[self.current_alert]
      written_frames = 0

      current_sound_frame = self.current_sound_frame % len(sound_data)
      loops = self.current_sound_frame // len(sound_data)

      while written_frames < frames and (num_loops is None or loops < num_loops):
        available_frames = sound_data.shape[0] - current_sound_frame
        frames_to_write = min(available_frames, frames - written_frames)
        ret[written_frames:written_frames+frames_to_write] = sound_data[current_sound_frame:current_sound_frame+frames_to_write]
        written_frames += frames_to_write
        self.current_sound_frame += frames_to_write

    return ret * (self.current_volume if not mute else 0)

  def callback(self, data_out: np.ndarray, frames: int, time, status) -> None:
    if status:
      cloudlog.warning(f"soundd stream over/underflow: {status}")
    data_out[:frames, 0] = self.get_sound_data(frames)

  def update_alert(self, new_alert, new_alert_is_nav_voice=False):
    # assuming current_alert is nav and new_alert is warning, we should allow it to override
    if not new_alert_is_nav_voice and self.current_alert_is_nav_voice:
      self.current_alert = AudibleAlert.none

    current_alert_played_once = self.current_alert == AudibleAlert.none or self.current_sound_frame > len(self.loaded_sounds[self.current_alert])
    if self.current_alert != new_alert and (new_alert != AudibleAlert.none or current_alert_played_once):
      self.current_alert = new_alert
      self.current_alert_is_nav_voice = new_alert_is_nav_voice
      self.current_sound_frame = 0

  def get_audible_alert(self, sm):
    if sm.updated['controlsState']:
      new_alert = sm['controlsState'].alertSound.raw
      self.update_nav_voice_queue(sm)
      if new_alert != AudibleAlert.none:
        self.update_alert(new_alert)
      # only handle nav voice when there is no alerts
      else:
        if len(self._dp_nav_sound_queue) > 0:
          # assuming current_alert is nav and we have finish playing and there is a queue, remove it from the queue
          if self.current_alert == self._dp_nav_sound_queue[0] and self.current_sound_frame > len(self.loaded_sounds[self.current_alert]):
            self._dp_nav_sound_queue.pop(0)
        # after clean up queue, if there is still queue, process it
        if len(self._dp_nav_sound_queue) > 0:
          # if we have queue, play the first one
          self.update_alert(self._dp_nav_sound_queue[0], True)
        else:
          self.update_alert(new_alert)
    elif check_controls_timeout_alert(sm):
      self.update_alert(AudibleAlert.warningImmediate)
      self.controls_timeout_alert = True
    elif self.controls_timeout_alert:
      self.update_alert(AudibleAlert.none)
      self.controls_timeout_alert = False

  # rick - create nav voice queue
  # 1. get voice info from navInstructionExt
  # 2. check if they are new distance / direction, if so, load them into loaded_sounds array
  # 3. if sound loaded correctly, add the voices into queue, so it could be [distance, direction] or [direction],
  #    we also don't want to queue up so clean up queue when new one is coming
  def update_nav_voice_queue(self, sm):
    if self._dp_nav_voice_guidance and sm.updated['navInstructionExt']:
      distance = sm['navInstructionExt'].voiceDistance
      direction = sm['navInstructionExt'].voiceDirection
      if distance != self._dp_nav_sound_distance_prev or direction != self._dp_nav_sound_direction_prev:
        # when distance is not empty, we should have both distance and direction
        if distance != "":
          if self.load_nav_sound(distance) and self.load_nav_sound(direction):
            self._dp_nav_sound_queue = []
            self._dp_nav_sound_queue.append(distance)
            self._dp_nav_sound_queue.append(direction)
        else:
          if self.load_nav_sound(direction):
            self._dp_nav_sound_queue = []
            self._dp_nav_sound_queue.append(direction)

        self._dp_nav_sound_distance_prev = distance
        self._dp_nav_sound_direction_prev = direction

  def calculate_volume(self, weighted_db):
    volume = ((weighted_db - AMBIENT_DB) / DB_SCALE) * (MAX_VOLUME - MIN_VOLUME) + MIN_VOLUME
    return math.pow(10, (np.clip(volume, MIN_VOLUME, MAX_VOLUME) - 1))

  @retry(attempts=7, delay=3)
  def get_stream(self, sd):
    # reload sounddevice to reinitialize portaudio
    sd._terminate()
    sd._initialize()
    return sd.OutputStream(channels=1, samplerate=SAMPLE_RATE, callback=self.callback, blocksize=SAMPLE_BUFFER)

  def soundd_thread(self):
    # sounddevice must be imported after forking processes
    import sounddevice as sd

    sm = messaging.SubMaster(['controlsState', 'microphone', 'navInstructionExt'])

    with self.get_stream(sd) as stream:
      rk = Ratekeeper(20)

      cloudlog.info(f"soundd stream started: {stream.samplerate=} {stream.channels=} {stream.dtype=} {stream.device=}, {stream.blocksize=}")
      while True:
        sm.update(0)

        if sm.updated['microphone'] and self.current_alert == AudibleAlert.none: # only update volume filter when not playing alert
          self.spl_filter_weighted.update(sm["microphone"].soundPressureWeightedDb)
          self.current_volume = self.calculate_volume(float(self.spl_filter_weighted.x))

        self.get_audible_alert(sm)

        rk.keep_time()

        assert stream.active


def main():
  s = Soundd()
  s.soundd_thread()


if __name__ == "__main__":
  main()
