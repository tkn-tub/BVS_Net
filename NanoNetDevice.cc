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

        vector<int> zero_bits(TESTPACKETSIZE, 0);

        vector<double> zero_samples(TESTPACKETSIZE, 0);

        m_mac_phy_data = new MAC_PHY_DATA;

        (*m_mac_phy_data).nanobot_ID = 0; 

        (*m_mac_phy_data).tissue_ID = 0;

        (*m_mac_phy_data).PDU_RX = zero_bits;

        (*m_mac_phy_data).PDU_TX = zero_bits;

        (*m_mac_phy_data).SEQ_RX = zero_samples;

        (*m_mac_phy_data).SEQ_TX = zero_samples;
    
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

        vector<int> random_bits(TESTPACKETSIZE, 0);
        vector<double> random_samples(TESTPACKETSIZE, 0);

        // create random bit and apply OOK modulation scheme
        for(int i = 0; i < TESTPACKETSIZE; i++ )
        {
            random_bits[i] = rand()%2;

            if (random_bits[i] == 1)
                random_samples[i] = 1;
            else
                random_samples[i] = -1;
        }

        (*m_mac_phy_data).PDU_TX = random_bits;

        (*m_mac_phy_data).SEQ_TX = random_samples;

        (*m_mac_phy_data).nanobot_ID = nanobot_ID;

        (*m_mac_phy_data).tissue_ID = tissue_ID;



    }

    
    MAC_PHY_DATA *NanoNetDevice::getMacPhyData()
    {
        return m_mac_phy_data;
    }

}
