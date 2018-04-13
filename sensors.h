#ifndef SENSORS_H
#define SENSORS_H
#include "BrickPi3.h"
#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <vector>
using namespace std;

/**  
 * This struct stores the highest and lowest values from the color and light
 * sensor
 */
struct sensorData{
	int highestRed; /**< Highest red reflection from colorsensor */
	int lowestRed; /**< Lowest red reflection from colorsensor */
	int highestReflection; /**< Highest reflection from light sensor */
	int lowestReflection; /**< lowest reflection from light sensor */
};

/** 
 * @param BP the brick pi controller
 * @param colors an empty vector for color sensor structs
 * @param lights an empty vector for light sensor structs
 * 
 * Measures color and light values for 2.5s, after which it will move backwards 
 * and measure again for 2.5s.
 * Measurements are stored in sensor structs, which are pushed in a vector.
 */
void calibrateLineSensors(BrickPi3 BP, vector<sensor_color_t> & colors, vector<sensor_light_t> & lights);

/**
 * @param colors vector filled with colorsensor readings
 * @param sensorReads empty vector in which to store processed data
 * 
 * Loops through the given vector of sensor structs, saving the lowest and 
 * highest reflected values in a vector
 */
void processCalibrationColor(const vector<sensor_color_t> & colors, vector<vector<int>> & sensorReads);

/**
 * @param lights vector filled with lightsensor readings
 * @param sensorReads empty vector in which to store processed data
 * 
 * Loops through the given vector of sensor structs, saving the lowest and 
 * highest reflected values in a vector
 */ 
void processCalibrationLight(const vector<sensor_light_t> & lights, vector<vector<int>> & sensorReads);

/** 
 * @param BP the brickpi controller
 * 
 * Initializes a struct sensorData and adds the gathered values from 
 * processCalibrationColor and processCalibrationLight
 */
sensorData processCalibration(BrickPi3 BP);

/** 
 * @param s the sensor data from which to print data
 * 
 * Prints the given struct in cout. 
 */ 
void printSensorCalibration(const sensorData & sensorReads);

/**
 * @param s sensorData with the calibration required
 * @param tollerance integer which indicates a percentage of tollerance (10 = 10%)
 * @param colorSensorReading integer of the value measured by the colorsensor
 * @return true if given colorSensorReading is between sensordata.lowestRed values with tollerance
 * 
 * This function determines of a given value of the colorsensor is on a black line.
 * It checks if the given value is between the lowestRed value of the sensordata.
 * LowestRed is increased and decreased with the tollerance to give a range of 
 * acceptable values.
 */
bool isColorOnBlack(const sensorData & s, const int & tollerance, const int & colorSensorReading);

/**
 * @param s sensorData with the calibration required
 * @param tollerance integer which indicates a percentage of tollerance (10 = 10%)
 * @param lightSensorReading integer of the value measured by the lightsensor
 * @return True if given lightSensorReading matches between sensorData.highestReflection with tollerance
 *  
 * This function determines of a given value of the lightsensor is on a black line.
 * It checks if the given value is between the lowestRed value of the sensordata.
 * LowestRed is increased and decreased with the tollerance to give a range of 
 * acceptable values.
 */
bool isLightOnBlack(const sensorData & s, const int & tollerance, const int & lightSensorReading);

/**
 * @param BP The brick pi controller
 * @return The distance that the sensor reads.
 */
int16_t ultrasoon_detectie(BrickPi3 BP,sensor_ultrasonic_t afstand);

#endif /* SENSORS_H */