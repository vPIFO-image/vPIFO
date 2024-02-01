# ifndef MYSCHEDULING_H
# define MYSCHEDULING_H

# include <cstring>
# include <iostream>
# include <unordered_map>
# include "ns3/ipv4-header.h"


namespace ns3 {
    class Scheduling {
        public:
            virtual long long RankComputation (int x1, int x2) = 0;
            virtual void Dequeue(int x1) = 0;
            virtual void InitializeSize(int flow, long long size);
    };


    class WFQ : public Scheduling {
        public:
            WFQ(int x);
            long long RankComputation (int tenant, int size) override;
            void Dequeue(int size) override;
        private:
            int num;
            long long virtual_time;
            std::unordered_map<int, long long> last_time;
    };


    class pFabric : public Scheduling {
        public:
            pFabric();
            void InitializeSize(int flow, long long size) override;
            long long RankComputation(int flow, int x2) override; 
            void Dequeue(int flow) override;
        private:
            std::unordered_map<int, long long> flow_size;
    };


    class SP : public Scheduling {
        public:
            long long RankComputation(int token, int x2) override;
            void Dequeue(int x1) override;
    };

}


# endif