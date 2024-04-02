#include <iostream>
#include <omp.h>
#include <math.h>

using namespace std;
const int N = 100;
const int K = 10;
const int d = 100;

// функция получения индекса
int dataIndex(int i, int j, int k, int NINT) {
	return k + NINT * j + NINT * NINT * i;
}

// функция для заполнения гистограммы по отдельным значениям
void fill_hist(int hist[], int value, int NINT, int boxsize) {
	int ind = value * K / boxsize;
	for (int i = 0; i <= ind; i++)
		hist[i]++;
}

// функция для полного заполнения гистограммы (параллельно)
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

// функция для рисования гистограммы
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

	// заполним массивы псевдослучайными значениями
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

	// создание гистограммы доз (большие или равные значения)
	int hist[K + 1];
	create_hist(hist, data, N, d);

	// рисуем гистограмму (последовательно и схематично в консоли)
	draw_hist(hist, N, d);
	// rand создает равномерное распределение, поэтому график линейный

	// добавляем 10, если в массиве flag 1
#pragma omp parallel for
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			for (int k = 0; k < N; k++) {
				if (flag[dataIndex(i, j, k, N)] == 1)
					data[dataIndex(i, j, k, N)] *= 10;
			}
		}
	}

	// создание гистограммы доз (большие или равные значения)
	int hist1[K + 1];
	create_hist(hist1, data, N, d * 10);

	// рисуем гистограмму (последовательно и схематично в консоли)
	draw_hist(hist1, N, d * 10);

	delete[] data;
	delete[] flag;
}