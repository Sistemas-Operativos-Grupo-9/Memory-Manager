#include "buddy_tree.h"

BuddyNode newBuddyNode(size_t start, size_t end) {
	BuddyNode newNode;
	newNode.start = start;
	newNode.end = end;
	newNode.allocState = FULL;
	newNode.left = NULL;
	newNode.right = NULL;

	return newNode;
}
size_t getMemorySpaceSize(BuddyNode *node) { return node->end - node->start; }

AllocState getNodeState(BuddyNode *node) { return node->allocState; }

void changeNodeState(BuddyNode *node, AllocState state) {
	node->allocState = state;
}

size_t getNodeStartAddress(BuddyNode *node) { return node->start; }

size_t getNodeEndAddress(BuddyNode *node) { return node->end; }

BuddyNode *addNodeToTree(BuddyTree *tree, BuddyNode node) {
	if (tree->nodeCount == MAX_NODES) {
		return NULL;
	}
	tree->nodes[tree->nodeCount++] = node;
	return &tree->nodes[tree->nodeCount - 1];
}

void treeInitialize(BuddyTree *tree, size_t start, size_t end) {
	tree->nodeCount = 0;
	tree->root = addNodeToTree(tree, newBuddyNode(start, end));
}

bool isLeaf(BuddyNode *node) {
	return node->left == NULL && node->right == NULL;
}

/** Look for node descendants and update its state accordingly
 *	Leaves' states will not be changed. This means that an updated node will
 *never have it's state changed to FREE, this can only be done via direct access
 *(when the memory zone that it represents is freed to the heap)
 */
void updateNodeState(BuddyNode *node) {
	if (isLeaf(node)) {
		return;
	}
	AllocState leftState = getNodeState(node->left);
	AllocState rightState = getNodeState(node->right);

	if (leftState == FULL && rightState == FULL) {
		changeNodeState(node, FULL);
	} else {
		changeNodeState(node, PARTIAL);
	}
}

//! copied from block_list
void listMoveSlice(BuddyNode list[MAX_NODES], size_t dest, size_t source,
                   size_t size) {
	if (source > dest)
		for (int i = 0; i < size; i++)
			list[dest + i] = list[source + i];
	else if (source < dest)
		for (int i = size - 1; i >= 0; i--)
			list[dest + i] = list[source + i];
}

size_t getNodeIndex(BuddyTree *tree, BuddyNode *node) {
	for (size_t i = 0; i < tree->nodeCount; i++) {
		if (node == &tree->nodes[i]) {
			return i;
		}
	}
	return -1;
}

bool deleteNode(BuddyTree *tree, BuddyNode *node) {
	size_t nodeIndex = getNodeIndex(tree, node);
	if (nodeIndex == -1) {
		return false;
	}
	listMoveSlice(tree->nodes, nodeIndex, nodeIndex + 1,
	              tree->nodeCount - nodeIndex - 1);
	tree->nodeCount--;
	return true;
}

BuddyNode *createNode(BuddyTree *tree, size_t start, size_t end) {
	BuddyNode newNode = newBuddyNode(start, end);
	return addNodeToTree(tree, newNode);
}