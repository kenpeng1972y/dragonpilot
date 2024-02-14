#include "car.h"

namespace {
#define DIM 9
#define EDIM 9
#define MEDIM 9
typedef void (*Hfun)(double *, double *, double *);

double mass;

void set_mass(double x){ mass = x;}

double rotational_inertia;

void set_rotational_inertia(double x){ rotational_inertia = x;}

double center_to_front;

void set_center_to_front(double x){ center_to_front = x;}

double center_to_rear;

void set_center_to_rear(double x){ center_to_rear = x;}

double stiffness_front;

void set_stiffness_front(double x){ stiffness_front = x;}

double stiffness_rear;

void set_stiffness_rear(double x){ stiffness_rear = x;}
const static double MAHA_THRESH_25 = 3.8414588206941227;
const static double MAHA_THRESH_24 = 5.991464547107981;
const static double MAHA_THRESH_30 = 3.8414588206941227;
const static double MAHA_THRESH_26 = 3.8414588206941227;
const static double MAHA_THRESH_27 = 3.8414588206941227;
const static double MAHA_THRESH_29 = 3.8414588206941227;
const static double MAHA_THRESH_28 = 3.8414588206941227;
const static double MAHA_THRESH_31 = 3.8414588206941227;

/******************************************************************************
 *                       Code generated with SymPy 1.12                       *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_786078590364264470) {
   out_786078590364264470[0] = delta_x[0] + nom_x[0];
   out_786078590364264470[1] = delta_x[1] + nom_x[1];
   out_786078590364264470[2] = delta_x[2] + nom_x[2];
   out_786078590364264470[3] = delta_x[3] + nom_x[3];
   out_786078590364264470[4] = delta_x[4] + nom_x[4];
   out_786078590364264470[5] = delta_x[5] + nom_x[5];
   out_786078590364264470[6] = delta_x[6] + nom_x[6];
   out_786078590364264470[7] = delta_x[7] + nom_x[7];
   out_786078590364264470[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_5919995043477341165) {
   out_5919995043477341165[0] = -nom_x[0] + true_x[0];
   out_5919995043477341165[1] = -nom_x[1] + true_x[1];
   out_5919995043477341165[2] = -nom_x[2] + true_x[2];
   out_5919995043477341165[3] = -nom_x[3] + true_x[3];
   out_5919995043477341165[4] = -nom_x[4] + true_x[4];
   out_5919995043477341165[5] = -nom_x[5] + true_x[5];
   out_5919995043477341165[6] = -nom_x[6] + true_x[6];
   out_5919995043477341165[7] = -nom_x[7] + true_x[7];
   out_5919995043477341165[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_3543183180667111266) {
   out_3543183180667111266[0] = 1.0;
   out_3543183180667111266[1] = 0;
   out_3543183180667111266[2] = 0;
   out_3543183180667111266[3] = 0;
   out_3543183180667111266[4] = 0;
   out_3543183180667111266[5] = 0;
   out_3543183180667111266[6] = 0;
   out_3543183180667111266[7] = 0;
   out_3543183180667111266[8] = 0;
   out_3543183180667111266[9] = 0;
   out_3543183180667111266[10] = 1.0;
   out_3543183180667111266[11] = 0;
   out_3543183180667111266[12] = 0;
   out_3543183180667111266[13] = 0;
   out_3543183180667111266[14] = 0;
   out_3543183180667111266[15] = 0;
   out_3543183180667111266[16] = 0;
   out_3543183180667111266[17] = 0;
   out_3543183180667111266[18] = 0;
   out_3543183180667111266[19] = 0;
   out_3543183180667111266[20] = 1.0;
   out_3543183180667111266[21] = 0;
   out_3543183180667111266[22] = 0;
   out_3543183180667111266[23] = 0;
   out_3543183180667111266[24] = 0;
   out_3543183180667111266[25] = 0;
   out_3543183180667111266[26] = 0;
   out_3543183180667111266[27] = 0;
   out_3543183180667111266[28] = 0;
   out_3543183180667111266[29] = 0;
   out_3543183180667111266[30] = 1.0;
   out_3543183180667111266[31] = 0;
   out_3543183180667111266[32] = 0;
   out_3543183180667111266[33] = 0;
   out_3543183180667111266[34] = 0;
   out_3543183180667111266[35] = 0;
   out_3543183180667111266[36] = 0;
   out_3543183180667111266[37] = 0;
   out_3543183180667111266[38] = 0;
   out_3543183180667111266[39] = 0;
   out_3543183180667111266[40] = 1.0;
   out_3543183180667111266[41] = 0;
   out_3543183180667111266[42] = 0;
   out_3543183180667111266[43] = 0;
   out_3543183180667111266[44] = 0;
   out_3543183180667111266[45] = 0;
   out_3543183180667111266[46] = 0;
   out_3543183180667111266[47] = 0;
   out_3543183180667111266[48] = 0;
   out_3543183180667111266[49] = 0;
   out_3543183180667111266[50] = 1.0;
   out_3543183180667111266[51] = 0;
   out_3543183180667111266[52] = 0;
   out_3543183180667111266[53] = 0;
   out_3543183180667111266[54] = 0;
   out_3543183180667111266[55] = 0;
   out_3543183180667111266[56] = 0;
   out_3543183180667111266[57] = 0;
   out_3543183180667111266[58] = 0;
   out_3543183180667111266[59] = 0;
   out_3543183180667111266[60] = 1.0;
   out_3543183180667111266[61] = 0;
   out_3543183180667111266[62] = 0;
   out_3543183180667111266[63] = 0;
   out_3543183180667111266[64] = 0;
   out_3543183180667111266[65] = 0;
   out_3543183180667111266[66] = 0;
   out_3543183180667111266[67] = 0;
   out_3543183180667111266[68] = 0;
   out_3543183180667111266[69] = 0;
   out_3543183180667111266[70] = 1.0;
   out_3543183180667111266[71] = 0;
   out_3543183180667111266[72] = 0;
   out_3543183180667111266[73] = 0;
   out_3543183180667111266[74] = 0;
   out_3543183180667111266[75] = 0;
   out_3543183180667111266[76] = 0;
   out_3543183180667111266[77] = 0;
   out_3543183180667111266[78] = 0;
   out_3543183180667111266[79] = 0;
   out_3543183180667111266[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_1497117634121373884) {
   out_1497117634121373884[0] = state[0];
   out_1497117634121373884[1] = state[1];
   out_1497117634121373884[2] = state[2];
   out_1497117634121373884[3] = state[3];
   out_1497117634121373884[4] = state[4];
   out_1497117634121373884[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8000000000000007*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_1497117634121373884[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_1497117634121373884[7] = state[7];
   out_1497117634121373884[8] = state[8];
}
void F_fun(double *state, double dt, double *out_7787221922284321900) {
   out_7787221922284321900[0] = 1;
   out_7787221922284321900[1] = 0;
   out_7787221922284321900[2] = 0;
   out_7787221922284321900[3] = 0;
   out_7787221922284321900[4] = 0;
   out_7787221922284321900[5] = 0;
   out_7787221922284321900[6] = 0;
   out_7787221922284321900[7] = 0;
   out_7787221922284321900[8] = 0;
   out_7787221922284321900[9] = 0;
   out_7787221922284321900[10] = 1;
   out_7787221922284321900[11] = 0;
   out_7787221922284321900[12] = 0;
   out_7787221922284321900[13] = 0;
   out_7787221922284321900[14] = 0;
   out_7787221922284321900[15] = 0;
   out_7787221922284321900[16] = 0;
   out_7787221922284321900[17] = 0;
   out_7787221922284321900[18] = 0;
   out_7787221922284321900[19] = 0;
   out_7787221922284321900[20] = 1;
   out_7787221922284321900[21] = 0;
   out_7787221922284321900[22] = 0;
   out_7787221922284321900[23] = 0;
   out_7787221922284321900[24] = 0;
   out_7787221922284321900[25] = 0;
   out_7787221922284321900[26] = 0;
   out_7787221922284321900[27] = 0;
   out_7787221922284321900[28] = 0;
   out_7787221922284321900[29] = 0;
   out_7787221922284321900[30] = 1;
   out_7787221922284321900[31] = 0;
   out_7787221922284321900[32] = 0;
   out_7787221922284321900[33] = 0;
   out_7787221922284321900[34] = 0;
   out_7787221922284321900[35] = 0;
   out_7787221922284321900[36] = 0;
   out_7787221922284321900[37] = 0;
   out_7787221922284321900[38] = 0;
   out_7787221922284321900[39] = 0;
   out_7787221922284321900[40] = 1;
   out_7787221922284321900[41] = 0;
   out_7787221922284321900[42] = 0;
   out_7787221922284321900[43] = 0;
   out_7787221922284321900[44] = 0;
   out_7787221922284321900[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_7787221922284321900[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_7787221922284321900[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_7787221922284321900[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_7787221922284321900[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_7787221922284321900[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_7787221922284321900[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_7787221922284321900[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_7787221922284321900[53] = -9.8000000000000007*dt;
   out_7787221922284321900[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_7787221922284321900[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_7787221922284321900[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_7787221922284321900[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_7787221922284321900[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_7787221922284321900[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_7787221922284321900[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_7787221922284321900[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_7787221922284321900[62] = 0;
   out_7787221922284321900[63] = 0;
   out_7787221922284321900[64] = 0;
   out_7787221922284321900[65] = 0;
   out_7787221922284321900[66] = 0;
   out_7787221922284321900[67] = 0;
   out_7787221922284321900[68] = 0;
   out_7787221922284321900[69] = 0;
   out_7787221922284321900[70] = 1;
   out_7787221922284321900[71] = 0;
   out_7787221922284321900[72] = 0;
   out_7787221922284321900[73] = 0;
   out_7787221922284321900[74] = 0;
   out_7787221922284321900[75] = 0;
   out_7787221922284321900[76] = 0;
   out_7787221922284321900[77] = 0;
   out_7787221922284321900[78] = 0;
   out_7787221922284321900[79] = 0;
   out_7787221922284321900[80] = 1;
}
void h_25(double *state, double *unused, double *out_6182971352921756001) {
   out_6182971352921756001[0] = state[6];
}
void H_25(double *state, double *unused, double *out_3458468969911740743) {
   out_3458468969911740743[0] = 0;
   out_3458468969911740743[1] = 0;
   out_3458468969911740743[2] = 0;
   out_3458468969911740743[3] = 0;
   out_3458468969911740743[4] = 0;
   out_3458468969911740743[5] = 0;
   out_3458468969911740743[6] = 1;
   out_3458468969911740743[7] = 0;
   out_3458468969911740743[8] = 0;
}
void h_24(double *state, double *unused, double *out_1709075460934367585) {
   out_1709075460934367585[0] = state[4];
   out_1709075460934367585[1] = state[5];
}
void H_24(double *state, double *unused, double *out_5631118568917240309) {
   out_5631118568917240309[0] = 0;
   out_5631118568917240309[1] = 0;
   out_5631118568917240309[2] = 0;
   out_5631118568917240309[3] = 0;
   out_5631118568917240309[4] = 1;
   out_5631118568917240309[5] = 0;
   out_5631118568917240309[6] = 0;
   out_5631118568917240309[7] = 0;
   out_5631118568917240309[8] = 0;
   out_5631118568917240309[9] = 0;
   out_5631118568917240309[10] = 0;
   out_5631118568917240309[11] = 0;
   out_5631118568917240309[12] = 0;
   out_5631118568917240309[13] = 0;
   out_5631118568917240309[14] = 1;
   out_5631118568917240309[15] = 0;
   out_5631118568917240309[16] = 0;
   out_5631118568917240309[17] = 0;
}
void h_30(double *state, double *unused, double *out_2313689328341983991) {
   out_2313689328341983991[0] = state[4];
}
void H_30(double *state, double *unused, double *out_3458221371579876012) {
   out_3458221371579876012[0] = 0;
   out_3458221371579876012[1] = 0;
   out_3458221371579876012[2] = 0;
   out_3458221371579876012[3] = 0;
   out_3458221371579876012[4] = 1;
   out_3458221371579876012[5] = 0;
   out_3458221371579876012[6] = 0;
   out_3458221371579876012[7] = 0;
   out_3458221371579876012[8] = 0;
}
void h_26(double *state, double *unused, double *out_7458579391457631528) {
   out_7458579391457631528[0] = state[7];
}
void H_26(double *state, double *unused, double *out_7199972288785796967) {
   out_7199972288785796967[0] = 0;
   out_7199972288785796967[1] = 0;
   out_7199972288785796967[2] = 0;
   out_7199972288785796967[3] = 0;
   out_7199972288785796967[4] = 0;
   out_7199972288785796967[5] = 0;
   out_7199972288785796967[6] = 0;
   out_7199972288785796967[7] = 1;
   out_7199972288785796967[8] = 0;
}
void h_27(double *state, double *unused, double *out_3146185301477423316) {
   out_3146185301477423316[0] = state[3];
}
void H_27(double *state, double *unused, double *out_1283458059779451101) {
   out_1283458059779451101[0] = 0;
   out_1283458059779451101[1] = 0;
   out_1283458059779451101[2] = 0;
   out_1283458059779451101[3] = 1;
   out_1283458059779451101[4] = 0;
   out_1283458059779451101[5] = 0;
   out_1283458059779451101[6] = 0;
   out_1283458059779451101[7] = 0;
   out_1283458059779451101[8] = 0;
}
void h_29(double *state, double *unused, double *out_2687983939318861654) {
   out_2687983939318861654[0] = state[1];
}
void H_29(double *state, double *unused, double *out_429904667090099932) {
   out_429904667090099932[0] = 0;
   out_429904667090099932[1] = 1;
   out_429904667090099932[2] = 0;
   out_429904667090099932[3] = 0;
   out_429904667090099932[4] = 0;
   out_429904667090099932[5] = 0;
   out_429904667090099932[6] = 0;
   out_429904667090099932[7] = 0;
   out_429904667090099932[8] = 0;
}
void h_28(double *state, double *unused, double *out_7768371330933531630) {
   out_7768371330933531630[0] = state[0];
}
void H_28(double *state, double *unused, double *out_5512303684159630506) {
   out_5512303684159630506[0] = 1;
   out_5512303684159630506[1] = 0;
   out_5512303684159630506[2] = 0;
   out_5512303684159630506[3] = 0;
   out_5512303684159630506[4] = 0;
   out_5512303684159630506[5] = 0;
   out_5512303684159630506[6] = 0;
   out_5512303684159630506[7] = 0;
   out_5512303684159630506[8] = 0;
}
void h_31(double *state, double *unused, double *out_5907777290637250112) {
   out_5907777290637250112[0] = state[8];
}
void H_31(double *state, double *unused, double *out_3427823008034780315) {
   out_3427823008034780315[0] = 0;
   out_3427823008034780315[1] = 0;
   out_3427823008034780315[2] = 0;
   out_3427823008034780315[3] = 0;
   out_3427823008034780315[4] = 0;
   out_3427823008034780315[5] = 0;
   out_3427823008034780315[6] = 0;
   out_3427823008034780315[7] = 0;
   out_3427823008034780315[8] = 1;
}
#include <eigen3/Eigen/Dense>
#include <iostream>

typedef Eigen::Matrix<double, DIM, DIM, Eigen::RowMajor> DDM;
typedef Eigen::Matrix<double, EDIM, EDIM, Eigen::RowMajor> EEM;
typedef Eigen::Matrix<double, DIM, EDIM, Eigen::RowMajor> DEM;

void predict(double *in_x, double *in_P, double *in_Q, double dt) {
  typedef Eigen::Matrix<double, MEDIM, MEDIM, Eigen::RowMajor> RRM;

  double nx[DIM] = {0};
  double in_F[EDIM*EDIM] = {0};

  // functions from sympy
  f_fun(in_x, dt, nx);
  F_fun(in_x, dt, in_F);


  EEM F(in_F);
  EEM P(in_P);
  EEM Q(in_Q);

  RRM F_main = F.topLeftCorner(MEDIM, MEDIM);
  P.topLeftCorner(MEDIM, MEDIM) = (F_main * P.topLeftCorner(MEDIM, MEDIM)) * F_main.transpose();
  P.topRightCorner(MEDIM, EDIM - MEDIM) = F_main * P.topRightCorner(MEDIM, EDIM - MEDIM);
  P.bottomLeftCorner(EDIM - MEDIM, MEDIM) = P.bottomLeftCorner(EDIM - MEDIM, MEDIM) * F_main.transpose();

  P = P + dt*Q;

  // copy out state
  memcpy(in_x, nx, DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
}

// note: extra_args dim only correct when null space projecting
// otherwise 1
template <int ZDIM, int EADIM, bool MAHA_TEST>
void update(double *in_x, double *in_P, Hfun h_fun, Hfun H_fun, Hfun Hea_fun, double *in_z, double *in_R, double *in_ea, double MAHA_THRESHOLD) {
  typedef Eigen::Matrix<double, ZDIM, ZDIM, Eigen::RowMajor> ZZM;
  typedef Eigen::Matrix<double, ZDIM, DIM, Eigen::RowMajor> ZDM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, EDIM, Eigen::RowMajor> XEM;
  //typedef Eigen::Matrix<double, EDIM, ZDIM, Eigen::RowMajor> EZM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> X1M;
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> XXM;

  double in_hx[ZDIM] = {0};
  double in_H[ZDIM * DIM] = {0};
  double in_H_mod[EDIM * DIM] = {0};
  double delta_x[EDIM] = {0};
  double x_new[DIM] = {0};


  // state x, P
  Eigen::Matrix<double, ZDIM, 1> z(in_z);
  EEM P(in_P);
  ZZM pre_R(in_R);

  // functions from sympy
  h_fun(in_x, in_ea, in_hx);
  H_fun(in_x, in_ea, in_H);
  ZDM pre_H(in_H);

  // get y (y = z - hx)
  Eigen::Matrix<double, ZDIM, 1> pre_y(in_hx); pre_y = z - pre_y;
  X1M y; XXM H; XXM R;
  if (Hea_fun){
    typedef Eigen::Matrix<double, ZDIM, EADIM, Eigen::RowMajor> ZAM;
    double in_Hea[ZDIM * EADIM] = {0};
    Hea_fun(in_x, in_ea, in_Hea);
    ZAM Hea(in_Hea);
    XXM A = Hea.transpose().fullPivLu().kernel();


    y = A.transpose() * pre_y;
    H = A.transpose() * pre_H;
    R = A.transpose() * pre_R * A;
  } else {
    y = pre_y;
    H = pre_H;
    R = pre_R;
  }
  // get modified H
  H_mod_fun(in_x, in_H_mod);
  DEM H_mod(in_H_mod);
  XEM H_err = H * H_mod;

  // Do mahalobis distance test
  if (MAHA_TEST){
    XXM a = (H_err * P * H_err.transpose() + R).inverse();
    double maha_dist = y.transpose() * a * y;
    if (maha_dist > MAHA_THRESHOLD){
      R = 1.0e16 * R;
    }
  }

  // Outlier resilient weighting
  double weight = 1;//(1.5)/(1 + y.squaredNorm()/R.sum());

  // kalman gains and I_KH
  XXM S = ((H_err * P) * H_err.transpose()) + R/weight;
  XEM KT = S.fullPivLu().solve(H_err * P.transpose());
  //EZM K = KT.transpose(); TODO: WHY DOES THIS NOT COMPILE?
  //EZM K = S.fullPivLu().solve(H_err * P.transpose()).transpose();
  //std::cout << "Here is the matrix rot:\n" << K << std::endl;
  EEM I_KH = Eigen::Matrix<double, EDIM, EDIM>::Identity() - (KT.transpose() * H_err);

  // update state by injecting dx
  Eigen::Matrix<double, EDIM, 1> dx(delta_x);
  dx  = (KT.transpose() * y);
  memcpy(delta_x, dx.data(), EDIM * sizeof(double));
  err_fun(in_x, delta_x, x_new);
  Eigen::Matrix<double, DIM, 1> x(x_new);

  // update cov
  P = ((I_KH * P) * I_KH.transpose()) + ((KT.transpose() * R) * KT);

  // copy out state
  memcpy(in_x, x.data(), DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
  memcpy(in_z, y.data(), y.rows() * sizeof(double));
}




}
extern "C" {

void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_25, H_25, NULL, in_z, in_R, in_ea, MAHA_THRESH_25);
}
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<2, 3, 0>(in_x, in_P, h_24, H_24, NULL, in_z, in_R, in_ea, MAHA_THRESH_24);
}
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_30, H_30, NULL, in_z, in_R, in_ea, MAHA_THRESH_30);
}
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_26, H_26, NULL, in_z, in_R, in_ea, MAHA_THRESH_26);
}
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_27, H_27, NULL, in_z, in_R, in_ea, MAHA_THRESH_27);
}
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_29, H_29, NULL, in_z, in_R, in_ea, MAHA_THRESH_29);
}
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_28, H_28, NULL, in_z, in_R, in_ea, MAHA_THRESH_28);
}
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_31, H_31, NULL, in_z, in_R, in_ea, MAHA_THRESH_31);
}
void car_err_fun(double *nom_x, double *delta_x, double *out_786078590364264470) {
  err_fun(nom_x, delta_x, out_786078590364264470);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_5919995043477341165) {
  inv_err_fun(nom_x, true_x, out_5919995043477341165);
}
void car_H_mod_fun(double *state, double *out_3543183180667111266) {
  H_mod_fun(state, out_3543183180667111266);
}
void car_f_fun(double *state, double dt, double *out_1497117634121373884) {
  f_fun(state,  dt, out_1497117634121373884);
}
void car_F_fun(double *state, double dt, double *out_7787221922284321900) {
  F_fun(state,  dt, out_7787221922284321900);
}
void car_h_25(double *state, double *unused, double *out_6182971352921756001) {
  h_25(state, unused, out_6182971352921756001);
}
void car_H_25(double *state, double *unused, double *out_3458468969911740743) {
  H_25(state, unused, out_3458468969911740743);
}
void car_h_24(double *state, double *unused, double *out_1709075460934367585) {
  h_24(state, unused, out_1709075460934367585);
}
void car_H_24(double *state, double *unused, double *out_5631118568917240309) {
  H_24(state, unused, out_5631118568917240309);
}
void car_h_30(double *state, double *unused, double *out_2313689328341983991) {
  h_30(state, unused, out_2313689328341983991);
}
void car_H_30(double *state, double *unused, double *out_3458221371579876012) {
  H_30(state, unused, out_3458221371579876012);
}
void car_h_26(double *state, double *unused, double *out_7458579391457631528) {
  h_26(state, unused, out_7458579391457631528);
}
void car_H_26(double *state, double *unused, double *out_7199972288785796967) {
  H_26(state, unused, out_7199972288785796967);
}
void car_h_27(double *state, double *unused, double *out_3146185301477423316) {
  h_27(state, unused, out_3146185301477423316);
}
void car_H_27(double *state, double *unused, double *out_1283458059779451101) {
  H_27(state, unused, out_1283458059779451101);
}
void car_h_29(double *state, double *unused, double *out_2687983939318861654) {
  h_29(state, unused, out_2687983939318861654);
}
void car_H_29(double *state, double *unused, double *out_429904667090099932) {
  H_29(state, unused, out_429904667090099932);
}
void car_h_28(double *state, double *unused, double *out_7768371330933531630) {
  h_28(state, unused, out_7768371330933531630);
}
void car_H_28(double *state, double *unused, double *out_5512303684159630506) {
  H_28(state, unused, out_5512303684159630506);
}
void car_h_31(double *state, double *unused, double *out_5907777290637250112) {
  h_31(state, unused, out_5907777290637250112);
}
void car_H_31(double *state, double *unused, double *out_3427823008034780315) {
  H_31(state, unused, out_3427823008034780315);
}
void car_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
void car_set_mass(double x) {
  set_mass(x);
}
void car_set_rotational_inertia(double x) {
  set_rotational_inertia(x);
}
void car_set_center_to_front(double x) {
  set_center_to_front(x);
}
void car_set_center_to_rear(double x) {
  set_center_to_rear(x);
}
void car_set_stiffness_front(double x) {
  set_stiffness_front(x);
}
void car_set_stiffness_rear(double x) {
  set_stiffness_rear(x);
}
}

const EKF car = {
  .name = "car",
  .kinds = { 25, 24, 30, 26, 27, 29, 28, 31 },
  .feature_kinds = {  },
  .f_fun = car_f_fun,
  .F_fun = car_F_fun,
  .err_fun = car_err_fun,
  .inv_err_fun = car_inv_err_fun,
  .H_mod_fun = car_H_mod_fun,
  .predict = car_predict,
  .hs = {
    { 25, car_h_25 },
    { 24, car_h_24 },
    { 30, car_h_30 },
    { 26, car_h_26 },
    { 27, car_h_27 },
    { 29, car_h_29 },
    { 28, car_h_28 },
    { 31, car_h_31 },
  },
  .Hs = {
    { 25, car_H_25 },
    { 24, car_H_24 },
    { 30, car_H_30 },
    { 26, car_H_26 },
    { 27, car_H_27 },
    { 29, car_H_29 },
    { 28, car_H_28 },
    { 31, car_H_31 },
  },
  .updates = {
    { 25, car_update_25 },
    { 24, car_update_24 },
    { 30, car_update_30 },
    { 26, car_update_26 },
    { 27, car_update_27 },
    { 29, car_update_29 },
    { 28, car_update_28 },
    { 31, car_update_31 },
  },
  .Hes = {
  },
  .sets = {
    { "mass", car_set_mass },
    { "rotational_inertia", car_set_rotational_inertia },
    { "center_to_front", car_set_center_to_front },
    { "center_to_rear", car_set_center_to_rear },
    { "stiffness_front", car_set_stiffness_front },
    { "stiffness_rear", car_set_stiffness_rear },
  },
  .extra_routines = {
  },
};

ekf_lib_init(car)
