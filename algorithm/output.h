/**
	\author: 	Trasier
	\date: 		2017.12.11
*/
#ifndef OUTPUT_H
#define OUTPUT_H

#include <bits/stdc++.h>
using namespace std;

void dumpResult(const string& execName, double ans, double usedTime=-1.0, double usedMemory=-1.0);

void dumpResult(const string& execName, double ans, double usedTime, double usedMemory) {
	printf("%s %.3lf", execName.c_str(), ans);
	if (usedTime >= 0)
		printf(" %.3lf", usedTime);
	if (usedMemory >= 0)
		printf(" %.3lf", usedMemory);
	putchar('\n');
}

#endif
