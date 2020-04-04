/**
	\author:	Trasier
	\date:		2017.12.11
*/
#include <bits/stdc++.h>
using namespace std;

#include "global.h"


const double eps = 1e-6;
const double inf = 1e25;
double usedTime = -1.0;
int usedMemory = -1;
worker_t *workers = NULL;
task_t* tasks = NULL;
int* orders = NULL;
int n = 0, m = 0;

int dcmp(double x) {
	if (fabs(x) < eps)
		return 0;
	return x<0 ? -1:1;
}

double Length(const position_t& a, const position_t& b) {
	return sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y));
}
