"""Install exception handler for process crash."""
import sentry_sdk
from enum import Enum
from typing import Tuple
from sentry_sdk.integrations.threading import ThreadingIntegration

from openpilot.common.params import Params
from openpilot.selfdrive.athena.registration import UNREGISTERED_DONGLE_ID, is_registered_device
from openpilot.system.hardware import HARDWARE, PC
from openpilot.common.swaglog import cloudlog
from openpilot.system.version import get_branch, get_commit, get_origin, get_version, \
                              is_comma_remote, is_dirty, is_tested_branch

import os
import traceback
from cereal import car
from datetime import datetime

class SentryProject(Enum):
  # python project
  SELFDRIVE = "https://980a0cba712a4c3593c33c78a12446e1@o273754.ingest.sentry.io/1488600"
  # native project
  SELFDRIVE_NATIVE = "https://980a0cba712a4c3593c33c78a12446e1@o273754.ingest.sentry.io/1488600"

CRASHES_DIR = '/data/media/0/crash_logs'
ip = "{{auto}}"

def report_tombstone(fn: str, message: str, contents: str) -> None:
  cloudlog.error({'tombstone': message})

  with sentry_sdk.configure_scope() as scope:
    scope.set_extra("tombstone_fn", fn)
    scope.set_extra("tombstone", contents)
    sentry_sdk.capture_message(message=message)
    sentry_sdk.flush()


def capture_exception(*args, **kwargs) -> None:
  save_exception(traceback.format_exc())
  cloudlog.error("crash", exc_info=kwargs.get('exc_info', 1))

  try:
    bind_user()
    sentry_sdk.capture_exception(*args, **kwargs)
    sentry_sdk.flush()  # https://github.com/getsentry/sentry-python/issues/291
  except Exception:
    cloudlog.exception("sentry exception")

def save_exception(exc_text):
  if not os.path.exists(CRASHES_DIR):
    os.makedirs(CRASHES_DIR)

  log_file = '{}/{}'.format(CRASHES_DIR, datetime.now().strftime('%Y-%m-%d-%H-%M-%S.log'))
  with open(log_file, 'w') as f:
    f.write(exc_text)
  print('Logged current crash to {}'.format(log_file))

def bind_user() -> None:
  dongle_id, gitname = get_properties()
  sentry_sdk.set_user({"id": dongle_id, "ip_address": ip, "name": gitname})

def capture_warning(warning_string: str) -> None:
  bind_user()
  sentry_sdk.capture_message(warning_string, level='warning')
  sentry_sdk.flush()

def capture_warning(info_string: str) -> None:
  bind_user()
  sentry_sdk.capture_message(info_string, level='info')
  sentry_sdk.flush()

def set_tag(key: str, value: str) -> None:
  sentry_sdk.set_tag(key, value)

def get_properties() -> Tuple[str, str]:
  params = Params()
  dongle_id = params.get("DongleId", encoding='utf-8')
  if dongle_id in (None, UNREGISTERED_DONGLE_ID):
    hardware_serial = params.get("HardwareSerial", encoding='utf-8')
    hardware_serial = "" if hardware_serial is None else hardware_serial
    dongle_id = UNREGISTERED_DONGLE_ID + hardware_serial
  gitname = params.get("GithubUsername", encoding='utf-8')
  if gitname is None:
    gitname = ""

  return dongle_id, gitname

def init(project: SentryProject) -> None:
  # forks like to mess with this, so double check
  #comma_remote = is_comma_remote() and "commaai" in get_origin()
  #if not comma_remote or not is_registered_device() or PC:
    #return False
  #params = Params()
  env = "release" if is_tested_branch() else "master"
  dongle_id, gitname = get_properties()
  try:
    cached_params = params.get("CarParams")
    if cached_params is not None:
      cached_params = car.CarParams.from_bytes(cached_params)
      car_name = cached_params.carFingerprint
    else:
      car_name = "None"
  except Exception:
    car_name = "None"


  integrations = []
  if project == SentryProject.SELFDRIVE:
    integrations.append(ThreadingIntegration(propagate_hub=True))

  sentry_sdk.init(project.value,
                  default_integrations=False,
                  release=get_version(),
                  integrations=integrations,
                  traces_sample_rate=1.0,
                  max_value_length=8192,
                  environment=env,
                  send_default_pii=True)

  sentry_sdk.set_user({"id": dongle_id})
  sentry_sdk.set_user({"gitname": gitname})
  sentry_sdk.set_user({"ip_address": ip})
  sentry_sdk.set_tag("dirty", is_dirty())
  sentry_sdk.set_tag("origin", get_origin())
  sentry_sdk.set_tag("branch", get_branch())
  sentry_sdk.set_tag("commit", get_commit())
  sentry_sdk.set_tag("device", HARDWARE.get_device_type())
  sentry_sdk.set_tag("model", car_name)

  if project == SentryProject.SELFDRIVE:
    sentry_sdk.Hub.current.start_session()

  return True
