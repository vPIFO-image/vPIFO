#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

using namespace ns3;

/*
0----        ----3
1---|--6--7--|---4
2----        ----5
*/

double simulation_time = 10.0;

void EnablePcapAll (PointToPointHelper p2phelper, std::string pcapFileName) {
    p2phelper.EnablePcapAll (pcapFileName, true);
}

int main (int argc, char *argv[])
{
  NodeContainer nodes;
  nodes.Create(8);

  PointToPointHelper p2pRouter;
  p2pRouter.SetDeviceAttribute ("DataRate", StringValue ("100Gbps"));
  p2pRouter.SetChannelAttribute ("Delay", StringValue ("1us"));

  PointToPointHelper p2pLeaf;
  p2pLeaf.SetDeviceAttribute ("DataRate", StringValue ("400Gbps"));
  p2pLeaf.SetChannelAttribute ("Delay", StringValue ("1us"));

  NetDeviceContainer devices1 = p2pLeaf.Install (nodes.Get(0), nodes.Get(6));
  NetDeviceContainer devices2 = p2pLeaf.Install (nodes.Get(1), nodes.Get(6));
  NetDeviceContainer devices3 = p2pLeaf.Install (nodes.Get(2), nodes.Get(6));

  NetDeviceContainer devices4 = p2pLeaf.Install (nodes.Get(7), nodes.Get(3));
  NetDeviceContainer devices5 = p2pLeaf.Install (nodes.Get(7), nodes.Get(4));
  NetDeviceContainer devices6 = p2pLeaf.Install (nodes.Get(7), nodes.Get(5));

  NetDeviceContainer devices7 = p2pRouter.Install (nodes.Get(6), nodes.Get(7));

  InternetStackHelper stack;
  stack.Install(nodes);

  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer iface1 = ipv4.Assign (devices1);
  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer iface2 = ipv4.Assign (devices2);
  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer iface3 = ipv4.Assign (devices3);
  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer iface4 = ipv4.Assign (devices4);
  ipv4.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer iface5 = ipv4.Assign (devices5);
  ipv4.SetBase ("10.1.6.0", "255.255.255.0");
  Ipv4InterfaceContainer iface6 = ipv4.Assign (devices6);
  ipv4.SetBase ("10.1.7.0", "255.255.255.0");
  Ipv4InterfaceContainer iface7 = ipv4.Assign (devices7);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  uint16_t port = 8080;

  BulkSendHelper source1("ns3::TcpSocketFactory", InetSocketAddress(iface4.GetAddress(1), port));
  ApplicationContainer sourceApps1 = source1.Install(nodes.Get(0));
  sourceApps1.Start(Seconds(1.0));
  sourceApps1.Stop(Seconds(simulation_time));

  PacketSinkHelper sink1("ns3::TcpSocketFactory", InetSocketAddress(iface4.GetAddress(1), port));
  ApplicationContainer sinkApps1 = sink1.Install(nodes.Get(3));
  sinkApps1.Start(Seconds(0.0));
  sinkApps1.Stop(Seconds(simulation_time));

  BulkSendHelper source12("ns3::TcpSocketFactory", InetSocketAddress(iface4.GetAddress(1), port));
  ApplicationContainer sourceApps12 = source12.Install(nodes.Get(0));
  sourceApps12.Start(Seconds(1.0));
  sourceApps12.Stop(Seconds(simulation_time));

  PacketSinkHelper sink12("ns3::TcpSocketFactory", InetSocketAddress(iface4.GetAddress(1), port));
  ApplicationContainer sinkApps12 = sink12.Install(nodes.Get(3));
  sinkApps12.Start(Seconds(0.0));
  sinkApps12.Stop(Seconds(simulation_time));

  BulkSendHelper source13("ns3::TcpSocketFactory", InetSocketAddress(iface4.GetAddress(1), port));
  ApplicationContainer sourceApps13 = source13.Install(nodes.Get(0));
  sourceApps13.Start(Seconds(1.0));
  sourceApps13.Stop(Seconds(simulation_time));

  PacketSinkHelper sink13("ns3::TcpSocketFactory", InetSocketAddress(iface4.GetAddress(1), port));
  ApplicationContainer sinkApps13 = sink13.Install(nodes.Get(3));
  sinkApps13.Start(Seconds(0.0));
  sinkApps13.Stop(Seconds(simulation_time));


  BulkSendHelper source2("ns3::TcpSocketFactory", InetSocketAddress(iface5.GetAddress(1), port));
  ApplicationContainer sourceApps2 = source2.Install(nodes.Get(1));
  sourceApps2.Start(Seconds(1.0));
  sourceApps2.Stop(Seconds(simulation_time));

  PacketSinkHelper sink2("ns3::TcpSocketFactory", InetSocketAddress(iface5.GetAddress(1), port));
  ApplicationContainer sinkApps2 = sink2.Install(nodes.Get(4));
  sinkApps2.Start(Seconds(0.0));
  sinkApps2.Stop(Seconds(simulation_time));

  BulkSendHelper source22("ns3::TcpSocketFactory", InetSocketAddress(iface5.GetAddress(1), port));
  ApplicationContainer sourceApps22 = source22.Install(nodes.Get(1));
  sourceApps22.Start(Seconds(1.0));
  sourceApps22.Stop(Seconds(simulation_time));

  PacketSinkHelper sink22("ns3::TcpSocketFactory", InetSocketAddress(iface5.GetAddress(1), port));
  ApplicationContainer sinkApps22 = sink22.Install(nodes.Get(4));
  sinkApps22.Start(Seconds(0.0));
  sinkApps22.Stop(Seconds(simulation_time));

  BulkSendHelper source23("ns3::TcpSocketFactory", InetSocketAddress(iface5.GetAddress(1), port));
  ApplicationContainer sourceApps23 = source23.Install(nodes.Get(1));
  sourceApps23.Start(Seconds(1.0));
  sourceApps23.Stop(Seconds(simulation_time));

  PacketSinkHelper sink23("ns3::TcpSocketFactory", InetSocketAddress(iface5.GetAddress(1), port));
  ApplicationContainer sinkApps23 = sink23.Install(nodes.Get(4));
  sinkApps23.Start(Seconds(0.0));
  sinkApps23.Stop(Seconds(simulation_time));


  BulkSendHelper source3("ns3::TcpSocketFactory", InetSocketAddress(iface6.GetAddress(1), port));
  ApplicationContainer sourceApps3 = source3.Install(nodes.Get(2));
  sourceApps3.Start(Seconds(1.0));
  sourceApps3.Stop(Seconds(simulation_time));

  PacketSinkHelper sink3("ns3::TcpSocketFactory", InetSocketAddress(iface6.GetAddress(1), port));
  ApplicationContainer sinkApps3 = sink3.Install(nodes.Get(5));
  sinkApps3.Start(Seconds(0.0));
  sinkApps3.Stop(Seconds(simulation_time));

  BulkSendHelper source32("ns3::TcpSocketFactory", InetSocketAddress(iface6.GetAddress(1), port));
  ApplicationContainer sourceApps32 = source32.Install(nodes.Get(2));
  sourceApps32.Start(Seconds(1.0));
  sourceApps32.Stop(Seconds(simulation_time));

  PacketSinkHelper sink32("ns3::TcpSocketFactory", InetSocketAddress(iface6.GetAddress(1), port));
  ApplicationContainer sinkApps32 = sink32.Install(nodes.Get(5));
  sinkApps32.Start(Seconds(0.0));
  sinkApps32.Stop(Seconds(simulation_time));

  BulkSendHelper source33("ns3::TcpSocketFactory", InetSocketAddress(iface6.GetAddress(1), port));
  ApplicationContainer sourceApps33 = source33.Install(nodes.Get(2));
  sourceApps33.Start(Seconds(1.0));
  sourceApps33.Stop(Seconds(simulation_time));

  PacketSinkHelper sink33("ns3::TcpSocketFactory", InetSocketAddress(iface6.GetAddress(1), port));
  ApplicationContainer sinkApps33 = sink33.Install(nodes.Get(5));
  sinkApps33.Start(Seconds(0.0));
  sinkApps33.Stop(Seconds(simulation_time));


  Simulator::Schedule (MilliSeconds (1), &EnablePcapAll, p2pRouter, "PcapTrace");

  Simulator::Stop(Seconds(simulation_time));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
