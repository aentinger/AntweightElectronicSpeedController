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
 * @brief this file implements the control unit of the firmware, doing the appopriate signal conditioning from the input signals for the motor control
 * @file control.h
 */

#ifndef CONTROL_H_
#define CONTROL_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum {TANK = 0, DELTA = 1} E_CONTROL_SELECT;
	
/** 
 * @brief initializes the control module
 */
void init_control();

/**
 * @brief updates the linear mapper 2 functions after a configuration via the pc
 */ 
void update_linear_mapper_2d();

/**
 * @brief callback function called when new data on channel 1 arrived
 */
void control_ch1_data_callback(uint16_t const pulse_duration);
	
/**
 * @brief callback function called when new data on channel 2 arrived
 */
void control_ch2_data_callback(uint16_t const pulse_duration);

#endif /* CONTROL_H_ */