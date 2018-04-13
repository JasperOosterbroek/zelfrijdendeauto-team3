#include "gridsteering.h"


using namespace std;

void exit_signal_handler(int signo);

BrickPi3 BP;




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
			row.push_back(0);
		}
		grid.push_back(row);
	}
	c.roadMap = grid;
}

void setVehicle(vehicle & v, const course & c){
	int chassisY = -1;
	int chassisX = -1;
	int directionY = 0;
	int directionX = 0;
	int turretDirection = 0;
	
//	ensure a value between 0 and height of course
	while(chassisY < 0 || chassisY > c.height){
		cout << "Op welke positie van de y-as staat het voertuig? ";
		cin >> chassisY;
	}

//	ensure a value between 0 and width of course
	while(chassisX < 0 || chassisX > c.width){
		cout << "Op welke positie van de x-as staat het voertuig? ";
		cin >> chassisX;
	}
	
//	ensure a value between -1 and 1
	cout << "Welke richting op de y-as gaat de wagen (1 = omhoog, 0 = niets, -1 = omlaag) ? " << endl;
	cin >> directionY;
	while(directionY < -1 || directionY > 1){
			cout << "Welke richting op de y-as gaat de wagen (1 = omhoog, 0 = niets, -1 = omlaag) ? " << endl;
			cin >> directionY;
	}
	
//	ensure a value between -1 and 1
	cout << "Welke richting op de x-as gaat de wagen (1 = rechts, 0 = niets, -1 = links) ? " << endl;
	cin >> directionX;
	while(directionX < -1 || directionX > 1){
			cout << "Welke richting op de x-as gaat de wagen (1 = rechts, 0 = niets, -1 = links) ? " << endl;
			cin >> directionX;
	}
	
	v.posX = chassisX;
	v.posY = chassisY;
	v.directionX = directionX;
	v.directionY = directionY;
	v.turretDirection = turretDirection;
}

void turnVehicle(BrickPi3 BP, const int & previousDirectionY, const int & previousDirectionX, const vehicle & v){
	cout << "previous coords = [" << previousDirectionX << "," << previousDirectionY << "]" << endl;
	cout << "New coords = [" << v.directionX << "," << v.directionY << "]" << endl;
	
	if(v.directionY != previousDirectionY || v.directionX != previousDirectionX){
		// 610 = 90 degree turn
//		180 turns
		if((previousDirectionX != v.directionX && previousDirectionY == v.directionY) || (previousDirectionY != v.directionY && previousDirectionX == v.directionX)){
			steering(BP, "right", 1220, 2.0);
			cout << "DRAAI OM AS HEEN";
//		90 degree turn from y to x
		}else if(previousDirectionX == 0 && previousDirectionX != v.directionX){
			cout << "DRAAI VAN HOOGTE NAAR BREEDTE";
			steering(BP, "right", 610 * v.directionX, 2.0);
//		90 degree turn from x to y
		}else if(previousDirectionY == 0 && previousDirectionY != v.directionY){
			cout << "DRAAI VAN BREEDTE NAAR HOOGTE";
			steering(BP, "right", 610 * v.directionY, 2.0);
		}
	}
}


void moveToIntersection(BrickPi3 BP, const sensorData & sensorReads){
    sensor_light_t Light3;
    sensor_color_t Color1;
	
	int intersectionTollerance = 25; // 10 % tollerance
	bool foundIntersection = false;
    int power = -20;
	while(!foundIntersection){
		// left sensor right motor
		BP.get_sensor(PORT_3, Light3);
		uint16_t valL = Light3.reflected;
		if(valL < sensorReads.lowestReflection) valL = sensorReads.lowestReflection;
		if(valL > sensorReads.highestReflection) valL = sensorReads.highestReflection;
        int16_t rightmotorpower = (100*(valL - sensorReads.lowestReflection)/(sensorReads.highestReflection - sensorReads.lowestReflection));
		
		BP.get_sensor(PORT_4, Color1);
		uint16_t valC = Color1.reflected_red;
		if(valC < sensorReads.lowestRed) valC = sensorReads.lowestRed;
		if(valC > sensorReads.highestRed) valC = sensorReads.highestRed;
        int16_t leftmotorpower = 100 -(100*(valC - sensorReads.lowestRed))/(sensorReads.highestRed - sensorReads.lowestRed);
		
		bool lightOnBlack = isLightOnBlack(sensorReads, intersectionTollerance, valL);
		bool colorOnBlack = isColorOnBlack(sensorReads, intersectionTollerance, valC);
		if(colorOnBlack && lightOnBlack){
			foundIntersection = true;
		}else{
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
		}
	}
	BP.set_motor_position_relative(PORT_B, -400);
	BP.set_motor_position_relative(PORT_C, -400);
	usleep(2000000);
}

void determineRoute(vehicle & ferdinand, course & grid){
	cout << "Nieuwe directie [" << ferdinand.directionX << "," << ferdinand.directionY << "]" << endl;
	// meaning the robot move left to right or right to left
	cout << "CURRENT position on the grid: [" << ferdinand.posX << "," << ferdinand.posY << "]" << endl;
	if(ferdinand.directionY == 0){
		// moving from the left to the right
		if(ferdinand.directionX == 1){
			// go to the right
			if(ferdinand.posX+1 < grid.roadMap[ferdinand.posY].size() && grid.roadMap[ferdinand.posY][ferdinand.posX+1] != 1){
				cout << "from left to right check right" << endl;
				ferdinand.directionX = 1;
				ferdinand.directionY = 0;
			// check downwards
			}else if(ferdinand.posY+1 < grid.roadMap.size() && grid.roadMap[ferdinand.posY+1][ferdinand.posX] != 1){
				cout << "from left to right check down" << endl;
				ferdinand.directionX = 0;
				ferdinand.directionY = 1;
			// check above
			}else if(ferdinand.posY-1 >= 0 && grid.roadMap[ferdinand.posY-1][ferdinand.posX] != 1){
				cout << "from left to right check up" << endl;
				ferdinand.directionX = 0;
				ferdinand.directionY = -1;
			}else{
				cout << "GRID ROUTE AFGEBLOKD OP: [" << ferdinand.posX << "," << ferdinand.posY << "]" << endl;
				ferdinand.directionX = -1;
				ferdinand.directionY = 0;
				grid.roadMap[ferdinand.posY][ferdinand.posX] = 1;
			}
		// moving from right to left
		}else if(ferdinand.directionX == -1){
			// downwards
			if(ferdinand.posY+1 < grid.roadMap.size() && grid.roadMap[ferdinand.posY+1][ferdinand.posX] != 1){
				cout << "from right to left check down" << endl;
				ferdinand.directionX = 0;
				ferdinand.directionY = 1;
			// above
			}else if(ferdinand.posY-1 >= 0 && grid.roadMap[ferdinand.posY-1][ferdinand.posX] != 1){
				cout << "from right to left check up" << endl;
				ferdinand.directionX = 0;
				ferdinand.directionY = -1;
			// to the left
			}else if(ferdinand.posX-1 < grid.roadMap[ferdinand.posY].size() && grid.roadMap[ferdinand.posY][ferdinand.posX-1] != 1){
				cout << "from right to left check left" << endl;
				ferdinand.directionX = -1;
				ferdinand.directionY = 0;
			}else{
				cout << "GRID ROUTE AFGEBLOKD OP: [" << ferdinand.posX << "," << ferdinand.posY << "]" << endl;
				ferdinand.directionX = 1;
				ferdinand.directionY = 0;
				grid.roadMap[ferdinand.posY][ferdinand.posX] = 1;
			}
		}
	// meaning the robot moves up to down or down to up
	}else if(ferdinand.directionX == 0){
		// moving from up to down
		if(ferdinand.directionY == 1){
			// to the right
			if(ferdinand.posX+1 < grid.roadMap[ferdinand.posY].size() && grid.roadMap[ferdinand.posY][ferdinand.posX+1] != 1){
				cout << "from up to down check right" << endl;
				ferdinand.directionX = 1;
				ferdinand.directionY = 0;
			// downwards
			}else if(ferdinand.posY+1 < grid.roadMap.size() && grid.roadMap[ferdinand.posY+1][ferdinand.posX] != 1){
				cout << "from up to down check down" << endl;
				ferdinand.directionX = 0;
				ferdinand.directionY = 1;
			// to the left
			}else if(ferdinand.posX-1 < grid.roadMap[ferdinand.posY].size() && grid.roadMap[ferdinand.posY][ferdinand.posX-1] != 1){
				cout << "from up to down check left" << endl;
				ferdinand.directionX = -1;
				ferdinand.directionY = 0;
			}else{
				cout << "GRID ROUTE AFGEBLOKD OP: [" << ferdinand.posX << "," << ferdinand.posY << "]" << endl;
				ferdinand.directionX = 0;
				ferdinand.directionY = -1;
				grid.roadMap[ferdinand.posY][ferdinand.posX] = 1;
			}
		// moving from down to up
		}else if(ferdinand.directionY == -1){
			 // to the right
			if(ferdinand.posX+1 < grid.roadMap[ferdinand.posY].size() && grid.roadMap[ferdinand.posY][ferdinand.posX+1] != 1){
				cout << "from down to up check right" << endl;
				ferdinand.directionX = 1;
				ferdinand.directionY = 0;
			// updwards
			}else if(ferdinand.posY-1 >= 0 && grid.roadMap[ferdinand.posY-1][ferdinand.posX] != 1){
				cout << "from down to up check up" << endl;
				ferdinand.directionX = 0;
				ferdinand.directionY = 1;
			// to the left
			}else if(ferdinand.posX-1 < grid.roadMap[ferdinand.posY].size() && grid.roadMap[ferdinand.posY][ferdinand.posX-1] != 1){
				cout << "from down to up check left" << endl;
				ferdinand.directionX = -1;
				ferdinand.directionY = 0;
			}else{
				cout << "GRID ROUTE AFGEBLOKD OP: [" << ferdinand.posX << "," << ferdinand.posY << "]" << endl;
				ferdinand.directionX = 0;
				ferdinand.directionY = -1;
				grid.roadMap[ferdinand.posY][ferdinand.posX] = 1;
			}
		}
	}
	
}

void gridsteering(BrickPi3 BP, const sensorData & sensorReads ){
    sensor_light_t Light3;
    sensor_color_t Color1;
	course grid;
	setCourse(grid);

	vehicle ferdinand;
	setVehicle(ferdinand, grid);


    int power = -20;
    while(ferdinand.posX != grid.targetX-1 || ferdinand.posY != grid.targetY-1){
		// ultrasonic sensor checking for obstacle pseudocode
		sensor_ultrasonic_t ult;
		int16_t distance = ultrasoon_detectie(BP, ult);
//		check 28cm of distance
		if(distance > 24 && distance <= 30){
			cout << "OBJECT FOUND" << endl;
//			Get pos of next intersection, and set it as obstacle.
			int nextX = ferdinand.posX + ferdinand.directionX;
			int nextY = ferdinand.posY + ferdinand.directionY;
			grid.roadMap[nextY][nextX] = 1;
			steering(BP, "right", 1220);
			moveToIntersection(BP, sensorReads);
		}
		
        // left sensor right motor
        BP.get_sensor(PORT_3, Light3);
        uint16_t valL = Light3.reflected;
        if(valL < sensorReads.lowestReflection) valL = sensorReads.lowestReflection;
        if(valL > sensorReads.highestReflection) valL = sensorReads.highestReflection;
        int16_t rightmotorpower = (100*(valL - sensorReads.lowestReflection)/(sensorReads.highestReflection - sensorReads.lowestReflection));

//        cout << "Light / right motor stats:" << endl;
//        cout << "    valL: " << valL << endl;
//        cout << "    sensorReads.lowestReflection: " << sensorReads.lowestReflection << endl;
//        cout << "    sensorReads.highestReflection: " << sensorReads.highestReflection << endl;
//        cout << "    rightmotorpower: " << rightmotorpower << endl;
//        cout << "    power + (rightmotorpower*-1): " << power + (rightmotorpower*-1) << endl;

        BP.get_sensor(PORT_4, Color1);
        uint16_t valC = Color1.reflected_red;
        if(valC < sensorReads.lowestRed) valC = sensorReads.lowestRed;
        if(valC > sensorReads.highestRed) valC = sensorReads.highestRed;
        int16_t leftmotorpower = 100 -(100*(valC - sensorReads.lowestRed))/(sensorReads.highestRed - sensorReads.lowestRed);

//        cout << "Color / left motor stats:" << endl;
//        cout << "    valC: " << valC << endl;
//        cout << "    sensorReads.lowestRed: " << sensorReads.lowestRed << endl;
//        cout << "    sensorReads.highestRed: " << sensorReads.highestRed << endl;
//        cout << "    leftmotorpower: " << leftmotorpower << endl;
//        cout << "    power + (leftmotorpower*-1): " << power + (leftmotorpower*-1) << endl;

		int intersectionTollerance = 20; // 10 % tollerance
		bool lightOnBlack = isLightOnBlack(sensorReads, intersectionTollerance, valL);
		bool colorOnBlack = isColorOnBlack(sensorReads, intersectionTollerance, valC);
		if(colorOnBlack && lightOnBlack){
		//	Saving the values to determine the turn needed to make
			int previousDirectionY = ferdinand.directionY;
			int previousDirectionX = ferdinand.directionX;
			ferdinand.posX += ferdinand.directionX;
			ferdinand.posY += ferdinand.directionY;
			cout << "Kruispunt gevonden!" << endl;
			
			cout << "Oude directie [" << previousDirectionX << "," << previousDirectionY << "]" << endl;
			determineRoute(ferdinand, grid);
			
			
			BP.set_motor_position_relative(PORT_B, -400);
			BP.set_motor_position_relative(PORT_C, -400);
			sleep(1);
			turnVehicle(BP,previousDirectionY,previousDirectionX, ferdinand);
			sleep(2);
		}
		
		
        if(rightmotorpower > 40){
            leftmotorpower = -40;
        }else if(leftmotorpower > 40){
            rightmotorpower = -40;
        }

//        cout << endl;

        // right sensor left motor
        if(leftmotorpower < 10 && rightmotorpower < 10){
            BP.set_motor_power(PORT_B,power);
            BP.set_motor_power(PORT_C,power);
        }else{
            BP.set_motor_power(PORT_B, power + (rightmotorpower*-1));
            BP.set_motor_power(PORT_C, power + (leftmotorpower*-1));
        }
//        cout << "motors: " << leftmotorpower << " - " << rightmotorpower << endl;
//        cout << endl;
    }
	cout << "WE ZIJN TROTS OP FERDINAND" << endl;
	
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

	sensorData sensorReads = processCalibration(BP);
	printSensorCalibration(sensorReads);
	sleep(5);
	gridsteering(BP, sensorReads);
	BP.reset_all();
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