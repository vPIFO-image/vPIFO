// version 5: add the pop lock
// lock from pop_root in task list to pop_A out task list
// implement scrolling groups of task list

# include <bits/stdc++.h>

using namespace std;

const int N = 5;
const int M = 10;
// Each user sends a Push task with a probability of P / M per cycle
const int P = 1; 
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
    int root, TTL, val;
    Task () {
        TTL = 0;
        type = Empty;
    }
    Task (enum Type t, int r, int ttl) {
        type = t;
        root = r;
        TTL = ttl;
    }
}RPU[N];

struct Tree {
    int min[2], num[2], son[2];
    Tree() {
        min[0] = min[1] = inf;
        num[0] = num[1] = 0;
        son[0] = son[1] = 0;
    }
} node[M<<N];

// record the root node of trees
int tree[M], tot;

int get_son (int root, int d) {
    if (node[root].son[d] != 0)
        return node[root].son[d];
    tot++;
    node[root].son[d] = tot;
    return tot;
}

void push (int root, int val) {
    // try to insert x in this node
    if (node[root].min[0] == inf) {
        node[root].min[0] = val;
        return;
    }
    if (node[root].min[1] == inf) {
        node[root].min[1] = val;
        return;
    }

    // insert x in one of the son
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

int pop (int root) {
    int ret = 0;
    int go_down = 0;
    if (node[root].min[0] > node[root].min[1])
        go_down = 1;
    ret = node[root].min[go_down];
    node[root].min[go_down] = inf;
    if (node[root].son[go_down] != 0) {
        node[root].min[go_down] = pop(node[root].son[go_down]);
        node[root].num[go_down]--;
    }
    return ret;
}

void tree_print (int fa, int root, int dep) {
    printf("node %d is son of %d, dep = %d\n", root, fa, dep);
    printf("min0 = %d, min1 = %d\n", node[root].min[0], node[root].min[1]);
    printf("num0 = %d, num1 = %d\n", node[root].num[0], node[root].num[1]);
    if (node[root].son[0])
        tree_print(root, node[root].son[0], dep + 1);
    if (node[root].son[1])
        tree_print(root, node[root].son[1], dep + 1);
}

queue<Task> task_list[N][2];
queue<int> root_queue;

// now_save and now_use are two 0/1 variable, means two groups of task lists
// new push tasks are added in now_save task lists
// switch now_save when now_use begin to be locked
// switch now_use until all tasks in now_use are sent to RPU
int lock_num, now_save, now_use;
int task_num[M][2], T, use_num;
double Q;

int main() {

    // initialize the roots of the trees
    for (int i = 0; i < M; ++i)
        tree[i] = ++tot;

    Task t;

    printf("N = %d, M = %d\n", N, M);

    while (T < 300) {
        T++;
        t = (Task){Push, 0, N};

        // insert push task
        // Attention: 0 is the root queue and it don't send push proactively
        for (int i = 1, j; i < M; ++i) {
            j = rand() % M;
            if (j <= P) {
                // pifo i send push val
                t.root = i;
                t.val = rand() % inf;
                task_list[i % N][now_save].push(t);
                task_num[i][now_save]++;

                // pifo 0 send push i
                t.root = 0;
                t.val = i;
                task_list[0][now_save].push(t);
                task_num[0][now_save]++;
            }
        }
        

        // insert pop root task every two cycle
        if (T % 2 == 0 && task_num[0][now_use] > 0) {
            t = (Task){Pop, 0, N};
            task_list[0][now_use].push(t);
            task_num[0][now_use]--;
            // pop root in task list, add a lock
            lock_num++;
            // when start the first lock, save push in the other task list
            if (lock_num == 1) 
                now_save ^= 1;
        }

        // statistics before updating status
        for (int i = 0; i < N; ++i) {
            // the RPU is running
            if (RPU[i].type == Push || RPU[i].type == Pop || RPU[i].type == WriteBack)
                use_num++;
            // after a root pop, now we know which tree to pop
            if (RPU[i].TTL == N && RPU[i].type == Pop && RPU[i].root == 0) {
                int ans = root_queue.front();
                root_queue.pop();
                t = (Task){Pop, ans, N};
                printf("log: add pop task %d\n", ans);
                task_list[ans % N][now_use].push(t);
                task_num[ans][now_use]--;
            }
        }

        // pass down the existing task
        Task nt[2]; // scrolling array used to pass the task
        nt[1] = RPU[N - 1];
        nt[1].TTL--;
        for (int i = 0, j; i < N; ++i) {
            j = i & 1;
            // pass task in RPUi to the next RPU
            nt[j] = RPU[i];
            nt[j].TTL--;
            // after Pop, set a WriteBack, the TTL remains the same
            if (RPU[i].type == Pop) {
                RPU[i].type = WriteBack;
            }
            // inherit task from the last RPU
            else if (nt[j ^ 1].TTL > 0) {
                RPU[i] = nt[j ^ 1];
            }
            // set the RPU as Empty
            else {
                RPU[i].type = Empty;
                RPU[i].TTL = 0;
            }
        }

        // switch the task list when now_use is empty
        if (now_use != now_save) {
            int try_switch = 1;
            for (int i = 0; i < N; ++i)
                if (!task_list[i][now_use].empty()) {
                    try_switch = 0;
                    break;
                }
            now_use ^= try_switch;
        }

        // send new task
        for (int i = 0, j; i < N; ++i) {
            if (RPU[i].type == Empty) {
                if (!task_list[i][now_use].empty()) {
                    Task t = task_list[i][now_use].front();
                    if (t.type == Push) {
                        RPU[i] = t;
                        if (t.root != 0)
                            push(tree[t.root], t.val);
                        else
                            root_queue.push(t.val);
                        printf("log: push val %d of tree %d in RPU %d\n", t.val, t.root, i);
                        task_list[i][now_use].pop();
                    }
                    else {
                        j = (i - 1 + N) % N;
                        if (RPU[j].TTL <= 1) {
                            RPU[i] = t;
                            task_list[i][now_use].pop();
                            if (i != 0)
                                pop(tree[t.root]);
                            // pop A out task list, release a lock
                            else 
                                lock_num--;
                            printf("log: pop tree %d in RPU %d\n", t.root, i);
                            // The last RPU should be locked
                            if (RPU[j].type == Empty)
                                RPU[j].type = Locked;
                            RPU[j].TTL = 1;
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < M; ++i) {
        printf("\nlog: Tree %d\n", i);
        tree_print(0, tree[i], 1);
    }

    Q = ((double)use_num) / ((double)(N * T));
    printf("use_num = %d, T = %d, Q = %.4lf\n", use_num, T, Q);

    return 0;
}