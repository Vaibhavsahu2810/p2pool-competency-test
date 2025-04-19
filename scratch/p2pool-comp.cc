#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("DecentralizedMeshSim");

// Handle received UDP packets and print event info
static void OnPacketReceive(Ptr<Socket> sock)
{
  Address sender;
  Ptr<Packet> receivedPacket;
  while ((receivedPacket = sock->RecvFrom(sender)))
    {
      uint32_t size = receivedPacket->GetSize();
      std::vector<uint8_t> buffer(size);
      receivedPacket->CopyData(buffer.data(), size);
      std::string data(reinterpret_cast<char*>(buffer.data()), size);

      std::cout << Simulator::Now().GetSeconds() << "s - Node["
                << sock->GetNode()->GetId() << "] received: \"" << data
                << "\" from " << InetSocketAddress::ConvertFrom(sender).GetIpv4()
                << std::endl;
    }
}

// Send a single "share" UDP message
static void TransmitDataOnce(Ptr<Socket> sock, Ipv4Address target, uint16_t portNum, std::string content)
{
  Ptr<Packet> payload = Create<Packet>((uint8_t*) content.c_str(), content.length());
  sock->SendTo(payload, 0, InetSocketAddress(target, portNum));

  std::cout << Simulator::Now().GetSeconds() << "s - Node["
            << sock->GetNode()->GetId() << "] sent: \"" << content
            << "\" to " << target << std::endl;
}

// Schedule repeated transmissions of UDP messages
static void PeriodicSender(Ptr<Socket> sock, Ipv4Address target, uint16_t portNum, std::string content, Time interval)
{
  TransmitDataOnce(sock, target, portNum, content);
  Simulator::Schedule(interval, &PeriodicSender, sock, target, portNum, content, interval);
}

int main(int argc, char *argv[])
{
  uint32_t totalNodes = 10;
  uint16_t listenPort = 9999;
  Time sendInterval = Seconds(2.0);

  CommandLine cmd;
  cmd.AddValue("totalNodes", "Number of nodes in the overlay", totalNodes);
  cmd.Parse(argc, argv);

  NodeContainer meshNodes;
  meshNodes.Create(totalNodes);

  InternetStackHelper internetStack;
  internetStack.Install(meshNodes);

  PointToPointHelper p2p;
  p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
  p2p.SetChannelAttribute("Delay", StringValue("2ms"));

  Ipv4AddressHelper addrAllocator;
  uint32_t netId = 1;
  std::vector<Ipv4InterfaceContainer> allInterfaces;

  for (uint32_t src = 0; src < totalNodes; ++src)
    for (uint32_t dst = src + 1; dst < totalNodes; ++dst)
      {
        NetDeviceContainer links = p2p.Install(NodeContainer(meshNodes.Get(src), meshNodes.Get(dst)));
        std::ostringstream subnet;
        subnet << "10." << netId++ << ".0.0";
        addrAllocator.SetBase(subnet.str().c_str(), "255.255.255.0");
        Ipv4InterfaceContainer iface = addrAllocator.Assign(links);
        allInterfaces.push_back(iface);
      }

  p2p.EnablePcapAll("mesh-network");

  AnimationInterface anim("mesh-sim.xml");
  anim.SetMaxPktsPerTraceFile(100000);

  std::vector<Ipv4Address> nodeAddresses(totalNodes);
  for (uint32_t i = 0; i < totalNodes; ++i)
    nodeAddresses[i] = meshNodes.Get(i)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();

  for (uint32_t i = 0; i < totalNodes; ++i)
    {
      Ptr<Socket> receiver = Socket::CreateSocket(meshNodes.Get(i), UdpSocketFactory::GetTypeId());
      receiver->Bind(InetSocketAddress(Ipv4Address::GetAny(), listenPort));
      receiver->SetRecvCallback(MakeCallback(&OnPacketReceive));
    }

  for (uint32_t i = 0; i < totalNodes; ++i)
    {
      Ptr<Socket> sender = Socket::CreateSocket(meshNodes.Get(i), UdpSocketFactory::GetTypeId());
      std::string msg = "node_" + std::to_string(i) + "_share";

      for (uint32_t j = 0; j < totalNodes; ++j)
        {
          if (i == j) continue;
          Time stagger = MilliSeconds(100 * (j + 1));
          Simulator::Schedule(Seconds(1.0) + stagger,
                              &PeriodicSender,
                              sender, nodeAddresses[j], listenPort, msg, sendInterval);
        }
    }

  Simulator::Stop(Seconds(15.0));
  Simulator::Run();
  Simulator::Destroy();
  return 0;
}
