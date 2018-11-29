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
#include <memory>

#include "Flow.h"



Flow::Flow() {}

//Flow::Flow() : packets_q() {};
int Flow::GetWeight(void) { return _weightFlow; };

void Flow::SetWeight(int value) { _weightFlow = value; };

void Flow::SetLast(float value) { _lastVal = value; };

float Flow::GetLastVal(void) { return _lastVal; };

