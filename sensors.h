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

void calibrateLineSensors(BrickPi3 BP, vector<sensor_color_t> & colors, vector<sensor_light_t> & lights);

void processCalibrationColor(const vector<sensor_color_t> & colors, vector<vector<int>> & sensorReads);

void processCalibrationLight(const vector<sensor_light_t> & lights, vector<vector<int>> & sensorReads);

sensorData processCalibration(BrickPi3 BP);

void printSensorCalibration(const sensorData & sensorReads);
#endif /* SENSORS_H */