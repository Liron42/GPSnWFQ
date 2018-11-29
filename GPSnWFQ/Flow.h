#pragma once

#define LINE_SIZE 100
#define DEFAULT_ENTRIES 1

#include <map>
#include <iostream>
#include <queue>

using namespace std;


class Flow
{
public:
	Flow();
	~Flow() {};
	int GetWeight(void);
	float GetLastVal(void);
	void SetWeight(int value);
	void SetLast(float value);
	int numOfEntries = 0;

private:
	float _lastVal;
	int _weightFlow = -1;

};
