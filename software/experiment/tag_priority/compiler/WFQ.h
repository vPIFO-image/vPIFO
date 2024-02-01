#pragma once

#include <map>
#include <string>
#include <queue>

typedef struct StrategyWFQ_* StrategyWFQ;

struct packetOut{
    double virDepartTime;
    double flowWeight;
};

struct packetOutCmp{
    bool operator () (packetOut &a, packetOut &b){
        return a.virDepartTime > b.virDepartTime;
    }
};

struct StrategyWFQ_{
    std::map<std::string, double> weightTable;
    double curWeightSum;
    double virTime;
    long long realTime;
    std::map<std::string, double> lastVirFinishTime;
    std::priority_queue<packetOut, std::vector<packetOut>, packetOutCmp> outQueue;
};

inline double getNextVirTime(long long curRealTime, double curVirTime, long long nextRealTime, double curWeightSum){
    return curVirTime + (nextRealTime - curRealTime) / curWeightSum;
}

inline double getNextRealTime(long long curRealTime, double curVirTime, long long nextVirTime, double curWeightSum){
    return curRealTime + (nextVirTime - curVirTime) * curWeightSum;
}

StrategyWFQ SchedStrategyWFQ();
void updateVirTimeTo(StrategyWFQ strategyWFQ, long long nextRealTime, double newFlowWeight);
int calWFQLeafPriority(unsigned char* user, const struct pcap_pkthdr* pkthdr, const unsigned char* packet, StrategyWFQ strategyWFQ);
int calWFQNonLeafPriority(int nodeId, StrategyWFQ strategyWFQ, const struct pcap_pkthdr* pkthdr);