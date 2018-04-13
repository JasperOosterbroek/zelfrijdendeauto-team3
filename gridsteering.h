#ifndef GRIDSTEERING_H
#define GRIDSTEERING_H

#include "BrickPi3.h"
#include "sensors.h"
#include "motors.h"

#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <vector>

using namespace std;

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
	int directionY;  /**< direction which the vehicle is pointing at 1 = down, 0 = none, -1 = up */
	int directionX;  /**< direction which the vehicle is pointing at 1 = right, 0 = none, -1 = left */
	int turretDirection; /**< the position of the turret proportional to the chassis 0 = the same direction as chassis*/
};

/**
 * @param c course to set the values of
 * Asks for input from cin for the setup of the course.
 *  - width and height for the course
 *  - targetX and targetY for the target
 */
void setCourse(course & c);

/**
 * @param v vehicle to be set
 * @param c course needed to prevent setting the vehicle out of bounds
 */
void setVehicle(vehicle & v, const course & c);

/**
 * @param BP Brickpi used for steering the vehicle
 * @param previousDirectionY integer containing the previous orientation on the y-axis
 * @param previousDirectionX integer containing the previous orientation on the x-axis
 * @param v vehicle struct with the target direction
 * 
 * This function turns a vehicle based on its previous coords and its new coords. 
 * It bases its turn on the difference between
 */
void turnVehicle(BrickPi3 BP, const int & previousDirectionY, const int & previousDirectionX, const vehicle & v);

/**
 * @param BP Brickpi used to move the vehicle 
 * @param sensorReads sensorData struct containing calibration values for line-driving
 * 
 * This function drives the vehicle on the line towards the next intersection on the line. 
 * After it found the intersection it moves forward so that the turning point is on the intersection.
 */
void moveToIntersection(BrickPi3 BP, const sensorData & sensorReads);

/**
 * @param ferdinand the vehicle which is used for storing the direction
 * @param grid used to check eventual obstacles and dead routes on the course
 * 
 * Follows a set of priorities to give directions to the vehicle.
 */
void determineRoute(vehicle & ferdinand, course & grid);

/**
 * @param BP Brickpi used to move the vehicle 
 * @param sensorReads sensorData struct containing calibration values for line-driving
 * 
 * Follows grid towards the target. Detects obstacles in its way and steers around them.
 */
void gridsteering(BrickPi3 BP, const sensorData & sensorReads);


#endif /* GRIDSTEERING_H */