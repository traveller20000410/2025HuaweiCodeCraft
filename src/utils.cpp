#include <cstdlib>
#include "globals.h"

void clean() {
    for (auto& obj : object) {
        for (int i = 1; i <= REP_NUM; i++) {
            free(obj.unit[i]);
            obj.unit[i] = nullptr;
        }
    }
}
