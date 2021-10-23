
#ifndef MTRN3500_VEHICLE_H
#define MTRN3500_VEHICLE_H

#include <iostream>
#include "Shape.hpp"
#include "VectorMaths.hpp"
#include <vector>

#include <smstructs.h>

class Vehicle : public Shape {
public:

	Vehicle();
	virtual ~Vehicle();

	virtual void update(double dt);
	virtual void update(double speed_, double steering_, double dt);

	virtual void draw() = 0;

	static const int MAX_FORWARD_SPEED_MPS = 1;
	static const int MAX_BACKWARD_SPEED_MPS = -1;
	static const int MAX_LEFT_STEERING_DEGS = 40;
	static const int MAX_RIGHT_STEERING_DEGS = -40;
	static const int VEHICLE_BOUNDS = 3;

	double getSpeed() const { return speed; }
	double getSteering() const { return steering; }

	void getLaser(SM_Laser);

protected:
	double speed;     // m/s
	double steering;  // degrees
	SM_Laser LaserData;

};

double clamp(double a, double n, double b);

#endif // for MTRN3500_VEHICLE_H

