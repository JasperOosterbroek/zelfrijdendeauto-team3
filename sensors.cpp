#include "sensors.h"

using namespace std;

void calibrateLineSensors(BrickPi3 BP, vector<sensor_color_t> & colors, vector<sensor_light_t> & lights){
/*
 * Measures color and light values for 2.5s, after which it will move backwards 
 * and measure again for 2.5s.
 * Measurements are stored in sensor structs, which are pushed in a vector.
 */
	sensor_color_t Color1;
	sensor_light_t Light3;
	
	int counter = 0;
	while(counter < 10){
		cout << counter << endl;
		if(counter == 5){
			BP.set_motor_position(PORT_B, 180);
			BP.set_motor_position(PORT_C, 180);
		}
		if(BP.get_sensor(PORT_4, Color1) == 0){
			colors.push_back(Color1);
		}
		if(BP.get_sensor(PORT_3, Light3) == 0){
			lights.push_back(Light3);
		}
		counter++;
		cout << endl;
		usleep(500000);
	}
}

void processCalibrationColor(const vector<sensor_color_t> & colors, vector<vector<int>> & sensorReads){
/*
 * 
 * Loops through the given vector of sensor structs, saving the lowest and 
 * highest reflected values in a vector
 * 
 */
	int lowestRed = -1;
	int highestRed = -1;
	for(unsigned int i = 0; i < colors.size(); i++){
		if(lowestRed == -1 || colors[i].reflected_red < lowestRed){
			lowestRed = colors[i].reflected_red;
		}

		if(highestRed == -1 || colors[i].reflected_red > highestRed){
			highestRed = colors[i].reflected_red;
		}
	}
	sensorReads[0].push_back(lowestRed); 
	sensorReads[0].push_back(highestRed);
}

void processCalibrationLight(const vector<sensor_light_t> & lights, vector<vector<int>> & sensorReads){
/* 
 * Loops through the given vector of sensor structs, saving the lowest and 
 * highest reflected values in a vector
 * 
 */ 
	int lowestReflected = -1;
	int highestReflected = -1;
	for(unsigned int i = 0; i < lights.size(); i++){
		if(lowestReflected == -1 || lights[i].reflected < lowestReflected){
			lowestReflected = lights[i].reflected;
		}
		if(highestReflected == -1 || lights[i].reflected > highestReflected){
			highestReflected = lights[i].reflected;
		}
	}
	sensorReads[1].push_back(lowestReflected);
	sensorReads[1].push_back(highestReflected);
}

sensorData processCalibration(BrickPi3 BP){
/* 
 * Initializes a struct sensorData and adds the gathered values from 
 * processCalibrationColor and processCalibrationLight
 * 
 */
 	vector<sensor_color_t> colors = {};
	vector<sensor_light_t> lights = {};
	// sensorreads i = type sensor (0 = color, 1 = light) j = type info (0,0 = reflected red | 1,0 = reflected ambient
	vector<vector<int>> sensorReads = {{}, {}};

	calibrateLineSensors(BP, colors, lights);
	processCalibrationColor(colors, sensorReads);
	processCalibrationLight(lights, sensorReads);
	sensorData s = {};
	s.highestRed = sensorReads[0][1];
	s.lowestRed = sensorReads[0][0];
	s.highestReflection = sensorReads[1][1];
	s.lowestReflection = sensorReads[1][0];
	return s;
}

void printSensorCalibration(const sensorData & s){
/*
 * Prints the given struct in cout. 
 */ 
	
	cout << "Color info: " << endl;
	cout << "    Min color: " << s.highestRed << endl;
	cout << "    Max color: " << s.lowestRed  << endl;
	cout << "    Color difference: " << s.highestRed  - s.lowestRed  << endl;
	
	cout << "Light info: " << endl;
	cout << "    Min reflect: "  << s.lowestReflection << endl;
	cout << "    Max reflect: "  << s.highestReflection << endl;
	cout << "    Reflect difference: "  << s.highestReflection - s.lowestReflection << endl;
}
