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
 *
 * ==== Full network simulation experiment for SimplePIFO
 * ==== 4 spine 9 leaf 144 server  leaf-spine topo
 *
 */

# include <algorithm>
# include <cstdio>
# include <fstream>
# include <iostream>
# include <map>
# include <unordered_map>

# include "ns3/applications-module.h"
# include "ns3/core-module.h"
# include "ns3/flow-monitor-module.h"
# include "ns3/internet-module.h"
# include "ns3/ipv4-header.h"
# include "ns3/netanim-module.h"
# include "ns3/network-module.h"
# include "ns3/point-to-point-module.h"
# include "ns3/random-variable-stream.h"
# include "ns3/stats-module.h"
# include "ns3/traffic-control-module.h"
# include "ns3/udp-header.h"
# include "ns3/packet.h"
# include "ns3/tag.h"
# include "ns3/Tenant-tag.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE("MyTopoTCP");

const int LEAF_CNT = 9;                              // number of leaf nodes
const int SPINE_CNT = 4;                             // number of spine nodes
const int SERVER_CNT = 144;                          // number of server nodes
const int LINK_CNT = LEAF_CNT * SPINE_CNT + SERVER_CNT; // number of links
const int FLOW_NUM = 5000;                           // number of WebSearch flow
const int TENANT_NUM = 100;                // number of tenant
uint32_t totalPktSize = 0;                 // count the total pkt # of all flows
uint32_t total_send = 0;
const char *ACCESS_RATE = "10Gbps";   // Access Link Bandwith 10G
const char *BACKBONE_RATE = "40Gbps"; // Leaf-spine Link Bandwith 10G
const char *DATARATE = "10Gbps";  // Bandwith 10G
const char *DELAY =
    "3us";         // Delay 3us (0.000001s -> 0.0001s)  (3ms = 0.001s -> 0.1s)
double simulator_stop_time = 4; // 0.0301; //0.0086404
const int PKTSIZE = 1448;        // pkt size payload

std::unordered_map<string, int> flow_index;
std::unordered_map<uint32_t, uint16_t> flow_port;
uint32_t websearch_count = 0;
struct FlowInfo {
  double time;
  int tenant, size;
} flow_info[FLOW_NUM];

int FlowComp(FlowInfo x, FlowInfo y) {
  if (x.tenant == y.tenant)
    return x.time < y.time;
  return x.tenant < y.tenant;
}

// ip address pairs on the same links
std::vector<Ipv4InterfaceContainer> interfaces(LINK_CNT);
// Create nodes: leaf + spine + server = nodes
NodeContainer leafNodes, spineNodes, serverNodes, nodes;


class MyApp : public Application
{
public:
  MyApp();
  virtual ~MyApp();
  /**
   * Register this type.
   * \return The TypeId.
   */
  static TypeId GetTypeId(void);
  void Setup(Ptr<Socket> socket, uint32_t sip, Ipv4Address saddr, 
      Address address, uint32_t packetSize,
      uint32_t nPackets, DataRate dataRate, uint32_t flowId, uint32_t tenant);

private:
  virtual void StartApplication(void);
  virtual void StopApplication(void);
  void ScheduleTx(void);
  void SendPacket(void);
  Ptr<Socket> m_socket;
  Address m_peer;
  uint32_t m_packetSize;
  uint32_t m_nPackets;
  DataRate m_dataRate;
  EventId m_sendEvent;
  bool m_running;
  uint32_t m_packetsSent;
  uint32_t m_flowId;
  uint32_t m_tenant;
};

MyApp::MyApp()
    : m_socket(0),
      m_peer(),
      m_packetSize(0),
      m_nPackets(0),
      m_dataRate(0),
      m_sendEvent(),
      m_running(false),
      m_packetsSent(0),
      m_tenant(0) {}

MyApp::~MyApp() { m_socket = 0; }

/* static */
TypeId MyApp::GetTypeId(void)
{
  static TypeId tid = TypeId("MyApp")
                          .SetParent<Application>()
                          .SetGroupName("Tutorial")
                          .AddConstructor<MyApp>();
  return tid;
}

void

MyApp::Setup(Ptr<Socket> socket, uint32_t sip, Ipv4Address saddr,
    Address address, uint32_t packetSize, 
    uint32_t nPackets, DataRate dataRate, uint32_t flowId, uint32_t tenant)
{
  m_socket = socket;
  if (flow_port.count(sip) == 0)
    flow_port[sip] = 49152;
  flow_port[sip]++;
  while (m_socket->Bind(InetSocketAddress(saddr, flow_port[sip])))
    flow_port[sip]++;
  /*Address localAddress;
  m_socket->GetSockName(localAddress);
  InetSocketAddress inetSocketAddress = InetSocketAddress::ConvertFrom(localAddress);;

  uint32_t sourceIp = inetSocketAddress.GetIpv4().Get();
  uint16_t sourcePort = inetSocketAddress.GetPort();*/

  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
  m_flowId = flowId;
  m_tenant = tenant;
}

void MyApp::StartApplication(void)
{
  m_running = true;
  m_packetsSent = 0;
  m_socket->Connect(m_peer);
  SendPacket();
}

void MyApp::StopApplication(void)
{
  m_running = false;
  if (m_sendEvent.IsRunning())
  {
    Simulator::Cancel(m_sendEvent);
  }
  if (m_socket)
  {
    m_socket->Close();
  }
}

void MyApp::SendPacket(void)
{
  Ptr<Packet> packet = Create<Packet>(m_packetSize);
  TenantTag tag;
  tag.SetTenantId(m_tenant);
  packet->AddPacketTag(tag);
  m_socket->Send(packet);    
  total_send += 1;
  if (++m_packetsSent < m_nPackets)
  {
    ScheduleTx();
  }
}

void MyApp::ScheduleTx(void)
{
  if (m_running)
  {
    Time tNext(Seconds(m_packetSize * 8 /
                       static_cast<double>(m_dataRate.GetBitRate())));
    m_sendEvent = Simulator::Schedule(tNext, &MyApp::SendPacket, this);
  }
}

ofstream Output;

class TraceFlow
{
private:
  ifstream flow;
  uint32_t flow_num;
  struct FlowInput
  {
    uint32_t src, dst, packet_count;
    uint16_t dport;
    double start_time;
    uint32_t idx;
    uint32_t tenant;
  };

  FlowInput flow_input = {0};

public:
  enum type
  {
    WebSearch,
    Hadoop
  } flow_type;
  
  TraceFlow(string file_name, type flow_t)
  {
    flow.open(file_name);
    flow >> flow_num;
    cout << "flow_num:" << flow_num << endl;
    if (flow_t == WebSearch)
      websearch_count = flow_num;
    flow_input.idx = 0;
    flow_type = flow_t;
  }

  void ReadFlowInput()
  {
    flow >> flow_input.src >> flow_input.dst >>
        flow_input.dport >> flow_input.packet_count >> flow_input.start_time;
    int tenant = rand() % 100;
    if (flow_type == Hadoop) {
      while (tenant % 10 <= tenant / 10)
        tenant = rand() % 100;
      flow_input.tenant = tenant;
    }
    else {
      while (tenant % 10 > tenant / 10)
        tenant = rand() % 100;
      flow_input.tenant = tenant;
      flow_info[flow_input.idx].tenant = flow_input.tenant;
      flow_info[flow_input.idx].size = flow_input.packet_count;
    }
    totalPktSize += flow_input.packet_count;
  }

  void ScheduleFlowInput()
  {
    // Server
    Address sinkAddress(InetSocketAddress(
        interfaces[flow_input.dst].GetAddress(1), flow_input.dport));
    PacketSinkHelper sinkHelper("ns3::TcpSocketFactory", 
      InetSocketAddress(Ipv4Address::GetAny(), flow_input.dport));
    ApplicationContainer sinkApp =
        sinkHelper.Install(serverNodes.Get(flow_input.dst));
    sinkApp.Start(Seconds(0));
    sinkApp.Stop(Seconds(simulator_stop_time));

    // Client
    TypeId tid = TypeId::LookupByName("ns3::TcpNewReno");
    Config::Set("/NodeList/*/$ns3::TcpL4Protocol/SocketType",
                TypeIdValue(tid));
    Ptr<Socket> ns3TcpSocket = Socket::CreateSocket(
        serverNodes.Get(flow_input.src), TcpSocketFactory::GetTypeId());
    ns3TcpSocket->SetAttribute(
        "SndBufSize", ns3::UintegerValue(1438000000));
    Ipv4Address saddr = interfaces[flow_input.src].GetAddress(1);
    uint32_t sip = interfaces[flow_input.src].GetAddress(1).Get();
    Ptr<MyApp> app = CreateObject<MyApp>();
    app->Setup(
        ns3TcpSocket, sip, saddr, sinkAddress, PKTSIZE, flow_input.packet_count,
        DataRate(DATARATE), flow_input.idx, flow_input.tenant);
    serverNodes.Get(flow_input.src)->AddApplication(app);

    if (flow_type == WebSearch) {
      stringstream ss;
      ss << sip << flow_port[sip];
      string flowlabel = ss.str();
      Output << flowlabel.c_str() << " ";
      Output << flow_input.src << " " << flow_input.dst << " ";
      Output << flow_input.packet_count << " " << flow_input.tenant << endl;
      
      flow_index[flowlabel] = flow_input.idx;
    }
    
    app->SetStartTime(Seconds(flow_input.start_time) - Simulator::Now());
  }

  void Input()
  {
    if (flow_type == WebSearch) {
      string path = "SimpleResult/flowlabel.txt";
      Output.open(path);
      Output << flow_num << endl;
    }
    while (flow_input.idx < flow_num) {
      ReadFlowInput();
      ScheduleFlowInput();
      flow_input.idx++;
    }
    flow.close();
    if (flow_type == WebSearch)
      Output.close();
  }

} web_search("scratch/traffic_ws.txt", TraceFlow::WebSearch), 
  hadoop("scratch/traffic_hd.txt", TraceFlow::Hadoop);

int main(int argc, char *argv[])
{
  // LogComponentEnable ("TcpSocketBase", LOG_LEVEL_INFO);
  // LogComponentEnable ("TcpCongestionOps", LOG_LEVEL_INFO);
  // LogComponentEnable ("PointToPointNetDevice", LOG_LEVEL_INFO);

  // time out settings
  Config::SetDefault("ns3::TcpSocket::DelAckTimeout", TimeValue(Seconds(0.0)));
  // duplicate ack time out = 3RTT = 36us  //100us
  Config::SetDefault("ns3::RttEstimator::InitialEstimation",
                     TimeValue(Seconds(0.000012))); // 12us
  Config::SetDefault("ns3::TcpSocket::ConnTimeout",
                     TimeValue(Seconds(0.000060))); // syn timeout 3rtt
  Config::SetDefault("ns3::TcpSocketBase::MinRto",
                     TimeValue(Seconds(0.000060))); // 3RTT=36us
  Config::SetDefault("ns3::TcpSocketBase::ClockGranularity",
                     TimeValue(MilliSeconds(0.012))); // RTO=3RTT=36us
  // Config::SetDefault ("ns3::TcpSocketBase::ConnCount", TimeValue(MilliSeconds
  // (0.012)));//ConnCount

  // MSS
  Config::SetDefault("ns3::TcpSocket::DataRetries", UintegerValue(100));
  Config::SetDefault("ns3::TcpSocket::ConnCount", UintegerValue(100));
  Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(1448));
  // SegmentSize

  clock_t begint, endt;
  begint = clock();
  CommandLine cmd;
  cmd.Parse(argc, argv);
  Time::SetResolution(Time::NS);

  // Create nodes
  serverNodes.Create(SERVER_CNT); // 0, 1, 2 ... 143
  leafNodes.Create(LEAF_CNT);     // 144, 145, 146 ... 152
  spineNodes.Create(SPINE_CNT);   // 153, 154, 155, 156
  
  nodes = NodeContainer(leafNodes, spineNodes, serverNodes);

  // Configure channels
  PointToPointHelper backboneHelper;
  backboneHelper.SetDeviceAttribute("DataRate", StringValue(BACKBONE_RATE));
  backboneHelper.SetChannelAttribute("Delay", StringValue(DELAY));
  backboneHelper.SetQueue("ns3::DropTailQueue",
                              "MaxPackets", UintegerValue(100));
  PointToPointHelper accessHelper;
  accessHelper.SetDeviceAttribute("DataRate", StringValue(ACCESS_RATE));
  accessHelper.SetChannelAttribute("Delay", StringValue(DELAY));
  accessHelper.SetQueue("ns3::DropTailQueue",
                            "MaxPackets", UintegerValue(100));

  // Install configurations to devices (NIC)
  std::vector<NetDeviceContainer> devices(LINK_CNT);
  int device_cnt = 0;
  for (int i = 0, j; i < SERVER_CNT; ++i)
  {
    j = i / (SERVER_CNT / LEAF_CNT);
    devices[device_cnt] = 
        accessHelper.Install(leafNodes.Get(j), serverNodes.Get(i));
    device_cnt++;
  }
  for (int i = 0; i < LEAF_CNT; ++i)
    for (int j = 0; j < SPINE_CNT; ++j)
    {
      devices[device_cnt] = 
          backboneHelper.Install(leafNodes.Get(i), spineNodes.Get(j));
      device_cnt++;
    }

  // Install network protocals
  InternetStackHelper stack;
  stack.Install(nodes); // install tcp, udp, ip...

  // Assign ip address
  Ipv4AddressHelper address;
  for (uint32_t i = 0; i < LINK_CNT; i++)
  {
    std::ostringstream subset;
    subset << "10.1." << i + 1 << ".0";
    // n0: 10.1.1.1    n1 NIC1: 10.1.1.2   n1 NIC2: 10.1.2.1   n2 NIC1: 10.1.2.2
    address.SetBase(subset.str().c_str(), "255.255.255.0"); // sebnet & mask
    interfaces[i] = address.Assign(
        devices[i]); // set ip addresses to NICs: 10.1.1.1, 10.1.1.2 ...
  }
  
  // unintall traffic control
  TrafficControlHelper tch;
  for (int i = 0; i < LINK_CNT; i++)
  {
    tch.Uninstall(devices[i]);
  }
  tch.SetRootQueueDisc("ns3::SimplePIFO");
  for (int i = 0; i < LINK_CNT; ++i)
  {
    tch.Install(devices[i]);
  }

  // Initialize routing database and set up the routing tables in the nodes.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  // Input the trace input and schedule send packet
  web_search.Input();
  hadoop.Input();

  // monitor throughput
  FlowMonitorHelper fmhelper;
  Ptr<FlowMonitor> monitor = fmhelper.Install(nodes);

  std::cout << "Running Simulation.\n";
  fflush(stdout);
  NS_LOG_INFO("Run Simulation.");
  Simulator::Stop(Seconds(simulator_stop_time));
  Simulator::Run();
  
  Ptr<Ipv4FlowClassifier> classifier = 
    DynamicCast<Ipv4FlowClassifier> (fmhelper.GetClassifier ());
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();

  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = 
    stats.begin (); i != stats.end (); ++i) {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
      stringstream ss;
      ss << t.sourceAddress.Get() << t.sourcePort;
      string flowlabel = ss.str();
      // skip the hadoop flow
      if (flow_index.count(flowlabel) == 0)
        continue;
      int idx = flow_index[flowlabel];
      double start_time = i->second.timeFirstRxPacket.GetSeconds();
      double end_time = i->second.timeLastRxPacket.GetSeconds();
      cout<<flowlabel<<" "<<idx<<" "<<start_time<<" "<<end_time<<endl;
      flow_info[idx].time = end_time - start_time;
  }
  
  sort(flow_info, flow_info + websearch_count, FlowComp);
  
  ofstream ss_fct("SimpleResult/AllFct.txt", std::ios::out | std::ios::app);
  for (uint32_t i = 0; i < websearch_count; ++i) {
    ss_fct << flow_info[i].time << " " << flow_info[i].size << " " <<
        flow_info[i].size * 8 * PKTSIZE / (1024 * 1024 * flow_info[i].time) << 
        " " << flow_info[i].tenant << endl; 
  }
  
  ofstream ss_95("SimpleResult/95Fct.txt", std::ios::out | std::ios::app);
  for (uint32_t i = 0, j, k; i < websearch_count;) {
    j = i;
    while (flow_info[j].tenant == flow_info[i].tenant)
      j++;
    if (j == i || j == i + 1)  k = i;
    else
      k = 0.95 * (j - i) + i - 1;
    ss_95 << flow_info[k].tenant <<" "<< k <<" "<< flow_info[k].time << endl; 
    i = j;
  }

  Simulator::Destroy();
  NS_LOG_INFO("Done.");

  endt = clock();
  cout << (double)(endt - begint) / CLOCKS_PER_SEC << "\n";
  cout << "totalPktSize:" << totalPktSize << std::endl;
  cout << "total_send:" << total_send << endl;
  
  return 0;
}
