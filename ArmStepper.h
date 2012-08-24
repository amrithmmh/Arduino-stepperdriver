#ifndef ArmStepper_h
#define ArmStepper_h

#include "Arduino.h"

class ArmStepper
{
public:
	ArmStepper(double _currentAngle, int _dirPin, int _stepPin, double _degPerStep, int _minMSD, double _maxSpeed, double _minSpeed);
	void setEase(double degFromBound); 
	void setDesiredAngle(double angle);
	void setCurrentAngle(double angle);
	double getDesiredAngle();
	double getCurrentAngle();
	bool step();
	void reverseDirection();
	bool isStopped();
	bool isMoving();
	bool isReady();
	void useSmoothing(bool _useSmooth);
	void setSpeed(double newCurrentSpeed);
	double getSpeed();
	void setMaxSpeed(double _maxSpeed);
	void setMinSpeed(double _minSpeed);

private:
	int dirReverse;

	double currentAngle, desiredAngle, lastAngle;
	int dirPin, stepPin;
	double degPerStep;
	int minMSD, MSD;
	double maxSpeed, minSpeed, currentSpeed;
	double easeRegion;
	bool stepPhaseIsOn;
	long lastStepTime;
	bool stopped;
	int currentDirection;
	bool useSmooth;

	void getCurrentSpeed();
};

#endif