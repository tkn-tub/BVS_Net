/*
 * GatewayNetDevice.cc
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
 *
 *  Created on: 2024. 02. 13.
 *      Author: Yerim Lee
 */


#include "GatewayNetDevice.h"

namespace ns3{

GatewayNetDevice::GatewayNetDevice() {

	m_channel = nullptr;

	m_node = nullptr;

    m_position = 0;

    vector<MAC_PHY_DATA> m_storage;

}

GatewayNetDevice::GatewayNetDevice(int pos) {

	m_channel = nullptr;

	m_node = nullptr;

    m_position = pos;

    vector<MAC_PHY_DATA> m_storage;
}

GatewayNetDevice::~GatewayNetDevice() {

}
/*
TypeId GatewayNetDevice::GetTypeId(void) {
	static TypeId tid = TypeId ("ns3::GatewayNetDevice").SetParent<Object> ().AddConstructor<GatewayNetDevice> ();
	return tid;
}
*/

void GatewayNetDevice::Receive(MAC_PHY_DATA *data, float vesselthickness)
{
    // OOK Demodulation
    vector<int> demodulatedData(300, 0);
    for (size_t i = 0; i < 300; ++i) {
        if (data->SEQ_RX[i] > 0) {
            demodulatedData[i] = 1;
        } else {
            demodulatedData[i] = 0;
        }
    }
    data->PDU_RX2 = demodulatedData;
    
    // Hamming Decoding
    vector<int> decodedData(200, 0);
    for (int i = 0; i < 25; ++i) {
        vector<int> block(12, 0);
        for (int j = 0; j < 12; ++j) {
            block[j] = data->PDU_RX2[i * 12 + j]; // extract 12bits
        }

        // decode each 12-bits block into 8-bits block
        vector<int> decodedBlock = decodeBlock(block);

        // integrate the decoded 8-bits block in a data packet
        for (int j = 0; j < 8; ++j) {
            decodedData[i * 8 + j] = decodedBlock[j];
        }
    }
    data->PDU_RX = decodedData;

    m_storage.push_back((*data));

    //cout << "Pathloss : " << pathl << endl;

    fstream aiout;
    aiout.open("gateway_data.csv", ios::out | ios::app);
    for(int i : data->PDU_RX2)
        aiout << i << ",";
    aiout << "\n";

    /*fstream afout;

    afout.open("gateway_amplitudes.csv", ios::out | ios::app);

    for (double i : data->SEQ_RX)
            afout << i << ",";

    afout << "\n";*/

    /*fstream fout;

    fout.open("gateway_pdurx.csv", ios::out | ios::app); 

    fout << data->nanobot_ID << "; " << data->tissue_ID << "; ";
    fout << "PDU_TX,";
    for (int i : data->PDU_TX) fout << i << ",";
    fout << "\n";
    for (double i : data->PDU_TX2) fout << i << ",";
    fout << "; ";
    for (double i : data->SEQ_TX) fout << i << ",";
    fout << "; ";
    for (double i : data->SEQ_RX) fout << i << ",";
    fout << "; ";
    for (double i : data->PDU_RX2) fout << i << ",";
    fout << "; ";
    fout << "PDU_RX,";
    for (double i : data->PDU_RX) fout << i << ",";
    fout << "; ";
    fout << "\n";*/

    //fstream foutber;

    //foutber.open("gateway_ber.csv", ios::out | ios::app);
    //double ber = CalculateErrorBits(data);
    //foutber << ber << ",";
    //foutber << size(data->PDU_RX) << "\n";

    //cout << "Bit Error Rate: " << ber << endl;

    StoreDataFrame(data);

    fstream foutberglob;
    foutberglob.open("gateway_ber_global.csv", ios::out | ios::app);
    if (allDataFrames.size() >= TESTPACKETSIZE) {
        double total_ber = CalculateOverallBER();
        foutberglob << total_ber << "\n";
    }
    double global_ber = CalculateOverallBER();
    foutberglob << global_ber << "\n";
    cout << "Bit Error Rate: " << global_ber << endl;

}

int GatewayNetDevice::CalculateErrorBits(MAC_PHY_DATA *data) {
    int retval = 0;

    for (size_t i = 0; i < data->PDU_RX.size(); ++i) {
        if (i >= data->PDU_TX.size()) break;

        for (int j = 0; j < TESTPACKETSIZE; ++j) {
            if (data->PDU_RX[i] != data->PDU_TX[i]) {
                if (((data->PDU_RX[i] >> j) & 1) != ((data->PDU_TX[i] >> j) & 1)) {
                    retval++;
                }
            }
        }
    }
    return retval;
}

void GatewayNetDevice::StoreDataFrame(MAC_PHY_DATA *data) {
    allDataFrames.push_back(*data); 
}

double GatewayNetDevice::CalculateOverallBER() {
    int totalErrors = 0;
    int totalBits = 0;

    for (MAC_PHY_DATA &data : allDataFrames) { 
        totalErrors += CalculateErrorBits(&data); 
        totalBits += data.PDU_TX.size() * TESTPACKETSIZE; 
    }

    if (totalBits > 0) {
        return static_cast<double>(totalErrors) / totalBits;
    } else {
        return 0.0;
    }
}

double GatewayNetDevice::CalculateBER(MAC_PHY_DATA *data) {
    int errorBits = CalculateErrorBits(data);

    double totalBits = static_cast<double>(data->PDU_TX.size() * TESTPACKETSIZE);

    if (totalBits > 0) {
        return static_cast<double>(errorBits) / totalBits;
    } else {
        return 0.0;
    }
}

Address GatewayNetDevice::GetAddress() const
     {
        return m_address;
     }

void GatewayNetDevice::SetAddress(Address address)
    {
        m_address = Mac48Address::ConvertFrom(address);
    }

Ptr<Channel> GatewayNetDevice::GetChannel() const
    {
        return m_channel;
    }


void GatewayNetDevice::SetChannel(Ptr<BVSChannel> channel) 
	{
		m_channel = channel;
		m_channel->Add (this);
	}

Ptr<Node> GatewayNetDevice::GetNode() const
    {
        return m_node;
    }

    void GatewayNetDevice::SetNode(Ptr<Node> node)
    {
        m_node = node;
    }

    bool GatewayNetDevice::Send(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber) 
    {
        return SendFrom(packet, m_address, dest, protocolNumber);
    }

    bool GatewayNetDevice::SendFrom(Ptr<Packet> packet,
                  const Address& source,
                  const Address& dest,
                  uint16_t protocolNumber)
    {
        // to be initliazed

        return false;
    }

    void GatewayNetDevice::installToNode(Ptr<Node> node)
    {
        node->AddDevice(this);
    }

    int GatewayNetDevice::getPosition()
    {
        return m_position;
    }

    vector<int> GatewayNetDevice::decodeBlock(const vector<int>& block) {
        int errorPosition = detectError(block);

        vector<int> data(8, 0);

        //extract data bits
        data[0] = block[2]; //d1
        data[1] = block[4]; //d2
        data[2] = block[5]; //d3
        data[3] = block[6]; //d4
        data[4] = block[8]; //d5
        data[5] = block[9]; //d6
        data[6] = block[10]; //d7
        data[7] = block[11]; //d8

        if (errorPosition > 0 && errorPosition <= 12) {
            int correctedIndex = errorPosition - 1;
            if (correctedIndex >= 2 && correctedIndex <= 11) {
                // flip the corresponing bit if data bit
                if (correctedIndex == 2) data[0] ^= 1;
                else if (correctedIndex == 4) data[1] ^= 1;
                else if (correctedIndex == 5) data[2] ^= 1;
                else if (correctedIndex == 6) data[3] ^= 1;
                else if (correctedIndex == 8) data[4] ^= 1;
                else if (correctedIndex == 9) data[5] ^= 1;
                else if (correctedIndex == 10) data[6] ^= 1;
                else if (correctedIndex == 11) data[7] ^= 1;
            }
        }
        return data;
    }

    int GatewayNetDevice::detectError(const vector<int>& block) {
        // calculate position of error using parity check, Hamming code detects only 1 error at one time
        int p1 = block[0] ^ block[2] ^ block[4] ^ block[6] ^ block[8] ^ block[10];
        int p2 = block[1] ^ block[2] ^ block[5] ^ block[6] ^ block[9] ^ block[10];
        int p3 = block[3] ^ block[4] ^ block[5] ^ block[6] ^ block[11];
        int p4 = block[7] ^ block[8] ^ block[9] ^ block[10] ^ block[11];

        int errorPos = (p4 << 3) | (p3 << 2) | (p2 << 1) | p1;
        return errorPos; // 0 means no error, 1 means 1st bit is error, 2 means ... 12 means 12th bit is error
    }


}


