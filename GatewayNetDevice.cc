/*
 * GatewayNetDevice.cc
 * Copyright (c) 2024 Technische Universität Berlin 
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
 *  Created on: 2023. 12. 6.
 *      Author: Laurenz Ebner
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
        bitset<48> encodedBlock = data->PDU_RX2[i];

        // divide 48 data block into 4 blocks
        vector<int> block1 = extractBlock(demodulatedData, i * 12);
        vector<int> block2 = extractBlock(demodulatedData, i * 12 + 12);
        vector<int> block3 = extractBlock(demodulatedData, i * 12 + 24);
        vector<int> block4 = extractBlock(demodulatedData, i * 12 + 36);

        // decode each block into 8 bits
        vector<int> decodedBlock1 = decodeBlock(block1);
        vector<int> decodedBlock2 = decodeBlock(block2);
        vector<int> decodedBlock3 = decodeBlock(block3);
        vector<int> decodedBlock4 = decodeBlock(block4);

        // integrate these 4 blocks into a 200bits frame
        for (int j = 0; j < 8; ++j) {
            decodedData[i * 8 + j] = decodedBlock1[j];
            decodedData[i * 8 + 8 + j] = decodedBlock2[j];
            decodedData[i * 8 + 16 + j] = decodedBlock3[j];
            decodedData[i * 8 + 24 + j] = decodedBlock4[j];
        }
    }
    data->PDU_RX = decodedData;

    m_storage.push_back((*data));



    fstream afout;

    afout.open("gateway_amplitudes.csv", ios::out | ios::app);

    for (double i : data->SEQ_RX)
        afout << i << ",";

    afout << "\n";
    

    /*fstream fout;

    fout.open("gateway_sen.csv", ios::out | ios::app); 

    fout  << data->nanobot_ID << "; "
            << data->tissue_ID << "; ";

    for (size_t i = 0; i < TESTPACKETSIZE; ++i) {
        const bitset<32>& packet = data->PDU_TX[i];
        for (size_t j = 0; j < 32; ++j) {
            fout << packet[j];
            if (j < 31) fout << ",";
        }
        fout << "\n";
    }*/

    fstream fout;

    fout.open("gateway.csv", ios::out | ios::app);

    fout << data->nanobot_ID << "; " << data->tissue_ID << "; ";
    for (int i : data->PDU_TX) fout << i << ",";
    fout << "; ";
    for (double i : data->PDU_TX2) fout << i << ",";
    fout << "; ";
    for (double i : data->SEQ_TX) fout << i << ",";
    fout << "; ";
    for (double i : data->SEQ_RX) fout << i << ",";
    fout << "; ";
    for (double i : data->PDU_RX2) fout << i << ",";
    fout << "; ";
    for (double i : data->PDU_RX) fout << i << ",";
    fout << "; ";
    fout << "\n";

    fstream foutber;

    foutber.open("gateway_ber.csv", ios::out | ios::app);
    double ber = CalculateBER(data);
    foutber << ber << ",";
    foutber << size(data->PDU_RX) << "\n";

    cout << "Bit Error Rate (BER): " << ber << endl;
    
    //foutber << be << ",";
    //foutber << size(data->PDU_RX) << ",";

    //fstream foutper;

    //foutper.open("gateway_per.csv", ios::out | ios::app);
    //double per = CalculatePER(data);
    //foutper << per << ",";
    //foutper << vesselthickness << "\n";
    
    //cout << "Packet Error Rate (PER): " << per << endl;
    //cout << "Vessel Thickness: " << vesselthickness << endl;

    /*if (be > 0)
        pe = 1;

    foutber << pe << ",";

    foutber << vesselthickness << "\n";*/
}

double GatewayNetDevice::CalculateBER(MAC_PHY_DATA *data) {
    int errorCount = 0;

    for (size_t i = 0; i < data->PDU_RX.size(); ++i) {
        if (i >= data->PDU_TX.size()) break;
        for (size_t j = 0; j < TESTPACKETSIZE; ++j) {
            if (data->PDU_RX[i] != data->PDU_TX[i]) {
                errorCount++;
            }
        }
    }

    double totalBits = static_cast<double>(data->PDU_TX.size() * 200);

    if (totalBits > 0) {
        return static_cast<double>(errorCount) / totalBits; // BER = erroneous bits / total bits in a whole test simulation
    } else {
        return 0.0;
    }
}

double GatewayNetDevice::CalculatePER(MAC_PHY_DATA *data) {
    int errorCount = 0;

    for (int i = 0; i < TESTPACKETSIZE; i++) {
        if (data->PDU_RX[i] != data->PDU_TX[i]) {
            errorCount++;
        }
    }

    double per = static_cast<double>(errorCount) / TESTPACKETSIZE;
    return per;
}

int GatewayNetDevice::CalculateErrorBits(MAC_PHY_DATA *data) {
    int retval = 0;

    for(int i = 0; i < TESTPACKETSIZE; i++) {
        if(data->PDU_RX[i] != data->PDU_TX[i])
        retval++;
    }
    return retval;
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

    vector<int> GatewayNetDevice::extractBlock(const vector<int>& data, int startBit) {
        vector<int> block(12, 0);
        for (int i = 0; i < 12; ++i) {
            block[i] = data[startBit + i];
        }
        return block;
    }

    
    
    

}


