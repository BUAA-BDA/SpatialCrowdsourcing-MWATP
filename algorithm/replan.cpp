/**
	\author:	Trasier
	\date:		2017.12.12
*/
#include <bits/stdc++.h>
using namespace std;

#include "input.h"
#include "output.h"
#include "global.h"

#ifdef USE_MEM
#include "monitor.h"
#endif

bool *mark;
double *FT;
// typedef unordered_map hash_map;

void Insertion(int workerId, int taskId, int& k, double& minCost);
bool judgeInsertion(int workerId, int taskId, int k, double minCost);

void init() {
	mark = new bool[n+m];

	memset(mark, false, sizeof(bool)*(n+m));
}

void freeMem() {
	delete[] workers;
	delete[] tasks;
	delete[] orders;
	delete[] mark;
}

vector<int> filterCand(int workerId, vector<int>& taskPool, double curTime) {
	worker_t& worker = workers[workerId];
	int taskId;
	vector<int> ret;

	for (int i=taskPool.size()-1; i>=0; --i) {
		taskId = taskPool[i];
		task_t& task = tasks[taskId];
		if (curTime+Length(worker.cw, task.lt)<=task.et && curTime+Length(worker.cw,task.lt)+Length(task.lt,worker.dw)<=worker.ew) {
			ret.push_back(taskId);
		}
	}

	return ret;
}

bool queryOneWorker(int workerId, vector<int>& taskIds, double curTime) {
	if (taskIds.empty()) return false;

	worker_t& worker = workers[workerId];
	position_t cw = worker.cw, dw = worker.dw;
	const int sz = taskIds.size();
	int taskId;
	pair<double,int> *ptmp = new pair<double,int>[sz];

	for (int i=taskIds.size()-1; i>=0; --i) {
		taskId = taskIds[i];
		double tmp = Length(cw, tasks[taskId].lt) + Length(tasks[taskId].lt, dw) - Length(cw, dw);
		if (tmp < eps) tmp = eps;
		ptmp[i].first = tasks[taskId].ut / tmp;
		ptmp[i].second = taskId;
	}
	sort(ptmp, ptmp+sz, greater<pair<double,int> >());

	double sw = curTime;
	for (int i=0; i<sz; ++i) {
		taskId = ptmp[i].second;
		if (sw+Length(cw, tasks[taskId].lt)+Length(tasks[taskId].lt, dw)<=worker.ew && sw+Length(cw, tasks[taskId].lt)<=tasks[taskId].et) {
			worker.route.push_back(taskId);
			mark[taskId] = true;
			sw += Length(cw, tasks[taskId].lt);
			cw = tasks[taskId].lt;
		}
	}

	return true;
}

void insertTask(int taskId, int bestWorker, int bestPlace) {
	worker_t& worker = workers[bestWorker];
	worker.route.insert(worker.route.begin()+bestPlace, taskId);
	mark[taskId] = true;
}

void updateTasks(vector<int>& taskPool, double curTime) {
	for (int i=taskPool.size()-1; i>=0; --i) {
		int taskId = taskPool[i];
		if (tasks[taskId].et <= curTime) {
			taskPool[i] = *taskPool.rbegin();
			taskPool.pop_back();
		}
	}
}

void updateWorkerInLine(int workerId, double curTime) {
	worker_t& worker = workers[workerId];
	vector<int>& route = worker.route;
	int sz = route.size();
	position_t src, des;

	if (sz == 0) {// line from cw to dw
		src = worker.cw;
		des = worker.dw;
	} else {
		src = worker.cw;
		des = tasks[route[0]].lt;
	}

	if (dcmp(worker.sw - curTime) == 0) {
		return ;
	}

	double t = Length(src, des);
	if (dcmp(t) == 0) {
		worker.sw = curTime;
	} else {
		double dx = (des.x - src.x) / t;
		double dy = (des.y - src.y) / t;

		// add a new move
		worker.cw.x = worker.cw.x + dx * (curTime - worker.sw);
		worker.cw.y = worker.cw.y + dy * (curTime - worker.sw);
		worker.sw = curTime;
	}
}

bool updateWorker(int workerId, double curTime) {
	worker_t& worker = workers[workerId];
	vector<int>& route = worker.route;
	int sz = route.size(), i = 0, j = 0, k = 0;
	position_t pre = worker.cw, cur;
	double sw = worker.sw;
	bool ret = false;

	for (i=0; i<sz; ++i) {
		int taskId = route[i];
		cur = tasks[taskId].lt;
		sw += Length(pre, cur);
		if (sw <= curTime) {
			mark[taskId] = true;
		} else {
			sw -= Length(pre, cur);
			break;
		}
		pre = cur;
	}
	if (i == sz) {
		route.clear();
		if (worker.ew<=curTime || max(sw,curTime)+Length(pre, worker.dw)>worker.ew) {
			worker.sw = curTime;
			worker.cw = worker.dw;
			ret = true;
		} else {
			worker.sw = sw;
			worker.cw = pre;
			updateWorkerInLine(workerId, curTime);
		}
	} else {
		j = 0, k = i;
		while (k < sz)
			route[j++] = route[k++];
		route.erase(route.begin()+j, route.end());

		worker.sw = sw;
		worker.cw = pre;
		updateWorkerInLine(workerId, curTime);
	}

	return ret;
}

void updateWorkers(vector<int>& workerPool, double curTime) {
	for (int i=workerPool.size()-1; i>=0; --i) {
		int workerId = workerPool[i];
		if ( updateWorker(workerId, curTime) ) {
			workerPool[i] = *workerPool.rbegin();
			workerPool.pop_back();
		}
		assert(dcmp(workers[workerId].sw - curTime) >= 0);
	}
}

double calcUtility() {
	double ret = 0.0;

	for (int i=0; i<m; ++i) {
		if (mark[i])
			ret += tasks[i].ut;
	}

	return ret;
}

void updateAssignedTasks(vector<int>& taskPool)  {
	int taskId;

	for (int i=taskPool.size()-1; i>=0; --i) {
		taskId = taskPool[i];
		if (mark[taskId]) {
			taskPool[i] = *taskPool.rbegin();
			taskPool.pop_back();
		}
	}
}

void updateExpiredTasks(vector<int>& taskPool, double curTime) {
	int taskId;

	for (int i=taskPool.size()-1; i>=0; --i) {
		taskId = taskPool[i];
		if (tasks[taskId].et < curTime) {
			taskPool[i] = *taskPool.rbegin();
			taskPool.pop_back();
		}
	}
}

double calcTimeCost(int workerId) {
	worker_t& worker = workers[workerId];
	int sz = worker.route.size();
	position_t pre = worker.cw;
	double ret = 0.0;

	for (int i=0; i<sz; ++i) {
		int taskId = worker.route[i];
		ret += Length(pre, tasks[taskId].lt);
		pre = tasks[taskId].lt;
	}
	ret += Length(pre, worker.dw);

	return ret;
}

void Insertion(int workerId, int taskId, int& k, double& minCost) {
	worker_t& worker = workers[workerId];
	task_t& task = tasks[taskId];
	int sz = worker.route.size();
	double orgCost = calcTimeCost(workerId);

	k = -1;
	minCost = inf;
	for (int i=sz; i>=0; --i) {
		position_t pre = worker.cw;
		double sw = task.rt;
		bool flag = true;
		for (int j=0; flag&&j<=sz; ++j) {
			if (j == i) {
				sw += Length(pre, task.lt);
				pre = task.lt;
				if (sw > task.et) {
					flag = false;
				}
			}
			if (j == sz) {
				sw += Length(pre, worker.dw);
				pre = worker.dw;
				if (sw > worker.ew) {
					flag = false;
				}
			} else {
				int taskId = worker.route[j];
				sw += Length(pre, tasks[taskId].lt);
				pre = tasks[taskId].lt;
				if (sw > tasks[taskId].et) {
					flag = false;
				}
			}
		}
		if (flag && sw<minCost) {
			minCost = sw;
			k = i;
		}
	}
	minCost = minCost - task.rt - orgCost;
}

bool insertReplan(vector<int>& workerPool, int taskId, int& bestWorker, int& bestPlace, double& minCost) {
	int sz = workerPool.size();
	int tmpPlace;
	double tmpCost;
	int workerId;

	bestWorker = bestPlace = -1;
	minCost = inf;
	for (int i=0; i<sz; ++i) {
		workerId = workerPool[i];
		Insertion(workerId, taskId, tmpPlace, tmpCost);
		if (tmpCost < minCost) {
			minCost = tmpCost;
			bestWorker = workerId;
			bestPlace = tmpPlace;
		}
	}

	return minCost < inf;
}

double solve() {
	int workerNum = 0, taskNum = 0;
	double curTime;
	vector<int> taskPool, workerPool;

	init();
	for (int i=0; i<n+m; ++i) {
		if (orders[i] >= n) {// it is a task
			curTime = tasks[taskNum].rt;
		} else {// it is a worker
			curTime = workers[workerNum].sw;
		}
		updateWorkers(workerPool, curTime);
		updateExpiredTasks(taskPool, curTime);
		if (orders[i] >= n) {// it is a task
			int taskId = taskNum;
			int bestWorker, bestPlace;
			double minCost;
			if (insertReplan(workerPool, taskId, bestWorker, bestPlace, minCost)) {
				insertTask(taskId, bestWorker, bestPlace);
			} else {
				taskPool.push_back(taskId);
			}
			++taskNum;
		} else {// it is a worker
			int workerId = workerNum;
			vector<int> Cand = filterCand(workerId, taskPool, curTime);
			queryOneWorker(workerId, Cand, curTime);
			workerPool.push_back(workerNum);
			updateAssignedTasks(taskPool);
			++workerNum;
		}
	}

	return calcUtility();
}

int main(int argc, char **argv) {
	string execName("replan");
	string fileName;

	if (argc > 1)
		freopen(argv[1], "r", stdin);
	if (argc > 2)
		freopen(argv[2], "w", stdout);

	readInput(cin, n, m, workers, tasks);
	reOrder(n, m, workers, tasks, orders);

	clock_t begTime, endTime;

	begTime = clock();

	double ans = solve();

	endTime = clock();

	double usedTime = (endTime - begTime)*1.0 / CLOCKS_PER_SEC;

#ifdef USE_MEM
	watchSolutionOnce(getpid(), usedMemory);
#endif

#ifdef USE_MEM
	printf("%s %.3lf %.8lf %d\n", execName.c_str(), ans, usedTime, usedMemory);
#else
	printf("%s %.3lf %.8lf\n", execName.c_str(), ans, usedTime);
#endif

	fflush(stdout);

	freeMem();

	return 0;
}

bool judgeInsertion(int workerId, int taskId, int k, double cost=inf) {
	worker_t& worker = workers[workerId];
	task_t& task = tasks[taskId];
	int sz = worker.route.size();
	double sw = task.rt;
	position_t pre = worker.cw, cur;

	for (int i=0; i<=sz; ++i) {
		if (i == k) {
			sw += Length(pre, task.lt);
			pre = task.lt;
			if (dcmp(sw - task.et) > 0) {
				return false;
			}
		}
		if (i == sz) {
			sw += Length(pre, worker.dw);
			pre = worker.dw;
			if (dcmp(sw - worker.ew) > 0) {
				return false;
			}
		}  else {
			int taskId = worker.route[i];
			sw += Length(pre, tasks[taskId].lt);
			pre = tasks[taskId].lt;
			if (dcmp(sw - tasks[taskId].et) > 0) {
				return false;
			}
		}
	}

	double tmpCost = sw - task.rt - calcTimeCost(workerId);
	if (cost<inf && dcmp(tmpCost-cost)!=0)
		return false;

	return true;
}
