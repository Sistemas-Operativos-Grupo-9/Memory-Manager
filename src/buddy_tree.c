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

// Returns true if the pointed space of the nodes array is available for writing
// a new node
bool spaceIsAvailable(BuddyNode *space) {
	// Technically I should check if every field is 0, but this check is enough
	return space->start == 0 && space->end == 0;
}

BuddyNode *addNodeToTree(BuddyTree *tree, BuddyNode node) {
	for (size_t i = 0; i < MAX_NODES; i++) {
		if (spaceIsAvailable(tree->nodes + i)) {
			tree->nodes[i] = node; // copy the given node
			tree->nodeCount++;
			return &tree->nodes[i];
		}
	}
	return NULL;
}

void treeInitialize(BuddyTree *tree, size_t start, size_t end) {
	for (size_t i = 0; i < MAX_NODES; i++) {
		tree->nodes[i] = (BuddyNode){0};
	}
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

void deleteNode(BuddyTree *tree, BuddyNode *node) {
	node->start = 0;
	node->end = 0;
	node->allocState = 0;
	node->left = 0;
	node->right = 0;

	tree->nodeCount--;
}

void deleteChildren(BuddyTree *tree, BuddyNode *node) {
	if (node->left != NULL) {
		deleteNode(tree, node->left);
		node->left = NULL;
	}
	if (node->right != NULL) {
		deleteNode(tree, node->right);
		node->right = NULL;
	}
}

BuddyNode *createNode(BuddyTree *tree, size_t start, size_t end) {
	BuddyNode newNode = newBuddyNode(start, end);
	return addNodeToTree(tree, newNode);
}
