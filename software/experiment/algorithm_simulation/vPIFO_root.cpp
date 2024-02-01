// version 4: add the specific tree "root"
// for every push i task in tree_A, also push A in tree_root
// every two cycle, add a pop task in tree_root
// if the result of this pop task is A, then add pop task in tree_A

// In this code the scheduling algorithm of root is FIFO(queue)

# include <bits/stdc++.h>

using namespace std;

const int N = 5;
const int M = 10;
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

queue<Task> task_list[N];
queue<int> root_queue;

int task_num[M], T, use_num;
double Q;

int main() {

    // initialize the roots of the trees
    for (int i = 0; i < M; ++i)
        tree[i] = ++tot;

    Task t;

    printf("N = %d, M = %d\n", N, M);

    while (T < 100) {
        T++;
        t = (Task){Push, 0, N};

        // insert push task
        // Attention: 0 is the root queue and it don't send push proactively
        for (int i = 1, j; i < M; ++i) {
            j = rand() % 10;
            if (j < 6) {
                // pifo i send push val
                t.root = i;
                t.val = rand();
                task_list[i % N].push(t);
                task_num[i]++;

                // pifo 0 send push i
                t.root = 0;
                t.val = i;
                task_list[0].push(t);
                task_num[0]++;
            }
        }

        // insert pop root task every two cycle
        if (T % 2 == 0 && task_num[0] > 0) {
            t = (Task){Pop, 0, N};
            task_list[0].push(t);
            task_num[0]--;
        }

        // statistics before updating status
        for (int i = 0; i < N; ++i) {
            // the RPU is running
            if (RPU[i].type == Push || RPU[i].type == Pop || RPU[i].type == WriteBack)
                use_num++;
            // after a root pop, now we know which tree to pop
            if (RPU[i].TTL == N && RPU[i].type == WriteBack && RPU[i].root == 0) {
                int ans = root_queue.front();
                root_queue.pop();
                t = (Task){Pop, ans, N};
                task_list[ans % N].push(t);
                task_num[ans]--;
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

        // send new task
        for (int i = 0, j; i < N; ++i) {
            if (RPU[i].type == Empty) {
                if (!task_list[i].empty()) {
                    Task t = task_list[i].front();
                    if (t.type == Push) {
                        RPU[i] = t;
                        if (t.root != 0)
                            push(tree[t.root], t.val);
                        else 
                            root_queue.push(t.val);
                        printf("log: push val %d of tree %d in RPU %d\n", t.val, t.root, i);
                        task_list[i].pop();
                    }
                    else {
                        j = (i - 1 + N) % N;
                        if (RPU[j].TTL <= 1) {
                            RPU[i] = t;
                            task_list[i].pop();
                            if (i != 0)
                                pop(tree[t.root]);
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