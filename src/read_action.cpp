#include <cstdio>
#include <cmath>
#include <algorithm>
#include <vector>
#include <unordered_set>
#include "globals.h"

static int jumpCounter = 0;
static std::unordered_set<int> readed_object_id;
static std::vector<int> completed_requests;

void read_action() {
    int n_read;
    scanf("%d", &n_read);

    // 读取所有请求，更新链表头和时间戳
    for (int i = 1; i <= n_read; i++) {
        int request_id, object_id;
        scanf("%d%d", &request_id, &object_id);
        request[request_id].object_id = object_id;
        request[request_id].prev_id = object[object_id].last_request_point;
        object[object_id].last_request_point = request_id;
        request[request_id].is_done = false;
        request[request_id].timestamp = current_timestamp;
    }

    // 对每个磁盘执行读/跳转/空闲等动作
    for (int i = 1; i <= N; i++) {
        disk_state[i].remain_token = G;

        // 第一块磁盘专门处理跳转逻辑
        if (i == 1) {
            // 回退 maxallpos[3] 找到最后一个非零位置
            while (true) {
                bool all_zero = true;
                for (int d = 0; d < 10; d++) {
                    if (disk[1 + d][maxallpos[3]] != 0) {
                        all_zero = false;
                        break;
                    }
                }
                if (all_zero) maxallpos[3]--;
                else break;
            }

            bool ok1 =  (disk_state[i].position > maxallpos[1] && disk_state[i].position < V/3 + 1 - 90);
            bool ok2 =  (disk_state[i].position > maxallpos[2] && disk_state[i].position < 2*V/3 + 1 - 190);
            bool ok3 =  (disk_state[i].position > maxallpos[3]);

            if (ok1 || ok2 || ok3) {
                int target = ok1 ? (V/3 + 1 - 80)
                           : ok2 ? (2*V/3 + 1 - 160)
                           : 1;
                for (int j = 1; j <= N; j++) {
                    printf("j %d\n", target);
                    disk_state[j].position = target;
                    disk_state[j].is_last_action_read = 0;
                    disk_state[j].remain_token = 0;
                }
                jumpCounter++;
                break;
            }
        }

        // 普通读操作前，尝试一次连续读（p）
        int next_ts = /*get_next_timestamp_size(current_timestamp)*/ 160;
        int recent_count = 0;
        for (int j = 0; j < next_ts; j++) {
            int pos = ((disk_state[i].position + j - 1) % V) + 1;
            int obj = disk[i][pos];
            if (obj != 0) {
                int last_req = object[obj].last_request_point;
                if (!request[last_req].is_done
                    && request[last_req].timestamp > current_timestamp - EXTRA_TIME) {
                    if (++recent_count >= 2) break;
                }
            }
        }
        if (recent_count < 1 && disk_state[i].remain_token >= next_ts) {
            for (int j = 0; j < next_ts; j++) {
                printf("p");
                disk_state[i].remain_token--;
                disk_state[i].position = (disk_state[i].position % V) + 1;
                disk_state[i].is_last_action_read = 0;
            }
        }

        // 持续读（r）直到 token 用完
        while (disk_state[i].remain_token > 0) {
            int cost = disk_state[i].is_last_action_read
                       ? std::max(16, (int)std::ceil(disk_state[i].is_last_action_read * 0.8))
                       : 64;
            if (disk_state[i].remain_token < cost) {
                printf("#\n");
                break;
            }
            printf("r");
            int pos = disk_state[i].position;
            int obj = disk[i][pos];
            if (obj) {
                // 标记该对象单元已读
                for (int j = 1; j <= REP_NUM; j++) {
                    if (i == object[obj].replica[j]) {
                        for (int k = 1; k <= object[obj].size; k++) {
                            if (object[obj].unit[j][k] == pos) {
                                object[obj].read_flag[k] = current_timestamp;
                                break;
                            }
                        }
                        break;
                    }
                }
                readed_object_id.insert(obj);
            }
            disk_state[i].remain_token -= cost;
            disk_state[i].is_last_action_read = cost;
            disk_state[i].position = (disk_state[i].position % V) + 1;
            if (disk_state[i].remain_token == 0) {
                printf("#\n");
            }
        }
    }

    // 检查哪些对象已经完全读取，准备完成通知
    for (auto it = readed_object_id.begin(); it != readed_object_id.end(); ) {
        int obj = *it;
        bool complete = true;
        for (int k = 1; k <= object[obj].size; k++) {
            if (object[obj].read_flag[k] == 0) {
                complete = false;
                break;
            }
        }
        if (complete) {
            if (!object[obj].is_delete) {
                int min_ts = current_timestamp;
                for (int k = 1; k <= object[obj].size; k++) {
                    min_ts = std::min(min_ts, object[obj].read_flag[k]);
                    object[obj].read_flag[k] = 0;
                }
                int rid = object[obj].last_request_point;
                while (rid && request[rid].timestamp <= min_ts
                       && request[rid].timestamp > current_timestamp - EXTRA_TIME) {
                    if (!request[rid].is_done) {
                        completed_requests.push_back(rid);
                    }
                    rid = request[rid].prev_id;
                }
            }
            it = readed_object_id.erase(it);
        } else {
            ++it;
        }
    }

    // 输出完成的请求列表
    if (!completed_requests.empty()) {
        printf("%zu\n", completed_requests.size());
        for (int rid : completed_requests) {
            request[rid].is_done = true;
            printf("%d\n", rid);
        }
    } else {
        printf("0\n");
    }
    completed_requests.clear();
    fflush(stdout);
}
