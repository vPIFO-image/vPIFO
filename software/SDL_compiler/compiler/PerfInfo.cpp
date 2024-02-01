#include <cassert>
#include "PerfInfo.h"

PerfInfo createPerfInfo(int bandWidth){
    PerfInfo perfInfo = new PerfInfo_;
    perfInfo->bandWidth = bandWidth;
    return perfInfo;
}

void sumPerfInfo(PerfInfo perfInfo, PerfInfo perfInfoSum){
    assert(perfInfoSum);
    if(perfInfo){
        perfInfoSum->bandWidth += perfInfo->bandWidth;
    }
}

bool checkPerfInfoMeet(PerfInfo actualPerf, PerfInfo minPerf){
    assert(actualPerf);
    if(!minPerf) return true;
    if(minPerf->bandWidth <= actualPerf->bandWidth){
        return true;
    }else{
        return false;
    }
}

void copyPerfInfo(PerfInfo perfInfoSrc, PerfInfo perfInfoDst){
    assert(perfInfoSrc);
    assert(perfInfoDst);
    perfInfoDst->bandWidth = perfInfoSrc->bandWidth;
}

void getMarginPerf(PerfInfo actualPerf, PerfInfo minPerf, PerfInfo marginPerf){
    assert(actualPerf);
    assert(marginPerf);
    if(!minPerf){
        copyPerfInfo(actualPerf, marginPerf);
    }else{
        marginPerf->bandWidth = actualPerf->bandWidth - minPerf->bandWidth;
    }
}

void dividePerfEqually(PerfInfo perfInfo, int num, PerfInfo dividedPerfInfo){
    assert(perfInfo);
    assert(dividedPerfInfo);
    assert(num > 0);
    dividedPerfInfo->bandWidth = perfInfo->bandWidth / num;
}

void printPerfInfo(PerfInfo perfInfo, ostream& os){
    os << "\tbandWidth: " << perfInfo->bandWidth << "\n";
}