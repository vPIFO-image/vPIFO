# include "MyScheduling.h"

namespace ns3 {
    void Scheduling::InitializeSize(int flow, long long size) {
        return;
    }

    
    WFQ::WFQ(int x) {
        virtual_time = 0;
        num = x;
        last_time.clear();
    }

    long long WFQ::RankComputation(int tenant, int size) {
        if (!last_time.count(tenant))
            last_time[tenant] = 0;
        last_time[tenant] = 
            std::max(virtual_time, last_time[tenant]) + size * num;
        return last_time[tenant];
    }

    void WFQ::Dequeue(int size) {
        virtual_time += size;
    }
    
    
    pFabric::pFabric() {
        flow_size.clear();
    }

    void pFabric::InitializeSize(int flow, long long size) {
        flow_size[flow] = size;
    }

    long long pFabric::RankComputation(int flow, int x2) {
        return flow_size[flow];
    }

    void pFabric::Dequeue(int flow) {
        flow_size[flow]--;
    }


    long long SP::RankComputation(int token, int x2) {
        if (token < 10)
            return token;
        return token % 10;
    }
    
    void SP::Dequeue(int x1) {
        return;
    }
}

