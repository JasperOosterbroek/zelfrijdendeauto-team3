#include "motors.h"

using namespace std;

void swerve(BrickPi3 BP){
    sensor_ultrasonic_t afstand;
    bool isTurretRotated;
    int16_t detectie;
    // reset turret position as 0
    int turret = BP.reset_motor_encoder(PORT_D);

    while(true){
        detectie = ultrasoon_detectie(BP, afstand);
        int32_t turret_positie = BP.get_motor_encoder(PORT_D);
        // cout << "Detectie: " << detectie << endl;
        if(turret_positie > -230 && turret_positie < -200){
        isTurretRotated = true;
        }else{
            isTurretRotated = false;
        }
    
        if(detectie >= 30){
            cout << "detectie = " << detectie << endl;
            //draai de onderkant van de tank terug als die niet meer het object ziet. kanon blijft nu gericht op de zijkant van het object
            if(isTurretRotated){
               cout << "turret rotated & detectie >= 30" << endl;
               BP.set_motor_position_relative(PORT_B, -360);
               BP.set_motor_position_relative(PORT_C, -360);
               cout << "sleep 2 sec" << endl;
               sleep(2);
               cout << "turn right" << endl;
               steering(BP , "right");
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
        }else if( detectie >=25 && detectie <30 ){
            cout << " detectie = " << detectie << endl;
                if(isTurretRotated){
                    cout << "gogogogo"<< endl;
                }else{
                    steering(BP, "left");
                    //BP.set_motor_position_relative(PORT_B, -360);

                    // turret
                    BP.set_motor_position(PORT_D,-210);
                    // wacht tot onderkant en turret is gedraait
                    sleep(2);
                    // rijden waneer die 25 cm afstand heeft met de konon gericht op het opject
                    BP.set_motor_power(PORT_B, -20);
                    BP.set_motor_power(PORT_C, -20);
                }
        }else if(detectie <25){
            cout << "detectie = " << detectie << endl;
            if(isTurretRotated){
                BP.set_motor_power(PORT_B, -20);
                BP.set_motor_power(PORT_C, -20);
            }else{
                BP.set_motor_power(PORT_B, 20);
                BP.set_motor_power(PORT_C, 20);
            }
        }
    }
}


/** 
 * @param BP the brickpi controller
 * @param sensorReads the struct of the calibration data
 * 
 * Function for following a line. reads the given sensor data and uses it for
 * comparisons between the currently measured data. Based on this the function
 * gives a percentage offset. If one motor has more than 30% offset, the other 
 * motor will drive that same offset in negative direction. This ensures quick
 * turning. 
 */
void startSteering(BrickPi3 BP, const sensorData & sensorReads, bool obstacles ){

    sensor_light_t Light3;
    sensor_color_t Color1;


    int power = -20;
    while(true){
        if(obstacles){
            swerve(BP);
        }
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

/**
 * @param BP The BrickPi3 controller
 * @param direction The direction the robot has to turn to
 *
 * This function wil turn the robot on it's axis.
 */

void steering(BrickPi3 BP, const string & direction, const int & steps){
    if(direction == "left"){
        cout << "motor gaat naar links" << endl;
        BP.set_motor_position_relative(PORT_B, steps*-1);
        BP.set_motor_position_relative(PORT_C, steps);
    }else if(direction == "right"){
        cout << "motor gaat naar rechts" << endl;
        BP.set_motor_position_relative(PORT_B, steps);
        BP.set_motor_position_relative(PORT_C, steps*-1); //610
    }else{
        cout << "foute uitvoer, ingevoerde uitvoer: " << direction << endl;
    }
    sleep(2);
}

