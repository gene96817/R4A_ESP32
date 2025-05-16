// SetMotor.ino
// Created by Eugene Chang on 3/7/25.
// This code takes SetMotor and converts it to a related Motor_Move command.
//

// #include "SetMotor.h"
// #include "Genes_Line_Following.ino"

#include <cmath>
#include <algorithm> // For std::min and std::max
#include <iostream>
#include "GeneFunctions.h"

// Constants for motor configuration (define actual values)
constexpr int MOTOR_SPEED_LV4 = 4000;
constexpr int MOTOR_SPEED_LV3 = 3000;
constexpr int MOTOR_SPEED_LV2 = 2500;
constexpr int MOTOR_SPEED_LV1 = 1500;
constexpr int MOTOR_SPEED_MIN = 1500;
// constexpr int MOTOR_SPEED_MIN = -4095;
constexpr int MOTOR_SPEED_MAX = 4095;
constexpr int MOTOR_1_DIRECTION = 1;
constexpr int MOTOR_2_DIRECTION = 1;
constexpr int MOTOR_3_DIRECTION = 1;
constexpr int MOTOR_4_DIRECTION = 1;


//  bool trace = true;    // this enables all the print statements for debugging.

// *************************************
// logging startes here
// *************************************

bool datalogger(bool trace, int speed, float direction, int leftWheelSpeed, int rightWheelSpeed) {
    if (trace) {
        Serial.print("  Move ");
        Serial.print(speed);
        Serial.print("  ");
        Serial.print(direction);
        Serial.print("  Left Wheel ");
        Serial.print(leftWheelSpeed);
        Serial.print("  Right Wheel ");
        Serial.print(rightWheelSpeed);
    }
    return true;
}

int speed;      //relative speed from -20 to 20
int direction;  //degrees
int DEGREE_TO_RADIAN = 2 * 3.1415 / 360;
//what is relative wheel speed -- is there an offset for cos & sin?
// for the relative wheel speed to be equal, we need the direction offset to be 45 degrees
float  Direction_Offset_Degrees = 45;

int leftWheelSpeed;
int rightWheelSpeed;

// Add this function declaration before SetMotor function
bool Motor_Move(int left1, int left2, int right1, int right2) {
    // Constrain all input values to the valid motor speed range
    left1 = constrain(left1, MOTOR_SPEED_MIN, MOTOR_SPEED_MAX);
    left2 = constrain(left2, MOTOR_SPEED_MIN, MOTOR_SPEED_MAX);
    right1 = constrain(right1, MOTOR_SPEED_MIN, MOTOR_SPEED_MAX);
    right2 = constrain(right2, MOTOR_SPEED_MIN, MOTOR_SPEED_MAX);

    // Apply direction multipliers for each motor
    left1 *= MOTOR_1_DIRECTION;
    left2 *= MOTOR_2_DIRECTION;
    right1 *= MOTOR_3_DIRECTION;
    right2 *= MOTOR_4_DIRECTION;

    // Debug output
    std::cout << "[Motor_Move] Setting motors: "
              << "Left1=" << left1 << ", "
              << "Left2=" << left2 << ", "
              << "Right1=" << right1 << ", "
              << "Right2=" << right2 << "\n";

/**********************************************************************
  Basic_Line_Following.ino
  File        :  Line Following Challenge ECv1
  Author      :  Eugene Chang
  Modification:  2025/03/02
  Version     :  1

  Perform basic line following, modified code from 04.2_Track_Car example
  Line following logic by Eugene Chang

  This code is for the Freenove 4WD Car with ESP32.
  The robot is expected to follow a black line on a white background.
  The robot will stop if it loses the track.
  The robot will search for the track if it loses it.

**********************************************************************/

//****************************************
// Constants
//****************************************

//  #include "SetMotor.ino"
// #include "seeTrack.ino"

#define glf_DEBUG_MOTORS 0
// #define glf_DEBUG_STATES 0

#define glf_SPEED_LV4 4000
#define glf_SPEED_LV3 3000
#define glf_SPEED_LV2 2500
#define glf_SPEED_LV1 1500
#define glf_SPEED_MIN 1500  // This can be varied according to the motor performance \
                            // Set this large enough to get consistent readings motor movement

#define BLT_SPEED_FAST 1500
#define BLT_SPEED_MEDIUM 1200
#define BLT_SPEED_SLOW 1000

#define BLT_TURN_THRESHOLD 100

const int NUM_SENSORS = 6;


//****************************************
// Global; Variables
//****************************************

bool trace = true;  // this enables all the print statements for debugging.

// glfTimeBefore is defined in Menu.ino
// int glfTimeBefore = 0;  //Record each non-blocking time
// int glfTimeCount = 0;   //Record the number of non-blocking times
// int glfTimeFlag = 0;    //Record the blink time

//initialize memory of track detection
int sensorValue[3];
int SensorReadings[6] = { 2, 2, 2, 2, 0, 0 };  // last 6 positions read
int theta[2] = { 0, 0 };                       // last 2 theta (headings) readings

// int speed;        //relative speed from -20 to 20
// float direction;  //degrees
// int leftWheelSpeed;
// int rightWheelSpeed;





// Set motor speeds using the robotMotorSetSpeeds function
return robotMotorSetSpeeds(leftWheelSpeed, rightWheelSpeed, nullptr); // Assuming `nullptr` for `Print* display`


}

// Function to compute motor speeds and set hardware pulse widths
// SetMotor (speed, direction) was Move (speed, direction)
void SetMotor(int speed = 0, float direction = 0) {
    // Constrain the speed value to the allowed range
    speed = constrain(speed, MOTOR_SPEED_MIN, MOTOR_SPEED_MAX);

    // Convert the direction to radians (with offset) and calculate power
    float directionRadians = (direction + Direction_Offset_Degrees) * DEGREE_TO_RADIAN
;
    // int power = calculatePower(speed);

    // Wheel direction calculations:
    float leftWheelFactor = sin(directionRadians);
    float rightWheelFactor = cos(directionRadians);

     // Apply power to each wheel based on the factors:
     int leftWheelSpeed = speed * leftWheelFactor;

     if (leftWheelSpeed < 0) {leftWheelSpeed = min(leftWheelSpeed, -MOTOR_SPEED_MIN);}
     else if (leftWheelSpeed == 0) {leftWheelSpeed = 0;}
     else if (leftWheelSpeed > MOTOR_SPEED_MIN) {leftWheelSpeed = max(leftWheelSpeed, MOTOR_SPEED_MIN);};


     int rightWheelSpeed = speed * rightWheelFactor;

     if (rightWheelSpeed < 0) {rightWheelSpeed = min(rightWheelSpeed, -MOTOR_SPEED_MIN);}
     else if (rightWheelSpeed == 0) {rightWheelSpeed = 0;}
     else if (rightWheelSpeed > MOTOR_SPEED_MIN) {rightWheelSpeed = max(rightWheelSpeed, MOTOR_SPEED_MIN);};



    // Output debug information
    std::cout << "[DEBUG] Speed: " << speed << ", Direction: " << direction << "\n";
    std::cout << "Left Wheel Speed: " << leftWheelSpeed
              << ", Right Wheel Speed: " << rightWheelSpeed << "\n";



}
