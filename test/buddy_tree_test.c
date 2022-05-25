#include "buddy_tree_test.h"
#include "buddy_tree.h"

#define DEFAULT_START_ADDRESS 0
#define DEFAULT_TREE_SIZE 64

// Basic Test Template:
// ---------------------------------
// void testSomething(CuTest *tc) {
// 	size_t start = DEFAULT_START_ADDRESS;
// 	size_t end = start + DEFAULT_TREE_SIZE;
// 	BuddyTree buddyTree;
// 	BuddyTree *treePtr = &buddyTree;
// 	treeInitialize(treePtr, start, end);
// }

// Local Utils
// -----------------------------------------------------------------------
void testCompareNode(CuTest *tc, BuddyNode *refNode, BuddyNode *testedNode) {
	CuAssertIntEquals(tc, getNodeStartAddress(refNode),
	                  getNodeStartAddress(testedNode));
	CuAssertIntEquals(tc, getNodeEndAddress(refNode),
	                  getNodeEndAddress(testedNode));
	CuAssertTrue(tc, getNodeState(refNode) == getNodeState(testedNode));
	CuAssertPtrEquals(tc, refNode->left, testedNode->left);
	CuAssertPtrEquals(tc, refNode->right, testedNode->right);
}

// Tests
// -----------------------------------------------------------------------

void testCreateNode(CuTest *tc) {
	size_t start = DEFAULT_START_ADDRESS;
	size_t end = start + DEFAULT_TREE_SIZE;
	BuddyTree buddyTree;
	BuddyTree *treePtr = &buddyTree;
	treeInitialize(treePtr, start, end);

	BuddyNode refNode = {
	    .start = 0, .end = 0, .allocState = FREE, .left = NULL, .right = NULL};

	BuddyNode *node = createNode(treePtr, 0, 15);
	refNode.end = 15;
	testCompareNode(tc, &refNode, node);

	node = createNode(treePtr, 1, 43);
	refNode.start = 1;
	refNode.end = 43;
	testCompareNode(tc, &refNode, node);

	node = createNode(treePtr, end - 8, end);
	refNode.start = end - 8;
	refNode.end = end;
	testCompareNode(tc, &refNode, node);

	// 3 nodes added + the root
	CuAssertIntEquals(tc, 3 + 1, buddyTree.nodeCount);
}

void testModifyingNodeState(CuTest *tc) {
	size_t start = DEFAULT_START_ADDRESS;
	size_t end = start + DEFAULT_TREE_SIZE;
	BuddyTree buddyTree;
	BuddyTree *treePtr = &buddyTree;
	treeInitialize(treePtr, start, end);

	BuddyNode *root = buddyTree.root;
	CuAssertTrue(tc, getNodeState(root) == FREE);

	setNodeState(root, FULL);
	CuAssertTrue(tc, getNodeState(root) == FULL);

	BuddyNode *leftChild = createNode(treePtr, 0, end / 2);
	setNodeState(leftChild, FULL);
	CuAssertTrue(tc, getNodeState(leftChild) == FULL);
	BuddyNode *rightChild = createNode(treePtr, end / 2, end);
	root->left = leftChild;
	root->right = rightChild;
	updateNodeState(root);
	// left child is FULL while right child is FREE, so root should be now
	// PARTIAL
	CuAssertTrue(tc, getNodeState(root) == PARTIAL);

	setNodeState(rightChild, FULL);
	updateNodeState(root);
	// now both children are FULL, so root should be FULL
	CuAssertTrue(tc, getNodeState(root) == FULL);
}

void testDeleteNode(CuTest *tc) {
	size_t start = DEFAULT_START_ADDRESS;
	size_t end = start + DEFAULT_TREE_SIZE;
	BuddyTree buddyTree;
	BuddyTree *treePtr = &buddyTree;
	treeInitialize(treePtr, start, end);

	size_t nodesToCreate = getUniform(5) + 5;
	BuddyNode *createdNodes[nodesToCreate];
	for (size_t i = 0; i < nodesToCreate; i++) {
		createdNodes[i] = createNode(treePtr, 0, 2);
	}

	CuAssertIntEquals(tc, nodesToCreate + 1, buddyTree.nodeCount);

	size_t nodesToDelete = getUniform(nodesToCreate - 1) + 1;
	for (size_t i = 0; i < nodesToDelete; i++) {
		bool deleteDone = deleteNode(treePtr, createdNodes[i]);
		CuAssertTrue(tc, deleteDone);
	}

	CuAssertIntEquals(tc, nodesToCreate + 1 - nodesToDelete,
	                  buddyTree.nodeCount);
}

void buddy_integratedTest(CuTest *tc) {}

// Suite
// -----------------------------------------------------------------------
CuSuite *getBuddyTreeSuite() {
	CuSuite *suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testCreateNode);
	SUITE_ADD_TEST(suite, testModifyingNodeState);
	SUITE_ADD_TEST(suite, testDeleteNode);
	SUITE_ADD_TEST(suite, buddy_integratedTest);

	return suite;
}
