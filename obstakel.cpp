#include "BrickPi3.h"
#include "sensors.h"
#include "motors.h"
#include <signal.h>

#include <iostream>
#include <unistd.h>
#include <signal.h>

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

int main(){
	/*
	 * Main function to start the initialisation of motors and sensors.
	 * 
	 */
	signal(SIGINT, exit_signal_handler);

	BP.detect();

	BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);
	BP.set_sensor_type(PORT_4, SENSOR_TYPE_NXT_COLOR_RED);
	BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
	BP.reset_motor_encoder(PORT_B); // Right motor
	BP.reset_motor_encoder(PORT_C); // Left motor

	sensorData sensorReads = processCalibration(BP);
	printSensorCalibration(sensorReads);
	sleep(5);

	startSteering(BP, sensorReads, true); // v1
	//startSteering2(BP); //v2
	return 0;
}

void exit_signal_handler(int signo){
	/*
	 * Handels ctrl + c to gracefully shutdown the program 
	 */ 
	if(signo == SIGINT){
		BP.reset_all();
		exit(-2);
	}
}