
#include "buddy_tree.h"

BuddyNode newBuddyNode(size_t start, size_t end) {
	BuddyNode newNode;
	newNode.start = start;
	newNode.end = end;
	newNode.allocState = FREE;
	newNode.left = NULL;
	newNode.right = NULL;

	return newNode;
}
size_t getMemorySpaceSize(BuddyNode *node) { return node->end - node->start; }

AllocState getNodeState(BuddyNode *node) { return node->allocState; }

void setNodeState(BuddyNode *node, AllocState state) {
	node->allocState = state;
}

size_t getNodeStartAddress(BuddyNode *node) { return node->start; }

size_t getNodeEndAddress(BuddyNode *node) { return node->end; }

BuddyNode *addNodeToTree(BuddyTree *tree, BuddyNode node) {
	if (tree->nodeCount == MAX_NODES) {
		return NULL;
	}
	tree->nodes[tree->nodeCount] = node;
	return &tree->nodes[tree->nodeCount++];
}

void treeInitialize(BuddyTree *tree, size_t start, size_t end) {
	tree->nodeCount = 0;
	tree->root = createNode(tree, start, end);
}

bool isLeaf(BuddyNode *node) {
	return node->left == NULL && node->right == NULL;
}

void updateNodeState(BuddyNode *node) {
	if (isLeaf(node)) {
		return;
	}
	AllocState leftState = getNodeState(node->left);
	AllocState rightState = getNodeState(node->right);

	if (leftState == FULL && rightState == FULL) {
		setNodeState(node, FULL);
	} else {
		setNodeState(node, PARTIAL);
	}
}

//! copied from block_list
void buddy_listMoveSlice(BuddyNode list[MAX_NODES], size_t dest, size_t source,
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

/**
 * TODO:
 * change node management inside structure:
 * - delete a node by setting it to {0}
 * 		- check if a node is deleted by asking if start == NULL && end == NULL
 * - add new nodes in free spaces (holes), instead of at the end
 * - add a function to delete all children from a node, by deleting them from
 *   the structure and setting left and right to NULL
 *
 * all of this is to fix the current problem in which when deleting a node, some
 * other nodes inside the tree's list are shifted from their original positions,
 * but the pointers to those nodes on their parents cannot be updated.
 */

bool deleteNode(BuddyTree *tree, BuddyNode *node) {
	size_t nodeIndex = getNodeIndex(tree, node);
	if (nodeIndex == -1) {
		return false;
	}
	buddy_listMoveSlice(tree->nodes, nodeIndex, nodeIndex + 1,
	                    tree->nodeCount - nodeIndex - 1);
	tree->nodeCount--;
	return true;
}

BuddyNode *createNode(BuddyTree *tree, size_t start, size_t end) {
	BuddyNode newNode = newBuddyNode(start, end);
	return addNodeToTree(tree, newNode);
}
