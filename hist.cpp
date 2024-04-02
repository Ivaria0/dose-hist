#include <iostream>
#include <omp.h>
#include <math.h>

using namespace std;
const int N = 100;
const int K = 10;
const int d = 100;

// function for getting of the index
int dataIndex(int i, int j, int k, int NINT) {
	return k + NINT * j + NINT * NINT * i;
}

// function for filling a histogram for separate values
void fill_hist(int hist[], int value, int NINT, int boxsize) {
	int ind = value * K / boxsize;
	for (int i = 0; i <= ind; i++)
		hist[i]++;
}

// function full filling of the histogram (in parallel)
void create_hist(int hist[], int* data, int NINT, int boxsize) {
	for (int i = 0; i <= K; i++)
		hist[i] = 0;
	
#pragma omp parallel
	{
		int private_hist[K + 1];
		for (int i = 0; i <= K; i++)
			private_hist[i] = 0;
		
#pragma omp for nowait
		for (int i = 0; i < NINT; i++) {
			for (int j = 0; j < NINT; j++) {
				for (int k = 0; k < NINT; k++) {
					fill_hist(private_hist, data[dataIndex(i, j, k, NINT)], NINT, boxsize);
				}
			}
		}
		
#pragma omp critical
		{
			for (int i = 0; i <= K; i++)
				hist[i] += private_hist[i];
		}
	}
}

// function for histogram drawing
void draw_hist(int hist[], int NINT, int boxsize) {
	for (int i = 10; i > 0; i--) {
		cout.width(8);
		cout << i * NINT * NINT * NINT / K << " | ";
		for (int j = 0; j <= K; j++) {
			if (round((double)(hist[j] * K) / (double)(NINT * NINT * NINT)) >= i)
				cout << "*    ";
			else
				cout << "     ";
		}
		cout << endl;
	}
	cout << "------------------------------------------------------------------" << endl;
	cout << "         ";
	for (int i = 0; i <= K; i++) {
		cout.width(4);
		cout << i * boxsize / K << " ";
	}
	cout << endl;
	cout << endl;
}

int main() {
	int* data = new int[N * N * N];
	int* flag = new int[N * N * N];

	// fill the arrays with pseudorandom values
#pragma omp parallel for
	for (int i = 0; i < N; i++) {
		srand(i);
		for (int j = 0; j < N; j++) {
			for (int k = 0; k < N; k++) {
				flag[dataIndex(i, j, k, N)] = rand() % 2;
				data[dataIndex(i, j, k, N)] = rand() % d;
			}
		}
	}

	// create a dose histogram (greater or equal values)
	int hist[K + 1];
	create_hist(hist, data, N, d);

	// draw a histogram (sequentially and schematically in the console)
	draw_hist(hist, N, d);
	// rand creates a uniform distribution, so the graph is linear

	// add 10, if flag=1 in array
#pragma omp parallel for
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			for (int k = 0; k < N; k++) {
				if (flag[dataIndex(i, j, k, N)] == 1)
					data[dataIndex(i, j, k, N)] *= 10;
			}
		}
	}

	// create a dose histogram (greater or equal values)
	int hist1[K + 1];
	create_hist(hist1, data, N, d * 10);

	// draw a histogram (sequentially and schematically in the console)
	draw_hist(hist1, N, d * 10);

	delete[] data;
	delete[] flag;
}
