#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define LEFT_GREATER 1
#define RIGHT_GREATER 2

#define SORT 1
#define BINOMIAL 0

#define PARANOID 1

#if SORT
void swap(int* arr, int index1, int index2) {
	int temp = arr[index1];
	arr[index1] = arr[index2];
	arr[index2] = temp;
}

int partition(int* arr, int lo, int hi) {
	int pivot = lo;
	int h = pivot+1, k = hi;

	while (h < k) {
		while (h != k && COMPARE(arr[h], arr[pivot]) == RIGHT_GREATER) {
			h++;
		}
		while (h != k && COMPARE(arr[k], arr[pivot]) == LEFT_GREATER) {
			k--;
		}
		if (h < k) {
			swap(arr, h, k);
			h++;
		}
	}
	swap(arr, lo, h-1);
	return h-1;
}

void quickSort(int* arr, int lo, int hi, int n, int k) {
	if (lo < hi) {
		int pi = partition(arr, lo, hi);
#if PARANOID
		quickSort(arr, pi+1, hi, n, k);
		quickSort(arr, lo, pi, n, k);
		for (int i = lo; i < hi; i++) {
			if (COMPARE(arr[i], arr[i+1]) == LEFT_GREATER) {
				printf("lo: %d, hi: %d\n", lo, hi);
				printf("h index: %d at %d, k index: %d at %d\n", arr[i], i, arr[i+1], i+1);
			}
		}
#else
		if (hi >= n-k) {
			quickSort(arr, pi+1, hi, n, k);
			if (pi >= n - k) {
				quickSort(arr, lo, pi, n, k);
			}
		}
#endif
	}
}

int doalg(int n, int k, int* Best) {
	int arr[n];
	for (int i = 0; i < n; i++) {
		arr[i] = i+1;
	}
	quickSort(arr, 0, n-1, n, k);
	for (int i =0; i < k; i++) {
		Best[i] = arr[n-i-1];	
	}
	return 1;
}
#endif

#if BINOMIAL

#define ALLOC_LIST_NODE() (struct Node*)malloc(sizeof(struct Node))
#define ALLOC_BINO_NODE() (struct BinomialTreeNode*)malloc(sizeof(struct BinomialTreeNode))

struct Node {
	struct Node* next;
	struct Node* prev;
	struct BinomialTreeNode* indexNode;
};

struct BinomialTreeNode {
	size_t index;
	struct Node* childNodes;
};

struct BinomialTree {
	struct BinomialTreeNode* head;
};

void pushBack(struct Node** head, struct Node* newNode) {

	if (*head == NULL) {
		*head = newNode;
	} else {
		struct Node* current = *head;
		while (current->next != NULL) {
			current = current->next;
		}

		current->next = newNode;
		newNode->prev = current;
	}
}

void freeList(struct Node** head) {
	struct Node* current;

	if (*head != NULL) {
		current = (*head)->next;
		while (current != NULL) {
			(*head)->next = current->next;
			free(current);
		}

		free(*head);
		*head = NULL;
	}
}

void removeNode(struct Node** head, struct Node* toDelete) {
	struct Node* temp;

	if (*head == toDelete) {
		*head = (*head)->next;
		if (*head != NULL)
			(*head)->prev = NULL;
	} else if (toDelete->next == NULL) {
		toDelete->prev->next = NULL;
	} else {
		toDelete->prev->next = toDelete->next;
		toDelete->next->prev = toDelete->prev;
	}

	toDelete->next = NULL;
	toDelete->prev = NULL;
}

void deleteNode(struct Node** head, struct Node* toDelete) {
	removeNode(head, toDelete);
	free(toDelete);
}

void initBinoTreeNode(struct BinomialTreeNode* node, size_t newIndex) {
	int i;
	node->index = newIndex;
	node->childNodes = NULL;
}

void freeBinoTreeNode(struct BinomialTreeNode* node) {
	int i;

	if (node != NULL) {
		freeList(&node->childNodes);
	}
}

void freeBinoTree(struct BinomialTree* tree) {
	freeBinoTreeNode(tree->head);
	tree->head = NULL;
}

bool initListOfIndices(struct Node** head, int num) {
	size_t i;
	struct Node* current;
	*head = ALLOC_LIST_NODE();
	(*head)->indexNode = ALLOC_BINO_NODE();
	(*head)->prev = NULL;

	initBinoTreeNode((*head)->indexNode, 1);

	current = *head;

	for (i = 2; i <= num; ++i) {
		current->next = ALLOC_LIST_NODE();
		
		if (current->next == NULL) {
			return false;
		}

		current->next->prev = current;
		current = current->next;
		current->indexNode = ALLOC_BINO_NODE();
		initBinoTreeNode(current->indexNode, i);
	}

	current->next = NULL;
	return true;
}

void printBinomialTreeNode(struct BinomialTreeNode node) {
	int i;
	struct Node* curr;
	printf("%5zu: ", node.index);

	curr = node.childNodes;
	for (curr = node.childNodes ; curr != NULL; curr = curr->next) {
		printf("%zu ", curr->indexNode->index);
	}
	putchar('\n');

	for (curr = node.childNodes; curr != NULL; curr = curr->next) {
		printBinomialTreeNode(*curr->indexNode);
	}
}

void initBinomialTree(struct BinomialTree* t, int numOfIndices) {
	int nodesLeft = numOfIndices;
	int compResult, i;

	struct Node* head;
	struct Node* curr;
	struct Node* temp;

	if (!initListOfIndices(&head, numOfIndices)) {
		perror("Could not allocate linked list.\n");
		exit(1);
	}

	while (nodesLeft > 1) {
		curr = head;
		while (curr != NULL && curr->next != NULL) {
			compResult = COMPARE(curr->indexNode->index, curr->next->indexNode->index);
			
			if (compResult == LEFT_GREATER) {
				temp = curr->next;
			} else if (compResult == RIGHT_GREATER) {
				temp = curr;
				curr = curr->next;
			} else {
				perror("ERROR WITH COMPARISONS!!!\n");
				exit(1);
			}

			removeNode(&head, temp);
			pushBack(&curr->indexNode->childNodes, temp);

			curr = curr->next;
			--nodesLeft;
		}
	}

	t->head = head->indexNode;
	free(head);
	head = NULL;
}

void mergeChildren(struct BinomialTreeNode* newHead, struct BinomialTreeNode* oldHead) {
	struct Node* currNew = newHead->childNodes;
	struct Node* currOld = oldHead->childNodes;
	struct Node *left, *right;
	int compResult;

	while (currNew != NULL && currOld != NULL) {
		compResult = COMPARE(currNew->indexNode->index, currOld->indexNode->index);
		
		left = currNew;
		right = currOld;

		currNew = currNew->next;
		currOld = currOld->next;

		if (compResult == LEFT_GREATER) {
			removeNode(&oldHead->childNodes, right);
			pushBack(&left->indexNode->childNodes, right);
		} else if (compResult == RIGHT_GREATER) {
			removeNode(&newHead->childNodes, left);
			removeNode(&oldHead->childNodes, right);
			pushBack(&right->indexNode->childNodes, left);
			pushBack(&newHead->childNodes, right);
		} else {
			perror("PANIC!!!!\n");
			exit(1);
		}
	}

	while (currOld != NULL) {
		left = currOld;
		currOld = currOld->next;
		removeNode(&oldHead->childNodes, left);
		pushBack(&newHead->childNodes, left);
	}
}

int removeLargest(struct BinomialTree* t) {
	int i, j;
	int compResult;
	int result = t->head->index;
	
	struct BinomialTreeNode* newHead;
	struct BinomialTreeNode* oldHead;
	struct Node* maxChild;
	int maxChildIndex;

	struct Node* curr;
	
	if (t->head->childNodes == NULL) {
		return -1;
	}
	maxChild = t->head->childNodes;
	maxChildIndex = t->head->childNodes->indexNode->index;
	curr = maxChild->next;

	while (curr != NULL) {
		compResult = COMPARE(maxChildIndex, curr->indexNode->index);

		if (compResult == RIGHT_GREATER) {
			maxChild = curr;
			maxChildIndex = curr->indexNode->index;
		}

		curr = curr->next;
	}

	newHead = maxChild->indexNode;
	deleteNode(&t->head->childNodes, maxChild);

	mergeChildren(newHead, t->head);
	oldHead = t->head;
	t->head = newHead;

	return result;
}

int doalg(int n, int k, int* Best) {
	int i;
	struct BinomialTree tournament;

	initBinomialTree(&tournament, n);

	for (i = 0; i < k; ++i) {
		Best[i] = removeLargest(&tournament);
	}

	return true;
}

#endif