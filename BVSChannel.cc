/*
 * BVSChannel.cc
 * Copyright (c) 2025 Technische Universität Berlin
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *  Created on: 2024. 02. 13.
 *      Author: Yerim Lee
 */

#include "BVSChannel.h"
#include "NanoNetDevice.h"
#include "GatewayNetDevice.h"
#include <cstdlib>
#include <ctime>

namespace ns3{

BVSChannel::BVSChannel(float vesselthickness)
{
 vector<Ptr<NetDevice> > devices;

 m_devices = devices;

 vector<Ptr<GatewayNetDevice> > gdev;

 m_gatedevices = gdev;

 m_vesselthickness = vesselthickness;

}

BVSChannel::~BVSChannel()
{

}


void BVSChannel::Add(Ptr<NetDevice> device) {

	m_devices.push_back(device);

}

size_t BVSChannel::GetNDevices (void) const
{
	return m_devices.size();
}

Ptr<NetDevice> BVSChannel::GetDevice (size_t i) const 
{
	return m_devices[i];
}

void BVSChannel::AddGateway(Ptr<GatewayNetDevice> device)
{
	m_gatedevices.push_back(device);
}

Ptr<GatewayNetDevice> BVSChannel::findGateway( int gatewayID)
{
	for( int i = 0; i <= (int) m_gatedevices.size(); i++ )
	{
		if (m_gatedevices[i]->getPosition() == gatewayID)
		{
			return m_gatedevices[i];
		}
		
			
	}

	return nullptr;
}


void BVSChannel::send(Ptr<NanoNetDevice> ndev, int pos)
{
	Ptr<GatewayNetDevice> gdev = this->findGateway(pos);

	MAC_PHY_DATA *mac_phy_data = new MAC_PHY_DATA;

	*(mac_phy_data) = *(ndev->getMacPhyData());

    for(int i = 0; i < 300; ++i) {
		(*mac_phy_data).SEQ_TX[i] = (*mac_phy_data).SEQ_TX[i] * sqrt(POWER);
		(*mac_phy_data).SEQ_RX[i] = (*mac_phy_data).SEQ_TX[i];
    }

	// length of channel, 100 samples, memory, MULITPLICATION for now -> no memory
	double comm_dist = sqrt(pow(DIST_INIT,2) + pow(SKIN_THICKNESS + TISSUETHICKNESS + m_vesselthickness,2));
	
	double pathl{convertdBToWatt(-pathLoss(FREQ_THZ, comm_dist, SKIN_THICKNESS, TISSUETHICKNESS, m_vesselthickness))};

	transform((*mac_phy_data).SEQ_RX.begin(),(*mac_phy_data).SEQ_RX.end(), (*mac_phy_data).SEQ_RX.begin(), [&pathl](auto& c){return c*sqrt(pathl);});

	cout << "Pathloss : " << pathl << endl;

	vector<double> noise = createNoiseSequence();

	/*for (int i = 0; i < 300; ++i) {
        cout << "Noise : " << noise[i] << endl;
    }*/
	

    for (int i = 0; i < 300; ++i) {
        (*mac_phy_data).SEQ_RX[i] += noise[i];
    }

	gdev->Receive(mac_phy_data, m_vesselthickness);
}

vector<double> BVSChannel::createNoiseSequence()
{
	
	random_device rd{};
	mt19937 gen{rd()};

	double c = 299792458.0; //speed of light in vacuum
	double kB = 1.38064852e-23; //Boltzman constant
	double t0 = 310; //reference temperature
	double k = 0.0072; //extinction coefficient from blood absoprtion coefficient

	double Tmol = t0 * (1- exp((-4*M_PI*FREQ_THZ*(SKIN_THICKNESS+TISSUETHICKNESS+m_vesselthickness)*k)/c));  
	double No = kB * Tmol;//noise density
	double variance = No * BW; //noise power

	double comm_dist = sqrt(pow(DIST_INIT,2) + pow(SKIN_THICKNESS + TISSUETHICKNESS + m_vesselthickness,2));
	double pathl{convertdBToWatt(-pathLoss(FREQ_THZ, comm_dist, SKIN_THICKNESS, TISSUETHICKNESS, m_vesselthickness))};
	double gamma = (pathl * POWER) / variance;
	
	double stddev = sqrt(variance); // standard deviation of noise

	normal_distribution d{0.0, stddev};
	
	vector<double> ret_val(300, 0);

	for(int i = 0; i < 300; i++)
	{
		ret_val[i] = d(gen);
	}

	return ret_val;
	
	//cout << "Eb/N0 : " << gamma << endl;
	//cout << "Eb/N0 : " << 10*log10(gamma) << " dB" << endl;
	//cout << "Noise density : " << No << endl;
	//cout << "Noise power : " << variance << endl;
}
	
   /*
	// Seed the random number generator
    std::srand(42); // Using a fixed seed value

	// Vector to store the output
    std::vector<double> ret_val(TESTPACKETSIZE);

    // Generate white Gaussian noise using Box-Muller Transform
    for (int j = 0; j < TESTPACKETSIZE; ++j) {
        double u1, u2, s;

        // Generate two uniform random numbers in (0, 1)
        u1 = (std::rand() + 1.0) / (RAND_MAX + 1.0); // +1 to avoid 0
        u2 = (std::rand() + 1.0) / (RAND_MAX + 1.0); // +1 to avoid 0

        // Box-Muller transform
        double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2); // Standard normal variable
        ret_val[j] = z0 * stddev; // Scale by the standard deviation
    }
	return ret_val;

	*/



}


