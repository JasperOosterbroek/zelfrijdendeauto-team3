#include "BrickPi3.h"
#include "sensors.h"
#include "motors.h"

#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <vector>

using namespace std;

void exit_signal_handler(int signo);

BrickPi3 BP;
/**
 * Struct for keeping tack of the data of the course. This struct will be configured at the start of the program via input.
 */
struct course{
	vector<vector<int>> roadMap; /**< Vector of vectors of ints. 1 means legit intersection, 2 means blocked intersection */
	int height; /**< Height of the roadmap */
	int width; /**< Width of the roadmap */
	int targetY; /**< Target location on y-axis */
	int targetX; /**< Target location on x-axis */
};

/**
 * Struct for keeping track of current location and which direction both turning parts are pointing towards.
 */
struct vehicle{
	int posY; /**< Position of the vehicle on the y-axis */
	int posX; /**< Position of the vehicle on the x-axis */
	int chassisDirection;  /**< direction which the vehicle is pointing at 1 = upward, 2 = to the right, 3 = downwards and 4 = left */
	int turretDirection; /**< the position of the turret proportional to the chassis 0 = the same direction as chassis*/
};


/**
 * @param c course to set the values of
 * Asks for input from cin for the setup of the course.
 *  - width and height for the course
 *  - targetX and targetY for the target
 */
void setCourse(course & c){
	int height = 0;
	int width = 0;
	int targetX = -1;
	int targetY = -1;

	cout << "Hoe breed is het wegennet? ";
	cin >> width;
	
	cout << "Hoe hoog is het wegennet? ";
	cin >> height;
	
	while(targetX < 0 || targetX > width){
		cout << "Op welke x-as is het doel? ";
		cin >> targetX;
	}

	while(targetY < 0 || targetY > height){
		cout << "Op welke y-as is het doel? ";
		cin >> targetY;
	}
	
	c.width = width;
	c.height = height;
	c.targetX = targetX;
	c.targetY = targetY;
	cout << "Wegennet is " << width << " bij " << height << endl;
	cout << "Het doelwit staat op [" << c.targetX << "," << c.targetY << "]" << endl;
	
	vector<vector<int>> grid;
	for(int i = 0; i < height; i++){
		vector<int> row = {};
		for(int j = 0; j < width; j++){
			row.push_back(1);
		}
		grid.push_back(row);
	}
	c.roadMap = grid;
}

void setVehicle(vehicle & v, const course & c){
	int chassisY = -1;
	int chassisX = -1;
	int chassisDirection = 2;
	int turretDirection = 0;
	
	while(chassisY < 0 || chassisY > c.height){
		cout << "Op welke positie van de y-as staat het voertuig? ";
		cin >> chassisY;
	}

	while(chassisX < 0 || chassisX > c.width){
		cout << "Op welke positie van de x-as staat het voertuig? ";
		cin >> chassisX;
	}
	
	v.posX = chassisX;
	v.posY = chassisY;
	v.chassisDirection = chassisDirection;
	v.turretDirection = turretDirection;
}

void gridsteering(BrickPi3 BP, const sensorData & sensorReads ){
    sensor_light_t Light3;
    sensor_color_t Color1;

    int power = -25;
	int intersectionTollerance = 10; // 10 % tollerance
    while(true){
		
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
		if(colorOnBlack && lightOnBlack){
			
		}
    }
}


/** main
 * Main function to start the initialisation of gridsteering
 * 
 */
int main(){
	signal(SIGINT, exit_signal_handler);

	BP.detect();

	BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);
	BP.set_sensor_type(PORT_4, SENSOR_TYPE_NXT_COLOR_RED);

	BP.reset_motor_encoder(PORT_B); // Right motor
	BP.reset_motor_encoder(PORT_C); // Left motor

	course grid;
	setCourse(grid);

	vehicle ferdinand;
	setVehicle(ferdinand)
	ferdinand.posX = 0;
	ferdinand.posY = 0;
	ferdinand.chassisDirection = 0;
	ferdinand.turretDirection = 0;

	sensorData sensorReads = processCalibration(BP);
	printSensorCalibration(sensorReads);
	sleep(5);

	gridsteering(BP, sensorReads);
}

/**
 * @param signo integer for the ctrl + c command
 * Handels ctrl + c to gracefully shutdown the program 
 */ 
void exit_signal_handler(int signo){
	if(signo == SIGINT){
		BP.reset_all();
		exit(-2);
	}
}