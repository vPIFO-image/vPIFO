#pragma once

#include <vector>
#include <ostream>
#include <string>
#include "util.h"
#include "SchedStrategy.h"
#include "PerfInfo.h"

typedef struct TreeNode_* TreeNode;

struct TreeNode_{
    int nodeId;
    SchedStrategy strategy;
    PerfInfo minPerf;
    PerfInfo actualPerf;
    TreeNode father;
    std::vector<TreeNode> children;
};

TreeNode createTreeNode(SchedStrategy strategy, PerfInfo minPerf=nullptr);
TreeNode createTreeRoot(SchedStrategy strategy, PerfInfo actualPerf, PerfInfo minPerf=nullptr);
void attachNode(TreeNode node, TreeNode father);
void attachFlow(std::string flowId, TreeNode father);
void attachNode(TreeNode node, TreeNode father, int priority);
void attachFlow(std::string flowId, TreeNode father, int priority);
void attachNode(TreeNode node, TreeNode father, double weight);
void attachFlow(std::string flowId, TreeNode father, double weight);
void checkMakeTree(TreeNode root, bool& hasPFabric);

void printTreeNode(TreeNode node, std::ostream& os);
void printTree(TreeNode node, std::ostream& os);
void printPushConvertTable(TreeNode root, ostream& os);

