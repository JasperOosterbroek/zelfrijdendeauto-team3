#include "BrickPi3.h"
#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <iomanip>
#include <vector>

using namespace std;

void exit_signal_handler(int signo);

BrickPi3 BP;

int main(){
	signal(SIGINT, exit_signal_handler);

	BP.detect();

	BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);
	BP.set_sensor_type(PORT_4, SENSOR_TYPE_NXT_COLOR_RED);

	sensor_color_t Color1;
	sensor_light_t Light3;

	vector<sensor_color_t> colors = {};
	vector<sensor_light_t> lights = {};

	int counter = 0;
	while(counter < 10){
		cout << counter << endl;
		if (counter == 5){
			BP.reset_motor_encoder(PORT_B);
			BP.reset_motor_encoder(PORT_C);
			BP.reset_motor_encoder(PORT_D);
			BP.set_motor_position(PORT_B,90);
			BP.set_motor_position(PORT_C,90);
			BP.set_motor_position(PORT_D,10);
		}
		if(BP.get_sensor(PORT_4, Color1) == 0){
			cout << "Color Sensor : " << (int) Color1.color << endl;
			cout << "    Red : " << setw(4) << Color1.reflected_red << endl;
			cout << "    Green : " << setw(4) << Color1.reflected_green << endl;
			cout << "    Blue : " << setw(4) << Color1.reflected_blue << endl;
			cout << "    Ambient : " << setw(4) << Color1.ambient << endl;
			colors.push_back(Color1);
		}

		if(BP.get_sensor(PORT_3, Light3) == 0){
			cout << "Light sensor (S3): " << endl;
			cout << "   reflected : " << Light3.reflected << endl;
			cout << "   ambient : " << Light3.ambient << endl;
			lights.push_back(Light3);
		}
		cout << endl;
		sleep(1);
		counter++;
	}

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
	lowestAmbient = -1;
	highestAmbient = -1;
	int lowestReflected = -1;
	int highestReflected = -1;
	for(unsigned int i = 0; i < lights.size(); i++){
		if(lowestAmbient == -1 || lights[i].ambient < lowestAmbient){
			lowestAmbient = lights[i].ambient;
		}
		if(highestAmbient == -1 || lights[i].ambient > highestAmbient){
			highestAmbient = lights[i].ambient;
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


}

void exit_signal_handler(int signo){
	if(signo == SIGINT){
		BP.reset_all();
		exit(-2);
	}
}
