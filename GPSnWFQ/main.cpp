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
#include <list> 

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

Flow::Flow(){}

//Flow::Flow() : packets_q() {};
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

	new_packet->SentinWFQ = 0;
	new_packet->CalculateHash();
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

	//x = time_f - curr_time;
	x = time - curr_time;

	if (sum_of_weights_f == 0)
		res = (round_t + (x / 1));
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


void HandleFlow(map <int, Flow> *flowHashTable, Packet* new_packet,int hash,int default_last)
{
	map <int, Flow> ::iterator findFlow2 = flowHashTable->find(hash);
	Flow *new_flow;
	new_flow = new Flow();

	if (findFlow2 != flowHashTable->end())
	{
		if (new_packet->GetWeight() != DEFAULT_WEIGHT)
			findFlow2->second.SetWeight(new_packet->GetWeight());
		else
			new_packet->SetWeight(findFlow2->second.GetWeight());
	}

	else
	{
		
		new_flow->SetWeight(new_packet->GetWeight());
		new_flow->SetLast(default_last);
		flowHashTable->insert(make_pair(hash, *new_flow));
		
	}

}

int CalculateWeights(map <int, Flow> flowHashTable, Packet* new_packet,int hash, int sum_of_weights)
{

	map <int, Flow> ::iterator findFlow2 = flowHashTable.find(hash);
	if (findFlow2 == flowHashTable.end() || findFlow2->second.numOfEntries == 0)
		sum_of_weights = sum_of_weights + new_packet->GetWeight();
	return sum_of_weights;
}

Packet* DecideWFQ(std::priority_queue<Packet*, std::vector<Packet*>, LessThanByLast> *packetsGPS_q)
{
	list <Packet*> queueSnapshot;
	list <Packet*>::iterator it;
	list <Packet*>::iterator it2;
	std::priority_queue<Packet*, std::vector<Packet*>, LessThanByLast> temp_q;
	int i = 0;

		while (!packetsGPS_q->empty())
		{
			temp_q.push(packetsGPS_q->top());
			queueSnapshot.push_back(packetsGPS_q->top());
			packetsGPS_q->pop();
			i++;
		}
		
		for (it = queueSnapshot.begin(); it != queueSnapshot.end(); ++it) {
			
			if ((*it)->SentinWFQ != 1)
			{
				(*it)->SentinWFQ = 1;
				break;
			}
			}
		for (it2 = queueSnapshot.begin(); it2 != queueSnapshot.end(); ++it2) {

			packetsGPS_q->push((*it2));
		}

		if (it != queueSnapshot.end())
			return (*it);
		else
			return NULL;
}

int HandleLeavingPacket(Packet* leaving_packet, int curr_time)
{
	if (curr_time < leaving_packet->GetTime())
		SendPacketWFQ(leaving_packet, leaving_packet->GetTime());
	else
		SendPacketWFQ(leaving_packet, curr_time);
	if (curr_time < leaving_packet->GetTime())
		curr_time = leaving_packet->GetTime() + leaving_packet->GetLength();
	else
		curr_time = curr_time + leaving_packet->GetLength();

	return curr_time;
}

int main()
{
	char newLine[LINE_SIZE];
	Packet *new_packet = NULL,
		*leaving_packet;
	int hash = 0,
		sum_of_weights = 0,
		curr_time = 0;
	float default_last = 0,
		round_t = 0,
		minLast = 0,
		GPS_time = 0,
		curr_round = 0,
		x_new = 0;
	

	NextEvent next_event = Arrival;

	map <int, Flow> flowHashTable;  map <int,Flow> :: iterator findFlow, findFlow2;
	std::priority_queue<Packet*, std::vector<Packet*>, LessThanByLast> packetsGPS_q;

		
	while (fgets(newLine, LINE_SIZE, stdin) != NULL) {

		new_packet = ProcessPacket(newLine);
		hash = new_packet->GetHash();
		
		HandleFlow(&flowHashTable, new_packet, hash, default_last); 
		
		curr_round = new_packet->CalculateRound(round_t, GPS_time, sum_of_weights, new_packet->GetTime());

		if (packetsGPS_q.empty())
			round_t = curr_round;

			while (!packetsGPS_q.empty())
			{

				minLast = packetsGPS_q.top()->GetLast();
				if (curr_round <= minLast)
					next_event = Arrival;
				else
					next_event = Leaving;
				
				if (next_event == Arrival)
				{

					if (new_packet->GetTime() > curr_time) { leaving_packet = DecideWFQ(&packetsGPS_q); }
					while (leaving_packet != NULL && new_packet->GetTime() > curr_time)
					{
						curr_time = HandleLeavingPacket(leaving_packet, curr_time);
						if (new_packet->GetTime() > curr_time) { leaving_packet = DecideWFQ(&packetsGPS_q); }
					}
					round_t = curr_round;
					goto HandlePacket;
				}

				if (next_event == Leaving)
				{

					leaving_packet = packetsGPS_q.top();
					if (leaving_packet->SentinWFQ == 0)
						curr_time = HandleLeavingPacket(leaving_packet, curr_time);

					x_new = new_packet->CalculateX(leaving_packet->GetLast(), round_t, sum_of_weights);
					round_t = leaving_packet->GetLast();
					GPS_time = GPS_time + x_new;
					packetsGPS_q.pop();
					findFlow2 = flowHashTable.find(leaving_packet->GetHash());
					findFlow2->second.numOfEntries--;
					if (findFlow2->second.numOfEntries == 0 && sum_of_weights != 0)
						sum_of_weights = sum_of_weights - leaving_packet->GetWeight();

					if (sum_of_weights != 0) { 
						curr_round = new_packet->CalculateRound(round_t, GPS_time, sum_of_weights, new_packet->GetTime()); 
					}
				}
		}


HandlePacket:
			GPS_time = new_packet->GetTime();
			findFlow = flowHashTable.find(hash);
			if (findFlow != flowHashTable.end())
				new_packet->SetLast(new_packet->CalculateLast(round_t, findFlow->second.GetLastVal(), new_packet->GetWeight(), new_packet->GetLength()));
			else
				new_packet->SetLast(new_packet->CalculateLast(round_t, default_last, new_packet->GetWeight(), new_packet->GetLength()));

			sum_of_weights = CalculateWeights(flowHashTable, new_packet, hash,sum_of_weights);
			findFlow->second.SetLast(new_packet->GetLast());
			findFlow->second.numOfEntries++;
			new_packet->SetRound(round_t);
			packetsGPS_q.push(new_packet);

	}

	if (new_packet->GetTime() > curr_time) { leaving_packet = DecideWFQ(&packetsGPS_q); }
	while (leaving_packet != NULL && new_packet->GetTime() > curr_time)
	{
		curr_time = HandleLeavingPacket(leaving_packet, curr_time);
		if (new_packet->GetTime() > curr_time) { leaving_packet = DecideWFQ(&packetsGPS_q); }
	}

}
