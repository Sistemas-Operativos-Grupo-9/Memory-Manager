#include "buddy_tree.h"
#include "memory_manager.h"

static BuddyTree freeTree = {0};
static MemoryState _memoryState = {0};

// TODO: maybe move to utils/math
uint32_t previousPowerOfTwo(uint32_t x) {
	// https://stackoverflow.com/questions/2679815/previous-power-of-2
	if (x == 0) {
		return 0;
	}
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return x - (x >> 1);
}

size_t getApropiateHeapSize(size_t start, size_t end) {
	return previousPowerOfTwo(end - start);
}

void memoryManagerInitialize(size_t start, size_t end) {
	size_t heapSize = getApropiateHeapSize(start, end);
	treeInitialize(&freeTree, start, start + heapSize);
	_memoryState.heapStart = start;
	_memoryState.totalMemory = heapSize;
	_memoryState.fragmentsAmount = 1;
	_memoryState.usedMemory = 0;
}

BuddyNode *malloc_Rec(BuddyNode *node, size_t size) {
	size_t nodeSize = getMemorySpaceSize(node);
	if (size > nodeSize || getNodeState(node) == FULL) {
		return NULL;
	}
	if (size > nodeSize / 2) {
		// This node has appropiate size
		if (node->right != NULL || node->left != NULL ||
		    getNodeState(node) == FULL) {
			return NULL;
		}
		changeNodeState(node, FULL);
		return node;
	}
	// Look further down the tree
	BuddyNode *retNode = NULL;
	if (node->left == NULL) {
		// Create node and add it to the tree
		BuddyNode newNode =
		    newBuddyNode(getNodeStartAddress(node), getNodeEndAddress(node));
		retNode = addNodeToTree(&freeTree, newNode);
		node->left = retNode;

		changeNodeState(retNode, FULL);
	} else if (node->right == NULL) {
		// Create node and add it to the tree
		BuddyNode newNode =
		    newBuddyNode(getNodeStartAddress(node), getNodeEndAddress(node));
		retNode = addNodeToTree(&freeTree, newNode);
		node->right = retNode;

		changeNodeState(retNode, FULL);
	} else {
		retNode = malloc_Rec(node->left, size);
		if (retNode == NULL) {
			retNode = malloc_Rec(node->right, size);
		}
	}
	if (retNode != NULL) {
		updateNodeState(node);
	}
	return retNode;
}

void *ourMalloc(size_t byteCount) {
	if (byteCount == 0) {
		return NULL;
	}
	BuddyNode *allocatedSpace = malloc_Rec(freeTree.root, byteCount);
	if (allocatedSpace == NULL) {
		return NULL;
	}
	_memoryState.usedMemory += getMemorySpaceSize(allocatedSpace);
	return allocatedSpace->start;
}

void tryMerge(BuddyNode *node) {
	BuddyNode *nl = node->left;
	BuddyNode *nr = node->right;
	if ((nl != NULL && getNodeState(nl) != FREE) ||
	    (nr != NULL && getNodeState(nr) != FREE)) {
		// no merge possible
		return;
	}
	if (nl != NULL) {
		removeNodeFromTree(&freeTree, nr);
		node->right = NULL;
	}
	if (nr != NULL) {
		removeNodeFromTree(&freeTree, nl);
		node->left = NULL;
	}
	if (nl == NULL && nr == NULL) {
		changeNodeState(node, FREE);
	}
}

bool free_Rec(BuddyNode *node, size_t address) {
	size_t startAddress = getNodeStartAddress(node);
	size_t endAddress = getNodeEndAddress(node);
	if (address < startAddress || endAddress < address) {
		return false;
	}

	if (startAddress == address) {
		AllocState state = getNodeState(node);
		if (node->left != NULL) {
			bool freeResult = free_Rec(node->left, address);
			tryMerge(node);
			return freeResult;
		}
		if (node->right != NULL) {
			return false;
		}
		if (state == FREE) {
			return false;
		}
		// free
		changeNodeState(node, FREE);
		return true;
	}
	size_t middleAddress = (startAddress + endAddress) / 2;
	bool freeResult = false;
	if (address < middleAddress) {
		if (node->left == NULL) {
			return false;
		}
		freeResult = free_Rec(node->left, address);
	} else {
		if (node->right == NULL) {
			return false;
		}
		freeResult = free_Rec(node->right, address);
	}
	if (freeResult) {
		tryMerge(node);
	}
	return freeResult;
}

void ourFree(void *memPtr) { free_Rec(freeTree.root, memPtr); }

// TODO
void getMemoryState(MemoryState *mState);