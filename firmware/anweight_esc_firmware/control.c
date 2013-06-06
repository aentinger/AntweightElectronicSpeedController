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
* @file control.cpp
*/

#include "control.h"
#include "motor_control.h"
#include "config.h"
#include "linear_mapper.h"
#include "linear_mapper_2d.h"
#include "filter.h"

// calibration flag for defining when calibration of neutral position is done
extern bool do_calibration_of_neutral_position;
// channel selection
typedef enum {CH1 = 0, CH2 = 1} E_CHANNEL_SELECT;
// maximum channel value
static uint16_t const MAX_CHANNEL_VALUE = 500; // max 2 ms pulsewidth
// adt data for the linear mapper
static linear_mapper map_ch1_fwd, map_ch1_bwd, map_ch2_fwd, map_ch2_bwd;
// adt date for the linear mapper in 2 dimensions
static linear_mapper_2d map_motor_left_2d, map_motor_right_2d;
// adt data for the filter
static filter filt[2];
	
/**
 * @brief this class is called when new data has arrived - its job is to calculate new data and transmit it to the motor drivers
 */
void control_update();
/** 
 * @brief provides a conditioning of the signal from the linear mapper to the motor control
 */
uint8_t speed_conditioning(int16_t const s);

/** 
 * @brief initializes the control module
 */
void init_control() {
	init_filter(&filt[CH1], 4, 125);
	init_filter(&filt[CH2], 4, 125);
	
	init_linear_mapper_2d(&map_motor_left_2d, -16320, -65, -65);
	init_linear_mapper_2d(&map_motor_right_2d, 0, -65, 65);
}

/**
 * @brief updates the linear mapper 2 functions after a configuration via the pc
 */ 
void update_linear_mapper_2d() {
	init_linear_mapper_2d(&map_motor_left_2d, configuration.r1, configuration.s1, configuration.t1);
	init_linear_mapper_2d(&map_motor_right_2d, configuration.r2, configuration.s2, configuration.t2);
}

/**
 * @brief callback function called when new data on channel 1 arrived
 */
void control_ch1_data_callback(uint16_t const pulse_duration) {
	if(pulse_duration <= MAX_CHANNEL_VALUE) {
		// subtract the 1 ms minimum pulsewidth which we always have, equals pulse_duration - 250, the max value of pulse_duration is thereby 250
		filter_add_value(&filt[CH1], pulse_duration - (MAX_CHANNEL_VALUE >> 1));
		// call the update function
		control_update();
	}		
}	
	
/**
 * @brief callback function called when new data on channel 2 arrived
 */
void control_ch2_data_callback(uint16_t const pulse_duration) {
	if(pulse_duration <= MAX_CHANNEL_VALUE) {
		// subtract the 1 ms minimum pulsewidth which we always have, equals pulse_duration - 250, the max value of pulse_duration is thereby 250
		filter_add_value(&filt[CH2], pulse_duration - (MAX_CHANNEL_VALUE >> 1));
		// call the update function
		control_update();
	}
}

/** 
 * @brief provides a conditioning of the signal from the linear mapper to the motor control
 */
uint8_t speed_conditioning(int16_t const s) {
	int16_t speed = s;
	speed >>= 5;
	if(speed < 0) speed = 0;
	else if(speed > 255) speed = 255;
	return (uint8_t)(speed);
}

/**
 * @brief this class is called when new data has arrived - its job is to calculate new data and transmit it to the motor drivers
 */
void control_update() {
	// initial middle value of the channels
	static int16_t MIDDLE_VALUE_CH[2] = {125, 125};
	// values for calibrating offsets from the middle value in delta mode
	static int16_t OFFSET_CH[2] = {0,0};
	
	// do the calibration of the neutral position
	if(do_calibration_of_neutral_position) {
		MIDDLE_VALUE_CH[CH1] = filter_get_value(&filt[CH1]);
		MIDDLE_VALUE_CH[CH2] = filter_get_value(&filt[CH2]);
		// configuration done
		do_calibration_of_neutral_position = false;	
		// adjust the linear mapping modules
		init_linear_mapper(&map_ch1_bwd, configuration.remote_control_min_value_ch_1, MIDDLE_VALUE_CH[CH1], MAX_MOTOR_VALUE, 0);
		init_linear_mapper(&map_ch1_fwd, MIDDLE_VALUE_CH[CH1], configuration.remote_control_max_value_ch_1, 0, MAX_MOTOR_VALUE);
		init_linear_mapper(&map_ch2_bwd, configuration.remote_control_min_value_ch_2, MIDDLE_VALUE_CH[CH2], MAX_MOTOR_VALUE, 0);
		init_linear_mapper(&map_ch2_fwd, MIDDLE_VALUE_CH[CH2], configuration.remote_control_max_value_ch_2, 0, MAX_MOTOR_VALUE);
		// calculate offset values for correcting offsets in delta mode
		OFFSET_CH[CH1] = (int16_t)(125) - MIDDLE_VALUE_CH[CH1];
		OFFSET_CH[CH2] = (int16_t)(125) - MIDDLE_VALUE_CH[CH2];
		// exit function here
		return;
	}
	
	uint16_t const m_ch_value[2] = {filter_get_value(&filt[CH1]), filter_get_value(&filt[CH2])};

	/************************************************************************/
	/* TANK DRIVE                                                           */
	/************************************************************************/
	if(configuration.control == TANK) {
		// Motor Left
		if(m_ch_value[CH1] > MIDDLE_VALUE_CH[CH1]) { // drive forward
			uint8_t const speed = speed_conditioning(linear_map(&map_ch1_fwd, m_ch_value[CH1]));
			if(speed > configuration.deadzone) set_pwm_motor_left(FWD, speed);
			else set_pwm_motor_left(FWD, 0);
		} else {
			uint8_t const speed = speed_conditioning(linear_map(&map_ch1_bwd, m_ch_value[CH1]));
			if(speed > configuration.deadzone) set_pwm_motor_left(BWD, speed);
			else set_pwm_motor_left(BWD, 0);
		}
		// Motor Right
		set_pwm_motor_right(FWD, 0);
		if(m_ch_value[CH2] > MIDDLE_VALUE_CH[CH2]) { // drive forward
			uint8_t const speed = speed_conditioning(linear_map(&map_ch2_fwd, m_ch_value[CH2]));
			if(speed > configuration.deadzone) set_pwm_motor_right(FWD, speed);
			else set_pwm_motor_right(FWD, 0);
		} else {
			uint8_t const speed = speed_conditioning(linear_map(&map_ch2_bwd, m_ch_value[CH2]));
			if(speed > configuration.deadzone) set_pwm_motor_right(BWD, speed);
			else set_pwm_motor_right(BWD, 0);
		}			
			
	/************************************************************************/
	/* DELTA DRIVE                                                          */
	/************************************************************************/
	} else if(configuration.control == DELTA) {
		// Motor Left
		{
			int32_t speed = (linear_map_2d(&map_motor_left_2d, (m_ch_value[CH1] + OFFSET_CH[CH1]), (m_ch_value[CH2] + OFFSET_CH[CH2])) >> 5);
			if(speed > 0) {
				if(speed > 255) speed = 255;
				if(speed > configuration.deadzone) set_pwm_motor_left(FWD, (uint8_t)(speed));
				else set_pwm_motor_left(FWD, 0);			
			} else {
				if(speed < -255) speed = -255;
				speed = 0 - speed; // * (-1)
				if(speed > configuration.deadzone) set_pwm_motor_left(BWD, (uint8_t)(speed));
				else set_pwm_motor_left(BWD, 0);
			}
		}		
		// Motor Right
		{
			int32_t speed = (linear_map_2d(&map_motor_right_2d, (m_ch_value[CH1] + OFFSET_CH[CH1]), (m_ch_value[CH2] + OFFSET_CH[CH2])) >> 5);
			if(speed > 0) {
				if(speed > 255) speed = 255;
				if(speed > configuration.deadzone) set_pwm_motor_right(FWD, (uint8_t)(speed));
				else set_pwm_motor_right(FWD, 0);
			} else {
				if(speed < -255) speed = -255;
				speed = 0 - speed; // * (-1)
				if(speed > configuration.deadzone) set_pwm_motor_right(BWD, (uint8_t)(speed));
				else set_pwm_motor_right(BWD, 0);
			}
		}		
	}
}
