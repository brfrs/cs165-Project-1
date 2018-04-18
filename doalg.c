#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define LEFT_GREATER 1
#define RIGHT_GREATER 2

#define GET_NODE_INDEX(node) (node->heapNode->index)

void* Malloc(size_t size) {
	void* ptr = malloc(size);
	
	if (ptr == NULL) {
		perror("ERROR: could not maslloc data\n");
		exit(EXIT_FAILURE);
	}

	return ptr;
}

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
/*
 * Initializes a list struct with NULLs and 
 */
void initList(struct List* list) {
	list->first = NULL;
	list->last = NULL;
	list->count = 0;
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

void initBattleHeapNode(struct BattleHeapNode* node, int newIndex) {
	node->index = newIndex;
	initList(&node->listOfChildren);
}

void initBattleHeapNode(struct Node* node, int i) {
	node->heapNode = (struct BattleHeapNode*)Malloc(sizeof(struct BattleHeapNode));
	node->next = NULL;
	node->prev = NULL;

	initBattleHeapNode(node->heapNode, i);
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

bool fillListWithIndices(struct List* list, int num) {
	int i;
	struct Node* current;

	for (i = 1; i <= num; ++i) {
		current = (struct Node*)Malloc(sizeof(struct Node));
		initBattleHeapNode(current, i);
		pushBack(list, current);
	}
	return true;
}

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

void createBattleHeap(struct BattleHeapNode** t, int numOfIndices) {
	struct List winners;
	
	initList(&winners);
	fillListWithIndices(&winners, numOfIndices);
	
	runTournament(&winners);

	*t = winners.first->heapNode;
	freeList(&winners);
}

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
		exit(EXIT_FAILURE);
	}
	if (n < 1) {
		perror("Invalid input. n must be greater than 0.\n");
		exit(EXIT_FAILURE);
	}
	if (Best == NULL) {
		perror("Nullptr for Best.\n");
		exit(EXIT_FAILURE);
	}

	int i;
	struct BattleHeapNode* head;

	if (n == 1) {
		Best[0] = 1;
		return 1;
	}

	createBattleHeap(&head, n);

	for (i = 0; i < k-1; ++i) {
		Best[i] = removeLargest(&head);
	}
	Best[i] = head->index;

	freeBattleHeapNode(head);
	return true;
}