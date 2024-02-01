#pragma once

typedef struct StrategyPFabric_* StrategyPFabric;

struct StrategyPFabric_{
    int palce_holder;
};

StrategyPFabric SchedStrategyPFabric();
void pFabricInitFlowRemainingSize(const char* pcap_file);
int calPFabricPriority(unsigned char* user, const struct pcap_pkthdr* pkthdr, const unsigned char* packet);