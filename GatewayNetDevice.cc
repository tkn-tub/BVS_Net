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

    /*for(int i = 0; i < TESTPACKETSIZE; i++)
    {
        if ((*data).SEQ_RX[i] < 0)
        (*data).PDU_RX2[i] = 0;
        else
        (*data).PDU_RX2[i] = 1;
    }*/
    // OOK Demodulation
    for (const auto& packet : data->SEQ_RX) {
        bitset<48> demodulatedPacket;
        for (size_t i = 0; i < packet.size(); i++) {
            demodulatedPacket[i] = (packet[i] > 0) ? 1 : 0;
        } 
        data->PDU_RX2.push_back(demodulatedPacket);
    }

    // Hamming Decoding
    for (size_t i = 0; i < TESTPACKETSIZE; i++) {
        bitset<48> encodedBlock = data->PDU_RX2[i];

        // divide 48 data block into 4 blocks
        bitset<12> block1 = extractBlock(encodedBlock, 0);
        bitset<12> block2 = extractBlock(encodedBlock, 12);
        bitset<12> block3 = extractBlock(encodedBlock, 24);
        bitset<12> block4 = extractBlock(encodedBlock, 36);

        // decode each block into 8 bits
        bitset<8> decodedBlock1 = decodeBlock(block1);
        bitset<8> decodedBlock2 = decodeBlock(block2);
        bitset<8> decodedBlock3 = decodeBlock(block3);
        bitset<8> decodedBlock4 = decodeBlock(block4);

        // integrate these 4 blocks into a 32bits frame
        bitset<32> decodedPacket(0);
        for (int j = 0; j < 8; j++) {
            decodedPacket[j] = decodedBlock1[j];
            decodedPacket[j + 8] = decodedBlock2[j];
            decodedPacket[j + 16] = decodedBlock3[j];
            decodedPacket[j + 24] = decodedBlock4[j];
        }
        data->PDU_RX.push_back(decodedPacket);
    }


    /*bitset<32> decodedData = decode((*data).PDU_RX2);
    
    for(int i = 0; i < TESTPACKETSIZE; i++) {
        (*data).PDU_RX[i] = decodedData[i];
    }

    m_storage.push_back((*data));*/

    //output decoded data
    /*for(int i = 0; i < TESTPACKETSIZE; i++) {
        cout << (*data).PDU_RX[i] << " | ";
    }
    cout << "\n";*/
    
    //uncomment for terminal output
    /* 
    cout << "dec : | ";

    for(int i = 0; i < TESTPACKETSIZE; i++)
    {
        cout << (*data).PDU_RX[i] << " | ";
    }

    cout << "\n";
    */

    fstream afout;

    afout.open("gateway_amplitudes.csv", ios::out | ios::app);

    for (size_t i = 0; i < data->SEQ_RX.size(); ++i) {
        const vector<double>& packet = data->SEQ_RX[i];
        for (size_t j = 0; j < packet.size(); ++j) {
            afout << packet[j];
            if (j != packet.size() - 1) {
                afout << ",";
            }
        }
        afout << "\n";
    }
    

    fstream fout;

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
    }

    fstream bout;

    bout.open("gateway_rec.csv", ios::out | ios::app);

    for (size_t i = 0; i < TESTPACKETSIZE; ++i) {
        const bitset<32>& packet = data->PDU_RX[i];
        for (size_t j = 0; j < 32; ++j) {
            bout << packet[j];
            if (j < 31) bout << ",";
        }
        bout << "\n";
    } 

    /*for (size_t i = 0; i < data->PDU_TX.size(); ++i) {
        int bit = data->PDU_TX[i];
        fout << bit << ",";
    }  
    fout << "; ";
    fout << "\n"; */                 

    /*for (int i : data->PDU_TX)
            fout << i << ",";

    fout << "; ";*/

    /*for (size_t i = 0; i < data->PDU_TX2.size(); ++i) {
        int bit = data->PDU_TX2[i];
        fout << bit << ",";
    }
    fout << "; ";
    fout << "\n";*/

    /*for (int i : data->PDU_TX2)
            fout << i << ",";

    fout << "; ";*/

    /*for (size_t i = 0; i < data->SEQ_TX.size(); ++i) {
        int bit = data->SEQ_TX[i];
        cout << bit << ",";
    }
    fout << "; ";
    fout << "\n";*/

    /*for (double i : data->SEQ_TX)
            fout << i << ",";

    fout << "; ";*/

    /*for (double i : data->SEQ_RX)
            fout << i << ",";
            
    fout << "; ";
    fout << "\n";*/

    /*for (size_t i = 0; i < data->PDU_RX2.size(); ++i) {
        int bit = data->PDU_RX2[i];
        fout << bit << ",";
    }
    fout << "; ";
    fout << "\n";*/

    /*for (int i : data->PDU_RX2)
            fout << i << ",";

    fout << "; ";*/

    /*for (size_t i = 0; i < data->PDU_RX.size(); ++i) {
        int bit = data->PDU_RX[i];
        fout << bit << ",";
    }
    fout << "; ";
    fout << "\n";*/

    /*for (int i : data->PDU_RX)
            fout << i << ",";

    fout << "; ";

    fout << "\n";*/


    //int be = CalculateErrorBits(data);

    //int pe = ;

    fstream foutber;

    foutber.open("gateway_ber.csv", ios::out | ios::app);
    double ber = CalculateBER(data);
    foutber << ber << ",";
    foutber << "\n";
    
    //foutber << be << ",";
    //foutber << size(data->PDU_RX) << ",";

    fstream foutper;

    foutper.open("gateway_per.csv", ios::out | ios::app);
    double per = CalculatePER(data);
    foutper << per << ",";
    foutper << vesselthickness << "\n";

    /*if (be > 0)
        pe = 1;

    foutber << pe << ",";

    foutber << vesselthickness << "\n";*/
}

double GatewayNetDevice::CalculateBER(MAC_PHY_DATA *data) {
    int errorCount = 0;

    for (int i = 0; i < TESTPACKETSIZE; i++) {
        errorCount += (data->PDU_RX[i] ^ data->PDU_TX[i]).count(); // calculate how many erroneous bits in a frame by using XOR
    } // XOR returns 0 if there is no differet bit, 1 if there is different bit

    double totalBits = static_cast<double>(TESTPACKETSIZE * 32);
    double ber = static_cast<double>(errorCount) / totalBits; // BER = erroneous bits / total bits in a whole test simulation
    return ber;
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

    bitset<8> GatewayNetDevice::decodeBlock(const bitset<12>& block) {
        int errorPosition = detectError(block);

        bitset<8> data;

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
            int correctedIndex = -1;

            switch (errorPosition) {
            case 3: correctedIndex = 0; break;  // d1 (block[2])
            case 5: correctedIndex = 1; break;  // d2 (block[4])
            case 6: correctedIndex = 2; break;  // d3 (block[5])
            case 7: correctedIndex = 3; break;  // d4 (block[6])
            case 9: correctedIndex = 4; break;  // d5 (block[8])
            case 10: correctedIndex = 5; break; // d6 (block[9])
            case 11: correctedIndex = 6; break; // d7 (block[10])
            case 12: correctedIndex = 7; break; // d8 (block[11])

            if (correctedIndex != -1) {
                data.flip(correctedIndex);  // correct error
                }
            }
            
            /*bitset<12> correctedBlock = block;
            correctedBlock.flip(errorPosition - 1);

            data[0] = correctedBlock[2];
            data[1] = correctedBlock[4];
            data[2] = correctedBlock[5];
            data[3] = correctedBlock[6];
            data[4] = correctedBlock[8];
            data[5] = correctedBlock[9];
            data[6] = correctedBlock[10];
            data[7] = correctedBlock[11];*/
        }

        return data;
    }

    int GatewayNetDevice::detectError(const bitset<12>& block) {
        // calculate position of error using parity check, Hamming code detects only 1 error at one time
        int p1 = block[0] ^ block[2] ^ block[4] ^ block[6] ^ block[8] ^ block[10];
        int p2 = block[1] ^ block[2] ^ block[5] ^ block[6] ^ block[9] ^ block[10];
        int p3 = block[3] ^ block[4] ^ block[5] ^ block[6] ^ block[11];
        int p4 = block[7] ^ block[8] ^ block[9] ^ block[10] ^ block[11];

        int errorPos = (p4 << 3) | (p3 << 2) | (p2 << 1) | p1;
        return errorPos; // 0 means no error, 1 means 1st bit is error, 2 means ... 12 means 12th bit is error
    }

    /*void GatewayNetDevice::flipBit(bitset<12>& block, int position) {
        //block[position] = block[position] == 1 ? 0 : 1;
        block.flip(position);
    }*/

    bitset<12> GatewayNetDevice::extractBlock(const bitset<48>& data, int startBit) {
       bitset<12> block;
        for (int i = 0; i < 12; i++) {
            block[i] = data[startBit + i];
        }
        return block;
    }

    
    //bitset<32> GatewayNetDevice::decodePacket(const bitset<48>& encodedPacket) {
    //    bitset<32> decodedPacket;

    //    for (int i = 0; i < 4; ++i) {
    //        bitset<12> block;
    //        for(int j = 0; j < 12; ++j) {
    //            block[j] = encodedPacket[i * 12 + j];
    //        }

    //        bitset<8> decodedBlock = decodeBlock(block);

    //        for (int j = 0; j < 8; ++j) {
    //            decodedPacket[i * 8 + j] = decodedBlock[j];
    //        }
    //    }

        // divide 48-bits in 4 blocks of 12-bits and decode each block
        /*for (int i = 0; i < 4; ++i) {
            bitset<12> block;

            for (int j = 0; j < 12; ++j) {
                block[j] = encodedPacket[i * 12 + j];
            }

            // detect the error position
            int errorPos = detectError(block);

            // if there is an error, flip the corresponding bit
            if (errorPos > 0 && errorPos <= 12) {
                flipBit(block, errorPos - 1);
            }

            // 12-bits block into 8-bits block
            bitset<8> dataBits = decodeBlock(block);

            // store as 32-bits data
            for (int j = 0; j < 8; ++j) {
                decodedData[i * 8 + j] = dataBits[j];
            }
        }*/
    //    return decodedPacket;
    //}
    

}


