// SetMotor.ino
// Created by Eugene Chang on 3/7/25.
// This code takes SetMotor and converts it to a related Motor_Move command.
//

// #include "SetMotor.h"
// #include "Genes_Basic_Line_Following.ino"


class SetMotor {
public:
    // Member variables
    int speed;         // Relative speed, ranges from -20 to 20
    float direction;   // Direction in degrees
    int leftWheelSpeed;
    int rightWheelSpeed;

    // Static constants for limits
    static constexpr int MAX_PWM = 4095;          // Maximum pulse-width modulation
    static constexpr int MIN_PWM = -4095;         // Minimum pulse-width modulation
    static constexpr int MAX_SPEED = 20;          // Maximum allowed speed
    static constexpr int MIN_SPEED = -20;         // Minimum allowed speed
    static constexpr float DEGREE_TO_RADIAN = M_PI / 180.0; // Conversion factor: degrees → radians
    static constexpr float DIRECTION_OFFSET_DEGREES = 45.0; // Wheel direction offset (for balance)

    // Helper function to adjust wheel speed
    int adjustWheelSpeed(float factor, int power) const {
        int wheelSpeed = static_cast<int>(power * factor);
        // Clamp wheel speeds to PWM range
        return std::max(std::min(wheelSpeed, MAX_PWM), MIN_PWM);
    }

    // Utility function to map one range to another
    static int map(int x, int in_min, int in_max, int out_min, int out_max) {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    // Calculate radians for direction (including offset for wheel balancing)
    static float calculateDirectionRadians(float direction) {
        return (direction + DIRECTION_OFFSET_DEGREES) * DEGREE_TO_RADIAN;
    }

    // Calculate motor power based on speed
    static int calculatePower(int speed) {
        // Map speed to PWM output based on min/max bounds
        return map(speed, MIN_SPEED, MAX_SPEED, MIN_PWM, MAX_PWM);
    }


    // Constructor
    SetMotor(int speed = 0, float direction = 0)
        : speed(speed), direction(direction), leftWheelSpeed(0), rightWheelSpeed(0) {
        // Ensure speed is within bounds
        if (speed > MAX_SPEED) this->speed = MAX_SPEED;
        else if (speed < MIN_SPEED) this->speed = MIN_SPEED;

        // Calculate runtime values
        float directionRadians = calculateDirectionRadians(direction); // Convert direction to radians
        int power = calculatePower(this->speed);                      // Map speed to motor power

        // Calculate relative wheel speeds using direction
        float leftWheelFactor = sin(directionRadians);  // Factor for left wheel speed
        float rightWheelFactor = cos(directionRadians); // Factor for right wheel speed

        // Adjust actual wheel speeds using power and scaling
        leftWheelSpeed = adjustWheelSpeed(leftWheelFactor, power);
        rightWheelSpeed = adjustWheelSpeed(rightWheelFactor, power);

    //    void run() {
    //      Serial.printnl("Motor is running");
    //      }
    };


    // Execute motor command
    void executeMotorMove() {
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