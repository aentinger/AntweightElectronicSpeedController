/*
	Copyright 2013 by Alexander Entinger, BSc

    This file is part of antweight_esc_firmware.

    antweight_esc_firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    antweight_esc_firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with antweight_esc_firmware.  If not, see <http://www.gnu.org/licenses/>.
*/

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
void set_pwm_motor_left(E_MOTOR_DIRECTION const dir, uint8_t const s);
	
/**
 * @brief sets the pwm value of the right motor
 * @param dir movement direction of the motor: either forward of backward
 * @param s speed value between 0 and 255 << 5 (0 % to 100 %), speed of 0 = BRAKE
 */
void set_pwm_motor_right(E_MOTOR_DIRECTION const dir, uint8_t const s);

#endif /* MOTOR_CONTROL_H_ */
