// version 1: only implement the vPIFO pipeline
// input is batch processed and random
// run until all the task list is empty
# include <bits/stdc++.h>

using namespace std;

const int N = 5;

enum Type {
    Push,
    Pop,
    WriteBack,
    Locked,
    Empty
};

struct Task {
    enum Type type;
    int root, TTL;
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

queue<Task> task_list[N];

int task_num, T, use_num;
double Q;

int main() {

    task_num = 100;
    
    for (int i = 1; i <= task_num; ++i) {
        int root = rand() % N;
        int type = rand() % 2;

        Task t;
        if (type > 0)
            t = (Task){Push, root, N};
        else 
            t = (Task){Pop, root, N};
        task_list[root].push(t);
    }

    printf("N = %d, task_num = %d, ", N, task_num);

    while (task_num > 0) {
        T++;
        // statistics before updating status
        for (int i = 0; i < N; ++i) {
            // the RPU is running
            if (r[i].type == Push || r[i].type == Pop || r[i].type == WriteBack)
                use_num++;
            // the task execution completes in this cycle
            if (r[i].TTL == 1 && (r[i].type == Push || r[i].type == WriteBack))
                task_num--;
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
                        task_list[i].pop();
                    }
                    else {
                        j = (i - 1 + N) % N;
                        if (r[j].TTL <= 1) {
                            r[i] = t;
                            task_list[i].pop();
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

    Q = ((double)use_num) / ((double)(N * T));
    printf("use_num = %d, T = %d, Q = %.4lf\n", use_num, T, Q);

    return 0;
}