#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define LEFT_GREATER 1
#define RIGHT_GREATER 2

#define DOALG_ERR 0
#define DOALG_SUCC 1

#define GET_NODE_INDEX(node) (node->heapNode->index)

/* Struct definitions */

struct List {
	struct Node* first;
	struct Node* last;
	int count;
};

struct Node {
	struct Node* next;
	struct Node* prev;
	struct BattleHeapNode* heapNode;
};

struct BattleHeapNode {
	int index;
	struct List listOfChildren;
};

/* Misc. Prototypes */
void initBattleHeapNode(struct BattleHeapNode*, int);
void freeList(struct List*);
void freeBattleHeapNode(struct BattleHeapNode*);

/* Linked List Functions */

void initList(struct List* list) {
	list->first = NULL;
	list->last = NULL;
	list->count = 0;
}

bool initNode(struct Node* node, int i) {
	node->heapNode = (struct BattleHeapNode*)malloc(sizeof(struct BattleHeapNode));
	node->next = NULL;
	node->prev = NULL;
	
	if (node->heapNode == NULL) {
		perror("Allocation failed on battle heap node.\n");
		return false;
	}

	initBattleHeapNode(node->heapNode, i);

	return true;
}

void pushBack(struct List* list, struct Node* newNode) {
	if (list->first == NULL) {
		list->first = newNode;
	} else {
		list->last->next = newNode;
		newNode->prev = list->last;
	}
	list->last = newNode;
	++list->count;
}

void removeNode(struct List* list, struct Node* toRemove) {
	if (list->first == toRemove) {
		list->first = list->first->next;
		if (list->first != NULL)
			list->first->prev = NULL;
	} else if (list->last == toRemove) {
		toRemove->prev->next = NULL;
		list->last = toRemove->prev;
	} else {
		toRemove->prev->next = toRemove->next;
		toRemove->next->prev = toRemove->prev;
	}

	toRemove->next = NULL;
	toRemove->prev = NULL;
	--list->count;
}

void deleteNode(struct List* list, struct Node* toDelete) {
	removeNode(list, toDelete);
	free(toDelete);
}

void freeList(struct List* list) {
	while (list->first != NULL) {
        deleteNode(list, list->first);
	}
}

void freeHeapNodesInList(struct List* list) {
	struct Node* curr = list->first;
	while (curr != NULL) {
		freeBattleHeapNode(curr->heapNode);
		curr->heapNode = NULL;
		curr = curr->next;
	}
}

void transferNode(struct List* donorList, struct Node* nodeToTransfer, struct List* recipientList) {
	removeNode(donorList, nodeToTransfer);
	pushBack(recipientList, nodeToTransfer);
}

void moveList(struct List* donorList, struct List* recipientList) {
	if (recipientList->first != NULL) {
		freeList(recipientList);
	}

	recipientList->first = donorList->first;
	recipientList->last = donorList->last;
	recipientList->count = donorList->count;

	donorList->first = NULL;
	donorList->last = NULL;
	donorList->count = 0;
}

/* Battle Heap Functions */

void initBattleHeapNode(struct BattleHeapNode* node, int newIndex) {
	node->index = newIndex;
	initList(&node->listOfChildren);
}

void freeBattleHeapNode(struct BattleHeapNode* node) {
	struct Node* curr;
	for (curr = node->listOfChildren.first; curr != NULL; curr = curr->next) {
		freeBattleHeapNode(curr->heapNode);
	}

	if (node != NULL) {
		freeList(&node->listOfChildren);
	}
	free(node);
}

/* doalg Helper Functions */

/*
 * Creates a linked list filled with all 1 to n index values.
 */
bool fillListWithIndices(struct List* list, int num) {
	int i;
	struct Node* current;
	bool nodeAllocSuccess;

	for (i = 1; i <= num; ++i) {
		current = (struct Node*)malloc(sizeof(struct Node));

		if (current == NULL) {
			perror("Error allocating a list node.\n");
			return false;
		}

		nodeAllocSuccess = initNode(current, i);

		if (!nodeAllocSuccess) {
			perror("Error initializing a list node.\n");
			free (current);
			return false;
		}

		pushBack(list, current);
	}
	return true;
}

/*
 * Runs a tournament on a list of indices. Losing indices are removed from the list and
 * become children of the index value that beat them. Terminates when there
 * is only one node (index of the largest value) in the list.
 */
void runTournament(struct List* list) {
	struct Node* curr;
	struct Node* temp;
	int compResult;
	int nodesLeft = list->count;

	while (list->count > 1) {
		curr = list->first;

		while (curr != NULL && curr->next != NULL) {
			compResult = COMPARE(GET_NODE_INDEX(curr), GET_NODE_INDEX(curr->next));

			if (compResult == LEFT_GREATER) {
				temp = curr->next;
			} else if (compResult == RIGHT_GREATER) {
				temp = curr;
				curr = curr->next;
			} else {
				perror("ERROR WITH COMPARISONS!!!\n");
				exit(EXIT_FAILURE);
			}

			transferNode(list, temp, &curr->heapNode->listOfChildren);
			curr = curr->next;		
		}
	}
}

/*
 * Creates a battle heap for this given private array and sets the root to t.
 */
bool createBattleHeap(struct BattleHeapNode** t, int numOfIndices) {
	struct List winners;
	bool listFilledSuccessfully;

	initList(&winners);
	listFilledSuccessfully = fillListWithIndices(&winners, numOfIndices);
	
	if (!listFilledSuccessfully) {
		freeHeapNodesInList(&winners);
		freeList(&winners);
		return false;
	}

	runTournament(&winners);

	*t = winners.first->heapNode;
	freeList(&winners);
	return true;
}

/*
 * Removes the largest element in the battle heap. Then
 * finds the next largest to be the root. 
 */
int removeLargest(struct BattleHeapNode** t) {
	struct BattleHeapNode* oldRoot;
	struct List winners;
	int result = (*t)->index;

	initList(&winners);
	oldRoot = *t;

	moveList(&oldRoot->listOfChildren, &winners);

	runTournament(&winners);

	*t = winners.first->heapNode;

	freeList(&winners);
	free(oldRoot);
	
	return result;
}

int doalg(int n, int k, int* Best) {
	//Invalid input checking
	if (k > n || k < 1) {
		perror("Invalid input. k must be less than n and greater than 0.\n");
		return DOALG_ERR;
	}
	if (n < 1) {
		perror("Invalid input. n must be greater than 0.\n");
		return DOALG_ERR;
	}
	if (Best == NULL) {
		perror("Nullptr for Best.\n");
		return DOALG_ERR;
	}

	int i;
	struct BattleHeapNode* root;
	bool battleHeapCreatedSuccessfully;

	if (n == 1) {
		Best[0] = 1;
		return DOALG_SUCC;
	}

	battleHeapCreatedSuccessfully = createBattleHeap(&root, n);

	if (!battleHeapCreatedSuccessfully) {
		return DOALG_ERR;
	}

	for (i = 0; i < k-1; ++i) {
		Best[i] = removeLargest(&root);
	}
	Best[i] = root->index; // Do not need to for the k-th element.

	freeBattleHeapNode(root);
	return DOALG_SUCC;
}