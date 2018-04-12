#include "BrickPi3.h"
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string>
using namespace std;
BrickPi3 BP;
void exit_signal_handler(int signo);
sensor_touch_t Touch1;
bool pressed_detectie(){
	BP.get_sensor(PORT_1,Touch1);
	bool pressed_value = Touch1.pressed;
	return pressed_value;
}
void steering(string directionRobot){
	if(directionRobot == "left"){
		BP.set_motor_position_relative(PORT_B, -610);
		BP.set_motor_position_relative(PORT_C, 610);
	}else if(directionRobot == "right"){
		BP.set_motor_position_relative(PORT_B, 610);
		BP.set_motor_position_relative(PORT_C, -610);
	}else{
		cout << "foute uitkomst, ingevoerde uitvoer: " << directionRobot << endl;
	}
}
int main(){
        signal(SIGINT, exit_signal_handler);
	BP.detect();
        BP.set_sensor_type(PORT_1,SENSOR_TYPE_TOUCH);
        BP.get_sensor(PORT_1,Touch1);
		bool pressed;
		while(true){
			pressed = pressed_detectie();
			if(pressed == true){		// robot will make a move to the left
				steering("right");
				sleep(2);
				BP.set_motor_position_relative(PORT_B, -1100);
				BP.set_motor_position_relative(PORT_C, -1100);					sleep(2);
				sleep(2);
				steering("left");
				sleep(2);
				BP.set_motor_position_relative(PORT_B, -1100);
				BP.set_motor_position_relative(PORT_C, -1100);
				sleep(2);
			if (pressed == true){
				sleep(2);
				steering("left");
				sleep(2);
				BP.set_motor_position_relative(PORT_B, -200);
				BP.set_motor_position_relative(PORT_C, -200);
				sleep(2);
				steering("right");
				sleep(2);
				BP.set_motor_position_relative(PORT_B, -800);
				BP.set_motor_position_relative(PORT_C, -800);
				sleep(2);
				steering("left");
				sleep(2);
				}
			}else{
				BP.set_motor_power(PORT_B, -20);
				BP.set_motor_power(PORT_C, -20);
			}
			usleep(10000);
		}
}
void exit_signal_handler(int signo){
        if(signo == SIGINT){
                BP.reset_all();
                exit(-2);
        }
}
