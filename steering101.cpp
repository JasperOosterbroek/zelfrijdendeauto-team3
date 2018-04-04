#include "BrickPi3.h"
#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <vector>

using namespace std;

void exit_signal_handler(int signo);

BrickPi3 BP;

void calibrate(vector<sensor_color_t> & colors, vector<sensor_light_t> & lights){
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
		cout << endl;
		counter++;
		sleep(1);
	}
}

void printCalibration(const vector<sensor_color_t> & v){
	cout << "Color info: " << endl;
	int lowestRed = -1;
	int highestRed = -1;
	for(unsigned int i = 0; i < v.size(); i++){
		if(lowestRed == -1 || v[i].reflected_red < lowestRed){
			lowestRed = v[i].reflected_red;
		}

		if(highestRed == -1 || v[i].reflected_red > highestRed){
			highestRed = v[i].reflected_red;
		}
	}
	cout << "    Min color: " << lowestRed << endl;
	cout << "    Max color: " << highestRed << endl;
	cout << "    highest difference: " << highestRed - lowestRed << endl;
}

void printCalibration(const vector<sensor_light_t> & v){
	cout << "Light info: " << endl;
	int lowestAmbientLight = -1;
	int highestAmbientLight = -1;
	int lowestReflected = -1;
	int highestReflected = -1;
	for(unsigned int i = 0; i < v.size(); i++){
		if(lowestAmbientLight == -1 || v[i].ambient < lowestAmbientLight){
			lowestAmbientLight = v[i].ambient;
		}
		if(highestAmbientLight == -1 || v[i].ambient > highestAmbientLight){
			highestAmbientLight = v[i].ambient;
		}

		if(lowestReflected == -1 || v[i].reflected < lowestReflected){
			lowestReflected = v[i].reflected;
		}
		if(highestReflected == -1 || v[i].reflected > highestReflected){
			highestReflected = v[i].reflected;
		}
	}
	cout << "    Min ambient: "  << lowestAmbient << endl;
	cout << "    Max ambient: "  << highestAmbient << endl;
	cout << "    Ambient difference: "  << highestAmbient - lowestAmbient << endl;
	cout << "    Min reflect: "  << lowestReflected << endl;
	cout << "    Max reflect: "  << highestReflected << endl;
	cout << "    Reflect difference: "  << highestReflected - lowestReflected << endl;
	cout << endl;
}

void startSteering(BrickPi3 controller){
	sensor_light_t Light3;
	sensor_color_t Color1;
   // current date/time based on current system
	time_t now = time(0);

	int counter = 0;
	int power = -20;
	while(true){

		// left sensor right motor
		controller.get_sensor(PORT_3, Light3);
		uint16_t val = Light3.reflected;
		if(val < lowestReflected) val = lowestReflected;
		if(val > highestReflected) val = highestReflected;
		int16_t rightmotorpower = (100*(val - lowestReflected))/(highestReflected - lowestReflected);

		controller.get_sensor(PORT_4, Color1);
		val = Color1.reflected_red;
		if(val < lowestAmbient) val = lowestAmbient;
		if(val > highestReflected) val = highestAmbient;
		int16_t leftmotorpower = (100*(val - highestAmbient))/(lowestAmbient- highestAmbient);

		// right sensor left motor
//		if(leftmotorpower < 10 && rightmotorpower < 10){
//			controller.set_motor_power(PORT_B,power);
//			controller.set_motor_power(PORT_C,power);
//		}else{
			controller.set_motor_power(PORT_B, power + (rightmotorpower*-1));
			controller.set_motor_power(PORT_C, power + (leftmotorpower*-1));
//		}
		cout << "motors: " << leftmotorpower << " - " << rightmotorpower << endl;
		cout << endl;
		counter++;
		if(counter == 100){
			break;
		}
	}
}

void startSteering2(BrickPi3 controller){
	sensor_color_t steerColor;
	sensor_light_t steerLight;

	float tollerance = 1.07; // in percentage
	int moveRightMotor = 50;
	int moveLeftMotor = 50;
	
	while(true){
		if(controller.get_sensor(PORT_3, steerColor) == 0){
			
		}
		if(controller.get_sensor(PORT_4, steerLight) == 0){
		
		}
	}
}

int main(){
	signal(SIGINT, exit_signal_handler);

	BP.detect();

	BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);
	BP.set_sensor_type(PORT_4, SENSOR_TYPE_NXT_COLOR_RED);

	BP.reset_motor_encoder(PORT_B); // Right motor
	BP.reset_motor_encoder(PORT_C); // Left motor

	vector<sensor_color_t> colors = {};
	vector<sensor_light_t> lights = {};

	calibrate(colors, lights);
	displayCalibration(colors);
	displayCalibration(lights);
		
	sleep(5);

	startSteering(BP); // v1
	//startSteering2(BP); //v2
}

void exit_signal_handler(int signo){
	if(signo == SIGINT){
		BP.reset_all();
		exit(-2);
	}
}