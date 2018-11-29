#pragma once


#define ADD_SIZE 17
#define DEFAULT_WEIGHT 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <iterator>
#include <iostream>
#include <algorithm> 
#include <queue>
#include <list>
#include <memory>

using namespace std;




class Packet
{
public:
	Packet(int timeI, char SaddI[ADD_SIZE], int SportI, char DaddI[ADD_SIZE], int DportI, int lengthI);
	~Packet() {};

	void CalculateHash(void);
	int CalculateHashHelper(char *str);
	int GetHash(void) const;
	int GetTime(void) const;
	int GetLength(void);
	float GetLast(void) const;
	int GetWeight(void);
	float GetRound(void);
	char* GetSadd(void);
	char* GetDadd(void);
	int GetSport(void);
	int GetDport(void);
	int SentinWFQ;



	void SetWeight(int weight_val);
	void SetLast(float last_val);
	void SetRound(float round_val);
	float CalculateRound(float round_t, float curr_time, int sum_of_weights, int time);
	float CalculateLast(float round_t, float last, int weight, int length);
	float CalculateX(float lastVal, float round_last, int sum_of_weights);


private:
	int _hash;
	char _Sadd[ADD_SIZE];
	int _Sport;
	char _Dadd[ADD_SIZE];
	int _Dport;
	int _time;
	int _length;
	float _last;
	int _weight;
	float _round;

};



