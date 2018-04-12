#ifndef MOTORS_H
#define MOTORS_H
#include "BrickPi3.h"
#include "sensors.h"
#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <vector>
using namespace std;

void startSteering(BrickPi3 BP, const sensorData & sensorReads, bool obstacles);
void steering(BrickPi3 BP, const string & direction, const int & steps);
void detectObject(BrickPi3 BP);
void swerve(BrickPi3 BP);
void findLined(BrickPi3 BP, string turnDirection);
#endif