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
 * @param steps The ammount of steps the function needs to make
 * @param sleepCount The ammount the robot needs to sleep for in seconds, default value of 1 second
 * 
 * This function wil turn the robot on it's axis.
 */
void steering(BrickPi3 BP, const string & direction, const int & steps, const float & sleepCount = 1.0);

/**
 * @param BP The BrickPi3 controller
 * @param sensorReads The struct of the calibration data
 * 
 * This function will detect objects infront of the robot
 */
void detectObject(BrickPi3 BP, const sensorData & sensorReads);

/** 
 * @param BP The BrickPi3 controller
 * @param sensorReads The struct of the calibration data
 * 
 * Swerve is the function that will dodge any objects the robots sees in the detectObject function.
 */
void swerve(BrickPi3 BP, const sensorData & sensorReads);

/**
 * @param BP The BrickPi3 controller
 * @param turnDirection The direction the robot wil turn to
 * @param sensorReads The struct of the calibration data
 * 
 * This function will try to find a line using previously acquired data in the direction given.
 */
void findLine(BrickPi3 BP, string turnDirection, const sensorData & sensorReads);
#endif