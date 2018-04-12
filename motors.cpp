#include "motors.h"

using namespace std;

void detectObject(BrickPi3 BP){
    sensor_ultrasonic_t afstand;
    bool isTurretRotated;
    int16_t detectie;
    // reset turret position as 0
    int turret = BP.reset_motor_encoder(PORT_D);
    detectie = ultrasoon_detectie(BP, afstand);
    if(detectie >= 30){
        cout << "detectie = " << detectie << endl;
    }else if (detectie >= 25 && detectie < 30){
        // punt gevonden
        swerve(BP);
    }else if(detectie == 0 ){
        //ignore
        cout << "sensor malfunction" << endl;
    }else if(detectie < 25){
        //oh shit terug
        while(detectie < 25){
            detectie = ultrasoon_detectie(BP, afstand);
            cout << "Detectie: " << detectie << endl;
            BP.set_motor_power(PORT_B, 20);
            BP.set_motor_power(PORT_C, 20);
        }
        swerve(BP);
    }


}

void swerve(BrickPi3 BP){
    sensor_ultrasonic_t afstand;
    int16_t detectie;
    // set turret
    int turret = BP.reset_motor_encoder(PORT_D);
    cout << "turret pos = "<< turret << endl;
    BP.set_motor_position(PORT_D,-210);
    // move to the left
    steering(BP, "left", 610);
    //go forward until the object ends
    detectie = ultrasoon_detectie(BP, afstand);
    int power = -25;
    while(detectie < 30){
        detectie = ultrasoon_detectie(BP, afstand);
        cout << "Detectie: " << detectie << endl;
        BP.set_motor_power(PORT_B,power);
        BP.set_motor_power(PORT_C,power);
    }
    BP.set_motor_power(PORT_B, 0);
    BP.set_motor_power(PORT_C, 0);
    // extrra spacing
    BP.set_motor_position_relative(PORT_B, -720);
    BP.set_motor_position_relative(PORT_C, -720);
    sleep(3);
    steering(BP, "right", 610);

    detectie = ultrasoon_detectie(BP, afstand);
    while(detectie < 40){
        detectie = ultrasoon_detectie(BP, afstand);
        cout << "Detectie: " << detectie << endl;
        BP.set_motor_power(PORT_B,power);
        BP.set_motor_power(PORT_C,power);
    }
    BP.set_motor_power(PORT_B, 0);
    BP.set_motor_power(PORT_C, 0);
    BP.set_motor_position_relative(PORT_B, -1080);
    BP.set_motor_position_relative(PORT_C, -1080);
    sleep(3);
    steering(BP, "right", 610);
    // past object
    // turret reset
    BP.set_motor_position_relative(PORT_D, 210);
    cout << "Best dodge in the history of dodges" << endl;
    sleep(2);
    //find the line

    

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
            detectObject(BP);
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

void findLine(BrickPi3 BP, string turnDirection){
    // init sensors
    sensor_light_t Light3;
    sensor_color_t Color1;
    
    int tolerance = 10; // tolerance in percentage
    
    // get sensor val
    BP.get_sensor(PORT_3, Light3);
    BP.get_sensor(PORT_4, Color1);
    uint16_t lightlow = Light3.reflected * (1 + percentage); 
    uint16_t colorlow = Color1.reflected_red * (1 - percemtage); 


    if(turnDirection == "left"){
        int powerleft = 20;
        int powerright = -20;
        BP.get_sensor(PORT_3, Light3);
        uint16_t val = Light3.reflected;
        while(val > lightlow){
            BP.get_sensor(PORT_3, Light3);
            uint16_t val = Light3.reflected;
            
        }

    }else if(turnDirection == "right"){
        int powerleft = -20;
        int powerright = 20;
        BP.get_sensor(PORT_4, Color1);
        val = Color1.reflected_red;
        while(val < colorlow){
            BP.get_sensor(PORT_4, Color1);
            val = Color1.reflected_red;

        }

    }else{
        cout << "Incorrect direction given in findline function." << endl;
        return;

    }

}