#include <fstream>
#include <map>
#include <string>
#include "../compiler/vPIFOLib.h"

using namespace std;

string getPcapFileName(int id1, int id2){
    return "../trace-udp/PcapTrace-" + to_string(id1) + "-" + to_string(id2) + ".pcap";
}

string getFlowIdFileName(int id1, int id2){
    return "../compiler_test/PcapTrace-" + to_string(id1) + "-" + to_string(id2) + ".flowId";
}

int main(int argc, char * argv[]){
    // for(int i=6; i<=7; ++i){
    //     for(int j=0; j<=3; ++j){
    //         extractFlowId(getPcapFileName(i, j).c_str(), getFlowIdFileName(i, j).c_str());
    //     }
    // }
    extractFlowId(getPcapFileName(6, 3).c_str(), getFlowIdFileName(6, 3).c_str());
    return 0;
}