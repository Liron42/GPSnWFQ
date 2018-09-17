// GPSnWFQ.cpp : Defines the entry point for the console application.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <iterator>
#include "stdafx.h"
#include <iostream>
#include <algorithm> 
#include <queue>

using namespace std;


#include "Data.h"

enum NextEvent
{
	Arrival = 1,
	Leaving = 0
};


Packet::Packet(int timeI,char SaddI[ADD_SIZE], int SportI, char DaddI[ADD_SIZE], int DportI, int lengthI)
{
	_time = timeI;
	_Sport = SportI;
	_Dport = DportI;
	_length = lengthI;
	strcpy_s(_Sadd,ADD_SIZE, SaddI);
	strcpy_s(_Dadd, ADD_SIZE, DaddI);

}

Flow::Flow() : packets_q() {};
int Flow::GetWeight(void) { return _weightFlow; };
void Flow::SetWeight(int value) { _weightFlow = value; };
void Flow::SetLast(float value) { _lastVal = value; };
float Flow::GetLastVal(void) { return _lastVal; };

Packet* ProcessPacket(char * newLine) {

	int location = 0,
		time = 0,
		Sport = 0,
		Dport = 0,
		length = 0;
	char *tmpEntry[9] = { '\0' },
		*token,
		*nextToken;
	Packet *new_packet = NULL;

	token = strtok_s(newLine, " ", &nextToken);
	tmpEntry[location] = token;
	location++;
	token = strtok_s(NULL, " ", &nextToken);
	while (token != NULL) {
		tmpEntry[location] = token;
		location++;
		if (strchr(token, '\n') != NULL) //remove '\n'
			token[strlen(token) - 1] = '\0';
		token = strtok_s(NULL, " ", &nextToken);
	}
	time = atoi(tmpEntry[0]); Sport = atoi(tmpEntry[2]); Dport = atoi(tmpEntry[4]); length = atoi(tmpEntry[5]);

	new_packet = new Packet(time, tmpEntry[1], Sport, tmpEntry[3], Dport, length); //DONT FORGET TO FREE MEMORY - THIS IS IN THE HEAP!!

	if (tmpEntry[6] == NULL)
		new_packet->SetWeight(DEFAULT_WEIGHT);
	else
		new_packet->SetWeight(atoi(tmpEntry[6]));

	return new_packet;




}

void Packet::CalculateHash(void)
{
	_hash = (CalculateHashHelper(_Sadd) + (_Sport) + CalculateHashHelper(_Dadd) + (_Dport));
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
		sum_of_weights_f = sum_of_weights,res = 0;

	x = time_f - curr_time;

	if (sum_of_weights_f == 0)
		return 0;
	else
		res =  (round_t + (x / sum_of_weights_f));
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
int Packet::GetHash(void) const {return _hash;}
int Packet::GetTime(void) const { return _time; };
int Packet::GetLength(void) { return _length; };
float Packet::GetLast (void) const { return _last; } ;
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

void SendPacketWFQ(Packet *data, int Time)
{

	fprintf(stdout, "%d", Time);
	fprintf(stdout, ": ");
	fprintf(stdout, "%d", data->GetTime());
	fprintf(stdout, " ");
	fprintf(stdout, data->GetSadd());
	fprintf(stdout, " ");
	fprintf(stdout, "%d", data->GetSport());
	fprintf(stdout, " ");
	fprintf(stdout, data->GetDadd());
	fprintf(stdout, " ");
	fprintf(stdout, "%d", data->GetDport());
	fprintf(stdout, " ");
	fprintf(stdout, "%d", data->GetLength());
	/*
	if (data->originWeight == 1)
	{
		fprintf(stdout, " ");
		fprintf(stdout, "%d", data->weight);
	}
	*/
	fprintf(stdout, " ");
	fprintf(stdout, "\n");

}

void FillWFQq(std::priority_queue<Packet*, std::vector<Packet*>, LessThanByLast> *packetsWFQ_q, std::queue<Packet*> *packetsWFQ_q_inter)
{
	int size_inter = packetsWFQ_q_inter->size();
	for (int i = 0; i < size_inter; i++)
	{
		packetsWFQ_q->push(packetsWFQ_q_inter->front());
		packetsWFQ_q_inter->pop();
	}
}

int HandleLeavingPacket(int curr_time, std::priority_queue<Packet*, std::vector<Packet*>, LessThanByLast> *packetsWFQ_q, Packet* new_packet)
{
	while (!packetsWFQ_q->empty() && packetsWFQ_q->top()->GetTime() <= curr_time && curr_time <= new_packet->GetTime())
	{
		Packet* to_send = packetsWFQ_q->top();
		packetsWFQ_q->pop();
		if (curr_time < to_send->GetTime())
			curr_time = to_send->GetTime();
		SendPacketWFQ(to_send, curr_time);

		if (curr_time + (to_send->GetLength()) > new_packet->GetTime())
			curr_time = curr_time + to_send->GetLength();

		else if (curr_time + (to_send->GetLength()) <= new_packet->GetTime())
		{
			if (!packetsWFQ_q->empty())
				curr_time = curr_time + to_send->GetLength();
			else
				curr_time = new_packet->GetTime();

		}
	}
	return curr_time;
}

Flow HandleFlow(map <int, Flow> *flowHashTable, Packet* new_packet,int hash,int default_last)
{
	map <int, Flow> ::iterator findFlow2 = flowHashTable->find(hash);
	Flow new_flow;

	if (findFlow2 != flowHashTable->end())
	{
		if (new_packet->GetWeight() != DEFAULT_WEIGHT)
			findFlow2->second.SetWeight(new_packet->GetWeight());
		else
			new_packet->SetWeight(findFlow2->second.GetWeight());
	}

	else
	{
		
		new_flow.SetWeight(new_packet->GetWeight());
		new_flow.SetLast(default_last);
		
	}
	return new_flow;
}

int CalculateTempWeight(map <int, Flow> flowHashTable, Packet* new_packet,int hash)
{
	int temp_weight = 0;

	map <int, Flow> ::iterator findFlow2 = flowHashTable.find(hash);
	if (findFlow2 == flowHashTable.end() || findFlow2->second.packets_q.empty())
		temp_weight = new_packet->GetWeight();
	return temp_weight;
}

int main()
{
	char newLine[LINE_SIZE];
	Packet *new_packet = NULL,
		*leaving_packet;
	int hash = 0,
		sum_of_weights = 0,
		curr_time = 0,
		temp_weight = 0,
		GPS_time = 0,
		size_inter = 0;
	float default_last = 0,
		round_t = 0,	
		minLast = 0,
		round_new = 0,
		x_new = 0;
	

	NextEvent next_event = Arrival;

	map <int, Flow> flowHashTable;  map <int,Flow> :: iterator findFlow1, findFlow2,findFlow3;
	std::priority_queue<Packet*, std::vector<Packet*>, LessThanByLast> packetsGPS_q;
	std::priority_queue<Packet*, std::vector<Packet*>, LessThanByLast> packetsWFQ_q;
	std::queue<Packet*> packetsWFQ_q_inter;

		
	while (fgets(newLine, LINE_SIZE, stdin) != NULL) {

		new_packet = ProcessPacket(newLine);
		/*
		if (new_packet->GetTime() == 309910)
			findFlow1 = flowHashTable.find(309910);
			*/
		if (new_packet->GetTime() > curr_time) {
			FillWFQq(&packetsWFQ_q, &packetsWFQ_q_inter);
			if (packetsWFQ_q.empty())
			{
				packetsWFQ_q.push(packetsGPS_q.top());
				findFlow1 = flowHashTable.find(packetsGPS_q.top()->GetHash());
				findFlow1->second.packets_q.pop();
				if (findFlow1->second.packets_q.empty())
					sum_of_weights = sum_of_weights - packetsGPS_q.top()->GetWeight();
				packetsGPS_q.pop();
			}
			curr_time = HandleLeavingPacket(curr_time, &packetsWFQ_q, new_packet);
		}

			
		
		new_packet->CalculateHash();
		hash = new_packet->GetHash();
		
		Flow new_flow = HandleFlow(&flowHashTable, new_packet, hash, default_last);
		if(new_flow.GetWeight() != -1) { flowHashTable[hash] = new_flow; }


		if (packetsGPS_q.size() == 0)
			minLast = 0;
		else
			minLast = packetsGPS_q.top()->GetLast();

		if (GPS_time == 0 && new_packet->GetTime() != 0)
			round_t = new_packet->CalculateRound(round_t, GPS_time, sum_of_weights, new_packet->GetTime());
		
		temp_weight = CalculateTempWeight(flowHashTable, new_packet, hash);
	
		findFlow2 = flowHashTable.find(hash);
		if (findFlow2 != flowHashTable.end())
			new_packet->SetLast(new_packet->CalculateLast(round_t, findFlow2->second.GetLastVal(), new_packet->GetWeight(), new_packet->GetLength()));
		else
			new_packet->SetLast(new_packet->CalculateLast(round_t, default_last, new_packet->GetWeight(), new_packet->GetLength()));

		round_t = new_packet->CalculateRound(round_t, GPS_time, sum_of_weights + temp_weight, new_packet->GetTime());
		if (round_t <= minLast)
			next_event = Arrival;
		else
			next_event = Leaving;

		if (next_event == Arrival)
		{
			new_packet->SetRound(round_t);
			packetsGPS_q.push(new_packet);
			findFlow2 = flowHashTable.find(hash);
			findFlow2->second.packets_q.push(new_packet);
			sum_of_weights = sum_of_weights + temp_weight;

		}

		if (next_event == Leaving)
		{
			packetsGPS_q.push(new_packet);
			findFlow2 = flowHashTable.find(hash);
			findFlow2->second.packets_q.push(new_packet);

			leaving_packet = packetsGPS_q.top();
			packetsGPS_q.pop();
			packetsWFQ_q_inter.push(leaving_packet);
			if (leaving_packet->GetHash() == new_packet->GetHash())
				findFlow2->second.packets_q.pop();
			
			x_new = new_packet->CalculateX(leaving_packet->GetLast(), leaving_packet->GetRound(), sum_of_weights);
			
			findFlow3 = flowHashTable.find(leaving_packet->GetHash());
			if (findFlow3->second.packets_q.empty())
				sum_of_weights = sum_of_weights - leaving_packet->GetWeight();

			temp_weight = CalculateTempWeight(flowHashTable, new_packet, hash);

			sum_of_weights = sum_of_weights + temp_weight;
			if (sum_of_weights == 0) { round_new = 0; }
			else { round_new = leaving_packet->GetLast() + (x_new / (sum_of_weights)); }
			
			new_packet->SetRound(round_new);
			round_t = round_new;


		}
		GPS_time = new_packet->GetTime();

	}
	if (!packetsWFQ_q_inter.empty())
	{
		FillWFQq(&packetsWFQ_q, &packetsWFQ_q_inter);
		curr_time = HandleLeavingPacket(curr_time, &packetsWFQ_q, new_packet);
	}
		


}
