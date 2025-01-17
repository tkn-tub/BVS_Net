/*
 * NanoNetDevice.cc
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



#include "NanoNetDevice.h"

namespace ns3 {
/*
    TypeId
    NanoNetDevice::GetTypeId()
    {
        static TypeId tid = TypeId("ns3::SimpleTag")
                                .SetParent<Tag>()
                                .SetGroupName("Network")
                                .AddConstructor<NanoNetDevice>();
        return tid;
    }
*/
    NanoNetDevice::NanoNetDevice ()
    {

        m_channel = nullptr;

        m_node = nullptr;

        //vector<int> zero_bits(TESTPACKETSIZE, 0);

        vector<vector<double>> SEQ_RX;

        vector<bitset<32>> PDU_TX;

        vector<bitset<48>> PDU_TX2;

        vector<vector<int>> SEQ_TX;

        //vector<vector<int>> zero_samples_decoding;

        //vector<bitset<48>> PDU_RX2;

        vector<bitset<32>> PDU_RX;

        m_mac_phy_data = new MAC_PHY_DATA;

        (*m_mac_phy_data).nanobot_ID = 0; 

        (*m_mac_phy_data).tissue_ID = 0;

        /*(*m_mac_phy_data).PDU_TX = zero_bits;

        (*m_mac_phy_data).PDU_TX2 = zero_bits_fec_encoding;

        (*m_mac_phy_data).SEQ_TX = zero_samples_encoding;

        (*m_mac_phy_data).SEQ_RX = zero_samples;

        (*m_mac_phy_data).PDU_RX2 = zero_bits_decoding;

        (*m_mac_phy_data).PDU_RX = zero_bits_fec_decoding;*/
    
    }



    NanoNetDevice::~NanoNetDevice () 
    {

    }

     Address NanoNetDevice::GetAddress() const
     {
        return m_address;
     }

    void NanoNetDevice::SetAddress(Address address)
    {
        m_address = Mac48Address::ConvertFrom(address);
    }

    Ptr<Channel> NanoNetDevice::GetChannel() const
    {
        return m_channel;
    }

    Ptr<BVSChannel> NanoNetDevice::GetBVSChannel()
    {
        return m_channel;   
    }

    void NanoNetDevice::SetChannel(Ptr<BVSChannel> channel)
    {
            m_channel = channel;
    }

    Ptr<Node> NanoNetDevice::GetNode() const
    {
        return m_node;
    }

    void NanoNetDevice::SetNode(Ptr<Node> node)
    {
        m_node = node;
    }

    bool NanoNetDevice::Send(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber) 
    {
        return SendFrom(packet, m_address, dest, protocolNumber);
    }
    bool NanoNetDevice::SendFrom(Ptr<Packet> packet,
                  const Address& source,
                  const Address& dest,
                  uint16_t protocolNumber)
    {

        return false;
    }

    void NanoNetDevice::installToNode(Ptr<Node> node)
    {
        node->AddDevice(this);
    }

    void NanoNetDevice::createMacPhyData(int tissue_ID, int nanobot_ID){

        //create random bits and samples and apply FEC encoder
        //vector<int> random_bits(TESTPACKETSIZE, 0);
        //bitset<32> random_bits;
        //bitset<32> data = random_bits;
        //random_device rd;
        //mt19937 gen(rd());
        //uniform_int_distribution<uint32_t> dis(0, 0xFFFFFFFF);
        //bitset<32> random_bits(dis(gen));
        //vector<bitset<32>> randompackets(200, random_bits); //generate 200 random packets which consist of 32bits packet

        //vector<double> random_samples(TESTPACKETSIZE,0);
        //bitset<32> random_samples;
        //bitset<32> sample = random_samples;

        //vector<bitset<48>> encodedPackets = encodePackets(randompackets);
        //bitset<48> encodedData = encode(data);
        //bitset<48> encodedSample = encode(sample);


        //(*m_mac_phy_data).PDU_TX = random_bits; //32bits

        vector<bitset<32>> random_bits(TESTPACKETSIZE);
        for (int i = 0; i < TESTPACKETSIZE; ++i) {
            for(int j = 0; j < 32; ++j) {
                random_bits[i][j] = rand() % 2;
            }
            //m_mac_phy_data->PDU_TX.push_back(bitset<32>(dis(gen)));
        }
        (*m_mac_phy_data).PDU_TX = random_bits; // PDU_TX is a storage for random 32 bits with 200 packet test size

        //(*m_mac_phy_data).PDU_TX2 = encodedData; //48bits
        vector<bitset<48>> encodedPDU_TX;
        encodedPDU_TX.reserve(TESTPACKETSIZE);
        for (int i = 0; i < TESTPACKETSIZE; ++i) {
            bitset<48> encodedData = encode(m_mac_phy_data->PDU_TX[i]);
            encodedPDU_TX.push_back(encodedData);
            //m_mac_phy_data->PDU_TX2.push_back(encodedData);
        }
        (*m_mac_phy_data).PDU_TX2 = encodedPDU_TX; // PDU_TX2 is a storage for the fec encoded data

        //output encoded data
        /*cout << "Original 32-bit data: " << data << endl;
        cout << "Encoded data (48 bits): ";
        for (const auto& bits : encodedData) {
            cout << bits;
        }
        cout << endl;*/

        /*apply OOK modulation scheme
        for(int i = 0; i < TESTPACKETSIZE; i++ )
        {
            encodedData[i] = rand()%2;

            if (encodedData[i] == 1)
                encodedData[i] = 1;
            else
                encodedData[i] = -1;
        }*/
        
        //OOK Modulation scheme
        for (int i = 0; i < TESTPACKETSIZE; ++i) {
            const bitset<48>& encodedPacket = m_mac_phy_data->PDU_TX2[i];
            vector<int> modulatedPacket(48);
            for (int j = 0; j < 48; ++j) {
                if (encodedPacket[j] == 1) {
                    modulatedPacket[j] = 1;
                } else {
                    modulatedPacket[j] = -1;
                }
            }
            (*m_mac_phy_data).SEQ_TX[i] = modulatedPacket;
            //m_mac_phy_data->SEQ_TX.push_back(modulatedPacket);
        }

        //(*m_mac_phy_data).SEQ_TX = modulatedSignal; //48bits

        //vector<int> random_bits(TESTPACKETSIZE, 0);
        //vector<double> random_samples(TESTPACKETSIZE, 0);

        // create random bit and apply OOK modulation scheme
        /*for(int i = 0; i < TESTPACKETSIZE; i++ )
        {
            random_bits[i] = rand()%2;

            if (random_bits[i] == 1)
                random_samples[i] = 1;
            else
                random_samples[i] = -1;
        }*/

        (*m_mac_phy_data).nanobot_ID = nanobot_ID;

        (*m_mac_phy_data).tissue_ID = tissue_ID;



    }

    
    MAC_PHY_DATA *NanoNetDevice::getMacPhyData()
    {
        return m_mac_phy_data;
    }

    /*vector<bitset<48>> NanoNetDevice::encodePackets(const vector<bitset<32>>& packets) {
        vector<bitset<48>> encodedPackets;

        for (const auto& packet : packets) {
            //encode each 32bits packet into 48bits
            encodedPackets.push_back(encode(packet));
        }
        return encodedPackets;
    }*/

    bitset<12> NanoNetDevice::encodeBlock(const bitset<8>& data) {
        bitset<12> encodedData;

        //data bits = 2,4,5,6,8,9,10,11
        encodedData[2] = data[0];
        encodedData[4] = data[1];
        encodedData[5] = data[2];
        encodedData[6] = data[3];
        encodedData[8] = data[4];
        encodedData[9] = data[5];
        encodedData[10] = data[6];
        encodedData[11] = data[7];

        //parity bits = 0,1,3,7
        encodedData[0] = encodedData[2] ^ encodedData[4] ^ encodedData[6] ^ encodedData[8] ^ encodedData[10]; // p1
        encodedData[1] = encodedData[2] ^ encodedData[5] ^ encodedData[6] ^ encodedData[9] ^ encodedData[10]; // p2
        encodedData[3] = encodedData[4] ^ encodedData[5] ^ encodedData[6] ^ encodedData[11];                  // p4
        encodedData[7] = encodedData[8] ^ encodedData[9] ^ encodedData[10] ^ encodedData[11];                 // p8

        return encodedData;
    }

    bitset<48> NanoNetDevice::encode(const bitset<32>& data) {
        bitset<48> encodedData;

        /*for(int i = 0; i < TESTPACKETSIZE; i += 8) {
            bitset<8> dataBlock;
            for(int j = 0; j < 8; ++j) {
                dataBlock[j] = data[i + j];
            }
            encodedData.push_back(encodeBlock(dataBlock));
        }*/

        for(int i = 0; i < 4; ++i) {
            bitset<8> block;

            for (int j = 0; j < 8; ++j) {
                block[j] = data[i * 8 + j];
            }

            bitset<12> encodedBlock = encodeBlock(block); // encode each 8-bits block into 12-bits
            
            for (int j = 0; j < 12; ++j) {
                encodedData[i * 12 + j] = encodedBlock[j];
            } // store encoded 12-bits block in 48-bits data storage
        }
        
        return encodedData; // 48bits
    }

}
