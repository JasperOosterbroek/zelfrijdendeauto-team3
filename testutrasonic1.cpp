#include "BrickPi3.h" // for BrickPi3
#include <iostream>      // for cin and cout
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

sensor_ultrasonic_t afstand;

int16_t ultrasoon_detectie() {

	BP.get_sensor(PORT_2, afstand);
	float val = afstand.cm;
	return val;
}

int main(){
 
    signal(SIGINT, exit_signal_handler);
// een limiet 
	BP.set_motor_limits(PORT_B, 60, 0);
	BP.set_motor_limits(PORT_D, 60, 0);
	BP.detect(); 

  BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
  BP.get_sensor(PORT_2, afstand);

int16_t detectie;
while(true){
    detectie = ultrasoon_detectie();
    if(detectie >= 30){
//draai de onderkant van de tank terug als die niet meer het object ziet. kanon blijft nu gericht op de zijkant van het object
BP.set_motor_position_relative(PORT_B,  90);
BP.set_motor_position_relative(PORT_C, -90);
       BP.set_motor_power(PORT_B, -20);
       BP.set_motor_power(PORT_C, -20);

    }
    else if( detectie >=25 && detectie <30 ){

BP.set_motor_position(PORT_B,-90);
BP.set_motor_position(PORT_C, 90);
// turret
BP.set_motor_position(PORT_D,-120);
// wacht tot onderkant en turret is gedraait
sleep(1);
// rijden waneer die 25 cm afstand heeft met de konon gericht op het opject
BP.set_motor_power(PORT_B, -20);
BP.set_motor_power(PORT_C, -20);
	}
    else if(detectie <20){
	BP.set_motor_power(PORT_B, -20);
	BP.set_motor_power(PORT_C, -25);
	}

 usleep(100000);//sleep 100 ms  
}
}

void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}
