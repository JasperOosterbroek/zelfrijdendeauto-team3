#include "BrickPi3.h"
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
using namespace std;

BrickPi3 BP;
void exit_signal_handler(int signo);

void moveTurretToObject(){
	BP.set_motor_position_relative(PORT_D,-200);
}
void moveToObject(){
	int powerB = -55;
	int powerC = 0;
	BP.set_motor_power(PORT_B,powerB);
	BP.set_motor_power(PORT_C,powerC);
	while(true){
		BP.set_motor_power(PORT_B,0);
		BP.set_motor_power(PORT_C,0);
		usleep(10000);
		break;
	}
}

void processTouchsensor(){
	sensor_touch_t Touch1;
	while(true){
		if(BP.get_sensor(PORT_1, Touch1) == 0){
			cout << "Druksensor: " << Touch1.pressed << endl;
			BP.set_motor_power(PORT_B,-20);
			BP.set_motor_power(PORT_C,-20);
			if(Touch1.pressed == true){
				moveToObject();
			}
		}
		usleep(10000);
	}
}

void moveAroundObject(){
	sensor_touch_t Touch1;
	if(Touch1.pressed == true){
		BP.set_motor_power(PORT_B,-35);
		BP.set_motor_power(PORT_C,-35);
	}
}
int main(){
	signal(SIGINT, exit_signal_handler);
	BP.detect();
	BP.set_sensor_type(PORT_1,SENSOR_TYPE_TOUCH);
	moveTurretToObject();
	processTouchsensor();
	moveToObject();
	moveAroundObject();
}
void exit_signal_handler(int signo){
	if(signo == SIGINT){
		BP.reset_all();
		exit(-2);
	}
}
