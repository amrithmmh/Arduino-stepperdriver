// For use with sparkfun easystepper

#include "Arduino.h"
#include "ArmStepper.h"


ArmStepper::ArmStepper(double _currentAngle, int _dirPin, int _stepPin, double _degPerStep, int _minMSD, double _maxSpeed, double _minSpeed)
{
	currentAngle = _currentAngle;
	desiredAngle = currentAngle;
	dirPin = _dirPin;
	stepPin = _stepPin;
	degPerStep = _degPerStep;
	minMSD = _minMSD;
	maxSpeed = _maxSpeed;
	minSpeed = _minSpeed;

	pinMode(dirPin, OUTPUT);
	pinMode(stepPin, OUTPUT);

	easeRegion = 5;
	currentSpeed = maxSpeed;
	lastStepTime = micros();
	stepPhaseIsOn = false;
	stopped = true;
	currentDirection = 0;
	useSmooth = true;

	dirReverse = 1;
}

void ArmStepper::setEase(double degFromBound)
{
	easeRegion = degFromBound;
}

void ArmStepper::setDesiredAngle(double angle)
{
	desiredAngle = angle;
	lastAngle = currentAngle;
}

void ArmStepper::setCurrentAngle(double angle)
{
	currentAngle = angle;
}

double ArmStepper::getDesiredAngle()
{
	return desiredAngle;
}

double ArmStepper::getCurrentAngle()
{
	return currentAngle;
}

bool ArmStepper::step()
{
	stopped = true;
	if (abs(currentAngle - desiredAngle) > degPerStep)
	{
		stopped = false;

		if (currentAngle*dirReverse < desiredAngle*dirReverse)
		{
			digitalWrite(dirPin, HIGH);
			currentDirection = 1;
		} else {
			digitalWrite(dirPin, LOW);
			currentDirection = -1;
		}
		currentDirection *= dirReverse;

		if (useSmooth) { getCurrentSpeed(); }

		MSD = (int)(minMSD/currentSpeed);

		if ((micros() - lastStepTime) >= MSD)
		{
			if (stepPhaseIsOn)
			{
				digitalWrite(stepPin, LOW);
				stepPhaseIsOn = false;
				currentAngle += degPerStep*currentDirection;
			} else {
				digitalWrite(stepPin, HIGH);
				stepPhaseIsOn = true;
			}

			lastStepTime = micros();
		}
	}

	// Check micros() overflow
	if (micros() < lastStepTime)
	{
		lastStepTime = 0;
	}
}

void ArmStepper::getCurrentSpeed()
{
	/*			^				GRAPH OF ANGLE VS SPEED
		S	 max|   ---------------------------------------------
		p		|  /										     \
		e		| /											      \
		e	 min|/											       \
		d	zero|____________________________________________________>
				lastAngle									desiredAngle
										Angle

		Ramp up/down is easeRegion degrees long (x axis)

		NEVER GO TO SPEED = 0 --> delay will be microsecond delay infinite/will divide by zero
	*/

	// Check if the current move is < than the ease region
	if (abs(lastAngle - desiredAngle) < easeRegion)
	{
		// Use -abs(x) looking speed ramp
		double travel = desiredAngle - lastAngle;
		double slope = (maxSpeed-minSpeed)/(abs(travel)/2);

		// If at or before halfway through
		if (abs(currentAngle) <= abs(lastAngle + travel/2))
		{
			currentSpeed = slope*abs(currentAngle - lastAngle) + minSpeed; // Up the ramp
		} else {
			currentSpeed = slope*abs(desiredAngle - currentAngle) + minSpeed; // Down the ramp
		}
	} else {

		// Use normal speed graph (above)
		double slope = (maxSpeed-minSpeed)/easeRegion;

		if (abs(currentAngle - lastAngle) < easeRegion) // Ramp up
		{
			currentSpeed = slope*abs(currentAngle - lastAngle) + minSpeed;
		} else {
			if (abs(desiredAngle - currentAngle) < easeRegion) // Ramp down
			{
				currentSpeed = slope*abs(desiredAngle - currentAngle) + minSpeed;
			} else {
				currentSpeed = maxSpeed; // Cruise (flat region)
			}
		}
	}
}

void ArmStepper::reverseDirection()
{
	dirReverse *= -1;
}

bool ArmStepper::isStopped()
{
	return stopped;
}

bool ArmStepper::isMoving()
{
	return !stopped;
}

bool ArmStepper::isReady()
{
	return stopped;
}

void ArmStepper::useSmoothing(bool _useSmooth)
{
	useSmooth = _useSmooth;
}

void ArmStepper::setSpeed(double newCurrentSpeed)
{
	currentSpeed = newCurrentSpeed;
}

double ArmStepper::getSpeed()
{
	return currentSpeed;
}

void ArmStepper::setMaxSpeed(double _maxSpeed)
{
	maxSpeed = _maxSpeed;
}

void ArmStepper::setMinSpeed(double _minSpeed)
{
	minSpeed = _minSpeed;
}