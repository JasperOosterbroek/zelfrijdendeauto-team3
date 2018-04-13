#include "motors.h"

using namespace std;

void detectObject(BrickPi3 BP, const sensorData & sensorReads){
    sensor_ultrasonic_t ultrasonic;
    int16_t detection;
    // reset turret position as 0
    detection = ultrasoon_detectie(BP, ultrasonic);
    if(detection >= 30){
        cout << "detectie = " << detection << endl;
    }else if (detection >= 25 && detection < 25){
        // punt gevonden
        swerve(BP, sensorReads);
    }else if(detection == 0 ){
        //ignore
        cout << "sensor malfunction" << endl;
    }else if(detection < 25){
        //oh shit terug
        while(detection < 25){
            detection = ultrasoon_detectie(BP, ultrasonic);
            cout << "Detectie: " << detection << endl;
            BP.set_motor_power(PORT_B, 20);
            BP.set_motor_power(PORT_C, 20);
        }
        swerve(BP, sensorReads);
    }


}

void swerve(BrickPi3 BP, const sensorData & sensorReads){
    sensor_ultrasonic_t ultrasonic;
    sensor_light_t Light3;
    sensor_color_t Color1;
    int intersectionTollerance = 25; // 25 % tollerance

    int16_t detection;
    // set turret
    int turret = BP.reset_motor_encoder(PORT_D);
    cout << "turret pos = "<< turret << endl;
    BP.set_motor_position(PORT_D,-210);
    // move to the left
    steering(BP, "left", 610);
    //go forward until the object ends
    detection = ultrasoon_detectie(BP, ultrasonic);
    int power = -25;
    while(detection < 30){
        detection = ultrasoon_detectie(BP, ultrasonic);
        cout << "Detectie: " << detection << endl;

        // left sensor right motor
        BP.get_sensor(PORT_3, Light3);
        uint16_t valL = Light3.reflected;
        if(valL < sensorReads.lowestReflection) valL = sensorReads.lowestReflection;
        if(valL > sensorReads.highestReflection) valL = sensorReads.highestReflection;
        
        BP.get_sensor(PORT_4, Color1);
        uint16_t valC = Color1.reflected_red;
        if(valC < sensorReads.lowestRed) valC = sensorReads.lowestRed;
        if(valC > sensorReads.highestRed) valC = sensorReads.highestRed;
        
        bool lightOnBlack = isLightOnBlack(sensorReads, intersectionTollerance, valL);
        bool colorOnBlack = isColorOnBlack(sensorReads, intersectionTollerance, valC);
        cout << "WHILE EEN " << endl;
        cout << "    lightOnBlack: " << lightOnBlack << endl;
        cout << "    colorOnBlack: " << colorOnBlack << endl;
        if(colorOnBlack || lightOnBlack){
            cout << "Found line!" << endl;
            findLine(BP, "left", sensorReads);
            return;
        }


        BP.set_motor_power(PORT_B,power);
        BP.set_motor_power(PORT_C,power);
    }

    BP.set_motor_power(PORT_B, 0);
    BP.set_motor_power(PORT_C, 0);
    // extrra spacing
    BP.set_motor_position_relative(PORT_B, -360);
    BP.set_motor_position_relative(PORT_C, -360);
    sleep(3);
    steering(BP, "right", 620);

    detection = ultrasoon_detectie(BP, ultrasonic);
    while(detection > 30 && detection != -1){
        detection = ultrasoon_detectie(BP, ultrasonic);
        cout << "Waar is dat object?" << endl;
        BP.set_motor_power(PORT_B, power);
        BP.set_motor_power(PORT_C, power);
    }
    cout << "Hier is dat object" << endl;
    detection = ultrasoon_detectie(BP, ultrasonic);

    while(detection < 30){
        detection = ultrasoon_detectie(BP, ultrasonic);
        cout << "Detectie: " << detection << endl;

        // left sensor right motor
        BP.get_sensor(PORT_3, Light3);
        uint16_t valL = Light3.reflected;
        if(valL < sensorReads.lowestReflection) valL = sensorReads.lowestReflection;
        if(valL > sensorReads.highestReflection) valL = sensorReads.highestReflection;
        
        BP.get_sensor(PORT_4, Color1);
        uint16_t valC = Color1.reflected_red;
        if(valC < sensorReads.lowestRed) valC = sensorReads.lowestRed;
        if(valC > sensorReads.highestRed) valC = sensorReads.highestRed;
        
        bool lightOnBlack = isLightOnBlack(sensorReads, intersectionTollerance, valL);
        bool colorOnBlack = isColorOnBlack(sensorReads, intersectionTollerance, valC);
        cout << "WHILE TWEE " << endl;
        cout << "    lightOnBlack: " << lightOnBlack << endl;
        cout << "    colorOnBlack: " << colorOnBlack << endl;
        if(colorOnBlack || lightOnBlack){
            cout << "Found line!" << endl;
            findLine(BP, "left", sensorReads);
            return;
        }

        BP.set_motor_power(PORT_B,power);
        BP.set_motor_power(PORT_C,power);
    }
    BP.set_motor_power(PORT_B, 0);
    BP.set_motor_power(PORT_C, 0);
    
    sleep(3);
    steering(BP, "right", 610);
    // past object
    
    BP.set_motor_power(PORT_B, 0);
    BP.set_motor_power(PORT_C, 0);
    // turret reset
    BP.set_motor_position_relative(PORT_D, 210);
    cout << "Best swerve in the history of swerves" << endl;
    sleep(2);
    //find the line
    float tolerance = 0.75;

    uint16_t lightlow = sensorReads.highestReflection * (1 + tolerance); 
    uint16_t colorlow = sensorReads.lowestReflection * (1 - tolerance); 

    BP.get_sensor(PORT_4, Color1);
    uint16_t valColor = Color1.reflected_red;

    BP.get_sensor(PORT_3, Light3);
    uint16_t valLight = Light3.reflected;
    cout << "read values:" << endl;
    cout << "val light = " << valLight << endl;
    cout << "val color = " << valColor << endl;

    cout << "compare values:" << endl;
    cout << "lightlow = " << lightlow << endl;
    cout << "colorlow = " << colorlow << endl;


    bool foundIntersection = false;
    while(!foundIntersection){
        // left sensor right motor
        BP.get_sensor(PORT_3, Light3);
        uint16_t valL = Light3.reflected;
        if(valL < sensorReads.lowestReflection) valL = sensorReads.lowestReflection;
        if(valL > sensorReads.highestReflection) valL = sensorReads.highestReflection;
        
        BP.get_sensor(PORT_4, Color1);
        uint16_t valC = Color1.reflected_red;
        if(valC < sensorReads.lowestRed) valC = sensorReads.lowestRed;
        if(valC > sensorReads.highestRed) valC = sensorReads.highestRed;
        
        bool lightOnBlack = isLightOnBlack(sensorReads, intersectionTollerance, valL);
        bool colorOnBlack = isColorOnBlack(sensorReads, intersectionTollerance, valC);
        cout << "WHILE DRIE " << endl;
        cout << "    lightOnBlack: " << lightOnBlack << endl;
        cout << "    colorOnBlack: " << colorOnBlack << endl;
        if(colorOnBlack && lightOnBlack){
            cout << "lijn gevonden" << endl;
            foundIntersection = true;
        }
        BP.set_motor_power(PORT_B, -20);
        BP.set_motor_power(PORT_C, -20);
    }


    // while(valColor > colorlow && valLight < lightlow){
    //     BP.get_sensor(PORT_4, Color1);
    //     uint16_t valColor = Color1.reflected_red;
    
    //     BP.get_sensor(PORT_3, Light3);
    //     uint16_t valLight = Light3.reflected;

    //     BP.set_motor_power(PORT_B, -20);
    //     BP.set_motor_power(PORT_C, -20);
    //     cout << "read values:" << endl;
    //     cout << "val light = " << valLight << endl;
    //     cout << "val color = " << valColor << endl;

    //     cout << "compare values:" << endl;
    //     cout << "lightlow = " << lightlow << endl;
    //     cout << "colorlow = " << colorlow << endl;
    // }
    BP.set_motor_power(PORT_B, 0);
    BP.set_motor_power(PORT_C, 0);
    findLine(BP, "left", sensorReads);
    cout << "done deal yo" << endl;
    sleep(1);

}

 


void startSteering(BrickPi3 BP, const sensorData & sensorReads, bool obstacles ){

    sensor_light_t Light3;
    sensor_color_t Color1;


    int power = -20;
    while(true){
        if(obstacles){
            detectObject(BP, sensorReads);
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

void steering(BrickPi3 BP, const string & direction, const int & steps, const float & sleepCount){
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
    usleep(sleepCount*1000000);
}

void findLine(BrickPi3 BP, string turnDirection, const sensorData & sensorReads){
    // init sensors
    sensor_light_t Light3;
    sensor_color_t Color1;
    
    float tolerance = 0.10; // tolerance in percentage
    
    // get sensor val

    uint16_t lightlow = sensorReads.highestReflection * (1 + tolerance); 
    uint16_t colorlow = sensorReads.lowestReflection * (1 - tolerance); 

    //make some space
    BP.set_motor_position_relative(PORT_C, -250);
    BP.set_motor_position_relative(PORT_B, -250);
    sleep(1);

    if(turnDirection == "left"){
        int intersectionTollerance = 25; // 10 % tollerance
        bool colorOnBlack = false;
        while(!colorOnBlack){            
            BP.get_sensor(PORT_4, Color1);
            uint16_t valC = Color1.reflected_red;
            if(valC < sensorReads.lowestRed) valC = sensorReads.lowestRed;
            if(valC > sensorReads.highestRed) valC = sensorReads.highestRed;
            steering(BP, "left", 30, 0.25);

            colorOnBlack = isColorOnBlack(sensorReads, intersectionTollerance, valC);

        }
        // int powerleft = 20;
        // int powerright = -20;
        // BP.get_sensor(PORT_3, Light3);
        // uint16_t val = Light3.reflected;
        // while(val > lightlow){
        //     BP.get_sensor(PORT_3, Light3);
        //     val = Light3.reflected;
        //     steering(BP, "left", 1);
        // }

    }else if(turnDirection == "right"){
        int intersectionTollerance = 25; // 10 % tollerance
        bool lightOnBlack = false;
        while(!lightOnBlack){
            // left sensor right motor
            BP.get_sensor(PORT_3, Light3);
            uint16_t valL = Light3.reflected;
            if(valL < sensorReads.lowestReflection) valL = sensorReads.lowestReflection;
            if(valL > sensorReads.highestReflection) valL = sensorReads.highestReflection;
            
            bool lightOnBlack = isLightOnBlack(sensorReads, intersectionTollerance, valL);
        }
        // int powerleft = -20;
        // int powerright = 20;
        // BP.get_sensor(PORT_4, Color1);
        // uint16_t val = Color1.reflected_red;
        // while(val < colorlow){
        //     BP.get_sensor(PORT_4, Color1);
        //     val = Color1.reflected_red;
        //     steering(BP, "right", 1);
        // }

    }else{
        cout << "Incorrect direction given in findline function." << endl;
        return;

    }

}