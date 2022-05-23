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

// AllocState computeNodeState(BuddyNode *node) {
// 	if (node->left == NULL && node->right == NULL) {
// 		if (getNodeState(node) == FULL) {
// 			return FULL;
// 		}
// 		return FREE;
// 	}
// 	if (node->left == NULL || node->right == NULL) {
// 		return PARTIAL;
// 	}
// 	if (computeNodeState(node->left) == FULL &&
// 	    computeNodeState(node->right) == FULL) {
// 		return FULL;
// 	}
// 	return PARTIAL;
// }

// BuddyNode *getNodeByAddress(BuddyTree *tree, size_t address) {
// 	BuddyNode *current = tree->root;
// 	while (current != NULL) {
// 		size_t currentStartAddress = getNodeStartAddress(current);
// 		size_t currentEndAddress = getNodeEndAddress(current);
// 		if (address < currentStartAddress || currentEndAddress < address) {
// 			return NULL;
// 		}
// 		if (currentStartAddress == address) {
// 			if (current->left == NULL) {
// 				return current;
// 			}
// 			current = current->left;
// 		} else {
// 			size_t middleAddress =
// 			    (currentStartAddress + currentEndAddress) / 2;
// 			if (address < middleAddress) {
// 				current = current->left;
// 			} else {
// 				current = current->right;
// 			}
// 		}
// 	}
// 	return NULL;
// }
// BuddyNode *getNodeBySpace(BuddyTree *tree, size_t requiredSpace) {
// 	if (requiredSpace > getMemorySpaceSize(tree->root)) {
// 		return NULL;
// 	}

// 	BuddyNode *current = tree->root;
// 	while (current != NULL) {
// 		size_t currentSpace = getMemorySpaceSize(current);
// 		if (requiredSpace > currentSpace / 2) {
// 			return current;
// 		}
// 		if (current->left != NULL) {
// 			current = current->left;
// 		} else {
// 			current = current->right;
// 		}
// 	}
// 	return NULL;
// }

// Look for node descendants and update its state accordingly
void updateNodeState(BuddyNode *node) {
	if (node->left == NULL && node->right == NULL) {
		node->allocState = FREE;
		return;
	}
	if (node->left != NULL && node->right != NULL &&
	    getNodeState(node->left) == FULL && getNodeState(node->right) == FULL) {
		return FULL;
	}
	return PARTIAL;
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

void removeNodeFromTree(BuddyTree *tree, BuddyNode *node) {
	size_t nodeIndex = getNodeIndex(tree, node);
	if (nodeIndex == -1) {
		return;
	}
	listMoveSlice(tree->nodes, nodeIndex, nodeIndex + 1,
	              tree->nodeCount - nodeIndex - 1);
	tree->nodeCount--;
}
