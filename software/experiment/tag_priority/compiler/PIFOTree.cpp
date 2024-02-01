#include <vector>
#include <iostream>
#include <ostream>
#include <cassert>
#include <map>
#include "PIFOTree.h"
#include "PerfInfo.h"
#include "SchedStrategy.h"

using namespace std;

static int nodeId;

map<string, TreeNode> flowNodeMap;

TreeNode createTreeNode(SchedStrategy strategy, PerfInfo minPerf){
    TreeNode node = new TreeNode_;
    node->nodeId = nodeId;
    nodeId += 10;
    node->strategy = strategy;
    node->minPerf = minPerf;
    node->actualPerf = new PerfInfo_;
    node->father = nullptr;
    node->children.clear();
    return node;
}

TreeNode createTreeRoot(SchedStrategy strategy, PerfInfo actualPerf, PerfInfo minPerf){
    TreeNode node = new TreeNode_;
    node->nodeId = nodeId;
    nodeId += 10;
    node->strategy = strategy;
    node->minPerf = minPerf;
    node->actualPerf = actualPerf;
    node->father = nullptr;
    node->children.clear();
    return node;
}

void attachFlow(string flowId, TreeNode father){
    if(father == nullptr) return;
    flowNodeMap[flowId] = father;
    switch(father->strategy->type){
    case UNKNOWNTYPE:{
        break;
    }
    case PFABRICTYPE:{
        break;
    }
    case SPTYPE:{
        assert(0);
        break;
    }
    case WFQTYPE:{
        assert(0);
        break;
    }
    }
}

void attachNode(TreeNode node, TreeNode father){
    if(father == nullptr) return;
    node->father = father;
    father->children.emplace_back(node);
    switch(father->strategy->type){
    case UNKNOWNTYPE:{
        break;
    }
    case PFABRICTYPE:{
        break;
    }
    case SPTYPE:{
        assert(0);
        break;
    }
    case WFQTYPE:{
        assert(0);
        break;
    }
    }
}

void attachFlow(string flowId, TreeNode father, int priority){
    if(father == nullptr) return;
    flowNodeMap[flowId] = father;
    switch(father->strategy->type){
    case UNKNOWNTYPE:{
        assert(0);
        break;
    }
    case PFABRICTYPE:{
        assert(0);
        break;
    }
    case SPTYPE:{
        StrategySP strategySP = father->strategy->u.SP;
        strategySP->priorityTable[flowId] = priority;
        break;
    }
    case WFQTYPE:{
        assert(0);
        break;
    }
    }
}

void attachNode(TreeNode node, TreeNode father, int priority){
    if(father == nullptr) return;
    node->father = father;
    father->children.emplace_back(node);
    switch(father->strategy->type){
    case UNKNOWNTYPE:{
        assert(0);
        break;
    }
    case PFABRICTYPE:{
        assert(0);
        break;
    }
    case SPTYPE:{
        StrategySP strategySP = father->strategy->u.SP;
        strategySP->priorityTable[to_string(node->nodeId)] = priority;
        break;
    }
    case WFQTYPE:{
        assert(0);
        break;
    }
    }
}

void attachFlow(string flowId, TreeNode father, double weight){
    if(father == nullptr) return;
    flowNodeMap[flowId] = father;
    switch(father->strategy->type){
    case UNKNOWNTYPE:{
        assert(0);
        break;
    }
    case PFABRICTYPE:{
        assert(0);
        break;
    }
    case SPTYPE:{
        assert(0);
        break;
    }
    case WFQTYPE:{
        StrategyWFQ strategyWFQ = father->strategy->u.WFQ;
        strategyWFQ->weightTable[flowId] = weight;
        break;
    }
    }
}

void attachNode(TreeNode node, TreeNode father, double weight){
    if(father == nullptr) return;
    node->father = father;
    father->children.emplace_back(node);
    switch(father->strategy->type){
    case UNKNOWNTYPE:{
        assert(0);
        break;
    }
    case PFABRICTYPE:{
        assert(0);
        break;
    }
    case SPTYPE:{
        assert(0);
        break;
    }
    case WFQTYPE:{
        StrategyWFQ strategyWFQ = father->strategy->u.WFQ;
        strategyWFQ->weightTable[to_string(node->nodeId)] = weight;
        break;
    }
    }
}

void collectLeafNode(vector<TreeNode>& leafNodes, TreeNode node){
    if(node->children.empty()){
        leafNodes.emplace_back(node);
    }
    for(auto &it : node->children){
        collectLeafNode(leafNodes, it);
    }
}

void upDeliverMinPerfToRoot(TreeNode leafNode, TreeNode root){
    if(!leafNode || !(leafNode->minPerf)) return;
    while(leafNode->father && leafNode->nodeId != root->nodeId){
        if(!(leafNode->father->minPerf)){
            leafNode->father->minPerf = new PerfInfo_;
        }
        sumPerfInfo(leafNode->minPerf, leafNode->father->minPerf);
        leafNode = leafNode->father;
    }
}

// void downDeliverMarginPerfToLeaf(TreeNode node){
//     if(node->children.empty()) return;
//     PerfInfo_ marginPerf_, dividedPerfInfo_;
//     getMarginPerf(node->actualPerf, node->minPerf, &marginPerf_);
//     dividePerfEqually(&marginPerf_, node->children.size(), &dividedPerfInfo_);
//     for(auto &it : node->children){
//         sumPerfInfo(&dividedPerfInfo_, it->actualPerf);
//         sumPerfInfo(it->minPerf, it->actualPerf);
//         downDeliverMarginPerfToLeaf(it);
//     }
// }

void checkMakeTree(TreeNode root, bool& hasPFabric){
    hasPFabric = false;
    vector<TreeNode> leafNodes;
    collectLeafNode(leafNodes, root);

    for(auto &node : leafNodes){
        upDeliverMinPerfToRoot(node, root);
    }

    if(!checkPerfInfoMeet(root->actualPerf, root->minPerf)){
        cout << "Error: The minimum requirement exceeds the actual capacity!" << endl;
    }
    // else{
    //     downDeliverMarginPerfToLeaf(root);
    // }

    for(auto &node : leafNodes){
        if(node->strategy->type == PFABRICTYPE){
            hasPFabric = true;
        }
    }
}

void printTreeNode(TreeNode node, ostream& os){
    os << "Node " << node->nodeId << " :\n";
    os << "SchedStrategy:\n";
    if(node->strategy){
        printSchedStrategy(node->strategy, os);
    }else{
        os << "\tnil\n";
    }
    os << "minPerf:\n";
    if(node->minPerf){
        printPerfInfo(node->minPerf, os);
    }else{
        os << "\tnil\n";
    }
    os << "actualPerf:\n";
    if(node->actualPerf){
        printPerfInfo(node->actualPerf, os);
    }else{
        os << "\tnil\n";
    }
    os << "father: ";
    if(node->father){
        os << node->father->nodeId;
    }else{
        os << "nil";
    }
    os << "\n";
    os << "children: ";
    if(!node->children.empty()){
        for(auto &it : node->children){
            os << it->nodeId << " ";
        }
    }else{
        os << "nil";
    }
    os << "\n\n";
}

void printTree(TreeNode node, ostream& os){
    if(!node) return;
    printTreeNode(node, os);
    for(auto &it : node->children){
        printTree(it, os);
    }
}

void getUpPath(TreeNode leafNode, map<int, vector<TreeNode>>& leafNodePathMap){
    TreeNode node = leafNode;
    while(node){
        leafNodePathMap[leafNode->nodeId].emplace_back(node);
        node = node->father;
    }
}

void printPushConvertTable(TreeNode root, ostream& os){
    vector<TreeNode> leafNodes;
    collectLeafNode(leafNodes, root);

    // node->nodeId to path
    map<int, vector<TreeNode>> leafNodePathMap;

    for(auto &node : leafNodes){
        getUpPath(node, leafNodePathMap);
    }

    for(auto &it : leafNodePathMap){
        auto leafNodeId = it.first;
        os << leafNodeId << ": ";
        auto path = it.second;
        for (auto node = path.begin(); node != path.end(); ++node) {
            os << (*node)->nodeId;
            if (node != std::prev(path.end())) {
                os << "->";
            }
        }
        os << "\n";
    }
}