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
 * Author: Regine Geyer <geyer@itm.uni-luebeck.de>
 */

#include "ns3/Bloodcircuit.h"

using namespace ns3;
using namespace std;

#define ITERATIONS 50

int
main (int argc, char *argv[])
{
  //Add default values:
  int numOfNanobots = 100;
  int simulationDuration = 500;
  int injectionVessel = 29; // 29 corresponds to left arm
  bool typeofsimulation = 0; // 0 is for single run, 1 is for sweep over tissue thickness
  int gatewayposition = 1;
  int tissue_ID_init = 94;
  float vesselthickness = 0.00016; // for single run, tissue thickness in [m]
  vector<int> gatewayPositions;
  vector<int> tissue_ID;


 

  // check input parameters
  CommandLine cmd;
  cmd.AddValue ("simulationDuration", "simulationDuration", simulationDuration);
  cmd.AddValue ("numOfNanobots", "numOfNanobots", numOfNanobots);
  cmd.AddValue ("injectionVessel", "injectionVessel", injectionVessel);
  cmd.AddValue ("typeofsimulation","typeofsimulation", typeofsimulation);
  cmd.AddValue ("gatewayposition", "gatewayposition", gatewayposition);
  cmd.AddValue ("tissue_ID", "tissue_ID", tissue_ID_init);

  cmd.Parse (argc, argv);

  // add position of gateway and tissue_ID
  gatewayPositions.push_back(gatewayposition);
  tissue_ID.push_back(tissue_ID_init);

  // check wether type of simulation is sweep or singel run
   // clear csv files for writing 

  remove("gateway.csv");
  remove("gateway_amplitudes.csv");
  if (typeofsimulation)
    remove("gateway_ber.csv");

 


  if (typeofsimulation)   // sweep
  {
    vector<float> vesselStep(ITERATIONS,0);

    float startval = 1*pow(10,-6);
    
    vesselStep[0] = startval;
    vesselStep[ITERATIONS-1] = 400*pow(10,-6);

    float a = (vesselStep[ITERATIONS-1] - vesselStep[0]) / ITERATIONS;

    //create equally spaced vector of tissue thickness
    generate(vesselStep.begin(),vesselStep.end(), [n = 0, &a] () mutable { return a * n++; }); //create equally spaced vector of tissue thickness

    for (int i = 0; i < ITERATIONS; i++){
      vesselStep[i] = vesselStep[i] + startval;
    }

  // Call each simulation ITERATION times
    for (int i = 0; i < ITERATIONS; i++){
      Bloodcircuit::BeginnSimulation (simulationDuration, numOfNanobots, injectionVessel, gatewayPositions, tissue_ID, vesselStep[i]);
    }

    return 0;
  }

  // single run
  return Bloodcircuit::BeginnSimulation (simulationDuration, numOfNanobots, injectionVessel, gatewayPositions, tissue_ID, vesselthickness);
}