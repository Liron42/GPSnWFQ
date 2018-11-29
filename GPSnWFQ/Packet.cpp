
#include "Flow.h"
#include "Packet.h"



bool operator< (const Packet& packet1, const Packet& packet2)
{
	return packet1.GetTime() < packet2.GetTime();
}


Packet::Packet(int timeI, char SaddI[ADD_SIZE], int SportI, char DaddI[ADD_SIZE], int DportI, int lengthI)
{
	_time = timeI;
	_Sport = SportI;
	_Dport = DportI;
	_length = lengthI;
	strcpy_s(_Sadd, ADD_SIZE, SaddI);
	strcpy_s(_Dadd, ADD_SIZE, DaddI);

}

void Packet::CalculateHash(void)
{
	_hash = (CalculateHashHelper(_Sadd) + (_Sport)+CalculateHashHelper(_Dadd) + (_Dport));
}

int Packet::CalculateHashHelper(char *str)
{
	int hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash % 5381;
}

float Packet::CalculateRound(float round_t, float curr_time, int sum_of_weights, int time)
{
	float x = 0;
	float time_f = time,
		sum_of_weights_f = sum_of_weights, res = 0;

	//x = time_f - curr_time;
	x = time - curr_time;

	if (sum_of_weights_f == 0)
		res = (round_t + (x / 1));
	else
		res = (round_t + (x / sum_of_weights_f));
	return res;
}

float Packet::CalculateLast(float round_t, float last, int weight, int length) {

	float weight_f = weight;
	float length_f = length;

	if (weight_f == 0)
		return 0;
	else
		return ((std::max(round_t, last) + (length_f / weight_f)));
}

int Packet::GetHash(void) const { return _hash; }

int Packet::GetTime(void) const { return _time; };

int Packet::GetLength(void) { return _length; };

float Packet::GetLast(void) const { return _last; };

int Packet::GetWeight(void) { return _weight; };

float Packet::GetRound(void) { return _round; };

char* Packet::GetSadd(void) { return _Sadd; };

char* Packet::GetDadd(void) { return _Dadd; };

int Packet::GetSport(void) { return _Sport; };

int Packet::GetDport(void) { return _Dport; };

void Packet::SetWeight(int weight_val) { _weight = weight_val; };

void Packet::SetLast(float last_val) { _last = last_val; };

void Packet::SetRound(float round_val) { _round = round_val; };

float Packet::CalculateX(float lastVal, float round_last, int sum_of_weights)
{
	return (sum_of_weights*(lastVal - round_last));
}



