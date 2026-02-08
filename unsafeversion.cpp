/************************************************************************
 * Written by: Lara Hisham Ahmad 
 * Course info.: CpE473 Operating Systems - Dr. Mohammad Al-Shboul
 * Version: Un-Safe version
 * Algorithms: Merge Sort, Quick Sort, Heap Sort, Radix Sort, Bitonic Sort
*************************************************************************/
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <string>
#include <cmath>
#include <barrier>
using namespace std;

int AboveThreshold = 0, EqualsThreshold = 0, BelowThreshold = 0, TH;

/************************************************************************
 * MERGE SORT FUNCTIONS
*************************************************************************/
void mergeH(int arrayA[], int LMIndex, int MidIndex, int RMIndex)
{
	int leftArraySize = MidIndex - LMIndex + 1;
	int rightArraySize = RMIndex - MidIndex;
	int* leftArray = new int[leftArraySize];
	int* rightArray = new int[rightArraySize];
	for (int i = 0; i < leftArraySize; i++)	leftArray[i] = arrayA[LMIndex + i];
	for (int j = 0; j < rightArraySize; j++) rightArray[j] = arrayA[MidIndex + 1 + j];
	int i = 0;
	int j = 0;
	int index = LMIndex;
	while (i < leftArraySize && j < rightArraySize) 
		arrayA[index++] = (leftArray[i] <= rightArray[j]) ? leftArray[i++] : rightArray[j++];
	while (i < leftArraySize) arrayA[index++] = leftArray[i++];
	while (j < rightArraySize) arrayA[index++] = rightArray[j++];
	delete[] leftArray;
	delete[] rightArray;
}

void mergeSort(int arrayA[], int LMIndex, int RMIndex) {
	if (LMIndex >= RMIndex) return;
	int MidIndex = LMIndex + (RMIndex - LMIndex) / 2;
	mergeSort(arrayA, LMIndex, MidIndex);
	mergeSort(arrayA, MidIndex + 1, RMIndex);
	mergeH(arrayA, LMIndex, MidIndex, RMIndex);
}

/************************************************************************
 * QUICK SORT FUNCTIONS
*************************************************************************/
int partition(int arr[], int low, int high) {
	int pivot = arr[high];
	int i = low - 1;
	for (int j = low; j < high; j++) {
		if (arr[j] <= pivot) {
			i++;
			swap(arr[i], arr[j]);
		}
	}
	swap(arr[i + 1], arr[high]);
	return i + 1;
}

void quickSort(int arr[], int low, int high, int depth = 0) {
	if (low < high) {
		int pi = partition(arr, low, high);
		
		// Use threading for top levels only
		if (depth < 2) {
			thread t1(quickSort, arr, low, pi - 1, depth + 1);
			thread t2(quickSort, arr, pi + 1, high, depth + 1);
			t1.join();
			t2.join();
		} else {
			quickSort(arr, low, pi - 1, depth + 1);
			quickSort(arr, pi + 1, high, depth + 1);
		}
	}
}

/************************************************************************
 * HEAP SORT FUNCTIONS
*************************************************************************/
void heapify(int arr[], int n, int i) {
	int largest = i;
	int left = 2 * i + 1;
	int right = 2 * i + 2;
	
	if (left < n && arr[left] > arr[largest])
		largest = left;
	if (right < n && arr[right] > arr[largest])
		largest = right;
	
	if (largest != i) {
		swap(arr[i], arr[largest]);
		heapify(arr, n, largest);
	}
}

void heapSort(int arr[], int n) {
	// Build max heap
	for (int i = n / 2 - 1; i >= 0; i--)
		heapify(arr, n, i);
	
	// Extract elements from heap
	for (int i = n - 1; i > 0; i--) {
		swap(arr[0], arr[i]);
		heapify(arr, i, 0);
	}
}

/************************************************************************
 * RADIX SORT FUNCTIONS
*************************************************************************/
int getMax(int arr[], int n) {
	int max = arr[0];
	for (int i = 1; i < n; i++)
		if (arr[i] > max)
			max = arr[i];
	return max;
}

void countSort(int arr[], int n, int exp) {
	int* output = new int[n];
	int count[10] = {0};
	
	for (int i = 0; i < n; i++)
		count[(arr[i] / exp) % 10]++;
	
	for (int i = 1; i < 10; i++)
		count[i] += count[i - 1];
	
	for (int i = n - 1; i >= 0; i--) {
		output[count[(arr[i] / exp) % 10] - 1] = arr[i];
		count[(arr[i] / exp) % 10]--;
	}
	
	for (int i = 0; i < n; i++)
		arr[i] = output[i];
	
	delete[] output;
}

void radixSort(int arr[], int n) {
	int max = getMax(arr, n);
	for (int exp = 1; max / exp > 0; exp *= 10)
		countSort(arr, n, exp);
}

/************************************************************************
 * BITONIC SORT FUNCTIONS
*************************************************************************/
void compAndSwap(int arr[], int i, int j, int dir) {
	if (dir == (arr[i] > arr[j]))
		swap(arr[i], arr[j]);
}

void bitonicMerge(int arr[], int low, int cnt, int dir) {
	if (cnt > 1) {
		int k = cnt / 2;
		for (int i = low; i < low + k; i++)
			compAndSwap(arr, i, i + k, dir);
		bitonicMerge(arr, low, k, dir);
		bitonicMerge(arr, low + k, k, dir);
	}
}

void bitonicSort(int arr[], int low, int cnt, int dir) {
	if (cnt > 1) {
		int k = cnt / 2;
		bitonicSort(arr, low, k, 1);
		bitonicSort(arr, low + k, k, 0);
		bitonicMerge(arr, low, cnt, dir);
	}
}

void bitonicSortWrapper(int arr[], int n) {
	// Pad to next power of 2 if needed
	int paddedSize = 1;
	while (paddedSize < n) paddedSize *= 2;
	
	if (paddedSize != n) {
		cout << "Note: Bitonic sort works best with power-of-2 sizes. Padding from " 
		     << n << " to " << paddedSize << endl;
	}
	
	bitonicSort(arr, 0, n, 1);
}

/************************************************************************
 * THREAD TASK FUNCTIONS
*************************************************************************/
void threadTaskMerge(int threadID, int* arr, int low, int high) {
	{
		cout << "Merge Sort Thread " << threadID << ": low = " << low << ", high = " << high << endl;
	}
	for (int i = low; i <= high; i++) {
		if (arr[i] > TH) AboveThreshold++;
		else if (arr[i] == TH) EqualsThreshold++;
		else BelowThreshold++;
	}
	mergeSort(arr, low, high);
}

void threadTaskQuick(int threadID, int* arr, int low, int high) {
	{
		cout << "Quick Sort Thread " << threadID << ": low = " << low << ", high = " << high << endl;
	}
	for (int i = low; i <= high; i++) {
		if (arr[i] > TH) AboveThreshold++;
		else if (arr[i] == TH) EqualsThreshold++;
		else BelowThreshold++;
	}
	quickSort(arr, low, high, 2); // Start at depth 2 to avoid too many threads
}

void threadTaskHeap(int threadID, int* arr, int low, int high) {
	{
		cout << "Heap Sort Thread " << threadID << ": low = " << low << ", high = " << high << endl;
	}
	for (int i = low; i <= high; i++) {
		if (arr[i] > TH) AboveThreshold++;
		else if (arr[i] == TH) EqualsThreshold++;
		else BelowThreshold++;
	}
	// Heap sort on the chunk
	int size = high - low + 1;
	heapSort(arr + low, size);
}

void threadTaskRadix(int threadID, int* arr, int low, int high) {
	{
		cout << "Radix Sort Thread " << threadID << ": low = " << low << ", high = " << high << endl;
	}
	for (int i = low; i <= high; i++) {
		if (arr[i] > TH) AboveThreshold++;
		else if (arr[i] == TH) EqualsThreshold++;
		else BelowThreshold++;
	}
	// Radix sort on the chunk
	int size = high - low + 1;
	radixSort(arr + low, size);
}

void threadTaskBitonic(int threadID, int* arr, int low, int high) {
	{
		cout << "Bitonic Sort Thread " << threadID << ": low = " << low << ", high = " << high << endl;
	}
	for (int i = low; i <= high; i++) {
		if (arr[i] > TH) AboveThreshold++;
		else if (arr[i] == TH) EqualsThreshold++;
		else BelowThreshold++;
	}
	// Bitonic sort on the chunk
	int size = high - low + 1;
	bitonicSortWrapper(arr + low, size);
}

/************************************************************************
 * MAIN FUNCTION
*************************************************************************/
void runSortingAlgorithm(const string& algoName, vector<int> data, int T, int N, 
                         void (*threadFunc)(int, int*, int, int)) {
	cout << "\n========================================" << endl;
	cout << "Running " << algoName << endl;
	cout << "========================================" << endl;
	
	// Reset counters
	AboveThreshold = 0;
	EqualsThreshold = 0;
	BelowThreshold = 0;
	
	vector<thread> threads;
	int chunkSize = N / T;
	
	for (int i = 0; i < T; i++) {
		int low = i * chunkSize;
		int high = (i == T - 1) ? N - 1 : (low + chunkSize - 1);
		if (i >= N) {
			cout << "Thread " << i << ": No work to do." << endl;
			continue;
		}
		threads.emplace_back(threadFunc, i, data.data(), low, high);
	}
	
	for (auto& t : threads) t.join();
	
	// Merge sorted chunks (using merge sort's merge function)
	int step = chunkSize;
	while (step < N) {
		for (int i = 0; i + step < N; i += 2 * step) {
			int mid = i + step - 1;
			int right = min(i + 2 * step - 1, N - 1);
			mergeH(data.data(), i, mid, right);
		}
		step *= 2;
	}
	
	cout << algoName << " - Above Threshold = " << AboveThreshold << endl;
	cout << algoName << " - Equals Threshold = " << EqualsThreshold << endl;
	cout << algoName << " - Below Threshold = " << BelowThreshold << endl;
	
	// Write output
	string filename = "out_unsafe_" + algoName + ".txt";
	for (char& c : filename) {
		if (c == ' ') c = '_';
	}
	ofstream out(filename);
	out << "Sorted array using " << algoName << ":\n";
	for (int x : data) out << x << " ";
	out << endl;
	out.close();
	
	cout << "Output written to " << filename << endl;
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		cout << "Usage: " << argv[0] << " <number_of_threads>" << endl;
		return 1;
	}
	
	int T = stoi(argv[1]);
	
	ifstream in("in.txt");
	if (!in) {
		cout << "Error: Cannot open in.txt" << endl;
		return 1;
	}
	
	int N;
	in >> N >> TH;
	vector<int> data(N);
	for (int i = 0; i < N; i++) in >> data[i];
	in.close();
	
	if (N % T != 0) {
		cout << "Error: N (" << N << ") is not divisible by T (" << T << ")." << endl;
		return 1;
	}
	
	cout << "Main: Starting sorting with N=" << N << ", TH=" << TH << ", Threads=" << T << endl;
	
	// Run all sorting algorithms
	runSortingAlgorithm("Merge_Sort", data, T, N, threadTaskMerge);
	runSortingAlgorithm("Quick_Sort", data, T, N, threadTaskQuick);
	runSortingAlgorithm("Heap_Sort", data, T, N, threadTaskHeap);
	runSortingAlgorithm("Radix_Sort", data, T, N, threadTaskRadix);
	runSortingAlgorithm("Bitonic_Sort", data, T, N, threadTaskBitonic);
	
	cout << "\n========================================" << endl;
	cout << "All sorting algorithms completed!" << endl;
	cout << "========================================" << endl;
	
	return 0;
}
