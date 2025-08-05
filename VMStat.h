#pragma once

#include <cstddef>
#include <iostream>

using namespace std;

class VMStat {
public:
    // Constructor
    VMStat(size_t totalMemory);
    static void initialize(size_t totalMemory);
    static VMStat* getInstance();

    // Memory tracking
    void addUsedMemory(size_t used);
    void subUsedMemory(size_t used);

    // CPU tick tracking
    void addIdleTicks();
    void addActiveTicks();
    size_t getTotalTicks();

    // Paging tracking
    void incrementPagedIn();
    void incrementPagedOut();

    // Output
    void print(size_t freeMemory);

private:
    static VMStat* vmstat;

    size_t totalMemory = 0;
    size_t usedMemory = 0;
    size_t idleTicks = 0;
    size_t activeTicks = 0;
    size_t pagedIn = 0; // loaded from BS
    size_t pagedOut = 0; // evicted into BS
};
