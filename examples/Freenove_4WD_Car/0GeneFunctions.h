#ifndef GeneFUNCTIONS_H
#define GeneFUNCTIONS_H

extern int stateHistory[6][5]; // Declare the array as global

void showsensor();

#endif // GeneFUNCTIONS_H


// Send calculated speeds to the motors
//   bool Motor_Move(int leftFrontWheelSpeed, int leftRearWheelSpeed, int rightFrontWheelSpeed, int rightRearWheelSpeed) {

// Set motor speeds using the robotMotorSetSpeeds function
// return robotMotorSetSpeeds(leftFrontWheelSpeed, rightFrontWheelSpeed, nullptr); // Assuming `nullptr` for `Print* display`
// }
