#!/usr/bin/env python3
# The MIT License
#
# Copyright (c) 2019-, Rick Lan, dragonpilot community, and a number of other of contributors.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.


import numpy as np
from openpilot.common.realtime import DT_MDL
from openpilot.common.numpy_fast import interp
from openpilot.selfdrive.controls.lib.lateral_mpc_lib.lat_mpc import LateralMpc
from openpilot.selfdrive.controls.lib.lateral_mpc_lib.lat_mpc import N as LAT_MPC_N
from openpilot.selfdrive.controls.lib.drive_helpers import CONTROL_N, MIN_SPEED, get_speed_error
from openpilot.selfdrive.controls.lib.lane_planner import LanePlanner

CAMERA_OFFSET = 0.04

PATH_COST = 1.0
LATERAL_MOTION_COST = 0.11
LATERAL_ACCEL_COST = 0.0
LATERAL_JERK_COST = 0.04
# Extreme steering rate is unpleasant, even
# when it does not cause bad jerk.
# TODO this cost should be lowered when low
# speed lateral control is stable on all cars
STEERING_RATE_COST = 700.0


class LateralMpcLanePlanner:
  def __init__(self, CP, trajectory_size, debug=False):
    self.trajectory_size = trajectory_size

    self._dp_lat_lane_priority_mode_speed = 0.
    self._dp_lat_lane_priority_mode_active = False
    self._dp_lat_lane_priority_mode_active_prev = False
    self.LP = LanePlanner()

    # Vehicle model parameters used to calculate lateral movement of car
    self.factor1 = CP.wheelbase - CP.centerToFront
    self.factor2 = (CP.centerToFront * CP.mass) / (CP.wheelbase * CP.tireStiffnessRear)
    # self.solution_invalid_cnt = 0

    self.path_xyz = np.zeros((self.trajectory_size, 3))
    self.velocity_xyz = np.zeros((self.trajectory_size, 3))
    self.plan_yaw = np.zeros((self.trajectory_size,))
    self.plan_yaw_rate = np.zeros((self.trajectory_size,))
    self.t_idxs = np.arange(self.trajectory_size)
    self.y_pts = np.zeros((self.trajectory_size,))
    self.v_plan = np.zeros((self.trajectory_size,))
    self.v_ego = 0.0

    self.debug_mode = debug

    self.lat_mpc = LateralMpc()
    self.reset_mpc(np.zeros(4))

    self._is_enabled = False
    self.v_ego_car = 0.0
    self.lll_prob = 0.
    self.rll_prob = 0.

  def reset_mpc(self, x0=None):
    if x0 is None:
      x0 = np.zeros(4)
    self.x0 = x0
    self.lat_mpc.reset(x0=self.x0)

  def parse_model(self, md):
    # Parse model predictions
    if len(md.position.x) == self.trajectory_size and len(md.orientation.x) == self.trajectory_size:
      self.path_xyz = np.column_stack([md.position.x, md.position.y, md.position.z])
      self.t_idxs = np.array(md.position.t)
      self.plan_yaw = np.array(md.orientation.z)
      self.plan_yaw_rate = np.array(md.orientationRate.z)
      self.velocity_xyz = np.column_stack([md.velocity.x, md.velocity.y, md.velocity.z])
      car_speed = np.linalg.norm(self.velocity_xyz, axis=1) - get_speed_error(md, self.v_ego_car)
      self.v_plan = np.clip(car_speed, MIN_SPEED, np.inf)
      self.v_ego = self.v_plan[0]

    # let LanePlanner to read model
    self.LP.parse_model(md)

  def update(self, curvature):
    # getting d path from LanePlanner instead of model
    path_xyz = self.LP.get_d_path(self.v_ego_car, self.t_idxs, self.path_xyz)

    self.lat_mpc.set_weights(PATH_COST, LATERAL_MOTION_COST,
                             LATERAL_ACCEL_COST, LATERAL_JERK_COST,
                             STEERING_RATE_COST)

    y_pts = path_xyz[:LAT_MPC_N+1, 1]
    heading_pts = self.plan_yaw[:LAT_MPC_N+1]
    yaw_rate_pts = self.plan_yaw_rate[:LAT_MPC_N+1]
    self.y_pts = y_pts

    assert len(y_pts) == LAT_MPC_N + 1
    assert len(heading_pts) == LAT_MPC_N + 1
    assert len(yaw_rate_pts) == LAT_MPC_N + 1
    lateral_factor = np.clip(self.factor1 - (self.factor2 * self.v_plan**2), 0.0, np.inf)
    p = np.column_stack([self.v_plan, lateral_factor])
    self.lat_mpc.run(self.x0,
                     p,
                     y_pts,
                     heading_pts,
                     yaw_rate_pts)
    # init state for next iteration
    # mpc.u_sol is the desired second derivative of psi given x0 curv state.
    # with x0[3] = measured_yaw_rate, this would be the actual desired yaw rate.
    # instead, interpolate x_sol so that x0[3] is the desired yaw rate for lat_control.
    self.x0[3] = interp(DT_MDL, self.t_idxs[:LAT_MPC_N + 1], self.lat_mpc.x_sol[:, 3])

    #  Check for infeasible MPC solution
    mpc_nans = np.isnan(self.lat_mpc.x_sol[:, 3]).any()
    # t = time.monotonic()
    if mpc_nans or self.lat_mpc.solution_status != 0:
      self.reset_mpc()
      self.x0[3] = curvature * self.v_ego
      # if t > self.last_cloudlog_t + 5.0:
      #   self.last_cloudlog_t = t
      #   cloudlog.warning("Lateral mpc - nan: True")

    # if self.lat_mpc.cost > 1e6 or mpc_nans:
    #   self.solution_invalid_cnt += 1
    # else:
    #   self.solution_invalid_cnt = 0

    return path_xyz[:,1].tolist(), \
      self.lat_mpc.x_sol[0:CONTROL_N, 2].tolist(), \
      (self.lat_mpc.x_sol[0:CONTROL_N, 3]/self.v_ego).tolist(), \
      [float(x/self.v_ego) for x in self.lat_mpc.u_sol[0:CONTROL_N - 1]] + [0.0]

  # allow lane_line probabilities to set from outside
  def set_lane_line_probs(self, lane_line_probs):
    self.lll_prob = lane_line_probs[1]
    self.rll_prob = lane_line_probs[2]

  def should_activate(self):
    # decide what mode should we use
    if (self.LP.lll_prob + self.LP.rll_prob)/2 < 0.3:
      self._dp_lat_lane_priority_mode_active = False
    if (self.LP.lll_prob + self.LP.rll_prob)/2 > 0.5:
      self._dp_lat_lane_priority_mode_active = True

    # when drive speed is below set speed, we set it to False
    if self._dp_lat_lane_priority_mode_active and self._dp_lat_lane_priority_mode_speed > 0 and self.v_ego_car < self._dp_lat_lane_priority_mode_speed:
      self._dp_lat_lane_priority_mode_active = False

    # reset mpc
    if self._dp_lat_lane_priority_mode_active and not self._dp_lat_lane_priority_mode_active_prev:
      self.reset_mpc()

    self._dp_lat_lane_priority_mode_active_prev = self._dp_lat_lane_priority_mode_active
    return self._dp_lat_lane_priority_mode_active

  def set_enabled(self, enabled: bool) -> None:
    self._is_enabled = enabled

  def is_enabled(self):
    return self._is_enabled

  def set_speed(self, speed: float) -> None:
    self._dp_lat_lane_priority_mode_speed = speed

  def set_v_ego_speed(self, speed: float) -> None:
    self.v_ego_car = speed
