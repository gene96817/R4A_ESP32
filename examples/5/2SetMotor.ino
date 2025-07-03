// SetMotor.ino
// Created by Eugene Chang on 3/7/25.
// This code takes iss called by 3stateMachine.ino with the speed and direction
// SetMotor and converts it to a related Motor_Move command.
//
/*
 * Sets motor speeds based on desired speed and direction
 *
 * @param speed: Overall speed (-glf_SPEED_MAX to glf_SPEED_MAX)
 * @param direction: Direction in degrees (0-359)
 * @param debug: Enable debug output if non-zero
 *
 * @return MOTOR_SUCCESS on success, error code otherwise
 */


#include <math.h>


/* motor_control.h */
#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

/* Motor configuration */
#define MOTOR_SPEED_MAX    4095
#define MOTOR_SPEED_MIN    1500

/* Direction constants */
#define MOTOR_DIR_FORWARD   1
#define MOTOR_DIR_REVERSE  -1

// Motor pin definitions
#define PIN_MOTOR_M1_IN1    8  // Motor 1 input 1
#define PIN_MOTOR_M1_IN2    9  // Motor 1 input 2
#define PIN_MOTOR_M2_IN1    10 // Motor 2 input 1
#define PIN_MOTOR_M2_IN2    11 // Motor 2 input 2


/* Function prototypes */
/****************** 
void SetMotor(int speed, float direction, int debug);
int datalogger(int trace, int speed, float direction,
               int left_speed, int right_speed);
**************/

#endif /* MOTOR_CONTROL_H */

/* Math constants */
// #define PI 3.14159265358979323846
// #define DEG_TO_RAD (PI / 180.0)
#define DIR_OFFSET_DEG 45.0f


/* Helper macros */
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define CONSTRAIN(x, low, high) (MAX(MIN((x), (high)), (low)))

// Define NUM_SENSORS before using it
const int NUM_SENSORS = 6;

/* Global variables */
static bool trace = true;
// static int sensorValue[NUM_SENSORS];
static int theta_history[2];

/* Motor speed levels */
#define glf_SPEED_MAX 4095
#define glf_SPEED_LV4 4000
#define glf_SPEED_LV3 3000
#define glf_SPEED_LV2 2500
#define glf_SPEED_MIN 1500

#define glf_SPEED_FAST 1500
#define glf_SPEED_MEDIUM 1200
#define glf_SPEED_SLOW 1000

#define glf_TURN_THRESHOLD 100

// glfTimeBefore is defined in Menu.ino
// int glfTimeBefore = 0;  //Record each non-blocking time
// int glfTimeCount = 0;   //Record the number of non-blocking times
// int glfTimeFlag = 0;    //Record the blink time


int speed;      //relative speed from -20 to 20
int direction;  //degrees

float DEGREE_TO_RADIAN = 2 * PI / 360;


//what is relative wheel speed -- is there an offset for cos & sin?
// for the relative wheel speed to be equal, we need the direction offset to be 45 degrees
float  Direction_Offset_Degrees = 45;
int leftWheelSpeed;
int rightWheelSpeed;
    // int power = calculatePower(speed);

    // Wheel direction calculations:

// Caution: Motor 1 & 2 should be opposite polarity of Motor 3 & 4

// #define MOTOR_1_DIRECTION  1
// #define MOTOR_2_DIRECTION  1
// #define MOTOR_3_DIRECTION  1
// #define MOTOR_4_DIRECTION  1
#define MOTOR_DIR_FORWARD  1
#define MOTOR_DIR_REVERSE  -1

/* Error codes */
#define MOTOR_SUCCESS     0
#define MOTOR_ERR_PARAM  -1
#define MOTOR_ERR_HW     -2

/* Function prototypes */

//A function to control the car motor
/*************************************
void Motor_Move(int m1_speed, int m2_speed, int m3_speed, int m4_speed)
{
  m1_speed = MOTOR_1_DIRECTION * constrain(m1_speed, MOTOR_SPEED_MIN, MOTOR_SPEED_MAX);
  m2_speed = MOTOR_2_DIRECTION * constrain(m2_speed, MOTOR_SPEED_MIN, MOTOR_SPEED_MAX);
  m3_speed = MOTOR_3_DIRECTION * constrain(m3_speed, MOTOR_SPEED_MIN, MOTOR_SPEED_MAX);
  m4_speed = MOTOR_4_DIRECTION * constrain(m4_speed, MOTOR_SPEED_MIN, MOTOR_SPEED_MAX);

  if (m1_speed >= 0)
  {
    pca9685.setChannelPulseWidth(PIN_MOTOR_M1_IN1, m1_speed);
    pca9685.setChannelPulseWidth(PIN_MOTOR_M1_IN2, 0);
  }
  else
  {
    m1_speed = -m1_speed;
    pca9685.setChannelPulseWidth(PIN_MOTOR_M1_IN1, 0);
    pca9685.setChannelPulseWidth(PIN_MOTOR_M1_IN2, m1_speed);
  }
  if (m2_speed >= 0)
  {
    pca9685.setChannelPulseWidth(PIN_MOTOR_M2_IN1, m2_speed);
    pca9685.setChannelPulseWidth(PIN_MOTOR_M2_IN2, 0);
  }
  else
  {
    m2_speed = -m2_speed;
    pca9685.setChannelPulseWidth(PIN_MOTOR_M2_IN1, 0);
    pca9685.setChannelPulseWidth(PIN_MOTOR_M2_IN2, m2_speed);
  }
  if (m3_speed >= 0)
  {
    pca9685.setChannelPulseWidth(PIN_MOTOR_M3_IN1, m3_speed);
    pca9685.setChannelPulseWidth(PIN_MOTOR_M3_IN2, 0);
  }
  else
  {
    m3_speed = -m3_speed;
    pca9685.setChannelPulseWidth(PIN_MOTOR_M3_IN1, 0);
    pca9685.setChannelPulseWidth(PIN_MOTOR_M3_IN2, m3_speed);
  }
  if (m4_speed >= 0)
  {
    pca9685.setChannelPulseWidth(PIN_MOTOR_M4_IN1, m4_speed);
    pca9685.setChannelPulseWidth(PIN_MOTOR_M4_IN2, 0);
  }
  else
  {
    m4_speed = -m4_speed;
    pca9685.setChannelPulseWidth(PIN_MOTOR_M4_IN1, 0);
    pca9685.setChannelPulseWidth(PIN_MOTOR_M4_IN2, m4_speed);
  }
}

void Motor_Move(int m1_direction, int m1_speed, int m2_direction, int m2_speed) {
    // Motor 1
    if (m1_direction == 1) {  // Forward
        pca9685.setPWM(PIN_MOTOR_M1_IN1, 0, m1_speed);
        pca9685.setPWM(PIN_MOTOR_M1_IN2, 0, 0);
    }
    else if (m1_direction == -1) {  // Backward
        pca9685.setPWM(PIN_MOTOR_M1_IN1, 0, 0);
        pca9685.setPWM(PIN_MOTOR_M1_IN2, 0, m1_speed);
    }

    // Motor 2
    if (m2_direction == 1) {  // Forward
        pca9685.setPWM(PIN_MOTOR_M2_IN1, 0, m2_speed);
        pca9685.setPWM(PIN_MOTOR_M2_IN2, 0, 0);
    }
    else if (m2_direction == -1) {  // Backward
        pca9685.setPWM(PIN_MOTOR_M2_IN1, 0, 0);
        pca9685.setPWM(PIN_MOTOR_M2_IN2, 0, m2_speed);
    }
}

*********************************/



void SetMotor(int speed, float direction, int debug) {
    float directionRadians;
    float leftWheelFactor, rightWheelFactor;
    int leftWheelSpeed, rightWheelSpeed;

    /* Parameter validation */
    if (speed < -glf_SPEED_MAX || speed > glf_SPEED_MAX) {
        return;
    }


    /* Convert direction to radians and calculate factors */
    directionRadians = (direction + DIR_OFFSET_DEG) * DEG_TO_RAD;
    leftWheelFactor = sinf(directionRadians);
    rightWheelFactor = cosf(directionRadians);

    /* Calculate wheel speeds */
    leftWheelSpeed = (int)(speed * leftWheelFactor);
    rightWheelSpeed = (int)(speed * rightWheelFactor);

    /* Constrain wheel speeds */
    leftWheelSpeed = CONSTRAIN(leftWheelSpeed, -glf_SPEED_MAX, glf_SPEED_MAX);
    rightWheelSpeed = CONSTRAIN(rightWheelSpeed, -glf_SPEED_MAX, glf_SPEED_MAX);


    /* Debug output */
    if (debug) {
        printf("[DEBUG] Speed: %d, Direction: %.2f\n", speed, direction);
        printf("Left Wheel Speed: %d, Right Wheel Speed: %d\n",
               leftWheelSpeed, rightWheelSpeed);
    }
    
    /* Send commands to motors */
   //  return Motor_Move(leftWheelSpeed, leftWheelSpeed,
   //                  rightWheelSpeed, rightWheelSpeed);
	robotMotorSetSpeeds(leftWheelSpeed, rightWheelSpeed);



}


// *************************************
// logging startes here
// *************************************

int datalogger(int trace, int speed, float direction, 
               int left_speed, int right_speed) {
    if (!trace) {
        return MOTOR_SUCCESS;
    }
    
    printf("Move %d %.2f Left:%d Right:%d\n",
           speed, direction, left_speed, right_speed);
    
    return MOTOR_SUCCESS;
}

///////////////////////////////////////////


//****************************************
// Global; Variables
//****************************************

//initialize memory of track detection
int theta[2] = { 0, 0 };                       // last 2 theta (headings) readings

// int speed;        //relative speed from -20 to 20
// float direction;  //degrees
// int leftWheelSpeed;
// int rightWheelSpeed;

