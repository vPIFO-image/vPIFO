# include "MyPipeline.h"

namespace ns3 {
    Pipeline::Pipeline() {
        for (int i = 0; i < N; ++i)
            while (!operation_list[i].empty())
                operation_list[i].pop();
        
        for (int i = 0; i < T; ++i)
            while (!tree[i].empty())
                tree[i].pop();
        
        // When the queue is empty, don't count wait_time
        memset(wait_time, -0x3f, sizeof(wait_time));
        memset(locked, 0, sizeof(locked));

        // Initially nobody is hungry
        beggar = -1;
        root_num = 0;
        
        empty_count = hungry_count = hungry_delay = starve_count = 0;
        
        buffer_num = start_num = 0;
        while(!buffer.empty())
            buffer.pop();
        
        list_map.clear();
        list_cnt = 0;

        Simulator::Schedule(Seconds(START_TIME), &Pipeline::WakeUp, this);
    }

    int Pipeline::GetListId (int user) {
        if (list_map.count(user))
            return list_map[user];
        list_map[user] = list_cnt;
        list_cnt = (list_cnt + 1) % N;
        return list_map[user];
    }

    void Pipeline::AddPush (int user, long long rank, int token) {
        if (user == ROOT) {
            root_num++;
        }
        int list_id = GetListId(user);
        if (operation_list[list_id].empty())
            wait_time[list_id] = 0;
        // printf("my log: add push user %d, queue %d\n", user, list_id);
        operation_list[list_id].push((Operation){user, rank, token});
    }

    void Pipeline::AddPop (int user) {
        if (user == ROOT) {
            root_num--;
            start_num++;
        }
        int list_id = GetListId(user);
        // printf("my log: add pop user %d, queue %d %d\n", user, list_id, root_num);
        if (operation_list[list_id].empty())
            wait_time[list_id] = 0;
        operation_list[list_id].push((Operation){user});
    }

    void Pipeline::SendOperation (int list_id) {
        operation_list[list_id].pop();
        if (wait_time[list_id] > S) {
            hungry_count++;
            hungry_delay += wait_time[list_id] - S;
            // Release lock
            if (beggar == list_id) {
                beggar = -1;
                memset(locked, 0, sizeof(locked));
            }
            if (operation_list[list_id].empty())
                wait_time[list_id] = -inf;
            else
                wait_time[list_id] = 0;
        }
    }

    int Pipeline::GetToken () {
        if (buffer_num == 0)
            return -1;
        buffer_num--, start_num--;
        int ret = buffer.front();
        buffer.pop();
        return ret;
    }

    bool Pipeline::CheckRealQueue(int id) {
        // Flow token must be real queue
        if (id >= 1000)
            return 1;
        // Group or type token can't be real queue
        if (id < 100)
            return 0;
        id -= 100;
        int group = id / 10;
        int index = id % 10;
        // Only hadoop tenant is the real queue
        return index > group;
    }

    void Pipeline::WakeUp () {
        // cout << "my log: hello wake up" << Simulator::Now() << " " << root_num << endl;
        // Insert pop root task if the pop buffer is not enough
        if (root_num > 0 && start_num < BUFFER_SIZE)
            AddPop(ROOT);
        
        int use_num = 0;
        
        // statistics before updating status
        for (int i = 0; i < N; ++i) {
            // The RPU is running
            if (RPU[i].type == Push || RPU[i].type == Pop || 
                RPU[i].type == WriteBack)
                use_num++;
            
            if (RPU[i].TTL == N && RPU[i].type == Pop) {
                //printf("my log: who to fault? %d\n", RPU[i].root);
                if (tree[RPU[i].root].empty()) {
                    printf("!!!!!!!!!!!!! log: empty queue");
                    cout<< RPU[i].root <<" "<<tree[RPU[i].root].size() << " "<< root_num << endl;
                }
                int ans = tree[RPU[i].root].top().second;
                tree[RPU[i].root].pop();
                // Send a real packet in this cycle
                if (CheckRealQueue(ans)) {
                    buffer_num++;
                    buffer.push(ans);
                }
                // After non-leaf pop, now we know which node next to pop
                else {
                    AddPop(ans);
                }
            }
        }

        // Pass down the existing operation
        Operation nt[2]; // Scrolling array used to pass the operation
        nt[1] = RPU[N - 1];
        nt[1].TTL--;
        for (int i = 0, j; i < N; ++i) {
            j = i & 1;
            // Pass task in RPUi to the next RPU
            nt[j] = RPU[i];
            nt[j].TTL--;
            // After Pop, set a WriteBack, the TTL remains the same
            if (RPU[i].type == Pop) {
                RPU[i].type = WriteBack;
            }
            // Inherit task from the last RPU
            else if (nt[j ^ 1].TTL > 0) {
                RPU[i] = nt[j ^ 1];
            }
            // Set the RPU as Empty
            else {
                RPU[i].type = Empty;
                RPU[i].TTL = 0;
            }
        }

        // Send new operation
        for (int i = 0, j; i < N; ++i) {
            wait_time[i]++;
            if (RPU[i].type == Empty && !locked[i]) {
                if (!operation_list[i].empty()) {
                    Operation t = operation_list[i].front();
                    if (t.type == Push) {
                        RPU[i] = t;
                        // Since priority_queue is a big root heap, 
                        // take the opposite number for rank
                        tree[t.root].push(make_pair(-t.rank, t.token));
                        SendOperation(i);
                    }
                    else {
                        j = (i - 1 + N) % N;
                        if (RPU[j].TTL <= 1) {
                            RPU[i] = t;
                            SendOperation(i);
                            // The last RPU should be locked
                            if (RPU[j].type == Empty)
                                RPU[j].type = Locked;
                            RPU[j].TTL = 1;
                        }
                    }
                }
            }
        }

        // Anti-starvation mechanism
        if (beggar == -1) {
            for (int i = 0; i < N; ++i)
                if (wait_time[i] > S) {
                    beggar = i;
                    break;
                }
        }
        if (beggar != -1) {
            int flag_nt = 0, flag_last = 0;
            int need_pos = 1;
            if (operation_list[beggar].front().type == Pop)
                need_pos++;
            for (int j = beggar - 1, k; ; --j) {
                flag_last = flag_nt;
                flag_nt = 0;
                if (locked[j] || RPU[j].TTL <= 
                    (j < beggar) ? (beggar - j) : (beggar + N - j))
                    flag_nt = 1;
                if (flag_nt + flag_last == need_pos) {
                    locked[j] = 1;
                    if (need_pos == 2)
                        locked[k] = 1;
                    need_pos = 0;
                    break;
                }
                if (need_pos == 0) break;
                k = j;
                if (j == beggar)  break;
                if (j == 0)  j = N;
            }
            // Lock the next wait_time - S RPU anyway
            if (need_pos) {
                starve_count++;
                need_pos = wait_time[beggar] - S;
                for (int j = beggar + 1; j < N; ++j) {
                    if (need_pos == 0)
                        break;
                    if (!locked[j]) {
                        need_pos--;
                        locked[j] = 1;
                    }
                }
                for (int j = 0; j < beggar; ++j) {
                    if (need_pos == 0)
                        break;
                    if (!locked[j]) {
                        need_pos--;
                        locked[j] = 1;
                    }
                }
            }
        }
        
        if (use_num)
            empty_count = 0;
        else
            empty_count++;
        if (empty_count < END_THRESHOLD)
            Simulator::Schedule(NanoSeconds(CYCLE), &Pipeline::WakeUp, this);
        else
            Simulator::Stop();
    }

}
