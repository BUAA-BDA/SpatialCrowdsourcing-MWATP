/**
	\author: 	Trasier
	\date: 		2017.12.11
*/
#ifndef INPUT_H
#define INPUT_H

#include <bits/stdc++.h>
using namespace std;

#include "global.h"

void readInput(const string& fileName, int& N, int& M, worker_t*& workers, task_t*& tasks);
void readInput(istream& fin, int& N, int& M, worker_t*& workers, task_t*& tasks);
void reOrder(int N, int M,  worker_t* workers, task_t* tasks, int*& orders);

#endif

/***
Line 1: numWorker numTask
Line 2-（numWorker+numTask+1）: Type（0：task，1：worker）
If type is a task：   0 location_x location_y releaseTime expireTime Utility
If type is a worker： 1 source_x source_y target_x target_y releaseTime expireTime
*/
