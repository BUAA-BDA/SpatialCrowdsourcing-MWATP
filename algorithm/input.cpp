/**
	\author:	Trasier
	\date:		2017.12.11
*/
#include "input.h"


void readInput(const string& fileName, int& N, int& M, worker_t*& workers, task_t*& tasks) {
	ifstream fin(fileName.c_str(), ios::in);
	if (!fin.is_open()) {
		fprintf(stderr, "FILE %s is invalid.", fileName.c_str());
		exit(1);
	}

	readInput(fin, N, M, workers, tasks);
	fin.close();
}

void readInput(istream& fin, int& N, int& M, worker_t*& workers, task_t*& tasks) {
	fin >> N >> M;
	if (workers == NULL)
		workers = new worker_t[N];
	if (tasks == NULL)
		tasks = new task_t[M];

	int workerIdx = 0, taskIdx = 0;
	int op;

	for (int i=0; i<N+M; ++i) {
		fin >> op;
		if (op == 0) {
			fin >> tasks[taskIdx].lt.x >> tasks[taskIdx].lt.y >> tasks[taskIdx].rt >> tasks[taskIdx].et >> tasks[taskIdx].ut;
			++taskIdx;
		} else {
			fin >> workers[workerIdx].cw.x >> workers[workerIdx].cw.y >> workers[workerIdx].dw.x >> workers[workerIdx].dw.y >> workers[workerIdx].sw >> workers[workerIdx].ew;
			// assert(workers[workerIdx].sw+Length(workers[workerIdx].cw, workers[workerIdx].dw) <= workers[workerIdx].ew);
			++workerIdx;
		}
	}
}

void reOrder(int N, int M,  worker_t* workers, task_t* tasks, int*& orders) {
	if (orders == NULL)
		orders = new int[N+M];

	int i = 0, j = 0, k = 0;

	while (i<N && j<M) {
		if (workers[i].sw < tasks[j].rt) {
			orders[k++] = i;
			++i;
		} else {
			orders[k++] = j+N;
			++j;
		}
	}
	while (i < N) {
		orders[k++] = i;
		++i;
	}
	while (j < M) {
		orders[k++] = j+N;
		++j;
	}
}
