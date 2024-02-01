#include <fstream>
#include <map>
#include <string>
#include "../compiler/vPIFOLib.h"

using namespace std;

extern map<string, int> FidMap;

int main(int argc, char * argv[]){

    TreeNode root = createTreeRoot(SchedStrategySP(SchedStrategySP()), createPerfInfo(100));
    TreeNode vvip_user = createTreeNode(SchedStrategySP(SchedStrategySP()));
    TreeNode vip_user = createTreeNode(SchedStrategySP(SchedStrategySP()));
    attachNode(vvip_user, root, 1);
    attachNode(vip_user, root, 2);

    attachFlow("10.1.1.1, 10.1.4.2, 49153, 8080, UDP", vvip_user, 1);
    attachFlow("10.1.2.1, 10.1.5.2, 49153, 8080, UDP", vip_user, 1);
    attachFlow("10.1.1.1, 10.1.4.2, 49154, 8080, UDP", vvip_user, 2);
    attachFlow("10.1.2.1, 10.1.5.2, 49154, 8080, UDP", vip_user, 2);
    attachFlow("10.1.1.1, 10.1.4.2, 49155, 8080, UDP", vvip_user, 3);
    attachFlow("10.1.2.1, 10.1.5.2, 49155, 8080, UDP", vip_user, 3);
    attachFlow("10.1.4.2, 10.1.1.1, 8080, 49153, UDP", vvip_user, 1);
    attachFlow("10.1.5.2, 10.1.2.1, 8080, 49153, UDP", vip_user, 1);
    attachFlow("10.1.4.2, 10.1.1.1, 8080, 49154, UDP", vvip_user, 2);
    attachFlow("10.1.5.2, 10.1.2.1, 8080, 49154, UDP", vip_user, 2);
    attachFlow("10.1.4.2, 10.1.1.1, 8080, 49155, UDP", vvip_user, 3);
    attachFlow("10.1.5.2, 10.1.2.1, 8080, 49155, UDP", vip_user, 3);

    FidMap["10.1.1.1, 10.1.4.2, 49153, 8080, UDP"] = 0;
    FidMap["10.1.1.1, 10.1.4.2, 49154, 8080, UDP"] = 1;
    FidMap["10.1.1.1, 10.1.4.2, 49155, 8080, UDP"] = 2;
    FidMap["10.1.2.1, 10.1.5.2, 49153, 8080, UDP"] = 3;
    FidMap["10.1.2.1, 10.1.5.2, 49154, 8080, UDP"] = 4;
    FidMap["10.1.2.1, 10.1.5.2, 49155, 8080, UDP"] = 5;
    FidMap["10.1.4.2, 10.1.1.1, 8080, 49153, UDP"] = 0;
    FidMap["10.1.4.2, 10.1.1.1, 8080, 49154, UDP"] = 1;
    FidMap["10.1.4.2, 10.1.1.1, 8080, 49155, UDP"] = 2;
    FidMap["10.1.5.2, 10.1.2.1, 8080, 49153, UDP"] = 3;
    FidMap["10.1.5.2, 10.1.2.1, 8080, 49154, UDP"] = 4;
    FidMap["10.1.5.2, 10.1.2.1, 8080, 49155, UDP"] = 5;

    bool hasPFabric = false;
    checkMakeTree(root, hasPFabric);

    ofstream OutStream;
    OutStream.open(argv[1]);

    printPushConvertTable(root, OutStream);

    OutStream.close();

    std::string inputFileName = argv[1];
    std::string outputFileName = getTraceFileName(inputFileName);
    tagPriority("../trace-udp/PcapTrace-6-3.pcap", outputFileName.c_str(), hasPFabric);
    outputFileName = getPFMapFileName(inputFileName);
    printPFMap("../trace-udp/PcapTrace-6-3.pcap", outputFileName.c_str());
    
    return 0;
}