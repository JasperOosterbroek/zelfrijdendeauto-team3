#include "motors.h"

using namespace std;


void startSteering(BrickPi3 BP, const sensorData & sensorReads ){
/*
 * Function for following a line. reads the given sensor data and uses it for
 * comparisons between the currently measured data. Based on this the function
 * gives a percentage offset. If one motor has more than 30% offset, the other 
 * motor will drive that same offset in negative direction. This ensures quick
 * turning. 
 */
    sensor_light_t Light3;
    sensor_color_t Color1;

    int power = -20;
    while(true){
        // left sensor right motor
        BP.get_sensor(PORT_3, Light3);
        uint16_t val = Light3.reflected;
        if(val < sensorReads.lowestReflection) val = sensorReads.lowestReflection;
        if(val > sensorReads.highestReflection) val = sensorReads.highestReflection;
        int16_t rightmotorpower = (100*(val - sensorReads.lowestReflection)/(sensorReads.highestReflection - sensorReads.lowestReflection));

		cout << "Light / right motor stats:" << endl;
		cout << "    Val: " << val << endl;
		cout << "    sensorReads.lowestReflection: " << sensorReads.lowestReflection << endl;
		cout << "    sensorReads.highestReflection: " << sensorReads.highestReflection << endl;
		cout << "    rightmotorpower: " << rightmotorpower << endl;
		cout << "    power + (rightmotorpower*-1): " << power + (rightmotorpower*-1) << endl;

        BP.get_sensor(PORT_4, Color1);
        val = Color1.reflected_red;
        if(val < sensorReads.lowestRed) val = sensorReads.lowestRed;
        if(val > sensorReads.highestRed) val = sensorReads.highestRed;
        int16_t leftmotorpower = 100 -(100*(val - sensorReads.lowestRed))/(sensorReads.highestRed - sensorReads.lowestRed);

		cout << "Color / left motor stats:" << endl;
		cout << "    Val: " << val << endl;
		cout << "    sensorReads.lowestRed: " << sensorReads.lowestRed << endl;
		cout << "    sensorReads.highestRed: " << sensorReads.highestRed << endl;
		cout << "    leftmotorpower: " << leftmotorpower << endl;
		cout << "    power + (leftmotorpower*-1): " << power + (leftmotorpower*-1) << endl;

		if(rightmotorpower > 40){
			leftmotorpower = -40;
		}else if(leftmotorpower > 40){
			rightmotorpower = -40;
		}

		cout << endl;

        // right sensor left motor
        if(leftmotorpower < 10 && rightmotorpower < 10){
            BP.set_motor_power(PORT_B,power);
            BP.set_motor_power(PORT_C,power);
        }else{
            BP.set_motor_power(PORT_B, power + (rightmotorpower*-1));
            BP.set_motor_power(PORT_C, power + (leftmotorpower*-1));
        }
        cout << "motors: " << leftmotorpower << " - " << rightmotorpower << endl;
        cout << endl;
    }
}
