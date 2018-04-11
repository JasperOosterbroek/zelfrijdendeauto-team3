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


void steering(string direction){
	if(direction == "left"){
		cout << "motor gaat naar links" << endl;
		BP.set_motor_position_relative(PORT_B, -610);
		BP.set_motor_position_relative(PORT_C, 610);
	}else if(direction == "right"){
		cout << "motor gaat naar rechts" << endl;
		BP.set_motor_position_relative(PORT_B, 610);
		BP.set_motor_position_relative(PORT_C, -610);
	}else{
		cout << "foute uitvoer, ingevoerde uitvoer: " << direction << endl;
	}
}

int test= BP.reset_motor_encoder(PORT_D);

int main(){

	signal(SIGINT, exit_signal_handler);
	// een limietinstellen wanneer de  motors een bocht gaat maken gebruikt die normaal de volle kracht
	//BP.set_motor_limits(PORT_B, 30, 0);
	//BP.set_motor_limits(PORT_C, 30, 0);
	//BP.set_motor_limits(PORT_D, 60, 0);
	BP.detect();

  BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
  BP.get_sensor(PORT_2, afstand);

int16_t detectie;
bool is_turret_rotated;
while(true){
	detectie = ultrasoon_detectie();
	int32_t turret_positie = BP.get_motor_encoder(PORT_D);
	cout << "test " << turret_positie << endl;
	if(turret_positie > -230 && turret_positie < -200){
		is_turret_rotated = true;
	}else{
		is_turret_rotated = false;
	}

	if(detectie >= 30){
	cout << "detectie = " << detectie << endl;
	//draai de onderkant van de tank terug als die niet meer het object ziet. kanon blijft nu gericht op de zijkant van het object
	if(is_turret_rotated){
		cout << "turret rotated & detectie >= 30" << endl;
		BP.set_motor_position_relative(PORT_B, -360);
		BP.set_motor_position_relative(PORT_C, -360);
		cout << "sleep 2 sec" << endl;
		sleep(2);
		cout << "turn right" << endl;
		steering("right");
		sleep(2);
		cout << "Make space" << endl;
		BP.set_motor_position_relative(PORT_B, -1440);
		BP.set_motor_position_relative(PORT_C,-1440);

		sleep(2);
	}else{
		cout << "turret not rotated & detectie >=30" << endl;
		BP.set_motor_position_relative(PORT_B, -720);
		BP.set_motor_position_relative(PORT_C, -720);
		//steering("right");
		//BP.set_motor_position_relative(PORT_B, -1080);
		//BP.set_motor_position_relative(PORT_C, -1080);
	}
    }
    else if( detectie >=25 && detectie <30 ){
	cout << " detectie = " << detectie << endl;
	if(is_turret_rotated){
		cout << "gogogogo"<< endl;
	}else{
	steering("left");
	//BP.set_motor_position_relative(PORT_B, -360);

	// turret
	BP.set_motor_position(PORT_D,-210);
	// wacht tot onderkant en turret is gedraait
	sleep(2);
	// rijden waneer die 25 cm afstand heeft met de konon gericht op het opject
	BP.set_motor_power(PORT_B, -20);
	BP.set_motor_power(PORT_C, -20);
	}
	}

    else if(detectie <25){
	cout << "detectie = " << detectie << endl;
	if(is_turret_rotated){
		BP.set_motor_power(PORT_B, -20);
		BP.set_motor_power(PORT_C, -20);
	}else{
		BP.set_motor_power(PORT_B, 20);
		BP.set_motor_power(PORT_C, 20);
	}
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
