#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex>

using namespace std;

//extern "C" {
//functions
//main function to compute the path loss in dB
double pathLoss(const double Freq_THz, const double Comm_dist, const double Skin_thickness, const double Tissue_thickness, const double Vessel_thickness);

//evaluates the path loss in the vessel in dB
double pathLossVessel(double comm_dist_Vessel, double Freq_THz, double carrier_lambda);
//evaluates the path loss in the Tissue in dB
double pathLossTissue(double comm_dist_Tissue, double Freq_THz, double carrier_lambda);
//evaluates the path loss in the Skin in dB
double pathLossSkin(double comm_dist_Skin, double Freq_THz, double carrier_lambda);

//Doppler effect
double doppler(const double Freq_THz, const double Comm_dist, const double Skin_thickness, const double Tissue_thickness, const double Vessel_thickness, const double blood_speed);

//transceiver and channel impact
void transceiver(const double Freq_THz, const double dist_init_x, const double Skin_thickness, const double Tissue_thickness, const double Vessel_thickness, const double blood_speed, const int mod_order, const double *in_bits, const int total_bits, const double bit_rate, const double A, double *Const_Tx_real, double *Const_Tx_imag, double *Const_Rx_real, double *Const_Rx_imag, double *nanosensor_pos_x);

//convert power in the units of Watts to decibel
double convertWattTodB(const double x);

//convert power from dB to the units of Watts
double convertdBToWatt(const double x);

//convert cartesian to polar coordinates
void convertCart2Polar(const double x, const double y, double *r, double *theta);
//}