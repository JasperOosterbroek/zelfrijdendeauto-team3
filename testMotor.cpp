#include "BrickPi3.h"
#include <stdio.h>
#include <iostream>
#include <signal.h>
#include <unistd.h>

using namespace std;

void exit_signal_handler(int signo);

BrickPi3 BP;


int main(){

	signal(SIGINT, exit_signal_handler);

	BP.detect();

	BP.offset_motor_encoder(PORT_D, BP.get_motor_encoder(PORT_D));
	while(true){
		BP.set_motor_power(PORT_D, -100);
		usleep(10000);
		BP.set_motor_power(PORT_D, 100);
		usleep(10000);
	}
}

void exit_signal_handler(int signo){
	if(signo == SIGINT){
		BP.reset_all();
		exit(-2);
	}
}
