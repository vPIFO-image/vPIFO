#include "vPIFO.h"

namespace ns3
{
    class Node;
    const int vPIFO::ROOT = 233;
    const int vPIFO::SIZE = ((1 << 10) - 1) / 3;
    
    NS_OBJECT_ENSURE_REGISTERED(vPIFO);
    TypeId vPIFO::GetTypeId(void)
	{
		static TypeId tid = TypeId("ns3::vPIFO")
			.SetParent<QueueDisc>()
			.SetGroupName("TrafficControl")
			.AddConstructor<vPIFO>();
		return tid;
	}

    bool vPIFO::IsHadoop(int x)
    {
        return (x % 10) > (x / 10);
    }

    vPIFO::vPIFO()
    {
        flow_map.clear();
        sch_tree.clear();
        queue_map.clear();
        while(!bypath_queue.empty())
            bypath_queue.pop();
        root_size = 0;
        
        // Build the scheduling tree

        // Root node: root to tenant group
        sch_tree[ROOT] = std::make_shared<SP>();
        
        // Layer 1: tenant group to tenant type
        for (int i = 0; i <= 9; ++i)
            sch_tree[i] = std::make_shared<SP>();

        // Layer 2: tenant type to tenant
        for (int i = 0, j; i <= 9; ++i)
        {
            j = 10 + i * 2;
            sch_tree[j] = std::make_shared<WFQ>(i + 1);
            sch_tree[j + 1] = std::make_shared<WFQ>(10 - i - 1);
        }

        // Layer 3: only for Web Search tenant, tenant to flow
        for (int i = 100; i <= 199; ++i)
        {
            if (IsHadoop(i - 100))
                continue;
            sch_tree[i] = std::make_shared<pFabric>();
        }
    }

    std::string vPIFO::GetFlowLabel(Ptr<QueueDiscItem> item)
    {
        //printf("my log: flow label?\n");
        Ptr<const Ipv4QueueDiscItem> ipItem =
            DynamicCast<const Ipv4QueueDiscItem>(item);

        const Ipv4Header ipHeader = ipItem->GetHeader();
        TcpHeader header;
        GetPointer(item)->GetPacket()->PeekHeader(header);

        std::stringstream ss;
        ss << ipHeader.GetSource().Get();
        ss << header.GetSourcePort();
        //ss << ipHeader.GetDestination().Get();
        //ss << header.GetDestinationPort();
        
        /*cout << "my log: find tenant log" << ipHeader.GetSource().Get() << " "<<ipHeader.GetDestination().Get()
            << " "<< header.GetSourcePort() <<" "<< header.GetDestinationPort() << endl;*/

        std::string flowLabel = ss.str();
        //cout << "my log: flow label " << flowLabel << endl;
        return flowLabel;
    }
    
    void vPIFO::InitializeParams(void) {
        // Read the Web Search traffic to initialize pFabric flow size
        uint32_t id = this->GetNetDevice()->GetNode()->GetId();
        std::ifstream infile;
        infile.open("vPIFOResult/flowlabel.txt");
        int no, size, tenant;
        uint32_t src_ip, dst_ip;
        string flow_label;
        infile >> no;
        while (no--)
        {
            infile >> flow_label >> src_ip >> dst_ip >> size >> tenant;
            // Server node
            if (id < 144) {
                if (src_ip != id && dst_ip != id)
                    continue;
            }
            // Leaf node
            /*else if (id < 144 + 9) {
                if (src_ip / 16 != id && dst_ip / 16 != id)
                    continue;
            }*/
            flow_map[flow_label] = ++queue_cnt;
            queue_map[queue_cnt] = std::queue<Ptr<QueueDiscItem>>();
            sch_tree[tenant + 100]->InitializeSize(queue_cnt, size);
        }
    }

    bool vPIFO::DoEnqueue(Ptr<QueueDiscItem> item)
    {
        TenantTag my_tag;
        Ptr<Packet> packet = GetPointer(item->GetPacket());
        packet->PeekPacketTag(my_tag);
        int tenant = my_tag.GetTenantId();
        
        // For TCP shake hands packets, bypath to output directly
        if (tenant == 10233) {
            bypath_queue.push(item);
            return true;
        }
        
        //printf("my log: DoEnqueue %d\n", tenant);
        
        // The root queue is full
        if (root_size >= SIZE) {
            Drop(item);
            return false;
        }
        root_size++;

        int group = tenant / 10;
        long long rk1 = sch_tree[ROOT]->RankComputation(group, 0);
        pipe.AddPush(ROOT, rk1, group);

        int type_id = 10 + group * 2 + IsHadoop(tenant);
        long long rk2 = sch_tree[group]->RankComputation(type_id, 0);
        pipe.AddPush(group, rk2, type_id);

        int tenant_id = 100 + tenant;
        int packet_size = packet->GetSize();
        long long rk3 = sch_tree[type_id]->RankComputation(
            tenant_id, packet_size);
        pipe.AddPush(type_id, rk3, tenant_id);

        // A Hadoop tenant, all packets can be set in the same real queue
        if (IsHadoop(tenant))
        {
            if (!queue_map.count(tenant_id))
                queue_map[tenant_id] = std::queue<Ptr<QueueDiscItem>>();
            queue_map[tenant_id].push(item);
        }
        // A WebSearch tenant, each flow need a real packet queue
        else
        {
            string flow_label = GetFlowLabel(item);
            int flow_id = flow_map[flow_label];
            long long rk4 = sch_tree[tenant_id]->RankComputation(flow_id, 0);
            //printf("my log: web search pfabric flow rank %d %d\n", tenant, rk4);
            pipe.AddPush(tenant_id, rk4, flow_id);
            queue_map[flow_id].push(item);
        }
        return true;
    }

    Ptr<QueueDiscItem> vPIFO::DoDequeue()
    {
        if (!bypath_queue.empty()) {
            Ptr<QueueDiscItem> item = bypath_queue.front();
            bypath_queue.pop();
            return item;
        }
        
        int ans = pipe.GetToken();
        // No packet in buffer now
        if (ans == -1) {
            //empty_queue++;
            // printf("? empty ans %d\n", empty_queue);
            return nullptr;
        }
        root_size--;
        Ptr<QueueDiscItem> item = queue_map[ans].front();
        queue_map[ans].pop();

        //printf("my log: has a real packet %d!", ans);
        // cout << Simulator::Now() << endl;
        TenantTag my_tag;
        Ptr<Packet> packet = GetPointer(item->GetPacket());
        int packet_size = packet->GetSize();
        packet->PeekPacketTag(my_tag);
        int tenant = my_tag.GetTenantId();

        int type_id = 10 + (tenant / 10) * 2 + IsHadoop(tenant);
        sch_tree[type_id]->Dequeue(packet_size);
        if (!IsHadoop(tenant))
            sch_tree[tenant + 100]->Dequeue(ans);
        
        return item;
    }
    
    Ptr<const QueueDiscItem> vPIFO::DoPeek(void) const {
        return 0;
    }
    
    bool vPIFO::CheckConfig(void) {
        return 1;
    }

}
