#ifndef MOTORS_H
#define MOTORS_H
#include "BrickPi3.h"
#include "sensors.h"
#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <vector>
using namespace std;

/**
 * @brief 
 * @param BP The BrickPi3 controller
 * @param sensorReads Data collected and processed from the calibration
 */
void startSteering(BrickPi3 BP, const sensorData & sensorReads);

#endif