// GPSnWFQ.cpp : Defines the entry point for the console application.
//

#include "Flow.h"
#include "Packet.h"
#include "Utils.h"

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

struct LessThanByLast
{
	bool operator()(std::shared_ptr <Packet> packet1, std::shared_ptr <Packet> packet2) const
	{
		if (packet1->GetLast() == packet2->GetLast())
			return packet1->GetTime() > packet2->GetTime();
		if (packet1->GetHash() == packet2->GetHash())
			return packet1->GetTime() > packet2->GetTime();
		return packet1->GetLast() > packet2->GetLast();
	}
};

std::shared_ptr <Packet> DecideWFQ(std::priority_queue<Packet*, std::vector<std::shared_ptr <Packet>>, LessThanByLast> *packetsGPS_q)
{
	list <std::shared_ptr <Packet>> queueSnapshot;
	list <std::shared_ptr <Packet>>::iterator it;
	list <std::shared_ptr <Packet>>::iterator it2;
	std::priority_queue<Packet*, std::vector<std::shared_ptr <Packet>>, LessThanByLast> temp_q;
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



int main()
{
	char newLine[LINE_SIZE];

	std::shared_ptr <Packet> leaving_packet = NULL,
		new_packet = NULL;
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

	map <int, Flow> flowHashTable;  map <int, Flow> ::iterator findFlow, findFlow2;
	std::priority_queue<Packet*, std::vector<std::shared_ptr <Packet>>, LessThanByLast> packetsGPS_q;


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

				if (sum_of_weights != 0) 
					curr_round = new_packet->CalculateRound(round_t, GPS_time, sum_of_weights, new_packet->GetTime());
				leaving_packet.reset();

			}
		}


	HandlePacket:
		GPS_time = new_packet->GetTime();
		findFlow = flowHashTable.find(hash);
		if (findFlow != flowHashTable.end())
			new_packet->SetLast(new_packet->CalculateLast(round_t, findFlow->second.GetLastVal(), new_packet->GetWeight(), new_packet->GetLength()));
		else
			new_packet->SetLast(new_packet->CalculateLast(round_t, default_last, new_packet->GetWeight(), new_packet->GetLength()));

		sum_of_weights = CalculateWeights(flowHashTable, new_packet, hash, sum_of_weights);
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
		leaving_packet.reset();
	}
	flowHashTable.clear();
}
