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

int tickGoal=0;
int error;

void moveBase (int speed)
{
	motor[leftMotorF] = speed;
	motor[leftMotorR] = speed;
	motor[rightMotorF] = speed;
	motor[rightMotorR] = speed;
}

void moveRightBase (int speed)
{
	motor[rightMotorF] = speed;
	motor[rightMotorR] = speed;
}

void moveLeftBase (int speed)
{
	motor[leftMotorF] = speed;
	motor[leftMotorR] = speed;
}

void turnBase (int speed) // positive is clockwise
{
	motor[leftMotorF] = speed;
	motor[leftMotorR] = speed;
	motor[rightMotorF] = -speed;
	motor[rightMotorR] = -speed;
}

int inchToTicks (float inch)
{
	float wheelCircumference = wheelDiameter*Pi;
	int ticks = 360/wheelCircumference;
	tickGoal = (ticks * inch) / 10;
	return tickGoal;
}

int fixTimerValue (float rawSeconds)
{
	int miliseconds;
	miliseconds = rawSeconds*1000;
	if (miliseconds < 250)
	{
		miliseconds = 250;
	}
	return miliseconds;
}

//>>>>>>>>>>>>>>>>>>>>>>>>------------------0-----------------<<<<<<<<<<<<<<<<<<<<<<<<<

void PIDBaseControl (float target, float waitTime, float maxPower = 1)
{
	float Kp = 0.2;
	float Ki = 0.05;
	float Kd = 0.5;

	float proportion;
	int integralRaw;
	float integral;
	int lastError;
	int derivative;

	float integralActiveZone = inchToTicks(3);
	float integralPowerLimit = 50/Ki;

	int finalPower;

	float Kp_C = 0.01;
	int error_drift;
	float proportion_drift;

	bool timerBool = true;

	SensorValue[BLEncoder] = 0;
	SensorValue[BREncoder] = 0;

	clearTimer(T1);

	while (time1[T1] < fixTimerValue(waitTime))
	{
		error = inchToTicks(target)-(SensorValue[BLEncoder]);

		proportion = Kp*error;

		if (abs(error) < integralActiveZone && error != 0)
		{
			integralRaw = integralRaw+error;
		}
		else
		{
			integralRaw = 0;
		}

		if (integralRaw > integralPowerLimit)
		{
			integralRaw = integralPowerLimit;
		}
		if (integralRaw < -integralPowerLimit)
		{
			integralRaw = -integralPowerLimit;
		}

		integral = Ki*integralRaw;

		derivative = Kd*(error-lastError);
		lastError = error;

		if (error == 0)
		{
			derivative = 0;
		}

		finalPower = proportion+integral+derivative; //proportion+derivative+integral

		if (finalPower > maxPower*127)
		{
			finalPower = maxPower*127;
		}
		else if (finalPower < -maxPower*127)
		{
			finalPower = -maxPower*127;
		}
		error_drift = SensorValue[BREncoder]-SensorValue[BLEncoder];
		proportion_drift = Kp_C*error_drift;

		moveLeftBase(finalPower+proportion_drift);
		moveRightBase(finalPower-proportion_drift);

		wait1Msec(40);

		if (error < 30)
		{
			timerBool = false;
		}

		if (timerBool)
		{
			clearTimer(T1);
		}
	}
	moveBase(0);
}

void PIDBaseTurn (float target, float waitTime, float maxPower = 1)
{
	float Kp = 0.2;
	float Ki = 0.05;
	float Kd = 0.5;

	int error;

	float proportion;
	int integralRaw;
	float integral;
	int lastError;
	int derivative;

	float integralActiveZone = inchToTicks(3);
	float integralPowerLimit = 50/Ki;

	int finalPower;

	bool timerBool = true;

	SensorValue[rightMotorR] = 0;
	SensorValue[leftMotorR] = 0;
	SensorValue[rightMotorF] = 0;
	SensorValue[leftMotorF] = 0;

	clearTimer(T1);

	while (time1[T1] < fixTimerValue(waitTime))
	{
		error = target-SensorValue[gyro];

		proportion = Kp*error;

		if (abs(error) < integralActiveZone && error != 0)
		{
			integralRaw = integralRaw+error;
		}
		else
		{
			integralRaw = 0;
		}

		if (integralRaw > integralPowerLimit)
		{
			integralRaw = integralPowerLimit;
		}
		if (integralRaw < -integralPowerLimit)
		{
			integralRaw = -integralPowerLimit;
		}

		integral = Ki*integralRaw;

		derivative = Kd*(error-lastError);
		lastError = error;

		if (error == 0)
		{
			derivative = 0;
		}

		finalPower = proportion+integral+derivative; //proportion+derivative+integral

		if (finalPower > maxPower*127)
		{
			finalPower = maxPower*127;
		}
		else if (finalPower < -maxPower*127)
		{
			finalPower = -maxPower*127;
		}

		turnBase(finalPower);

		wait1Msec(40);

		if (error < 30)
		{
			timerBool = false;
		}

		if (timerBool)
		{
			clearTimer(T1);
		}
	}
	turnBase(0);
}

task main ()
{
	PIDBaseControl(100,0,0.7); // move forward 12 inches with 0 sec delay;
	//PIDBaseTurn(100,1,0.7); // turn right 100 degrees with 1 sec delay;
	while(true)
	{
		wait1Msec(20);
	}
}
