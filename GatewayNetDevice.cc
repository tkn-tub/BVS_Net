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

    for(int i = 0; i < TESTPACKETSIZE; i++)
    {
        if ((*data).SEQ_RX[i] < 0)
        (*data).PDU_RX[i] = 0;
        else
        (*data).PDU_RX[i] = 1;
    }

    bitset<32> decodedData = decode48bits((*data).PDU_RX);
    
    for(int i = 0; i < TESTPACKETSIZE; i++) {
        (*data).PDU_RX[i] = decodedData[i];
    }

    m_storage.push_back((*data));

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

    for (double i : data->SEQ_RX)
            afout << i << ",";

    afout << "\n";

    fstream fout;

    fout.open("gateway.csv", ios::out | ios::app); 

    fout  << data->nanobot_ID << "; "
            << data->tissue_ID << "; ";

    for (int i : data->PDU_TX)
            fout << i << ",";

    fout << "; ";

    for (double i : data->SEQ_TX)
            fout << i << ",";

    fout << "; ";

    for (double i : data->SEQ_RX)
            fout << i << ",";
            
    fout << "; ";

    for (int i : data->PDU_RX)
            fout << i << ",";

    fout << "; ";

    fout << "\n";


    int be = CalculateErrorBits(data);

    int pe = 0;

    fstream foutber;

    foutber.open("gateway_ber.csv", ios::out | ios::app); 

    foutber <<  be << ",";

    foutber << size(data->PDU_RX) << ",";

    if (be > 0)
        pe = 1;

    foutber << pe << ",";

    foutber << vesselthickness << "\n";
}

int GatewayNetDevice::CalculateErrorBits(MAC_PHY_DATA *data)
{
int retval = 0;

for(int i = 0; i < TESTPACKETSIZE; i++)
{
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

    bitset<8> GatewayNetDevice::decode(const bitset<12>& encodedData) {
        bitset<8> data;

        //extract data bits
        data[0] = encodedData[2];
        data[1] = encodedData[4];
        data[2] = encodedData[5];
        data[3] = encodedData[6];
        data[4] = encodedData[8];
        data[5] = encodedData[9];
        data[6] = encodedData[10];
        data[7] = encodedData[11];

        //calcualte the error position using parity check
        int p1 = encodedData[0] ^ encodedData[2] ^ encodedData[4] ^ encodedData[6] ^ encodedData[8] ^ encodedData[10];
        int p2 = encodedData[1] ^ encodedData[2] ^ encodedData[5] ^ encodedData[6] ^ encodedData[9] ^ encodedData[10];
        int p4 = encodedData[3] ^ encodedData[4] ^ encodedData[5] ^ encodedData[6] ^ encodedData[11];
        int p8 = encodedData[7] ^ encodedData[8] ^ encodedData[9] ^ encodedData[10] ^ encodedData[11];

        int errorPosition = (p8 << 3) | (p4 << 2) | (p2 << 1) | p1;

        //correct error if it exists
        if (errorPosition != 0) {
            encodedData.flip(errorPosition - 1); //Bei flip werden die Bitwerte umgedreht, indem Nullen in Einsen und Einsen in Nullen umgewandelt werden
        }

        //extract the modified data
        data[0] = encodedData[2];
        data[1] = encodedData[4];
        data[2] = encodedData[5];
        data[3] = encodedData[6];
        data[4] = encodedData[8];
        data[5] = encodedData[9];
        data[6] = encodedData[10];
        data[7] = encodedData[11];

        return data;
    }

    bitset<32> GatewayNetDevice::decode48bits(const vector<bitset<12>>& encodedData) {
        bitset<32> data;

        for (int i = 0; i < 4; ++i) {
            bitset<8> decodedBlock = decode(encodedData[i]);
            for (int j = 0; j < 8; ++j) {
                data[i * 8 + j] = decodedBlock[j];
            }
        }
        return data;
    }

}


