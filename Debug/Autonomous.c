#pragma config(Sensor, in1,    gyro,           sensorGyro)
#pragma config(Sensor, dgtl1,  BLEncoder,      sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  BREncoder,      sensorQuadEncoder)
#pragma config(Motor,  port2,           leftMotorF,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           leftMotorR,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           rightMotorF,   tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           rightMotorR,   tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           hMotor1,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           hMotor2,       tmotorVex393_MC29, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//Storing Constants for Wheel Diameter and Pi
#define wheelDiameter 4
#define Pi 3.14

#define abs(X) ((X < 0) ? -1 * X : X)
#define MAX_POWER_OUT										127
#define MIN_POWER_OUT								  	-127

int tickGoal;
float driveCoeff = 0.25;

int leftError=-100;
int rightError=100;

//Ensure Motor Power is in Range
int limitMotorPower(int power,int maxPower)																								//
{																																													//
	int	outputPower;																																				//
	//
	outputPower = power;																																		//
	if(outputPower > maxPower)																												     	//
	{																																												//
		outputPower = maxPower;																													      //
	}																																												//
	else if(outputPower < (maxPower*-1))																										//
	{																																												//
		outputPower = (maxPower*-1);																													//
	}																																												//
	return(outputPower);																																		//
}

//Drive Forward with Target in Inches (Custom P Control)
void driveForwardP(int tenthsOfIn,int maxPower)
{
	SensorValue[BLEncoder] = 0; // It is good practice to reset encoder values at the start of a function.
	SensorValue[BREncoder] = 0;

	float wheelCircumference = wheelDiameter*Pi;
	int ticks = 360/wheelCircumference;
	tickGoal = (ticks * tenthsOfIn) / 10;

	int leftPower;
	int rightPower;

	while((abs(leftError) > 1)||((abs(rightError) > 1)))
	{
		leftError = (tickGoal - SensorValue[BLEncoder]);
		leftPower = (leftError * driveCoeff);
		leftPower = limitMotorPower(leftPower,maxPower);
		motor[leftMotorF] = leftPower;
		motor[leftMotorR] = leftPower;

		rightError = (tickGoal - SensorValue[BREncoder]);
		rightPower = (rightError * driveCoeff);
		rightPower = limitMotorPower(rightPower,maxPower);
		motor[rightMotorF] = rightPower;
		motor[rightMotorR] = rightPower;

	}

	motor[leftMotorF] = 0;  // stop motors
	motor[leftMotorR] = 0;
	motor[rightMotorF] = 0;
	motor[rightMotorR] = 0;

	wait1Msec(500); // used to read final encoder values on screen before ending program
}

void driveStraightDistance(int tenthsOfIn, int masterPower)
{
	float wheelCircumference = wheelDiameter*Pi;
	int ticks = 360/wheelCircumference;
	int tickGoal = (ticks * tenthsOfIn) / 10;

	//This will count up the total encoder ticks despite the fact that the encoders are constantly reset.
	int totalTicks = 0;

	//Initialise slavePower as masterPower - 5 so we don't get huge error for the first few iterations. The
	//-5 value is based off a rough guess of how much the motors are different, which prevents the robot from
	//veering off course at the start of the function.
	int slavePower = masterPower - 5;

	int error = 0;

	int kp = 10;

	SensorValue[BLEncoder] = 0;
	SensorValue[BREncoder] = 0;

	//Monitor 'totalTicks', instead of the values of the encoders which are constantly reset.
	while(abs(totalTicks) < tickGoal)
	{
		//Proportional algorithm to keep the robot going straight.
		motor[leftMotorF] = masterPower;
		motor[leftMotorR] = masterPower;
		motor[rightMotorF] = slavePower;
		motor[rightMotorR] = slavePower;
		error = SensorValue[BLEncoder] - SensorValue[BREncoder];

		slavePower += error / kp;

		SensorValue[BLEncoder] = 0;
		SensorValue[BREncoder] = 0;

		wait1Msec(100);

		//Add this iteration's encoder values to totalTicks.
		totalTicks+= SensorValue[BLEncoder];
	}
	motor[leftMotorF] = 0; // Stop the loop once the encoders have counted up the correct number of encoder ticks.
	motor[leftMotorR] = 0;
	motor[rightMotorF] = 0;
	motor[rightMotorR] = 0;
}

task main()
{
	//driveStraightDistance(100,50);
	driveForwardP(100,50);
	wait1Msec(100);
}
