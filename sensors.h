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
	int highestRed;
	int lowestRed;
	int highestReflection;
	int lowestReflection;
};

void calibrateLineSensors(BrickPi3 BP, vector<sensor_color_t> & colors, vector<sensor_light_t> & lights);

void processCalibrationColor(const vector<sensor_color_t> & colors, vector<vector<int>> & sensorReads);

void processCalibrationLight(const vector<sensor_light_t> & lights, vector<vector<int>> & sensorReads);

sensorData processCalibration(BrickPi3 BP);

void printSensorCalibration(const sensorData & sensorReads);
#endif /* SENSORS_H */