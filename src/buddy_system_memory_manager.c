#ifdef BUDDY_MM

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
// TODO: maybe move to utils/math
// Range is inclusive
bool insideRange(size_t value, size_t min, size_t max) {
	return min <= value && value <= max;
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
	AllocState nodeState = getNodeState(node);

	if (size > nodeSize || nodeState == FULL) {
		return NULL;
	}

	if (size > nodeSize / 2 || nodeSize == MIN_SIZE) {
		// This node has appropiate size
		if (nodeState != FREE) {
			return NULL;
		}

		// allocate space
		setNodeState(node, FULL);
		return node;
	}

	BuddyNode *allocatedNode = NULL;
	if (nodeState == FREE) {
		size_t startAddress = getNodeStartAddress(node);
		size_t endAddress = getNodeEndAddress(node);
		size_t middleAddress = (startAddress + endAddress) / 2;

		node->left = createNode(&freeTree, startAddress, middleAddress);
		node->right = createNode(&freeTree, middleAddress, endAddress);

		// allocate left
		allocatedNode = malloc_Rec(node->left, size);
	} else {
		// nodeState == PARTIAL
		allocatedNode = malloc_Rec(node->left, size);
		if (allocatedNode == NULL) {
			allocatedNode = malloc_Rec(node->right, size);
		}
	}
	if (allocatedNode != NULL) {
		updateNodeState(node);
	}
	return allocatedNode;
}

void *ourMalloc(size_t byteCount) {
	if (byteCount == 0) {
		return NULL;
	}
	BuddyNode *allocatedSpace = malloc_Rec(freeTree.root, byteCount);
	if (allocatedSpace == NULL) {
		return NULL;
	}
	return (void *)allocatedSpace->start;
}

bool tryMerge(BuddyNode *node) {
	if (node->left == NULL || node->right == NULL) {
		return false;
	}
	AllocState leftState = getNodeState(node->left);
	AllocState rightState = getNodeState(node->right);

	if (leftState == FREE && rightState == FREE) {
		BuddyNode leftNode = *node->left;
		BuddyNode rightNode = *node->right;

		bool ok = deleteNode(&freeTree, node->left);
		if (!ok) {
			return false;
		}
		ok = deleteNode(&freeTree, node->right);
		if (!ok) {
			// deletion failed, restore left node
			node->left = createNode(&freeTree, leftNode.start, leftNode.end);
			setNodeState(node->left, getNodeState(&leftNode));
			return false;
		}
		node->left = NULL;
		node->right = NULL;
		return true;
	}
	return false;
}

bool free_Rec(BuddyNode *node, size_t address) {
	size_t startAddress = getNodeStartAddress(node);
	size_t endAddress = getNodeEndAddress(node);
	if (!insideRange(address, startAddress, endAddress)) {
		return false;
	}

	if (startAddress == address) {
		AllocState state = getNodeState(node);
		if (isLeaf(node) && state == FULL) {
			// free
			setNodeState(node, FREE);
			return true;
		}

		if (node->left != NULL) {
			bool freed = free_Rec(node->left, address);
			if (freed) {
				updateNodeState(node);
				if (tryMerge(node)) {
					setNodeState(node, FREE);
				}
			}
			return freed;
		}
		return false;
	}

	if (isLeaf(node)) {
		return false;
	}

	// search in children for the pointed space
	size_t middleAddress = (startAddress + endAddress) / 2;
	bool freed = false;
	if (address < middleAddress) {
		freed = free_Rec(node->left, address);
	} else {
		freed = free_Rec(node->right, address);
	}
	if (freed) {
		updateNodeState(node);
		if (tryMerge(node)) {
			setNodeState(node, FREE);
		}
	}
	return freed;
}

void ourFree(void *memPtr) { free_Rec(freeTree.root, (size_t)memPtr); }

size_t getAmountOfFragments_Rec(BuddyNode *node) {
	if (isLeaf(node)) {
		if (getNodeState(node) == FREE) {
			return 1;
		}
		return 0;
	}
	return getAmountOfFragments_Rec(node->left) +
	       getAmountOfFragments_Rec(node->right);
}

size_t getAmountOfAllocatedSpace_Rec(BuddyNode *node) {
	AllocState nodeState = getNodeState(node);
	if (nodeState == FREE) {
		return 0;
	}
	if (nodeState == FULL) {
		return getMemorySpaceSize(node);
	}
	return getAmountOfAllocatedSpace_Rec(node->left) +
	       getAmountOfAllocatedSpace_Rec(node->right);
}

void getMemoryState(MemoryState *mState) {
	mState->totalMemory = _memoryState.totalMemory;
	mState->heapStart = _memoryState.heapStart;
	mState->usedMemory = getAmountOfAllocatedSpace_Rec(freeTree.root);
	mState->fragmentsAmount = getAmountOfFragments_Rec(freeTree.root);
}

#endif