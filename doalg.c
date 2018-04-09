#define SORT 1
#define BINOMIAL 0
#define BLANK 0
#define PARANOID 1

#if SORT
#include <time.h>
#include <stdlib.h>
void swap(int* arr, int index1, int index2) {
	int temp = arr[index1];
	arr[index1] = arr[index2];
	arr[index2] = temp;
}

int partition(int* arr, int low, int high) {
	int partIndex = low;
	int left = partIndex+1, right = high;

	while (left < right) {
		while (left != right && COMPARE(arr[left], arr[partIndex]) == 2) {
			left++;
		}
		while (left != right && COMPARE(arr[right], arr[partIndex]) == 1) {
			right--;
		}
		if (left < right) {
			swap(arr, left, right);
			//left++;
		}
	}
	swap(arr, low, left-1);
	return left-1;
}

void quickSort(int* arr, int low, int high, int n, int k) {
	if (low < high) {
		int pi = partition(arr, low, high);
#if PARANOID
		quickSort(arr, pi+1, high, n, k);
		quickSort(arr, low, pi, n, k);
#else
		if (high >= n-k) {
			quickSort(arr, pi+1, high, n, k);
			if (pi >= n - k) {
				quickSort(arr, low, pi, n, k);
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

#if BLANK
int doalg(int n, int k, int* Best) {
	return 1;
}
#endif