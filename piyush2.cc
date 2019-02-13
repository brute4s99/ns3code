/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

//  Network Topology
//        0        1               X        0        1                 0        1
//       [N] ---- [N](client)     [N]      [N] ---- [N](server)       [N] ---- [N](client)
//        |   P2P 10.1.1.0         |        |    P2P 10.1.2.0         |   P2P 10.1.3.0
//        =============================================================
//                                LAN 20.1.1.0


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SecondScriptExample");

int 
main (int argc, char *argv[])
{
  bool verbose = true;
  uint32_t nCsma = 1; // extra nodes in CSMA bus

  CommandLine cmd;
  cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);

  cmd.Parse (argc,argv);

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

  nCsma = nCsma == 0 ? 1 : nCsma;

  NodeContainer p2pNodes1, p2pNodes2, p2pNodes3; // container of nodes
  p2pNodes1.Create (2); // will have a client at second node
  p2pNodes2.Create (2); // will have a server at second node
  p2pNodes3.Create (2); // will have a client at second node

  NodeContainer csmaNodes; // container of nodes
//   Adding nodes in container now
  csmaNodes.Add (p2pNodes1.Get (0));
  csmaNodes.Create (nCsma); // create an extra node in CSMA bus
  csmaNodes.Add (p2pNodes2.Get (0));
  csmaNodes.Add (p2pNodes3.Get (0));

  PointToPointHelper pointToPoint; // set up p2p connection details
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices1, p2pDevices2, p2pDevices3; // TO READ
  p2pDevices1 = pointToPoint.Install (p2pNodes1);
  p2pDevices2 = pointToPoint.Install (p2pNodes2);
  p2pDevices3 = pointToPoint.Install (p2pNodes3);

  CsmaHelper csma; // set up CSMA connection details
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices; // TO READ
  csmaDevices = csma.Install (csmaNodes);

  InternetStackHelper stack; // set up Internet Stack ONCE ONLY in every Node
  stack.Install (p2pNodes1.Get (1));
  stack.Install (p2pNodes2.Get (1));
  stack.Install (p2pNodes3.Get (1));
  stack.Install (csmaNodes);

  Ipv4AddressHelper address; // helps in setting up Ipv4InterfaceContainer
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces1, p2pInterfaces2, p2pInterfaces3;
  p2pInterfaces1 = address.Assign (p2pDevices1);
  address.SetBase ("10.1.2.0", "255.255.255.0");
  p2pInterfaces2 = address.Assign (p2pDevices2);
  address.SetBase ("10.1.3.0", "255.255.255.0");
  p2pInterfaces3 = address.Assign (p2pDevices3);

  address.SetBase ("20.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (p2pNodes2.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (p2pInterfaces2.GetAddress (1), 9); // target IP (server)
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps1 = echoClient.Install (p2pNodes1.Get (0));
  ApplicationContainer clientApps2 = echoClient.Install (p2pNodes3.Get (0));
  clientApps1.Start (Seconds (2.0));
  clientApps1.Stop (Seconds (10.0));
  clientApps2.Start (Seconds (2.0));
  clientApps2.Stop (Seconds (10.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  pointToPoint.EnablePcapAll ("second");
  csma.EnablePcap ("second", csmaDevices.Get (1), true);

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
