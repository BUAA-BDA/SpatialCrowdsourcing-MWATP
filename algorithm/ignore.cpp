/**
	\author:	Trasier
	\date:		2017.12.11
*/
#include <bits/stdc++.h>
using namespace std;

#include "global.h"
#include "input.h"
#include "output.h"

#ifdef USE_MEM
#include "monitor.h"
#endif

bool *mark;
bool *visit;
// typedef unordered_map hash_map;
pair<double,int> *pdiTmp1, *pdiTmp2;

void init() {
	mark = new bool[n+m];
	memset(mark, false, sizeof(bool)*(n+m));
	visit = new bool[n+m];
	memset(visit, false, sizeof(bool)*(n+m));
	pdiTmp1 = new pair<double,int>[m+5];
}

void freeMem() {
	delete[] workers;
	delete[] tasks;
	delete[] orders;
	delete[] mark;
	delete[] visit;
	delete[] pdiTmp1;
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

bool queryOneWorker_(int workerId, vector<int>& taskIds, double curTime) {
	if (taskIds.empty()) return false;

	worker_t& worker = workers[workerId];
	position_t cw = worker.cw, dw = worker.dw;
	const int sz = taskIds.size();
	int taskId;
	pair<double,int> *ptmp1 = pdiTmp1;
	pair<double,int> *ptmp2 = pdiTmp2;

	for (int i=taskIds.size()-1; i>=0; --i) {
		taskId = taskIds[i];
		double tmp1 = Length(cw, tasks[taskId].lt);
		double tmp2 = Length(tasks[taskId].lt, dw);
		ptmp1[i] = make_pair(tasks[taskId].ut/tmp1, taskId);
		ptmp2[i] = make_pair(tasks[taskId].ut/tmp2, taskId);
	}
	sort(ptmp1, ptmp1+sz, greater<pair<double,int> >());
	sort(ptmp2, ptmp2+sz, greater<pair<double,int> >());


	{
		int i = 0, j = 0;
		int taskId;
		double sw = curTime;

		while (i<sz && j<sz) {
			if (ptmp1[i].first < ptmp2[j].first) {
				taskId = ptmp1[i].second;
				++i;
			} else {
				taskId = ptmp2[j].second;
				++j;
			}
			if (visit[taskId]) {
				if (sw+Length(cw, tasks[taskId].lt)+Length(tasks[taskId].lt, dw)<=worker.ew && sw+Length(cw, tasks[taskId].lt)<=tasks[taskId].et) {
					worker.route.push_back(taskId);
					mark[taskId] = true;
					sw += Length(cw, tasks[taskId].lt);
					cw = tasks[taskId].lt;
				}
			} else {
				visit[taskId] = true;
			}
		}
		while (i < sz) {
			taskId = ptmp1[i++].second;
			if (sw+Length(cw, tasks[taskId].lt)+Length(tasks[taskId].lt, dw)<=worker.ew && sw+Length(cw, tasks[taskId].lt)<=tasks[taskId].et) {
				worker.route.push_back(taskId);
				mark[taskId] = true;
				sw += Length(cw, tasks[taskId].lt);
				cw = tasks[taskId].lt;
			}
		}
		while (j < sz) {
			taskId = ptmp2[j++].second;
			if (sw+Length(cw, tasks[taskId].lt)+Length(tasks[taskId].lt, dw)<=worker.ew && sw+Length(cw, tasks[taskId].lt)<=tasks[taskId].et) {
				worker.route.push_back(taskId);
				mark[taskId] = true;
				sw += Length(cw, tasks[taskId].lt);
				cw = tasks[taskId].lt;
			}
		}
	}

	for (int i=0; i<sz; ++i)
		visit[ptmp1[i].second] = false;

	return true;
}

bool queryOneWorker(int workerId, vector<int>& taskIds, double curTime) {
	if (taskIds.empty()) return false;

	worker_t& worker = workers[workerId];
	position_t cw = worker.cw, dw = worker.dw;
	const int sz = taskIds.size();
	int taskId;
	pair<double,int> *ptmp = pdiTmp1;

	for (int i=taskIds.size()-1; i>=0; --i) {
		taskId = taskIds[i];
		double tmp = Length(cw, tasks[taskId].lt) + Length(tasks[taskId].lt, dw) - Length(cw, dw);
		if (tmp < eps) tmp = eps;
		ptmp[i].first = tasks[taskId].ut / tmp;
		ptmp[i].second = taskId;
	}
	sort(ptmp, ptmp+sz, greater<pair<double,int> >());

	double sw = curTime;
	worker.route.clear();
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

	double preTime = worker.sw;

	if (dcmp(preTime - curTime) == 0) {
		worker.sw = curTime;
		return ;
	}

	double t = Length(src, des);
	double dx = (des.x - src.x) / t;
	double dy = (des.y - src.y) / t;

	// add a new move
	worker.cw.x = worker.cw.x + dx * (curTime - worker.sw);
	worker.cw.y = worker.cw.y + dy * (curTime - worker.sw);
	worker.sw = curTime;
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
		if (sw+Length(pre, worker.dw) <= curTime) {
			worker.sw = sw;
			worker.cw = worker.dw;
			ret = true;
		} else {
			worker.sw = sw;
			worker.cw = (sz > 0) ? tasks[*route.rbegin()].lt : worker.cw;
			updateWorkerInLine(workerId, curTime);
		}
	} else {
		j = 0, k = i;
		while (k < sz)
			route[j++] = route[k++];
		route.erase(route.begin()+i, route.end());

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

vector<int> updateBusyWorkers(vector<pair<int,double> >& busyWorkers, double curTime) {
	int sz = busyWorkers.size(), i = 0;
	vector<int> ret;
	position_t pre, cur;
	double t;

	for (i=0; i<sz; ++i) {
		if (busyWorkers[i].second > curTime)
			break;
		int workerId = busyWorkers[i].first;
		worker_t& worker = workers[workerId];
		if (worker.route.empty())
			pre = worker.cw;
		else
			pre = tasks[*worker.route.rbegin()].lt;
		cur = worker.dw;
		t = Length(pre, cur) + busyWorkers[i].first;
		if (curTime+Length(pre,cur) < worker.ew) {
			ret.push_back(workerId);
		}
		worker.route.clear();
	}
	if (sz>0 && i>0)
		busyWorkers.erase(busyWorkers.begin(), busyWorkers.begin()+i);

	return ret;
}

void updateFreeWorkers(vector<int>& freeWorkers, double curTime) {
	int sz = freeWorkers.size();
	int workerId;

	for (int i=sz-1; i>=0; --i) {
		workerId = freeWorkers[i];
		worker_t& worker = workers[workerId];
		if (max(worker.sw,curTime)+Length(worker.cw, worker.dw) <= worker.ew) {
			freeWorkers[i] = *freeWorkers.rbegin();
			freeWorkers.pop_back();
		}
		worker.sw = curTime;
	}
}

void insertToBusyWorkers(vector<pair<int,double> >& busy, pair<int,double> p) {
	busy.push_back(p);
	int sz = busy.size(), i;

	for (i=sz-2; i>=0; --i) {
		if (p.second < busy[i].second) {
			busy[i+1] = busy[i];
		} else {
			break;
		}
	}
	busy[i+1] = p;
}

bool assignToFreeWorkers(int taskId, vector<int>& freeWorkers, vector<pair<int,double> >& busyWorkers) {
	double curTime = tasks[taskId].rt;
	int sz = freeWorkers.size();
	int workerId;
	position_t lt = tasks[taskId].lt;
	double minCost = inf, tmpCost;
	int v = -1;

	for (int i=0; i<sz; ++i) {
		workerId = freeWorkers[i];
		worker_t& worker = workers[workerId];
		if (worker.sw+Length(worker.cw,lt)+Length(lt,worker.dw) <= worker.ew) {
			tmpCost = Length(worker.cw,lt)+Length(lt,worker.dw) - Length(worker.cw, worker.dw);
			if (tmpCost < minCost) {
				minCost = tmpCost;
				v = i;
			}
		}
	}

	if (v == -1)
		return false;

	{// insert into busyWorkers
		workerId = freeWorkers[v];
		worker_t& worker = workers[workerId];
		double t = worker.sw + Length(worker.cw, lt);
		pair<int,double> p = make_pair(workerId, t);
		insertToBusyWorkers(busyWorkers, p);
		worker.route.push_back(taskId);
		mark[taskId] = true;
	}
	{// remove from freeWorkers
		freeWorkers[v] = *freeWorkers.rbegin();
		freeWorkers.pop_back();
	}

	return true;
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
		if (tasks[taskId].et <= curTime) {
			taskPool[i] = *taskPool.rbegin();
			taskPool.pop_back();
		}
	}
}

double routeLength(int workerId) {
	worker_t& worker = workers[workerId];
	position_t pre = worker.cw, cur;
	int sz = worker.route.size();
	double ret = 0;

	for (int i=0; i<sz; ++i) {
		cur = tasks[worker.route[i]].lt;
		ret += Length(pre, cur);
		pre = cur;
	}

	return ret;
}

double solve() {
	int workerNum = 0, taskNum = 0;
	double curTime;
	vector<int> taskPool, freeWorkers;
	vector<pair<int,double> > busyWorkers;

	init();
	for (int i=0; i<n+m; ++i) {
		if (orders[i] >= n) {// it is a task
			curTime = tasks[taskNum].rt;
		} else {// it is a worker
			curTime = workers[workerNum].sw;
		}
		vector<int> newQuery = updateBusyWorkers(busyWorkers, curTime);
		updateFreeWorkers(freeWorkers, curTime);
		updateExpiredTasks(taskPool, curTime);
		if (orders[i] >= n) {// it is a task
			if (!assignToFreeWorkers(taskNum, freeWorkers, busyWorkers)) {
				taskPool.push_back(taskNum);
			}
			++taskNum;
		} else {// it is a worker
			newQuery.push_back(workerNum);
			++workerNum;
		}
		for (int j=newQuery.size()-1; j>=0; --j) {
			int workerId = newQuery[j];
			vector<int> Cand = filterCand(workerId, taskPool, curTime);
			bool flag = queryOneWorker(workerId, Cand, curTime);
			if (flag) {
				pair<int,double> p = make_pair(workerId, curTime+routeLength(workerId));
				insertToBusyWorkers(busyWorkers, p);
			} else {
				freeWorkers.push_back(workerId);
			}
		}
		updateAssignedTasks(taskPool);
	}

	return calcUtility();
}

int main(int argc, char **argv) {
	string execName("ignore");
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
