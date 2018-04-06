#ifndef MOTORS_H
#define MOTORS_H
#include "BrickPi3.h"
#include "sensors.h"
#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <vector>
using namespace std;

void startSteering(BrickPi3 BP, const sensorData & sensorReads);

#endif