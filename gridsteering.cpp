#include "BrickPi3.h"
#include "sensors.h"
#include "motors.h"

#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <vector>

using namespace std;

void exit_signal_handler(int signo);

BrickPi3 BP;

struct course{
	vector<vector<int>> roadMap; /**< Vector of vectors of ints. 1 means legit intersection, 2 means blocked intersection */
	int width; /**< Width of the roadmap */
	int height; /**< Height of the roadmap */
	int targetY; /**< Target location on y-axis */
	int targetX; /**< Target location on x-axis */
};
/**
 * @param c course to set the values of
 * Asks for input from cin for the setup of the course.
 *  - width and height for the course
 *  - targetX and targetY for the target
 */
void setCourse(course & c){
	int height = 0;
	int width = 0;
	int targetX = -1;
	int targetY = -1;

	cout << "Hoe breed is het wegennet? ";
	cin >> width;
	
	cout << "Hoe hoog is het wegennet? ";
	cin >> height;
	
	while(targetX < 0 || targetX > width){
		cout << "Op welke x-as is het doel? ";
		cin >> targetX;
	}

	while(targetY < 0 || targetY > height){
		cout << "Op welke y-as is het doel? ";
		cin >> targetY;
	}
	
	c.width = width;
	c.height = height;
	c.targetX = targetX;
	c.targetY = targetY;
	cout << "Wegennet is " << width << " bij " << height << endl;
	cout << "Het doelwit staat op [" << c.targetX << "," << c.targetY << "]" << endl;
	
	vector<vector<int>> grid;
	for(int i = 0; i < height; i++){
		vector<int> row = {};
		for(int j = 0; j < width; j++){
			row.push_back(1);
		}
		grid.push_back(row);
	}
	c.roadMap = grid;
}

/** \fn main
 * Main function to start the initialisation of motors and sensors.
 * 
 */
int main(){
	signal(SIGINT, exit_signal_handler);

	BP.detect();

	BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);
	BP.set_sensor_type(PORT_4, SENSOR_TYPE_NXT_COLOR_RED);

	BP.reset_motor_encoder(PORT_B); // Right motor
	BP.reset_motor_encoder(PORT_C); // Left motor

	course grid;
	setCourse(grid);

	sensorData sensorReads = processCalibration(BP);
	printSensorCalibration(sensorReads);
	sleep(5);

	startSteering(BP, sensorReads);
}

/**
 * @param signo integer for the ctrl + c command
 * Handels ctrl + c to gracefully shutdown the program 
 */ 
void exit_signal_handler(int signo){
	if(signo == SIGINT){
		BP.reset_all();
		exit(-2);
	}
}

