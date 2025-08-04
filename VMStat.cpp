#include "VMStat.h"
#include <iostream>

using namespace std;

VMStat* VMStat::vmstat = nullptr;

// Constructor
VMStat::VMStat(size_t totalMemory)
    : totalMemory(totalMemory) {
}

// Memory tracking
void VMStat::addUsedMemory(size_t used) {
    usedMemory += used;
}

void VMStat::subUsedMemory(size_t used) {
    usedMemory -= used;
}

// CPU ticks
void VMStat::addIdleTicks() {
    idleTicks++;
}

void VMStat::addActiveTicks() {
    activeTicks++;
}

size_t VMStat::getTotalTicks() {
    return activeTicks + idleTicks;
}

// Paging
void VMStat::incrementPagedIn() {
    pagedIn++;
}

void VMStat::incrementPagedOut() {
    pagedOut++;
}

void VMStat::initialize(size_t totalMemory) {
    vmstat = new VMStat(totalMemory);
}

VMStat* VMStat::getInstance() {
    return vmstat;
}

// Print stats
void VMStat::print(size_t freeMemory) {
    cout << "================================================" << endl;
    cout << "                 vmstat details                 " << endl;
    cout << "================================================" << endl;
    cout << "Total Memory:     | " << totalMemory << " bytes" << endl;
    cout << "Used Memory:      | " << usedMemory << " bytes" << endl;
    cout << "Free Memory:      | " << freeMemory << " bytes" << endl;
    cout << "Active CPU Ticks: | " << activeTicks << endl;
    cout << "Idle CPU Ticks:   | " << idleTicks << endl;
    cout << "Total CPU Ticks:  | " << getTotalTicks() << endl;
    cout << "Pages Paged In:   | " << pagedIn << endl;
    cout << "Pages Paged Out:  | " << pagedOut << endl;
    cout << "================================================" << endl;
}