#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void live_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_9(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_12(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_35(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_32(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_33(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_H(double *in_vec, double *out_3062539955778785123);
void live_err_fun(double *nom_x, double *delta_x, double *out_997632986539046101);
void live_inv_err_fun(double *nom_x, double *true_x, double *out_3959944895181216754);
void live_H_mod_fun(double *state, double *out_8702234888023658226);
void live_f_fun(double *state, double dt, double *out_7459231211064544538);
void live_F_fun(double *state, double dt, double *out_8962950086978682024);
void live_h_4(double *state, double *unused, double *out_1777498685180261346);
void live_H_4(double *state, double *unused, double *out_4137287014202993190);
void live_h_9(double *state, double *unused, double *out_4762408851873785060);
void live_H_9(double *state, double *unused, double *out_4378476660832583835);
void live_h_10(double *state, double *unused, double *out_769477702419128333);
void live_H_10(double *state, double *unused, double *out_607115477708369913);
void live_h_12(double *state, double *unused, double *out_5594886434556651779);
void live_H_12(double *state, double *unused, double *out_9156743422234954985);
void live_h_35(double *state, double *unused, double *out_7828238170709544847);
void live_H_35(double *state, double *unused, double *out_7503949071575600566);
void live_h_32(double *state, double *unused, double *out_1198587467706715008);
void live_H_32(double *state, double *unused, double *out_5141490629926338492);
void live_h_13(double *state, double *unused, double *out_4446144489578117053);
void live_H_13(double *state, double *unused, double *out_8954114104359138418);
void live_h_14(double *state, double *unused, double *out_4762408851873785060);
void live_H_14(double *state, double *unused, double *out_4378476660832583835);
void live_h_33(double *state, double *unused, double *out_8645294225009481236);
void live_H_33(double *state, double *unused, double *out_7792237997495093446);
void live_predict(double *in_x, double *in_P, double *in_Q, double dt);
}