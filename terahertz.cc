#include "terahertz.h"


const double PI = 3.1415926535897932384626433;

const double m_skin_thickness = 0.1e-3;
const double m_fat_thickness = 1.25e-3;
const double m_blood_thickness = 2.7e-3;

const double full_thickness = 4.05e-3;

const double left_hand_gateway_x = 20;
const double right_hand_gateway_x = -30;
const double gateway_y = -42.73;
const double gateway_z = 0;

const double ALPHA_1_DERMIS = 0.92;
const double ALPHA_1_EPIDERMIS = 0.95;

const double ALPHA_2_DERMIS = 0.97;

const double BETA_1_DERMIS = 0.8;
const double BETA_1_EPIDERMIS = 0.96;

const double BETA_2_DERMIS = 0.99;

const double EPSILON_0 = 8.85E-12;

const double EPSILON_INF_BLOOD = 2.1;
const double EPSILON_INF_DERMIS = 4.0;
const double EPSILON_INF_EPIDERMIS = 3.0;

const double EPSILON_1_BLOOD = 130.0;
const double EPSILON_1_DERMIS = 5.96;
const double EPSILON_1_EPIDERMIS = 89.61;

const double EPSILON_2_BLOOD = 3.8;
const double EPSILON_2_DERMIS = 380.4;

const double TAU_1_BLOOD = 14.4E-12;
const double TAU_1_DERMIS = 1.6E-12;
const double TAU_1_EPIDERMIS = 15.9E-12;

const double TAU_2_BLOOD = 0.1E-12;
const double TAU_2_DERMIS = 159.0E-9;

const double SIGMA_DERMIS = 0.1;
const double SIGMA_EPIDERMIS = 0.01;

const double c_0 = 299792458.0; //speed of light in vacuum


/**
     * @brief main function to compute the path loss in dB
     * 
     * @param Comm_dist as the distance between the nanosensor and the Gateway 
     * @param Freq_THz as the carrier frequency in the units of Hertz 
     * @param Skin_thickness as the thickness of the Skin in the units of meters 
     * @param Tissue_thickness as the thickness of the Tissue in the units of meters
     * @param Vessel_thickness as the thickness of the Vessel in the units of meters
     * @return double 
     */
double pathLoss(const double Freq_THz, const double Comm_dist, const double Skin_thickness, const double Tissue_thickness, const double Vessel_thickness)
{   
    //spatial parameters
    //angle between the nanosensor and the gateway
    double sin_angle=(Skin_thickness+Tissue_thickness+Vessel_thickness)/Comm_dist;
    //communication distance in the vessel
    double comm_dist_Vessel=Vessel_thickness/sin_angle;
    //communication distance in the Tissue
    double comm_dist_Tissue=Tissue_thickness/sin_angle;
    //communication distance in the Skin
    double comm_dist_Skin=Skin_thickness/sin_angle;

    //signal parameters
    double carrier_lambda = c_0 / Freq_THz;


    //evaluating the path loss per segment in dB
    double Vessel_pathLoss = pathLossVessel(comm_dist_Vessel, Freq_THz, carrier_lambda);
    double Tissue_pathLoss = pathLossTissue(comm_dist_Tissue, Freq_THz, carrier_lambda);
    double Skin_pathLoss = pathLossSkin(comm_dist_Skin, Freq_THz, carrier_lambda);

    //return Tissue_pathLoss;
    return Vessel_pathLoss+Tissue_pathLoss+Skin_pathLoss;
    
}

/**
     * @brief evaluates the path loss in the vessel in dB
     * 
     * @param Comm_dist_Vessel as the distance between the nanosensor and the Gateway
     * @param Freq_THz as the carrier frequency in the units of Hertz 
     * @param carrier_lambda as the thickness of the Skin in the units of meters 
     * @return double 
     */

double pathLossVessel(double comm_dist_Vessel, double Freq_THz, double carrier_lambda)
{    
    const   complex<double> i(0.0,1.0);
    // refractiv index
    complex<double> sum1 = (EPSILON_1_BLOOD - EPSILON_2_BLOOD) / (1.0 + i * (2*PI*Freq_THz) * TAU_1_BLOOD);
    complex<double> sum2 = (EPSILON_2_BLOOD - EPSILON_INF_BLOOD) / (1.0 + i * (2*PI*Freq_THz) * TAU_2_BLOOD);
    complex<double> epsilon_r = EPSILON_INF_BLOOD + sum1 + sum2;

    complex<double> n = sqrt(epsilon_r);
    double n_1 = real(n);
    double n_2 = -imag(n);

    //computing the effective Wavelength
    double lambda_g = carrier_lambda/n_1;
    //computing the molecular Absorption Coefficient
    double mu_abs = (4 * PI * n_2) / lambda_g;
    //computing the spreading Loss
    double L_spr = pow((4 * PI * comm_dist_Vessel) / lambda_g, 2);
    //computing the absorption Loss
    double L_abs = exp(mu_abs * comm_dist_Vessel);
   
    return convertWattTodB(L_abs * L_spr);

}

/**
     * @brief evaluates the path loss in the Tissue in dB
     * 
     * @param comm_dist_Tissue as the distance between the nanosensor and the Gateway
     * @param Freq_THz as the carrier frequency in the units of Hertz 
     * @param carrier_lambda as the thickness of the Skin in the units of meters 
     * @return double 
     */
double pathLossTissue(double comm_dist_Tissue, double Freq_THz, double carrier_lambda)
{
    const   complex<double> i(0.0,1.0);
    // refractive index dermis
    complex<double> sum1 = EPSILON_1_DERMIS / pow(1.0 + pow(i * (2*PI*Freq_THz) * TAU_1_DERMIS, ALPHA_1_DERMIS), BETA_1_DERMIS);
    complex<double> sum2 = EPSILON_2_DERMIS / pow(1.0 + pow(i * (2*PI*Freq_THz) * TAU_2_DERMIS, ALPHA_2_DERMIS), BETA_2_DERMIS);
    complex<double> epsilon_r = EPSILON_INF_DERMIS + sum1 + sum2 - i * SIGMA_DERMIS / ((2*PI*Freq_THz) * EPSILON_0);

    complex<double> n = sqrt(epsilon_r);
    double n_1 = real(n); 
    double n_2 = -imag(n); //ERROR WHEN COMPILING WITH MATLAB; RETURNS 0

    //computing the effective Wavelength
    double lambda_g = carrier_lambda/n_1;
    //computing the molecular Absorption Coefficient
    double mu_abs = (4 * PI * n_2) / lambda_g;
    //computing the spreading Loss
    double L_spr = pow((4 * PI * comm_dist_Tissue) / lambda_g, 2);
    //computing the absorption Loss
    double L_abs = exp(mu_abs * comm_dist_Tissue);

    return convertWattTodB(L_abs * L_spr);
}

/**
     * @brief evaluates the path loss in the Skin in dB
     * 
     * @param comm_dist_Tissue as the distance between the nanosensor and the Gateway
     * @param Freq_THz as the carrier frequency in the units of Hertz 
     * @param carrier_lambda as the thickness of the Skin in the units of meters 
     * @return double 
     */
double pathLossSkin(double comm_dist_Skin, double Freq_THz, double carrier_lambda)
{      
    const   complex<double> i(0.0,1.0);
    // refractive index epidermis
    complex<double> sum1 = EPSILON_1_EPIDERMIS / pow(1.0 + pow(i * (2*PI*Freq_THz) * TAU_1_EPIDERMIS, ALPHA_1_EPIDERMIS), BETA_1_EPIDERMIS);
    complex<double> sum2 = i * SIGMA_EPIDERMIS / ((2*PI*Freq_THz) * EPSILON_0);
    complex<double> epsilon_r = EPSILON_INF_EPIDERMIS + sum1 - sum2;

    complex<double> n = sqrt(epsilon_r);
    double n_1 = real(n);
    double n_2 = -imag(n);

    //computing the effective Wavelength
    double lambda_g = carrier_lambda/n_1;
    //computing the molecular Absorption Coefficient
    double mu_abs = (4 * PI * n_2) / lambda_g;
    //computing the spreading Loss
    double L_spr = pow((4 * PI * comm_dist_Skin) / lambda_g, 2);
    //computing the absorption Loss
    double L_abs = exp(mu_abs * comm_dist_Skin);

    return convertWattTodB(L_abs * L_spr);
}

/**
     * @brief main function to compute the path loss in dB
     * 
     * @param Comm_dist as the distance between the nanosensor and the Gateway 
     * @param Freq_THz as the carrier frequency in the units of Hertz 
     * @param Skin_thickness as the thickness of the Skin in the units of meters 
     * @param Tissue_thickness as the thickness of the Tissue in the units of meters
     * @param Vessel_thickness as the thickness of the Vessel in the units of meters
     * @param blood_speed as the nanosensor speed in the vessel
     * @return double array for the Doppler term nu
     */
double doppler(const double Freq_THz, const double Comm_dist, const double Skin_thickness, const double Tissue_thickness, const double Vessel_thickness, const double blood_speed)
{
    //angle between the nanosensor and the gateway
    double sin_angle=(Skin_thickness+Tissue_thickness+Vessel_thickness)/Comm_dist;
    double cos_angle=sqrt(1-pow(sin_angle,2));
    //doppler term
    return blood_speed*cos_angle*Freq_THz/c_0;    
}

/**
     * @brief main function to compute the path loss in dB
     * 
     * @param dist_init_x as the initial nanosensor distance from the center in the direction of movement
     * @param Freq_THz as the carrier frequency in the units of Hertz 
     * @param Skin_thickness as the thickness of the Skin in the units of meters 
     * @param Tissue_thickness as the thickness of the Tissue in the units of meters
     * @param Vessel_thickness as the thickness of the Vessel in the units of meters
     * @param blood_speed as the nanosensor speed in the vessel
     * @param mod_order as the modulation order: 2-BPSK, 4-QPSK
     * @param in_bits as the sequence of bits to communicate   
     * @param total_bits as the total of bits to communicate
     * @param bit_rate as the bit rate for transmissions
     * @param A as the amplitude of emmited constellation points
     * @return Const_Tx_real real component of the emmited constellation points
     * @return Const_Tx_real imag component of the emmited constellation points
     * @return Const_Rx_real real component of the received constellation points
     * @return Const_Rx_real imag component of the received constellation points
     */
void transceiver(const double Freq_THz, const double dist_init_x, const double Skin_thickness, const double Tissue_thickness, const double Vessel_thickness, const double blood_speed, const int mod_order, const double *in_bits, const int total_bits, const double bit_rate, const double A, double *Const_Tx_real, double *Const_Tx_imag, double *Const_Rx_real, double *Const_Rx_imag, double *nanosensor_pos_x)
{
    int i=0;
    double comm_dist = 0;//to evalute the distance between the nanosensor and the gateway with time
    double path_Loss = 0;//to evaluate the path Loss with time
    //double g_d = 0;//to evaluate the channel gain with time
    double nu = 0; //to evaluate the doppler term nu
    double doppler_phase = 0; //to evaluate the doppler phase
    double t_time = 0; //time evolution of the process
    double mag = 0; //to evaluate the magnitude of received constellation points
    double theta = 0; //to evaluate the phase of received constellation points

    for (i=0; i<total_bits; i++)
    {
        //conforming the constellation
        //BPSK
        if(mod_order==2 || mod_order==0)
        {
            if(in_bits[i]<1)
            {
                Const_Tx_real[i] = -A;
            }
            else
            {
                Const_Tx_real[i] = A;
            }           

        }

        //channel transmission
        //computing the nanosensor position in the x axis
        nanosensor_pos_x[i] = -dist_init_x + blood_speed*t_time;
        //computing the comm. distance between nanosensor and the gateway with time
        comm_dist=sqrt(pow(nanosensor_pos_x[i],2)+pow(Skin_thickness+Tissue_thickness+Vessel_thickness,2));
        //computing the path Loss
        path_Loss=pathLoss(Freq_THz, comm_dist, Skin_thickness, Tissue_thickness, Vessel_thickness);
        //computing the doppler effect
        nu=doppler(Freq_THz, comm_dist, Skin_thickness, Tissue_thickness, Vessel_thickness,blood_speed);
        doppler_phase=2*PI*nu*t_time;
        //time evolution of the process
        t_time=t_time+i/bit_rate;
        //transforming the emmited constellation from Polar to 
        //evaluating the received constellation
        //Const_Rx_real[i] = convertdBToWatt(-path_Loss)*Const_Tx_real[i]*cos(doppler_phase);
        //Const_Rx_imag[i] = convertdBToWatt(-path_Loss)*Const_Tx_real[i]*sin(doppler_phase);
        convertCart2Polar(Const_Tx_real[i],Const_Tx_imag[i],&mag,&theta);
        Const_Rx_real[i] = mag*convertdBToWatt(-path_Loss)*cos(theta+doppler_phase);
        Const_Rx_imag[i] = mag*convertdBToWatt(-path_Loss)*sin(theta+doppler_phase);
//         Const_Rx_real[i] = mag*cos(theta+doppler_phase);
//         Const_Rx_imag[i] = mag*sin(theta+doppler_phase);
    }
}

void convertCart2Polar(const double x, const double y, double *r, double *theta)
{
    //magnitude
    *r=sqrt(pow(x,2)+pow(y,2));
    //angle
    *theta=atan(y/x);
    //looking for the proper quadrant
    if(x<0 && y>=0)//second quadrant
        *theta = PI + *theta;
    if(x<0 && y<0)//third quadrant
        *theta = PI - *theta;
}

/**
     * @brief convert power in the units of Watts to decibels
     * 
     * @param x as an input in the units of Watts
     * @return double in decibels
     */
double convertWattTodB(const double x)
{
    return 10 * log10(x);
}

/**
     * @brief convert dB to the units of power in Watts
     * 
     * @param x as an input in decibels
     * @return double in the units of Watts
     */
double convertdBToWatt(const double x)
{
    return pow(10,x/10);
}

