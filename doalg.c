#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define LEFT_GREATER 1
#define RIGHT_GREATER 2

#define BINOMIAL 1
#define PARANOID 0

#ifdef SORT

void swap(int* arr, int index1, int index2) {
	int temp = arr[index1];
	arr[index1] = arr[index2];
	arr[index2] = temp;
}

int partition(int* arr, int lo, int hi) {
	int pivot = lo;
	int h = pivot+1, k = hi;

	if (h < k) {
		while (h < k) {
			while (h < k && (COMPARE(arr[h], arr[pivot]) == RIGHT_GREATER)) {
				h++;
			}
			while (h <= k && (COMPARE(arr[k], arr[pivot]) == LEFT_GREATER)) {
				k--;
			}
			if (h < k) {
				swap(arr, h, k);
			}
		}
		swap(arr, pivot, k);
		return k;
	} else if (COMPARE(arr[h], arr[pivot]) == RIGHT_GREATER) {
		swap(arr, pivot, k);
	}
	return k;
}

void quickSort(int* arr, int lo, int hi, int n, int k) {
	if (lo < hi) {
		int pi = partition(arr, lo, hi);
#if PARANOID
		quickSort(arr, pi+1, hi, n, k);
		quickSort(arr, lo, pi-1, n, k);
		for (int i = lo; i < hi; i++) {
			if (COMPARE(arr[i], arr[i+1]) == LEFT_GREATER) {
				printf("lo: %d, hi: %d, pi: %d\n", lo, hi, pi);
				printf("h index: %d at %d, k index: %d at %d\n", arr[i], i, arr[i+1], i+1);
			}
		}
#else
		if (hi >= n-k) {
			quickSort(arr, pi+1, hi, n, k);
			if (pi-1 >= n - k) {
				quickSort(arr, lo, pi-1, n, k);
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

#elif BINOMIAL

#define ALLOC_LIST_NODE() (struct Node*)malloc(sizeof(struct Node))
#define ALLOC_BINO_NODE() (struct BinomialTreeNode*)malloc(sizeof(struct BinomialTreeNode))
#define GET_NODE_INDEX(node) (node->indexNode->index)

struct Node {
	struct Node* next;
	struct Node* prev;
	struct BinomialTreeNode* indexNode;
};

struct BinomialTreeNode {
	size_t index;
	struct Node* childNodes;
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

	newNode->next = NULL;
}

void printList(struct Node* head) {
	struct Node* curr;
	for (curr = head; curr != NULL; curr = curr->next) {
		printf("%zu ", GET_NODE_INDEX(curr));
	}
	putchar('\n');
}

int countElems(struct Node* head) {
	int count = 0;
	struct Node* curr = head;
	while (curr != NULL) {
		++count;
		curr = curr->next;
	}
	return count;
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

void transferNode(struct Node** donorHead, struct Node* nodeToTransfer, struct Node** recipientHead) {
	removeNode(donorHead, nodeToTransfer);
	pushBack(recipientHead, nodeToTransfer);
}

void freeList(struct Node** head) {
	struct Node* current = NULL;

	while (*head != NULL) {
        deleteNode(head, *head);
	}
}

void initBinoTreeNode(struct BinomialTreeNode* node, size_t newIndex) {
	int i;
	node->index = newIndex;
	node->childNodes = NULL;
}

void freeBinoTreeNode(struct BinomialTreeNode* node) {
	struct Node* curr;
	for (curr = node->childNodes; curr != NULL; curr = curr->next) {
		freeBinoTreeNode(curr->indexNode);
	}

	if (node != NULL) {
		freeList(&node->childNodes);
	}
	free(node);
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
		printf("%zu ", GET_NODE_INDEX(curr));
	}
	putchar('\n');

	for (curr = node.childNodes; curr != NULL; curr = curr->next) {
		printBinomialTreeNode(*curr->indexNode);
	}
}

void initBinomialTree(struct BinomialTreeNode** t, int numOfIndices) {
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
			compResult = COMPARE(GET_NODE_INDEX(curr), GET_NODE_INDEX(curr->next));
			
			if (compResult == LEFT_GREATER) {
				temp = curr->next;
			} else if (compResult == RIGHT_GREATER) {
				temp = curr;
				curr = curr->next;
			} else {
				perror("ERROR WITH COMPARISONS!!!\n");
				exit(1);
			}

			transferNode(&head, temp, &curr->indexNode->childNodes);

			curr = curr->next;
			--nodesLeft;
		}
	}

	*t = head->indexNode;
	free(head);
	head = NULL;
}

void mergeChildren(struct BinomialTreeNode* newHead, struct BinomialTreeNode* oldHead) {
	struct Node* currNew = newHead->childNodes;
	struct Node* currOld = oldHead->childNodes;
	struct Node *left, *right;
	int compResult;

	while (currNew != NULL && currOld != NULL) {
		compResult = COMPARE(GET_NODE_INDEX(currNew), GET_NODE_INDEX(currOld));
		
		left = currNew;
		right = currOld;

		currNew = currNew->next;
		currOld = currOld->next;

		if (compResult == LEFT_GREATER) {
			transferNode(&oldHead->childNodes, right, &left->indexNode->childNodes);
		} else if (compResult == RIGHT_GREATER) {
			transferNode(&newHead->childNodes, left, &right->indexNode->childNodes);
			transferNode(&oldHead->childNodes, right, &newHead->childNodes);
		} else {
			perror("PANIC!!!!\n");
			exit(1);
		}
	}

	while (currOld != NULL) {
		left = currOld;
		currOld = currOld->next;
		transferNode(&oldHead->childNodes, left, &newHead->childNodes);
	}
}

int removeLargest(struct BinomialTreeNode** t) {
	/*
	int compResult;
	int result = (*t)->index;
	
	struct BinomialTreeNode* newHead;
	struct BinomialTreeNode* oldHead;
	struct Node* maxChild;
	int maxChildIndex;

	struct Node* curr;

	if ((*t)->childNodes == NULL) {
		return -1;
	}

	printList((*t)->childNodes);

	maxChild = (*t)->childNodes;
	maxChildIndex = GET_NODE_INDEX((*t)->childNodes);
	curr = maxChild->next;

	while (curr != NULL) {
		compResult = COMPARE(maxChildIndex, GET_NODE_INDEX(curr));

		if (compResult == RIGHT_GREATER) {
			maxChild = curr;
			maxChildIndex = GET_NODE_INDEX(curr);
		}

		curr = curr->next;
	}

	newHead = maxChild->indexNode;
	deleteNode(&((*t)->childNodes), maxChild);

	mergeChildren(newHead, *t);

	oldHead = *t;
	free(oldHead);

	*t = newHead;

	return result;
	*/
	int compResult;
	int nodeCount = countElems((*t)->childNodes);
	int result = (*t)->index;
	struct BinomialTreeNode* oldHead;
	struct Node* oldHeadChildren = (*t)->childNodes;
	struct Node* curr, *temp;
	
	oldHead = *t;

	while (nodeCount > 1) {
		curr = oldHeadChildren;

		while (curr != NULL && curr->next != NULL) {
			compResult = COMPARE(GET_NODE_INDEX(curr), GET_NODE_INDEX(curr->next));

			if (compResult == LEFT_GREATER) {
				temp = curr->next;
			} else if (compResult == RIGHT_GREATER) {
				temp = curr;
				curr = curr->next;
			} else {
				perror("ERROR WITH COMPARISONS!!!\n");
				exit(1);
			}

			transferNode(&oldHeadChildren, temp, &curr->indexNode->childNodes);
			--nodeCount;
			curr = curr->next;		
		}
	}

	*t = oldHeadChildren->indexNode;
	free(oldHead);
	return result;
}

int doalg(int n, int k, int* Best) {
	int i;
	struct BinomialTreeNode* head;

	initBinomialTree(&head, n);

	for (i = 0; i < k-1; ++i) {
		Best[i] = removeLargest(&head);
	}
	Best[i] = head->index;

	freeBinoTreeNode(head);
	return true;
}

#endif