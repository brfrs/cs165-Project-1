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
int ARR[] = {93, 63, 36, 80, 32, 77, 26, 42, 4, 46, 43, 31, 1, 49, 98, 79, 90, 19, 59, 28, 41, 38, 73, 11, 61};
bool COMPARE(int index1, int index2) {
    return (index1 > index2 ? 1 : 2);
}
void swap(int* index1, int* index2) {
    static int swapcount = 0;
    printf("swapcount: %d\n", ++swapcount);
	int temp = *index1;
	*index1 = *index2;
	*index2 = temp;
}

int partition(int* arr, int lo, int hi) {
	int pivot = lo;
	int h = pivot+1, k = hi;

	while (h < k) {
		while (h <= k && COMPARE(arr[h], arr[pivot]) == RIGHT_GREATER) {
			h++;
		}
		while (h <= k && COMPARE(arr[k], arr[pivot]) == LEFT_GREATER) {
			k--;
		}
		if (h < k) {
			swap(&arr[h], &arr[k]);
			h++;
		}
	}
	swap(&arr[pivot], &arr[k]);
	return k;
}

void quickSort(int* arr, int lo, int hi, int n, int k) {
	if (lo < hi) {
		int pi = partition(arr, lo, hi);
		quickSort(arr, pi+1, hi, n, k);
		quickSort(arr, lo, pi-1, n, k);
	}
}
#endif

void printArray(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", ARR[i]);
    }
    printf("\n");
}
int main() {
    printArray(ARR, 25);
    quickSort(ARR, 0, 25, 25, 5);
    printArray(ARR, 25);
    return 0;   
}