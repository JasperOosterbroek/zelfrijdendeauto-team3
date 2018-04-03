#include "BrickPi3.h"
#include <iostream>
#include <unistd.h>
#include <signal.h>

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

int main(){
	signal(SIGINT, exit_signal_handler);

	BP.detect();

	BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_ULTRASONIC);

	sensor_ultrasonic_t Ultrasonic2;

	int counter = 0;
	while(counter < 10){
		if(BP.get_sensor(PORT_1, Ultrasonic2) == 0){
			cout << "Ultrasonic sensor : " << Ultrasonic2.cm << "cm" << " is aanwezig? " << Ultrasonic2.presence << endl;
		}else{
			cout << "DIT IS EEN RONDE ZONDER ULTRASONE DETECTIE" << endl;
		}
		sleep(1);
		counter++;
	}
}

void exit_signal_handler(int signo){
	if(signo == SIGINT){
		BP.reset_all();
		exit(-2);
	}
}
