// version 3: implement the BMW Tree
// now when the task are sent into RPU, 
// have the work done in Tree at the same time

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
    Task() {
        TTL = 0;
        type = Empty;
    }
    Task(enum Type t, int r, int ttl) {
        type = t;
        root = r;
        TTL = ttl;
    }
}r[N];

struct Tree {
    int min[2], num[2], son[2];
    Tree() {
        min[0] = min[1] = inf;
        num[0] = num[1] = 0;
        son[0] = son[1] = 0;
    }
}node[M<<N];

// record the root node of trees
int tree[M], tot;

int get_son(int r, int d) {
    if (node[r].son[d] != 0)
        return node[r].son[d];
    tot++;
    node[r].son[d] = tot;
    return tot;
}

void push(int r, int x) {
    // try to insert x in this node
    if (node[r].min[0] == inf) {
        node[r].min[0] = x;
        return;
    }
    if (node[r].min[1] == inf) {
        node[r].min[1] = x;
        return;
    }

    // insert x in one of the son
    int go_down = 0;
    if (node[r].num[0] > node[r].num[1])
        go_down = 1;
    if (x < node[r].min[go_down]) {
        push(get_son(r, go_down), node[r].min[go_down]);
        node[r].min[go_down] = x;
    }
    else
        push(get_son(r, go_down), x);
    node[r].num[go_down]++;
}

int pop(int r) {
    int ret = 0;
    int go_down = 0;
    if (node[r].min[0] > node[r].min[1])
        go_down = 1;
    ret = node[r].min[go_down];
    node[r].min[go_down] = inf;
    if (node[r].son[go_down] != 0) {
        node[r].min[go_down] = pop(node[r].son[go_down]);
        node[r].num[go_down]--;
    }
    return ret;
}

void tree_print(int fa, int r, int dep) {
    printf("node %d is son of %d, dep = %d\n", r, fa, dep);
    printf("min0 = %d, min1 = %d\n", node[r].min[0], node[r].min[1]);
    printf("num0 = %d, num1 = %d\n", node[r].num[0], node[r].num[1]);
    if (node[r].son[0])
        tree_print(r, node[r].son[0], dep + 1);
    if (node[r].son[1])
        tree_print(r, node[r].son[1], dep + 1);
}

queue<Task> task_list[N];

int task_num[M], T, use_num;
double Q;

int main() {

    // initialize the roots of the trees
    for (int i = 0; i < M; ++i)
        tree[i] = ++tot;

    Task t;
    int round_robin = 0;

    printf("N = %d, M = %d\n", N, M);

    while (T < 100) {
        T++;
        t = (Task){Push, 0, N};

        // insert push task
        for (int i = 0, j; i < M; ++i) {
            j = rand() % 10;
            if (j < 6) {
                t.root = i;
                t.val = rand();
                task_list[i % N].push(t);
                task_num[i]++;
            }
        }

        // insert pop task every two cycle
        if (T % 2 == 0) {
            if (task_num[round_robin] > 0) {
                t = (Task){Pop, round_robin, N};
                task_list[round_robin % N].push(t);
                task_num[round_robin]--;
            }
            round_robin++;
            if (round_robin == M)
                round_robin = 0;
        }

        // statistics before updating status
        for (int i = 0; i < N; ++i) {
            // the RPU is running
            if (r[i].type == Push || r[i].type == Pop || r[i].type == WriteBack)
                use_num++;
        }

        // pass down the existing task
        Task nt[2]; // scrolling array used to pass the task
        nt[1] = r[N - 1];
        nt[1].TTL--;
        for (int i = 0, j; i < N; ++i) {
            j = i & 1;
            // pass task in RPUi to the next RPU
            nt[j] = r[i];
            nt[j].TTL--;
            // after Pop, set a WriteBack, the TTL remains the same
            if (r[i].type == Pop) {
                r[i].type = WriteBack;
            }
            // inherit task from the last RPU
            else if (nt[j ^ 1].TTL > 0) {
                r[i] = nt[j ^ 1];
            }
            // set the RPU as Empty
            else {
                r[i].type = Empty;
                r[i].TTL = 0;
            }
        }

        // send new task
        for (int i = 0, j; i < N; ++i) {
            if (r[i].type == Empty) {
                if (!task_list[i].empty()) {
                    Task t = task_list[i].front();
                    if (t.type == Push) {
                        r[i] = t;
                        push(tree[t.root], t.val);
                        printf("log: push val %d of tree %d in RPU %d\n", t.val, t.root, i);
                        task_list[i].pop();
                    }
                    else {
                        j = (i - 1 + N) % N;
                        if (r[j].TTL <= 1) {
                            r[i] = t;
                            task_list[i].pop();
                            pop(tree[t.root]);
                            printf("log: pop tree %d in RPU %d\n", t.root, i);
                            // The last RPU should be locked
                            if (r[j].type == Empty)
                                r[j].type = Locked;
                            r[j].TTL = 1;
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