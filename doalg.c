#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define LEFT_GREATER 1
#define RIGHT_GREATER 2

#define BATTLEHEAP 1
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

#elif BATTLEHEAP

#define GET_NODE_INDEX(node) (node->heapNode->index)

typedef struct _BattleHeapNode {
	int index;
	List listOfChildren;
} BattleHeapNode;

typedef struct _Node {
	Node* next;
	Node* prev;
	BattleHeapNode* heapNode;
} Node;

typedef struct _List {
	Node* first;
	Node* last;
	int count;
} List;


void initList(List* list) {
	list->first = NULL;
	list->last = NULL;
	list->count = 0;
}

void pushBack(List* list, Node* newNode) {
	if (list->first == NULL) {
		list->first = newNode;
	} else {
		list->last->next = newNode;
		newNode->prev = list->last;
	}
	list->last = newNode;
	++list->count;
}

void removeNode(List* list, Node* toRemove) {
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

void deleteNode(List* list, Node* toDelete) {
	removeNode(list, toDelete);
	free(toDelete);
}

void freeList(List* list) {
	while (list->first != NULL) {
        deleteNode(list, list->first);
	}
}

void transferNode(List* donorList, Node* nodeToTransfer, List* recipientList) {
	removeNode(donorList, nodeToTransfer);
	pushBack(recipientList, nodeToTransfer);
}

void moveList(List* donorList, List* recipientList) {
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

void printList(List list) {
	Node* curr;
	for (curr = list.first; curr != NULL; curr = curr->next) {
		printf("%5d ", GET_NODE_INDEX(curr));
	}
	putchar('\n');
}

void initBattleHeapNode(BattleHeapNode* node, int newIndex) {
	node->index = newIndex;
	initList(&node->listOfChildren);
}

void initNode(Node* node, int i) {
	node->heapNode = (BattleHeapNode*)malloc(sizeof(BattleHeapNode));
	node->next = NULL;
	node->prev = NULL;

	initBattleHeapNode(node->heapNode, i);
}

void freeBattleHeapNode(BattleHeapNode* node) {
	Node* curr;
	for (curr = node->listOfChildren.first; curr != NULL; curr = curr->next) {
		freeBattleHeapNode(curr->heapNode);
	}

	if (node != NULL) {
		freeList(&node->listOfChildren);
	}
	free(node);
}

bool fillListWithIndices(List* list, int num) {
	int i;
	Node* current;

	for (i = 1; i <= num; ++i) {
		current = (Node*)malloc(sizeof(Node));
		initNode(current, i);
		pushBack(list, current);
	}
	return true;
}

void visualize(BattleHeapNode node, int n, int k) {
	static char BUF[256];
	sprintf(BUF, "%d-%d.dot", n ,k);
	FILE* outfile = fopen(BUF, "w");
	fprintf(outfile, "digraph G {\ngraph [ranksep=0];\n");
	int remaining = 0;
	visualizeDFS(outfile, node);
}

void visualizeDFS(FILE* outfile, BattleHeapNode node) {
	BattleHeapNode* curr;
	while (curr != NULL) {
		
		//curr = curr->next;
	}
}

void printBinomialTreeNode(BattleHeapNode node) {
	Node* curr;

	printf("%5d: ", node.index);
	printList(node.listOfChildren);

	for (curr = node.listOfChildren.first; curr != NULL; curr = curr->next) {
		printBinomialTreeNode(*curr->heapNode);
	}
}


void runTournament(List* list) {
	Node* curr;
	Node* temp;
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
				exit(1);
			}

			transferNode(list, temp, &curr->heapNode->listOfChildren);
			curr = curr->next;		
		}
	}
}

void createTournamentHeap(BattleHeapNode** t, int numOfIndices) {
	List winners;
	
	initList(&winners);
	fillListWithIndices(&winners, numOfIndices);
	
	runTournament(&winners);

	*t = winners.first->heapNode;
	freeList(&winners);
}

int removeLargest(BattleHeapNode** t) {
	BattleHeapNode* oldRoot;
	List winners;
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
	int i;
	BattleHeapNode* head;

	if (n == 1) {
		Best[0] = 1;
		return 1;
	}

	createTournamentHeap(&head, n);

	for (i = 0; i < k-1; ++i) {
		Best[i] = removeLargest(&head);
	}
	Best[i] = head->index;

	freeBattleHeapNode(head);
	return true;
}

#elif MIN_HEAP

static int COUNTER;

void swap(int* left, int* right) {
	int temp = *left;
	*left = *right;
	*right = temp;	
}

typedef struct _Min_Heap {
	int* arr;
	int numElements;
} Min_Heap;

Min_Heap* MIN_HEAP_ALLOC(int k) {
	Min_Heap* ret = (Min_Heap*)malloc(sizeof(Min_Heap));
	ret->arr = (int*)malloc(sizeof(int)*(k+1));
	ret->arr[0] = k;
	ret->numElements = k;
	return ret;
}

void MIN_HEAP_FREE(Min_Heap* heap) {
	free(heap->arr);
	free(heap);
}

void MIN_HEAP_heapify(Min_Heap* heap, int i) {
	int j = i;
	int k;
	while (j <= heap->numElements/2) {
		if (2*j == heap->numElements)
			k = 2*j;
		else {
			k = (COMPARE(heap->arr[2*j], heap->arr[2*j+1]) == RIGHT_GREATER) ? 2*j : (2*j)+1;
			COUNTER++;
		} 

		if (COMPARE(heap->arr[k], heap->arr[j]) == RIGHT_GREATER) {
			swap(&heap->arr[k], &heap->arr[j]);
			j = k;
		} else {
			j = heap->numElements;
		}
		COUNTER++;
	}
}

void MIN_HEAP_fill(Min_Heap* heap) {
	for (int i = 1; i <= heap->numElements; i++) {
		heap->arr[i] = i;
	}
	for (int i = heap->numElements/2; i > 0; i--) {
		MIN_HEAP_heapify(heap, i);
	}
}

void MIN_HEAP_insert(Min_Heap* heap, int num) {
	if (COMPARE(heap->arr[1], num) == RIGHT_GREATER) {
		heap->arr[1] = num;
		MIN_HEAP_heapify(heap, 1);
	}
	COUNTER++;
}

int MIN_HEAP_remove(Min_Heap* heap) {
	int ret = heap->arr[1];
	heap->arr[1] = heap->arr[heap->numElements];
	heap->numElements--;
	MIN_HEAP_heapify(heap, 1);
	return ret;
}

int doalg(int n, int k, int* Best) {
	COUNTER = 0;
	Min_Heap* heap = MIN_HEAP_ALLOC(k);
	MIN_HEAP_fill(heap);
	printf("Comparisons at: %d\n", COUNTER);

	for (int i = k+1; i <= n; i++) {
		MIN_HEAP_insert(heap, i);
	}
	printf("Comparisons at: %d\n", COUNTER);

	for (int i = k-1; i >= 0; i--) {
		Best[i] = MIN_HEAP_remove(heap);
	}
	printf("Comparisons at: %d\n", COUNTER);

	MIN_HEAP_FREE(heap);
	return true;
}

#endif