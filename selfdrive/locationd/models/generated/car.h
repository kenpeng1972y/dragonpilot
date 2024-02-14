#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_err_fun(double *nom_x, double *delta_x, double *out_786078590364264470);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_5919995043477341165);
void car_H_mod_fun(double *state, double *out_3543183180667111266);
void car_f_fun(double *state, double dt, double *out_1497117634121373884);
void car_F_fun(double *state, double dt, double *out_7787221922284321900);
void car_h_25(double *state, double *unused, double *out_6182971352921756001);
void car_H_25(double *state, double *unused, double *out_3458468969911740743);
void car_h_24(double *state, double *unused, double *out_1709075460934367585);
void car_H_24(double *state, double *unused, double *out_5631118568917240309);
void car_h_30(double *state, double *unused, double *out_2313689328341983991);
void car_H_30(double *state, double *unused, double *out_3458221371579876012);
void car_h_26(double *state, double *unused, double *out_7458579391457631528);
void car_H_26(double *state, double *unused, double *out_7199972288785796967);
void car_h_27(double *state, double *unused, double *out_3146185301477423316);
void car_H_27(double *state, double *unused, double *out_1283458059779451101);
void car_h_29(double *state, double *unused, double *out_2687983939318861654);
void car_H_29(double *state, double *unused, double *out_429904667090099932);
void car_h_28(double *state, double *unused, double *out_7768371330933531630);
void car_H_28(double *state, double *unused, double *out_5512303684159630506);
void car_h_31(double *state, double *unused, double *out_5907777290637250112);
void car_H_31(double *state, double *unused, double *out_3427823008034780315);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}