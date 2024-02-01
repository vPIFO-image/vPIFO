#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <cassert>
#include <pcap.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include "WFQ.h"
#include "util.h"

StrategyWFQ SchedStrategyWFQ(){
    StrategyWFQ strategyWFQ = new StrategyWFQ_;
    strategyWFQ->curWeightSum = 0;
    strategyWFQ->virTime = 0;
    strategyWFQ->realTime = 0;
    strategyWFQ->lastVirFinishTime.clear();
    while(!strategyWFQ->outQueue.empty()){
        strategyWFQ->outQueue.pop();
    }
    return strategyWFQ;
}

void updateVirTimeTo(StrategyWFQ strategyWFQ, long long nextRealTime, double newFlowWeight) {
    while(!strategyWFQ->outQueue.empty() && strategyWFQ->outQueue.top().virDepartTime <= getNextVirTime(strategyWFQ->realTime, strategyWFQ->virTime, nextRealTime, strategyWFQ->curWeightSum)){
        double virDepartTime = strategyWFQ->outQueue.top().virDepartTime;
        double flowWeight = strategyWFQ->outQueue.top().flowWeight;
        strategyWFQ->outQueue.pop();

        strategyWFQ->realTime = getNextRealTime(strategyWFQ->realTime, strategyWFQ->virTime, virDepartTime, strategyWFQ->curWeightSum);
        strategyWFQ->curWeightSum -= flowWeight;
        strategyWFQ->virTime = virDepartTime;
    }

    strategyWFQ->realTime = nextRealTime;
    strategyWFQ->curWeightSum += newFlowWeight;
    strategyWFQ->virTime = getNextVirTime(strategyWFQ->realTime, strategyWFQ->virTime, nextRealTime, strategyWFQ->curWeightSum);
}

int calWFQLeafPriority(unsigned char* user, const struct pcap_pkthdr* pkthdr, const unsigned char* packet, StrategyWFQ strategyWFQ) {
    std::string flowId = getFlowId(user, pkthdr, packet);

    assert(strategyWFQ->weightTable.find(flowId) != strategyWFQ->weightTable.end());

    double weight = strategyWFQ->weightTable[flowId];

    updateVirTimeTo(strategyWFQ, tsToCycle(&(pkthdr->ts)), weight);

    double virStartTime = 0;
    if(strategyWFQ->lastVirFinishTime.find(flowId) == strategyWFQ->lastVirFinishTime.end()){
        virStartTime = strategyWFQ->virTime;
    }else{
        virStartTime = std::max(strategyWFQ->virTime, strategyWFQ->lastVirFinishTime[flowId]);
    }

    strategyWFQ->lastVirFinishTime[flowId] = virStartTime + pkthdr->len / weight;

    packetOut out = {virStartTime + pkthdr->len / weight, weight};
    strategyWFQ->outQueue.push(out);

    return (int)virStartTime;
}

int calWFQNonLeafPriority(int nodeId, StrategyWFQ strategyWFQ, const struct pcap_pkthdr* pkthdr) {
    std::string nodeId_str = std::to_string(nodeId);
    if(strategyWFQ->weightTable.find(nodeId_str) == strategyWFQ->weightTable.end()){
        std::cout<<"nodeId_str: "<<nodeId_str<<std::endl;
    }
    assert(strategyWFQ->weightTable.find(nodeId_str) != strategyWFQ->weightTable.end());

    double weight = strategyWFQ->weightTable[nodeId_str];

    updateVirTimeTo(strategyWFQ, tsToCycle(&(pkthdr->ts)), weight);

    double virStartTime = 0;
    if(strategyWFQ->lastVirFinishTime.find(nodeId_str) == strategyWFQ->lastVirFinishTime.end()){
        virStartTime = strategyWFQ->virTime;
    }else{
        virStartTime = std::max(strategyWFQ->virTime, strategyWFQ->lastVirFinishTime[nodeId_str]);
    }

    strategyWFQ->lastVirFinishTime[nodeId_str] = virStartTime + pkthdr->len / weight;

    packetOut out = {virStartTime + pkthdr->len / weight, weight};
    strategyWFQ->outQueue.push(out);

    return (int)virStartTime;
}
