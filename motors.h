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
 * @param BP the brickpi controller
 * @param sensorReads the struct of the calibration data
 * 
 * Function for following a line. reads the given sensor data and uses it for
 * comparisons between the currently measured data. Based on this the function
 * gives a percentage offset. If one motor has more than 30% offset, the other 
 * motor will drive that same offset in negative direction. This ensures quick
 * turning. 
 */
void startSteering(BrickPi3 BP, const sensorData & sensorReads, bool obstacles);

/**
 * @param BP The BrickPi3 controller
 * @param direction The direction the robot has to turn to
 *
 * This function wil turn the robot on it's axis.
 */
void steering(BrickPi3 BP, const string & direction, const int & steps, const float & sleepCount = 1.0);

void detectObject(BrickPi3 BP, const sensorData & sensorReads);

void swerve(BrickPi3 BP, const sensorData & sensorReads);

void findLine(BrickPi3 BP, string turnDirection, const sensorData & sensorReads);
#endif