// SetMotor.ino
// Created by Eugene Chang on 3/7/25.
// This code takes SetMotor and converts it to a related Motor_Move command.
//

// #include "SetMotor.h"
// #include "Genes_Line_Following.ino"

#include <cmath>
#include <algorithm> // For std::min and std::max

// Shared constants
constexpr int MAX_PWM = 4095;
constexpr int MIN_PWM = -4095;
constexpr int MAX_SPEED = 20;
constexpr int MIN_SPEED = -20;
constexpr float DEGREE_TO_RADIAN = M_PI / 180.0;
constexpr float DIRECTION_OFFSET_DEGREES = 45.0;

// Utility function to map one range to another
int map(int x, int in_min, int in_max, int out_min, int out_max) {
    if (in_min == in_max) return out_min; // Prevent division by zero
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Helper function to calculate radians for direction
float calculateDirectionRadians(float direction) {
    return (direction + DIRECTION_OFFSET_DEGREES) * DEGREE_TO_RADIAN;
}

// Helper function to calculate motor power based on speed
int calculatePower(int speed) {
    return map(speed, MIN_SPEED, MAX_SPEED, MIN_PWM, MAX_PWM);
}

// Helper function to adjust wheel speed
int adjustWheelSpeed(float factor, int power) {
    int wheelSpeed = static_cast<int>(power * factor);
    return std::max(std::min(wheelSpeed, MAX_PWM), MIN_PWM);
}

// The new global SetMotor function
void SetMotor(int speed = 0, float direction = 0) {
    // Ensure speed is within bounds
    speed = std::max(std::min(speed, MAX_SPEED), MIN_SPEED);

    // Calculate runtime values
    float directionRadians = calculateDirectionRadians(direction); // Convert direction to radians
    int power = calculatePower(speed);                            // Map speed to motor power

    // Calculate relative wheel speeds using direction
    float leftWheelFactor = sin(directionRadians);
    float rightWheelFactor = cos(directionRadians);

    // Adjust actual wheel speeds using power and scaling
    int leftWheelSpeed = adjustWheelSpeed(leftWheelFactor, power);
    int rightWheelSpeed = adjustWheelSpeed(rightWheelFactor, power);

    // Execute motor movement
    Motor_Move(leftWheelSpeed, leftWheelSpeed, rightWheelSpeed, rightWheelSpeed);
}

    // Accessors for testing or debugging
    int getLeftWheelSpeed() const { return leftWheelSpeed; }
    int getRightWheelSpeed() const { return rightWheelSpeed; }
};


/*   Usage hints
SetMotor motor(15, 30.0); // Speed = 15, Direction = 30 degrees
motor.executeMotorMove(); // Executes motor movement command

// Debug speeds
std::cout << "Left Wheel Speed: " << motor.getLeftWheelSpeed();
std::cout << "Right Wheel Speed: " << motor.getRightWheelSpeed();
 */