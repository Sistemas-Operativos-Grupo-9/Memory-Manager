#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_NODES 255 // for a mm data size of 8 KiB
#define MIN_SIZE 64

typedef enum allocState { FREE, PARTIAL, FULL } AllocState;

typedef struct buddyNode_s {
	size_t start;
	size_t end;
	AllocState allocState;
	struct buddyNode_s *left;
	struct buddyNode_s *right;
} BuddyNode;

typedef struct buddyTree_s {
	BuddyNode nodes[MAX_NODES];
	size_t nodeCount;
	BuddyNode *root;
} BuddyTree;

size_t getMemorySpaceSize(BuddyNode *node);

AllocState getNodeState(BuddyNode *node);
/** Looks for node descendants and updates its state accordingly
 *	Leafs' states will not be changed. This means that an updated node will
 *never have it's state changed to FREE, this can only be done via direct access
 *(when the memory zone that it represents is freed to the heap)
 */
void updateNodeState(BuddyNode *node);
void setNodeState(BuddyNode *node, AllocState state);

size_t getNodeStartAddress(BuddyNode *node);
size_t getNodeEndAddress(BuddyNode *node);

bool isLeaf(BuddyNode *node);

/////////////////

void treeInitialize(BuddyTree *tree, size_t start, size_t end);

//! decisión de diseño:
// no hay función para agregar nodos al árbol porque todos los
// nodos que se agregan son añadidos directamente (modificando el valor de
// node->left). Esto es para hacer más rápido y directo el proceso de añadir un
// nodo, ya que de otra forma debería añadirse navegando recursivamente desde la
// raíz, algo que puede llegar a ser bastante costoso.

// Creates a node and adds it to the tree's internal list.
// This function does not add the node to the tree structure (i.e. as a child of
// some other node), that has to be done manually
BuddyNode *createNode(BuddyTree *tree, size_t start, size_t end);

// Deletes node from the tree's internal list, it does not remove it from the
// tree data structure
void deleteNode(BuddyTree *tree, BuddyNode *node);

// Deletes both children from node and sets them to NULL
void deleteChildren(BuddyTree *tree, BuddyNode *node);