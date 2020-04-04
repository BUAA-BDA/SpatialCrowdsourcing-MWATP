/**
	\author:	Trasier
	\date:		2017.12.11
*/
#ifndef GLOBAL_H
#define GLOBAL_H

// #define GLOBAL_DEBUG

extern const double eps;
extern const double inf;
int dcmp(double x);

struct position_t {
	double x, y;

	position_t(double x=0., double y=0.):
		x(x), y(y) {}

	bool operator<(const position_t& o) const {
		if (x == o.x)
			return y < o.y;
		else
			return x < o.x;
	}

	bool operator==(const position_t& oth) const {
		return dcmp(x-oth.x)==0 && dcmp(y-oth.y)==0;
	}

	bool operator!=(const position_t& oth) const {
		return dcmp(x-oth.x)!=0 || dcmp(y-oth.y)!=0;
	}
};

double Length(const position_t& a, const position_t& b);

struct task_t {
	position_t lt;
	double rt, et, ut;

	bool operator<(const task_t& o) const {
		return rt < o.rt;
	}

	double isExpired() {
		return rt + eps >= et;
	}
};

struct worker_t {
	double sw, ew;
	position_t cw, dw;
	vector<int> route;

	bool isEmpty() const {
		return route.empty();
	}

	bool empty() const {
		return this->isEmpty();
	}

	void push_back(int taskId) {
		route.push_back(taskId);
	}

	void pop_front() {
		if (!route.empty())
			route.erase(route.begin());
	}

	void erase(vector<int>::iterator biter, vector<int>::iterator eiter) {
		route.erase(biter, eiter);
	}

	void clear() {
		route.clear();
	}

	double leftTime() {
		return ew - sw;
	}

	double freeTime() {
		return ew - sw - Length(cw, dw);
	}

	bool isExpired() {
		return Length(cw, dw) + sw + eps >= ew;
	}
};

extern worker_t *workers;
extern task_t *tasks;
extern int *orders;
extern double usedTime;
extern int usedMemory;
extern int n, m;

#endif
