#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_NODES 100
#define MIN_SIZE 32

typedef enum allocState { FREE, PARTIAL, FULL } AllocState;

typedef struct buddyNode {
	size_t start;
	size_t end;
	AllocState allocState;
	BuddyNode *left;
	BuddyNode *right;
} BuddyNode;

typedef struct buddyTree {
	BuddyNode nodes[MAX_NODES];
	size_t nodeCount;
	BuddyNode *root;
} BuddyTree;

BuddyNode newBuddyNode(size_t start, size_t end);

size_t getMemorySpaceSize(BuddyNode *node);

AllocState getNodeState(BuddyNode *node);
void updateNodeState(BuddyNode *node);
void changeNodeState(BuddyNode *node, AllocState state);

size_t getNodeStartAddress(BuddyNode *node);
size_t getNodeEndAddress(BuddyNode *node);

bool isLeaf(BuddyNode *node);

/////////////////

void treeInitialize(BuddyTree *tree, size_t start, size_t end);

BuddyNode *createNode(BuddyTree *tree, size_t start, size_t end);
bool deleteNode(BuddyTree *tree, BuddyNode *node);
