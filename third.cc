
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SecondScriptExample");

/*                                          subnet4
                                        (R) ------- [N]
                                       /
                                     / subnet3
SERVER -> [N] -------- (R) ------- (R)
               subnet1     subnet2   \ subnet6
                                      \    subnet5
                                      (R) ------- [N] -> Client
*/
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

NodeContainer nodes, routers, subnet1, subnet2, subnet3, subnet4, subnet5, subnet6;
nodes.Create(3);
routers.Create(4);

subnet1.Add(nodes.Get(0)); subnet1.Add(routers.Get(0));
subnet2.Add(routers.Get(0)); subnet2.Add(routers.Get(1));
subnet3.Add(routers.Get(1)); subnet3.Add(routers.Get(2));
subnet4.Add(routers.Get(2)); subnet4.Add(nodes.Get(1));
subnet5.Add(routers.Get(3)); subnet5.Add(nodes.Get(2));
subnet6.Add(routers.Get(1)); subnet6.Add(routers.Get(3));

PointToPointHelper p2p1, p2p2, p2p3, p2p4, p2p5, p2p6;
NetDeviceContainer subnet1devices, subnet2devices, subnet3devices, subnet4devices, subnet5devices, subnet6devices;

subnet1devices = p2p1.Install(subnet1);
subnet2devices = p2p2.Install(subnet2);
subnet3devices = p2p3.Install(subnet3);
subnet4devices = p2p4.Install(subnet4);
subnet5devices = p2p5.Install(subnet5);
subnet6devices = p2p6.Install(subnet6);

InternetStackHelper stack;
stack.Install(nodes);
stack.Install(routers);

Ipv4AddressHelper subnet1address, subnet2address, subnet3address, subnet4address, subnet5address, subnet6address;
subnet1address.SetBase ("10.1.1.0","255.255.255.0");
subnet2address.SetBase ("20.1.1.0","255.255.255.0");
subnet3address.SetBase ("30.1.1.0","255.255.255.0");
subnet4address.SetBase ("40.1.1.0","255.255.255.0");
subnet5address.SetBase ("50.1.1.0","255.255.255.0");
subnet6address.SetBase ("60.1.1.0","255.255.255.0");

Ipv4InterfaceContainer subnet1interface = subnet1address.Assign (subnet1devices);
Ipv4InterfaceContainer subnet2interface = subnet2address.Assign (subnet2devices);
Ipv4InterfaceContainer subnet3interface = subnet3address.Assign (subnet3devices);
Ipv4InterfaceContainer subnet4interface = subnet4address.Assign (subnet4devices);
Ipv4InterfaceContainer subnet5interface = subnet5address.Assign (subnet5devices);
Ipv4InterfaceContainer subnet6interface = subnet6address.Assign (subnet6devices);

UdpEchoServerHelper echoServer (9);
  ApplicationContainer serverApp = echoServer.Install( nodes.Get(0) );
    serverApp.Start (Seconds (1.0));
    serverApp.Stop (Seconds (10.0));


UdpEchoClientHelper echoClient (subnet1interface.GetAddress(0), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue(1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApp = echoClient.Install( nodes.Get(2) );
    clientApp.Start(Seconds(2.0));
    clientApp.Stop(Seconds(10.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
Simulator::Run();
Simulator::Destroy();
return 0;
}