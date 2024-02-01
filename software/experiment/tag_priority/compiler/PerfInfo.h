#pragma once

#include <ostream>

using std::ostream;

typedef struct PerfInfo_* PerfInfo;

struct PerfInfo_{
    double bandWidth;
};

PerfInfo createPerfInfo(int bandWidth);
void sumPerfInfo(PerfInfo perfInfo, PerfInfo perfInfoSum);
bool checkPerfInfoMeet(PerfInfo actualPerf, PerfInfo minPerf);
void copyPerfInfo(PerfInfo perfInfoSrc, PerfInfo perfInfoDst);
void getMarginPerf(PerfInfo actualPerf, PerfInfo minPerf, PerfInfo marginPerf);
void dividePerfEqually(PerfInfo perfInfo, int num, PerfInfo dividedPerfInfo);


void printPerfInfo(PerfInfo perfInfo, ostream& os);