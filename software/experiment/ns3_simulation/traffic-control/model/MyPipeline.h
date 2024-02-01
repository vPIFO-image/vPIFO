# ifndef MYPIPELINE_H
# define MYPIPELINE_H

# include <cstring>
# include <queue>
# include <random>
# include <unordered_map>

# include "ns3/simulator.h"

using namespace std;

namespace ns3 {
    class Pipeline {
        public:
            void AddPush(int list_id, long long rank, int token);
            int GetToken();
            Pipeline();

        private:
            // BUFFER_SIZE is the size of pop buffer we want to keep
            static const int BUFFER_SIZE = 4;
            // CYCLE is the cycle time of Pipeline in nanoseconds
            static const int CYCLE = 200;
            // START_TIME is time in seconds for the Pipeline to start running
            static const int START_TIME = 2;
            // The index of root
            static const int ROOT = 233;
            // S is the threshold that triggers the anti-starvation mechanism
            static const int S = 240;
            // N is the number of RPUs, Operation Lists and height of B
            static const int N = 5;
            // T is the number of Scheduling nodes, as well as number of trees
            static const int T = 240;
            // END_THRESHOLD is the criterion to end the simulation
            static const int END_THRESHOLD = 1000000;
            static const int inf = 1e9 + 7;

            enum Type {
                Push,
                Pop,
                WriteBack,
                Locked,
                Empty
            };

            struct Operation {
                enum Type type;
                int root, TTL, rank, token;
                Operation () {
                    TTL = 0;
                    type = Empty;
                }
                Operation (int rt, long long rk, int tk) {
                    type = Push;
                    root = rt;
                    TTL = N;
                    rank = rk;
                    token = tk;
                }
                Operation (int rt) {
                    type = Pop;
                    root = rt;
                    TTL = N;
                }
            } RPU[N];

            queue<Operation> operation_list[N];

            priority_queue<pair<long long, int>> tree[T];

            int root_num, wait_time[N], locked[N], beggar;

            // Log information
            int empty_count, hungry_count, hungry_delay, starve_count;

            // Save the result of pop, start means starting a pop request
            int buffer_num, start_num;
            queue<int> buffer;

            std::unordered_map<int, int> list_map;
            int list_cnt;

            int GetListId(int user);

            void AddPop(int list_id);

            void SendOperation(int list_id);

            bool CheckRealQueue(int id);

            void WakeUp();
    };
}

# endif