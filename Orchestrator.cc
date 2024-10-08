
/* -*-  Mode: C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 Universität zu Lübeck [GEYER]
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
 * Author: Regine Geyer <geyer@itm.uni-luebeck.de>, Chris Deter <chris.deter@student.uni-luebeck.de>
 */
#include "Orchestrator.h"

using namespace std;
namespace ns3 {

Orchestrator::Orchestrator ()
{
  output.open("csvNano.csv", ios::out | ios::trunc);
}

Orchestrator::~Orchestrator ()
{
  output.flush();
  output.close ();
}

void
Orchestrator::Call (Ptr<Nanobot> n, int vesselID)
{
  int id = n->GetNanobotID ();
  double x = n->GetPosition ().x;
  double y = n->GetPosition ().y;
  double z = n->GetPosition ().z;
  double m_start = Simulator::Now ().GetSeconds();
  int BvID = vesselID;

 

  int st = n->GetStream ();
  output << id << "," << x << "," << y << "," << z << "," << m_start << "," << BvID << "," << st
         << "\n";

   vector<int> GatewayPos = n->GetGatewayPositions();

   vector<int> IDs = n->Gettissue_ID();

  Ptr<NetDevice> dev =  n->GetDevice();

  Ptr<NanoNetDevice> nanodev =  n->GetNanoNetDevice();

  Ptr<BVSChannel> channel = nanodev->GetBVSChannel();

  for (int i : IDs)
  {
    if(i == vesselID){
    nanodev->createMacPhyData(i,id);

    cout << "new data measured by " << id << " at tissue " << vesselID << "\n";
    }


  }



  for(int i : GatewayPos){
    if(i == vesselID){
      cout << "Nanobot " << id << " found gateway at body part " << vesselID << "\n";
      
      channel->send(nanodev, i);
    }
  }

  
}

void
Orchestrator::PrintSomeNanobots (list<Ptr<Nanobot>> nbl, int vesselID)
{
  for (const Ptr<Nanobot> &bot : nbl)
    {
      Call (bot, vesselID);
    }
}

void
Orchestrator::PrintInTerminal (vector<Ptr<Bloodstream>> streamsOfVessel, int vesselIDl)
{
  cout.precision (3);
  cout << "VESSEL  ----------------" << vesselIDl << "--------" << endl;
  cout << "Time  ----------------" << Simulator::Now () << "--------" << endl;
  for (uint j = 0; j < streamsOfVessel.size (); j++)
    {
      cout << "Stream " << j + 1 << " ------------------------" << endl;
      for (uint i = 0; i < streamsOfVessel[j]->CountNanbots (); i++)
        {
          Ptr<Nanobot> n = streamsOfVessel[j]->GetNanobot (i);
          cout << n->GetNanobotID () << ":" << n->GetPosition ().x << ":" << n->GetPosition ().y
               << ":" << n->GetPosition ().z << ":" << streamsOfVessel[j]->GetVelocity () << endl;
        }
      cout << "----------------------" << endl;
    }
}

} // namespace ns3