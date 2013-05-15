/**
 * @author Alexander Entinger, BSc
 * @brief this file represents a motor control for the two channels of the esc
 * @file motor_control.h
 */

#ifndef MOTOR_CONTROL_H_
#define MOTOR_CONTROL_H_

#include <stdint.h>

typedef enum {ENABLED = 0, DISABLED = 1} E_MOTOR_STATE;
typedef enum {FWD = 0, BWD = 1} E_MOTOR_DIRECTION;
	
static volatile uint16_t const MAX_MOTOR_VALUE = 8160; // //255 << 5;

/** 
 * @brief initializes the motor control object
 */
void init_motor_control();

/** 
 * @brief enables the motor outputs
 */	
void enable_motors();

/** 
 * @brief disables the motor outputs (both Motor + and - are at zero)
 */	
void disable_motors();

/**
 * @brief sets the pwm value of the left motor
 * @param dir movement direction of the motor: either forward of backward
 * @param s speed value between 0 and 255 << 5 (0 % to 100 %), speed of 0 = BRAKE
 */
void set_pwm_motor_left(E_MOTOR_DIRECTION const dir, uint16_t const s);
	
/**
 * @brief sets the pwm value of the right motor
 * @param dir movement direction of the motor: either forward of backward
 * @param s speed value between 0 and 255 << 5 (0 % to 100 %), speed of 0 = BRAKE
 */
void set_pwm_motor_right(E_MOTOR_DIRECTION const dir, uint16_t const s);

#endif /* MOTOR_CONTROL_H_ */
