#include "Flow.h"
#include "Packet.h"

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






void HandleFlow(map <int, Flow> *flowHashTable, std::shared_ptr <Packet> new_packet, int hash, int default_last)
{
	map <int, Flow> ::iterator findFlow2 = flowHashTable->find(hash);
	//Flow *new_flow;
	//new_flow = new Flow();
	auto new_flow = std::shared_ptr <Flow>(new Flow());

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

int CalculateWeights(map <int, Flow> flowHashTable, std::shared_ptr <Packet> new_packet, int hash, int sum_of_weights)
{

	map <int, Flow> ::iterator findFlow2 = flowHashTable.find(hash);
	if (findFlow2 == flowHashTable.end() || findFlow2->second.numOfEntries == 0)
		sum_of_weights = sum_of_weights + new_packet->GetWeight();
	return sum_of_weights;
}

std::shared_ptr <Packet> ProcessPacket(char * newLine) {

	int location = 0,
		time = 0,
		Sport = 0,
		Dport = 0,
		length = 0;
	char *tmpEntry[9] = { '\0' },
		*token,
		*nextToken;
	//Packet *new_packet = NULL;

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

	std::shared_ptr <Packet> new_packet = std::shared_ptr <Packet>(new Packet(time, tmpEntry[1], Sport, tmpEntry[3], Dport, length));
	//new_packet = new Packet(time, tmpEntry[1], Sport, tmpEntry[3], Dport, length); //DONT FORGET TO FREE MEMORY - THIS IS IN THE HEAP!!

	if (tmpEntry[6] == NULL)
		new_packet->SetWeight(DEFAULT_WEIGHT);
	else
		new_packet->SetWeight(atoi(tmpEntry[6]));

	new_packet->SentinWFQ = 0;
	new_packet->CalculateHash();
	return new_packet;




}

void SendPacketWFQ(std::shared_ptr <Packet> data, int Time)
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
	fprintf(stdout, " ");
	fprintf(stdout, "\n");

}

int HandleLeavingPacket(std::shared_ptr <Packet> leaving_packet, int curr_time)
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




