#pragma once

#include <map>
#include <string>

typedef struct StrategySP_* StrategySP;

struct StrategySP_{
    std::map<std::string, int> priorityTable;
};

StrategySP SchedStrategySP();
int calSPLeafPriority(unsigned char* user, const struct pcap_pkthdr* pkthdr, const unsigned char* packet, StrategySP strategySP);
int calSPNonLeafPriority(int nodeId, StrategySP strategySP);