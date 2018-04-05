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
//			BP.set_motor_position(PORT_B, 180);
//			BP.set_motor_position(PORT_C, 180);
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

void processCalibrationColor(const vector<sensor_color_t> & v, vector<vector<int>> & sensorReads){
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
	sensorReads[0].push_back(lowestRed); 
	sensorReads[0].push_back(highestRed);
}

void processCalibrationLight(const vector<sensor_light_t> & v, vector<vector<int>> & sensorReads){
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
	sensorReads[1].push_back(lowestAmbientLight);
	sensorReads[1].push_back(highestAmbientLight);
	sensorReads[1].push_back(lowestReflected);
	sensorReads[1].push_back(highestReflected);
}

void printCalibration(const vector<vector<int>> & sensorReads){
	cout << "Color info: " << endl;
	cout << "    Min color: " << sensorReads[0][0] << endl;
	cout << "    Max color: " << sensorReads[0][1]  << endl;
	cout << "    highest difference: " << sensorReads[0][1]  - sensorReads[0][0]  << endl;
	
	cout << "Light info: " << endl;
	cout << "    Min ambient: "  << sensorReads[1][0] << endl;
	cout << "    Max ambient: "  << sensorReads[1][1] << endl;
	cout << "    Ambient difference: "  << sensorReads[1][1] - sensorReads[1][0] << endl;
	cout << "    Min reflect: "  << sensorReads[1][2] << endl;
	cout << "    Max reflect: "  << sensorReads[1][3] << endl;
	cout << "    Reflect difference: "  << sensorReads[1][3] - sensorReads[1][2] << endl;
}

void startSteering(const vector<vector<int>> & sensorReads ){
	sensor_light_t Light3;
	sensor_color_t Color1;
	
	int kp = 2; //1000
	int ki = 0; //100
	int kd = 0; //10000
	int offsetC = (sensorReads[0][0] + sensorReads[0][1]) /2;
	int offsetL = (sensorReads[1][2] + sensorReads[1][3]) /2;
	
	int integralC = 0;
	int integralL = 0;
	int lastErrorC = 0;
	int lastErrorL = 0;
	int derivativeC = 0;
	int derivativeL = 0;
	int errorC = 0;
	int errorL = 0;
	float tolleranceC = 1.0; //Percentage
	float tolleranceL = 1.0; //Percentage
	
	
	int motorPower = -50;
	
	while(true){
		BP.get_sensor(PORT_4, Color1);
		uint16_t valC = Color1.reflected_red;
		// makes it so it cant go lower or higher
//		if(valC < sensorReads[0][0]) valC = sensorReads[0][0];
//		if(valC > sensorReads[0][1]) valC = sensorReads[0][1];
		//int16_t leftmotorpower = (100*(val - sensorReads[0][1]))/(sensorReads[0][0]- sensorReads[0][1]);
		if(valC > offsetC * ((100-tolleranceC)/100) && valC < offsetC * ((100+tolleranceC)/100)){
			errorC = 0;
		}else{
			errorC = valC - offsetC;
		}
		integralC = integralC + errorC;
		derivativeC = errorC - lastErrorC;
		int turnC = (kp*errorC) + (ki*integralC) + (kd*derivativeC);
		turnC = turnC/100;
//		turnC = turnC * -1;
		
		BP.get_sensor(PORT_3, Light3);
		uint16_t valL = Light3.reflected;
		// makes it so it cant go lower or higher
//		if(valL < sensorReads[1][2]) valL = sensorReads[1][2];
//		if(valL > sensorReads[1][3]) valL = sensorReads[1][3];
		//int16_t rightmotorpower = (100*(val - sensorReads[1][2]))/(sensorReads[1][3] - sensorReads[1][2]);
		if(valL > offsetL * ((100-tolleranceL)/100) && valL < offsetL * ((100+tolleranceL)/100)){
			errorL = 0;
		}else{
			errorL = valL - offsetL;
		}
		integralL = integralL + errorL;
		derivativeL = errorL - lastErrorL;
		int turnL = (kp*errorL) + (ki*integralL) + (kd*derivativeL);
		turnL = turnL/100;
//		turnL = turnL * -1;

		int16_t rightMotorPower = motorPower + turnC;
		int16_t leftMotorPower = motorPower + turnL;
		
//		if(leftmotorpower < 10 && rightmotorpower < 10){
//			BP.set_motor_power(PORT_B,motorPower);
//			BP.set_motor_power(PORT_C,motorPower);
//		}else{
//			BP.set_motor_power(PORT_B, motorPower + (rightmotorpower*-1));
//			BP.set_motor_power(PORT_C, motorPower + (leftmotorpower*-1));
//		}
		BP.set_motor_power(PORT_B, rightMotorPower);
		BP.set_motor_power(PORT_C, leftMotorPower);
		
		cout << endl;
		cout << "MotorC debug:" << endl;
		cout << "    valC: " << valC << endl;
		cout << "    errorC: " << errorC << endl;
		cout << "    lastErrorC: " << lastErrorC << endl;
		cout << "    offsetC: " << offsetC << endl;
		cout << "        sensorReads[0][0]: " << sensorReads[0][0] << endl;
		cout << "        sensorReads[0][1]: " << sensorReads[0][1] << endl;
		cout << "    integralC: " << integralC << endl;
		cout << "    derivativeC: " << derivativeC << endl;
		cout << "    turnC: " << turnC << endl;
		cout << "    rightMotorPower: " << rightMotorPower << endl;
		
		cout << "MotorL debug:" << endl;
		cout << "    valL: " << valL << endl;
		cout << "    errorL: " << errorL << endl;
		cout << "    lastErrorL: " << lastErrorL << endl;
		cout << "    offsetL: " << offsetL << endl;
		cout << "        sensorReads[0][0]: " << sensorReads[1][2] << endl;
		cout << "        sensorReads[0][1]: " << sensorReads[1][3] << endl;
		cout << "    integralL: " << integralL << endl;
		cout << "    derivaticeL: " << derivativeL << endl;
		cout << "    turnL: " << turnL << endl;
		cout << "    leftMotorPower: " << leftMotorPower << endl;
		cout << endl;
			
		
		
		
		lastErrorC = errorC;
		lastErrorL = errorL;
//		cout << "motors: " << leftMotorPower << " | " << rightMotorPower << endl;
	}
	BP.reset_all();
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
	// sensorreads i = type sensor (0 = color, 1 = light) j = type info (0,0 = reflected red | 1,0 = reflected ambient
	vector<vector<int>> sensorReads = {{}, {}};

	calibrate(colors, lights);
	processCalibrationColor(colors, sensorReads);
	processCalibrationLight(lights, sensorReads);
	
	printCalibration(sensorReads);
	sleep(1);

	startSteering(sensorReads); // v1
	//startSteering2(BP); //v2
}

void exit_signal_handler(int signo){
	if(signo == SIGINT){
		BP.reset_all();
		exit(-2);
	}
}
