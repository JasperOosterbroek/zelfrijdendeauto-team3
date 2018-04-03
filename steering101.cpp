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

	cout << "Color info: " << endl;
	int lowestAmbient = -1;
	int highestAmbient = -1;
	for(unsigned int i = 0; i < colors.size(); i++){
		if(lowestAmbient == -1 || colors[i].reflected_red < lowestAmbient){
			lowestAmbient = colors[i].reflected_red;
		}
		if(highestAmbient == -1 || colors[i].reflected_red > highestAmbient){
			highestAmbient = colors[i].reflected_red;
		}
	}
	cout << "    Min color: " << lowestAmbient << endl;
	cout << "    Max color: " << highestAmbient << endl;
	cout << "    highest difference: " << highestAmbient - lowestAmbient << endl;

	cout << "Light info: " << endl;
	int lowestAmbientLight = -1;
	int highestAmbientLight = -1;
	int lowestReflected = -1;
	int highestReflected = -1;
	for(unsigned int i = 0; i < lights.size(); i++){
		if(lowestAmbientLight == -1 || lights[i].ambient < lowestAmbientLight){
			lowestAmbientLight = lights[i].ambient;
		}
		if(highestAmbientLight == -1 || lights[i].ambient > highestAmbientLight){
			highestAmbientLight = lights[i].ambient;
		}

		if(lowestReflected == -1 || lights[i].reflected < lowestReflected){
			lowestReflected = lights[i].reflected;
		}
		if(highestReflected == -1 || lights[i].reflected > highestReflected){
			highestReflected = lights[i].reflected;
		}
	}
	cout << "    Min ambient: "  << lowestAmbient << endl;
	cout << "    Max ambient: "  << highestAmbient << endl;
	cout << "    Ambient difference: "  << highestAmbient - lowestAmbient << endl;
	cout << "    Min reflect: "  << lowestReflected << endl;
	cout << "    Max reflect: "  << highestReflected << endl;
	cout << "    Reflect difference: "  << highestReflected - lowestReflected << endl;

	cout << endl;
	sleep(5);

// Deciding to steer
/* 
	sensor_color_t steerColor;
	sensor_light_t steerLight;

	float tollerance = 1.07; // in percentage
	while(true){
		int moveRightMotor = -50;
		int moveLeftMotor = -50;
		if(BP.get_sensor(PORT_3, steerColor) == 0){
			if(steerColor.reflected_red > (lowestAmbient * tollerance) ){
				moveLeftmotor -= 10;
			}
		}
		if(BP.get_sensor(PORT_4, steerLight) == 0){
			if(steerLight.reflected){
			
			}
		}
	}
*/
	sensor_light_t Light3;
	sensor_color_t Color1;

	int power = -20;
	while(true){

		// left sensor right motor
		BP.get_sensor(PORT_3, Light3);
		uint16_t val = Light3.reflected;
		if(val < lowestReflected) val = lowestReflected;
		if(val > highestReflected) val = highestReflected;
		int16_t rightmotorpower = (100*(val - lowestReflected))/(highestReflected - lowestReflected);

		BP.get_sensor(PORT_4, Color1);
		val = Color1.reflected_red;
		if(val < lowestAmbient) val = lowestAmbient;
		if(val > highestReflected) val = highestAmbient;
		int16_t leftmotorpower = (100*(val - highestAmbient))/(lowestAmbient- highestAmbient);

		// right sensor left motor
//		if(leftmotorpower < 10 && rightmotorpower < 10){
//			BP.set_motor_power(PORT_B,power);
//			BP.set_motor_power(PORT_C,power);
//		}else{
			BP.set_motor_power(PORT_B, power + (rightmotorpower*-1));
			BP.set_motor_power(PORT_C, power + (leftmotorpower*-1));
//		}
		cout << "motors: " << leftmotorpower << " - " << rightmotorpower << endl;
		cout << endl;
	}
}

void exit_signal_handler(int signo){
	if(signo == SIGINT){
		BP.reset_all();
		exit(-2);
	}
}
