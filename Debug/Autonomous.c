#pragma config(Sensor, in1,    gyro,           sensorGyro)
#pragma config(Sensor, in2,    pot,            sensorPotentiometer)
#pragma config(Sensor, dgtl1,  BLEncoder,      sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  BREncoder,      sensorQuadEncoder)
#pragma config(Motor,  port2,           leftMotorF,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           leftMotorR,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           rightMotorF,   tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           rightMotorR,   tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           hMotor1,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           hMotor2,       tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           liftMotor,     tmotorVex393_MC29, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*

//Storing Constants for Wheel Diameter and Pi
#define wheelDiameter 4
#define Pi 3.14159

int tickGoal=0;

int rightError;
int rightPower;

int leftError;
int leftPower;

int turnError;
int turnPower;

#define liftDown 800
#define liftUp 2150

#define MAX_POWER_OUT										127
#define MIN_POWER_OUT								  	-127

typedef struct PIDData {
	float kp, ki, kd;

	float lastError;
	float lastIntegral;

	float maxIntegral;
} PIDData;

void pidDataInit(PIDData *data,
		float kp, float ki, float kd,
		float maxIntegral) {
	data->kp = kp;
	data->ki = ki;
	data->kd = kd;
	data->maxIntegral = maxIntegral;

	pidDataReset(&data);
}

void pidDataReset(PIDData *data) {
	data->lastError = 0.0;
	data->lastIntegral = 0.0;
}

float pidNextIteration(PIDData *data, float error) {
	float proportional, integral, derivative;

	proportional = error;

	data->lastIntegral += error;

	// Bound integral
	if (data->lastIntegral > data->maxIntegral)
	{
		data->lastIntegral = data->maxIntegral;
	}
	else if (data->lastIntegral < -data->maxIntegral)
	{
		data->lastIntegral = -data->maxIntegral;
	}

	integral = data->lastIntegral;

	derivative = error - data->lastError;
	data->lastError = error;

	return
		data->kp * proportional +
		data->ki * integral +
		data->kd * derivative;
}


int actualLiftAngle;
int desiredLiftAngle;
int liftPowerOut;
int errorLiftAngle;
float liftIntegral;
const float integralLimit = 100;	// with respect to input
bool liftToggle;

int limitMotorPower(int power)
{
	int	outputPower;

	outputPower = power;
	if(outputPower > MAX_POWER_OUT)
	{
		outputPower = MAX_POWER_OUT;
	}
	else if(outputPower < MIN_POWER_OUT)
	{
		outputPower = MIN_POWER_OUT;
	}
	return(outputPower);
}

task liftControl()
{
	// Separate variables to simplify tuning
	const float kp = 0.2;
	const float ki = 0.0; //0.7;
	const float kd = 0.0;

	PIDData data;
	pidDataInit(&data, kp, ki, kd, 100.0);

	while (true)
	{
		if (liftToggle)
		{
			hogCPU();

			errorLiftAngle = desiredLiftAngle - SensorValue[pot];

			liftPowerOut = limitMotorPower(pidNextIteration(&data, errorLiftAngle));

			motor[liftMotor] = liftPowerOut;

			releaseCPU();
		}
		else
		{
			motor[liftMotor] = 0;
		}

		wait1Msec(20);
	}
}

void setLiftAngle(int liftAngle)
{
	hogCPU();
	desiredLiftAngle = liftAngle;
	releaseCPU();
}

void stopMotors ()
{
	motor[leftMotorF] = 0;
	motor[leftMotorR] = 0;
	motor[rightMotorF] = 0;
	motor[rightMotorR] = 0;
}

int inchToTicks (float inch)
{
	float wheelCircumference = wheelDiameter*Pi;
	int ticks = 360/wheelCircumference;
	tickGoal = (ticks * inch) / 10;
	return tickGoal;
}

//>>>>>>>>>>>>>>>>>>>>>>>>------------------0-----------------<<<<<<<<<<<<<<<<<<<<<<<<<

void baseControl(float target)
{
	// Separate variables to simplify tuning
	const float kp = 0.3;
	const float ki = 0.0;
	const float kd = 2.0;

	// Do PID on each side separately to help account for physical inaccuracies
	PIDData rightData;
	PIDData leftData;

	pidDataInit(&leftData, kp, ki, kd, 100.0);
	pidDataInit(&rightData, kp, ki, kd, 100.0);

	SensorValue[BLEncoder] = 0;
	SensorValue[BREncoder] = 0;

	while (true) {
		hogCPU();

		rightError = SensorValue[BREncoder] - target;
		rightPower = limitMotorPower(pidNextIteration(&rightData, rightError));

		leftError = SensorValue[BLEncoder] - target;
		leftPower = limitMotorPower(pidNextIteration(&rightData, rightError));

		motor[rightMotorF] = -rightPower;
		motor[rightMotorB] = -rightPower;
		motor[leftMotorF]  = -leftPower;
		motor[leftMotorB]  = -leftPower;

		releaseCPU();
		wait1Msec(20);
	}
}

void baseTurn(float target)
{
	// Separate variables to simplify tuning
	const float kp = 0.08;
	const float ki = 0.0;
	const float kd = 0.0;

	PIDData data;

	pidDataInit(&data, kp, ki, kd, 100.0);

	while (true) {
		hogCPU();

		turnError = SensorValue[gyro] - target;
		turnPower = limitMotorPower(pidNextIteration(&data, turnError));

		// TODO Should we check that each side moves the same amount and adjust them
		// afterwards if not?
		motor[rightMotorF] = -turnPower;
		motor[rightMotorB] = -turnPower;
		motor[leftMotorF]	 =  turnPower;
		motor[leftMotorB]	 =  turnPower;

		releaseCPU();
		wait1Msec(20);
	}
}

task main ()
{
	startTask(liftControl);
	setLiftAngle(liftUp);
	liftToggle = true;

	SensorValue[gyro] = 0;

	wait1Msec(500);
	baseControl(240); // move forward 10 inches with 0 sec delay;
	wait1Msec(500);
	/*setLiftAngle(liftDown);
		wait1Msec(500);
		PIDBaseControl(50,0,1); // move forward 3 inches with 0 sec delay;
		wait1Msec(500);
		setLiftAngle(liftUp);
		wait1Msec(500);
		PIDBaseControl(-130,0,.6); // move backwards 13 inches with 0 sec delay;
		wait1Msec(500);
		stopMotors();
		*/
}
