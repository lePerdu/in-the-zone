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

// Controller 1/2, Stick L/R, Axis X/Y
#define C1RY						                vexRT[Ch2]
#define C1LY						                vexRT[Ch3]
#define C1RX							              vexRT[Ch1]
#define C1LX								            vexRT[Ch4]

//H-Drive using 4 Inputs
task hDrive(){
	while (true)
	{
		motor[leftMotorF]  = (C1LY - C1RX)*.8;
    motor[rightMotorF] = (C1LY + C1RX)*.8;
    motor[leftMotorR]  = (C1LY - C1RX)*.8;
    motor[rightMotorR] = (C1LY + C1RX)*.8;

		//Center Wheel Control
		motor[hMotor1] = (vexRT[Btn5U] - vexRT[Btn6U])*127;
		motor[hMotor2] = (vexRT[Btn5U] - vexRT[Btn6U])*127;
		// Motor values can only be updated every 20ms
		wait1Msec(20);
	}
}

task main()
{
	startTask(hDrive);
	while (true)
	{
		// Motor values can only be updated every 20ms
		wait1Msec(20);
	}
}
