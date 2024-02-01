#pragma once

#include <ostream>
#include "PFabric.h"
#include "SP.h"
#include "WFQ.h"

using std::ostream;

typedef struct SchedStrategy_* SchedStrategy;

typedef enum {
    UNKNOWNTYPE,
    PFABRICTYPE,
    SPTYPE,
    WFQTYPE
} SchedStrategyType;

struct SchedStrategy_{
    SchedStrategyType type;
    union {
        StrategyPFabric pFabric;
        StrategySP SP;
        StrategyWFQ WFQ;
    } u;
};

SchedStrategy SchedStrategyUnknown();
SchedStrategy SchedStrategyPFabric(StrategyPFabric pFabric);
SchedStrategy SchedStrategySP(StrategySP SP);
SchedStrategy SchedStrategyWFQ(StrategyWFQ WFQ);

void tagPriority(const char* pcap_file, const char* trace_file, bool hasPFabric);
void printPFMap(const char* pcap_file, const char* PFMap_file);

void printSchedStrategy(SchedStrategy strategy, ostream& os);