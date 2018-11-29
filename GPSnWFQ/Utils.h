#pragma once

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
#include <map>

using namespace std;
class Packet;
class Flow;



enum NextEvent
{
	Arrival = 1,
	Leaving = 0
};


void SendPacketWFQ(std::shared_ptr <Packet> data, int Time);
std::shared_ptr <Packet> ProcessPacket(char * newLine);
int HandleLeavingPacket(std::shared_ptr <Packet> leaving_packet, int curr_time);
void HandleFlow(map <int, Flow> *flowHashTable, std::shared_ptr <Packet> new_packet, int hash, int default_last);
int CalculateWeights(map <int, Flow> flowHashTable, std::shared_ptr <Packet> new_packet, int hash, int sum_of_weights);
