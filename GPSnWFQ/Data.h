#ifndef DATA_H
#define DATA_H

#define ADD_SIZE 17
#define LINE_SIZE 100
#define DEFAULT_WEIGHT 1
#define DEFAULT_ENTRIES 1

#include <map>
#include <iostream>
#include <queue>

using namespace std;

class Packet
{
public:
	//Packet();
	Packet(int timeI, char SaddI[ADD_SIZE],int SportI, char DaddI[ADD_SIZE],int DportI, int lengthI);
	//~Packet();

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
	float CalculateRound(float round_t, float curr_time, int sum_of_weights,int time);
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

class Flow
{
public:
	Flow();
	int GetWeight(void);
	float GetLastVal(void);
	void SetWeight(int value);
	void SetLast(float value);
	int numOfEntries;
	priority_queue<Packet*, vector<Packet*>, less<vector<Packet*>::value_type> > packets_q;

private:
	float _lastVal;
	int _weightFlow = -1;
	

	


};

bool operator< (const Packet& packet1, const Packet& packet2)
{
	return packet1.GetTime() < packet2.GetTime();
}

struct LessThanByLast
{
	bool operator()(Packet* packet1,Packet* packet2) const
	{
		if (packet1->GetLast() == packet2->GetLast())
			return packet1->GetTime() > packet2->GetTime();
		if (packet1->GetHash() == packet2->GetHash())
			return packet1->GetTime() > packet2->GetTime();
		return packet1->GetLast() > packet2->GetLast();
	}
};





#endif /* DATA_H */
