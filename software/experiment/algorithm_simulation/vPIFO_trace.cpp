// version 7: input the trace with priority and output the priority sequence

#include <bits/stdc++.h>

using namespace std;


const char *TypeNames[] = {"Push", "Pop", "WriteBack","Locked",""};

// To csl: const parameters is here!

// INPUT is the name of input file, trace with priority
const char * INPUT = "test2_1.trace";
// OUTPUT is the name of output file, the priority sequence
const char * OUTPUT = "output2_1.out";
// L is the cycle needed to solve a task in the BMW Tree 
const int L = 5;
// M is the number of users, but user 0 is root
const int M = 3;
// N is the number of RPUs, also the number of Task Lists
const int N = 5;
// S is the threshold that triggers the anti-starvation mechanism
const int S = 240;
// In BMW Tree, min = inf means the node is empty
const int inf = 1e9 + 7;

enum Type {
    Push,
    Pop,
    WriteBack,
    Locked,
    Empty
};

struct Task {
    enum Type type;
    // For real queue, val is packet size; for root queue, val is user id
    int root, TTL, rank, val;
    Task () {
        TTL = 0;
        type = Empty;
    }
    Task (enum Type t, int r) {
        type = t;
        root = r;
        TTL = L;
    }
    Task (enum Type t, int r, int rk, int v) {
        type = t;
        root = r;
        rank = rk;
        val = v;
        TTL = L;
    }
} RPU[N];

struct Tree {
    pair<int, int> min[2];
    int num[2];
    int son[2];
    Tree() {
        min[0].first = min[1].first = inf;
        num[0] = num[1] = 0;
        son[0] = son[1] = 0;
    }
} node[M<<L];

// Record the root node of BMW trees
int tree[M], tot;

int get_son (int root, int d) {
    if (node[root].son[d] != 0)
        return node[root].son[d];
    tot++;
    node[root].son[d] = tot;
    return tot;
}

void push (int root, pair<int, int> val) {
    // Try to insert x in this node
    if (node[root].min[0].first == inf) {
        node[root].min[0] = val;
        return;
    }
    if (node[root].min[1].first == inf) {
        node[root].min[1] = val;
        return;
    }

    // Insert x in one of the son
    int go_down = 0;
    if (node[root].num[0] > node[root].num[1])
        go_down = 1;
    if (val < node[root].min[go_down]) {
        push(get_son(root, go_down), node[root].min[go_down]);
        node[root].min[go_down] = val;
    }
    else
        push(get_son(root, go_down), val);
    node[root].num[go_down]++;
}

pair<int, int> pop (int root) {
    pair<int, int> ret;
    int go_down = 0;
    if (node[root].min[0] > node[root].min[1])
        go_down = 1;
    ret = node[root].min[go_down];
    node[root].min[go_down].first = inf;
    if (node[root].son[go_down] != 0) {
        node[root].min[go_down] = pop(node[root].son[go_down]);
        node[root].num[go_down]--;
    }
    return ret;
}

void tree_print (int fa, int root, int dep) {
    printf("node %d is son of %d, dep = %d\n", root, fa, dep);
    printf("min0 = %.2lf, min1 = %.2lf\n", node[root].min[0].first, node[root].min[1].first);
    printf("num0 = %d, num1 = %d\n", node[root].num[0], node[root].num[1]);
    if (node[root].son[0])
        tree_print(root, node[root].son[0], dep + 1);
    if (node[root].son[1])
        tree_print(root, node[root].son[1], dep + 1);
}

int task_num[M], use_num, Q, wait_time[N];
int starve_count, hungry_count, hungry_delay, send_count[N], locked[N], beggar;
int push_num[M], pop_num[M], push_sum, pop_sum;

queue<Task> task_list[N];

void push_task (int i, Task t) {
    if (task_list[i].empty())
        wait_time[i] = 0;
    task_list[i].push(t);
    // printf("Add Task %s in RPU %d \n",TypeNames[t.type], i);
}

void pop_task (int i) {
    task_list[i].pop();
    if (wait_time[i] > S) {
        hungry_count++;
        hungry_delay += wait_time[i] - S;
        // release lock
        if (beggar == i) {
            beggar = -1;
            memset(locked, 0, sizeof(locked));
        }
    }
    if (task_list[i].empty())
        wait_time[i] = -inf;
    else
        wait_time[i] = 0;


}

// To speed up the simulation, skip the idle phase with a flag
long long skipping_flag = 0;
long long cycle = 1;
int type, treeid, meta, p0, p1;
string type_s, idle_s, treeid_s, meta_s, p0_s, p1_s;

int get_number(string s, int skip_num) {
    int ret = 0;
    for (int i = 0; i < s.size(); ++i)
        if (s[i] <= '9' && s[i] >= '0') {
            if (skip_num) {
                skip_num--;
                continue;
            }
            ret = ret * 10 + s[i] - '0';
        }
    return ret;
}

// To csl: input is here!
int read_line() {
    if (cin >> type_s) {
        type = get_number(type_s, 0);
        // Add a pop task at root
        if (type == 2) {
            push_task(0, (Task){Pop, 0});
            task_num[0]--;
            // printf("log: read pop\n");
        }
        // Add a push task
        if (type == 1) {
            cin >> treeid_s >> meta_s >> p0_s >> p1_s;
            treeid = get_number(treeid_s, 0);
            meta = get_number(meta_s, 0);
            p0 = get_number(p0_s, 1);
            p1 = get_number(p1_s, 1);
            push_task(treeid % N, (Task){Push, treeid, p0, meta});
            push_task(0, (Task){Push, 0, p1, treeid});
            task_num[0]++, task_num[treeid]++;
            /* printf("log: read push treeid %d, meta %d, p0 %d, p1 %d\n",
                    treeid, meta, p0, p1); */
        }
        if (type == 0) {
            cin >> idle_s;
            skipping_flag = cycle + get_number(idle_s, 0);
        }
        return 1;
    }
    // The file is EOF
    else
        return 0;
}

// To csl: output is here!
void write_line(int priority) {
    printf("%d", priority);
    printf(" %d", cycle);
    printf("\n");
}


int main() {

    freopen(INPUT, "r", stdin);
    freopen(OUTPUT, "w", stdout);

    // Initialize the roots of the trees
    for (int i = 0; i < M; ++i)
        tree[i] = ++tot;
    // When the queue is empty, don't count wait_time
    memset(wait_time, -0x3f, sizeof(wait_time));
    // Initially nobody is hungry
    beggar = -1;
    

    Task t;
    pair<int, int> pa;

    while(1) {
        // shortcut: skipping_flag != 0  -> no read, just run a cycle
        if (!skipping_flag && !read_line())
            break;

        cycle++;
        // Processed cycle has excceeded the flag, no need to skip
        if (cycle > skipping_flag)
            skipping_flag = 0;

        // Statistics before updating status
        for (int i = 0; i < N; ++i) {
            // The RPU is running
            if (RPU[i].type == Push || RPU[i].type == Pop || RPU[i].type == WriteBack)
                use_num++;
            
            if (RPU[i].TTL == L) {
                if (RPU[i].type == Pop) {
                    pa = pop(tree[RPU[i].root]);
                    // After a root pop, now we know which tree to pop
                    if (RPU[i].root == 0) {
                        t = (Task){Pop, pa.second};
                        // printf("log: add pop task %d\n", pa.second);
                        push_task(pa.second % N, t);
                        task_num[pa.second]--;
                    }
                    // Send a real packet in this cycle
                    else {
                        pop_sum++;
                        pop_num[RPU[i].root]++;
                        write_line(pa.first);
                    }
                }
                else if (RPU[i].root != 0) {
                    push_sum++;
                    push_num[RPU[i].root]++;
                }
            }
        }

        /* printf("#######\n");
        for(int i = 0; i<N; i++)
        {
            printf("Task List %d: ", i);
            queue<Task> temp_q = task_list[i];
            while (!temp_q.empty()) {
                printf("%s ", TypeNames[temp_q.front().type]);
                temp_q.pop();
            }
            printf("\n");
        }
        printf("#######\n"); */
        
        // Pass down the existing task
        Task nt[2]; // Scrolling array used to pass the task
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

        // Send new task
        for (int i = 0, j; i < N; ++i) {
            wait_time[i]++;
            if (RPU[i].type == Empty && !locked[i]) {
                if (!task_list[i].empty()) {
                    Task t = task_list[i].front();
                    send_count[i]++;
                    if (t.type == Push) {
                        RPU[i] = t;
                        pa.first = t.rank;
                        pa.second = t.val;
                        push(tree[t.root], pa);
                        // printf("log: push val %d of tree %d in RPU %d\n", t.val, t.root, i);
                        pop_task(i);
                    }
                    else {
                        j = (i - 1 + N) % N;
                        if (RPU[j].TTL <= 1) {
                            RPU[i] = t;
                            pop_task(i);
                            // printf("log: pop tree %d in RPU %d\n", t.root, i);
                            // The last RPU should be locked
                            if (RPU[j].type == Empty)
                                RPU[j].type = Locked;
                            RPU[j].TTL = 1;
                        }
                    }
                }
            }
        }




        /* printf("---------\n");
        for(int i = 0; i<N; i++)
        {
            printf("RPU %d is %s\n", i, TypeNames[RPU[i].type]);
           
        }
        printf("-------\n"); */

        // Anti-starvation mechanism
        if (beggar == -1) {
            for (int i = 0; i < N; ++i)
                if (wait_time[i] > S) {
                    beggar = i;
                    //printf("log: task list %d is hungry!\n", beggar);
                    break;
                }
        }
        if (beggar != -1) {
            int flag_nt = 0, flag_last = 0;
            int need_pos = 1;
            if (task_list[beggar].front().type == Pop)
                need_pos++;
            for (int j = beggar - 1, k; ; --j) {
                flag_last = flag_nt;
                flag_nt = 0;
                if (locked[j] || RPU[j].TTL <= (j < beggar) ? (beggar - j) : (beggar + N - j))
                    flag_nt = 1;
                if (flag_nt + flag_last == need_pos) {
                    beggar = beggar;
                    locked[j] = 1;
                    if (need_pos == 2)
                        locked[k] = 1;
                    need_pos = 0;
                    break;
                }
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

        // TODO: jump to the skipping flag
        if (skipping_flag != 0) {
            int empty_flag = 1;
            for (int i = 0; i < N; ++i)
                if (RPU[i].type != Empty) {
                    empty_flag = 0;
                    break;
                }
            if (empty_flag) {
                cycle = skipping_flag;
                skipping_flag = 0;
            }
        }

        // printf("Cycle %d\n", cycle);
        /* printf("push sum: %d, pop sum: %d\n", push_sum, pop_sum);
        for (int i = 1; i < M; ++i)
            printf("user %d  push num: %d, pop num: %d\n", i, push_num[i], pop_num[i]);
        if (hungry_count > 0) {
            printf("hungry time: %d, starve time: %d\n", hungry_count, starve_count);
            printf("average hungry delay: %.2lf\n", 1.0 * hungry_delay / hungry_count);
        } */
    }


    /* for (int i = 0; i < N; ++i)
        printf("throughput of task list %d : %.2lf\n", i, 1.0 * send_count[i] / T);

    Q = ((int)use_num) / ((int)(N * T));
    printf("use_num = %d, T = %d, Q = %.4lf\n", use_num, T, Q); */

    return 0;
}