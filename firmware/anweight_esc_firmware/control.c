/**
* @author Alexander Entinger, BSc
* @brief this file implements the control unit of the firmware, doing the appopriate signal conditioning from the input signals for the motor control
* @file control.cpp
*/

#include "control.h"
#include "motor_control.h"
#include "config.h"
#include "linear_mapper.h"

// calibration flag for defining when calibration of neutral position is done
extern bool do_calibration_of_neutral_position;
// channel selection
typedef enum {CH1 = 0, CH2 = 1} E_CHANNEL_SELECT;
// stores the actual value of the channel	
static volatile uint16_t m_ch_value[2] = {0, 0};
// maximum channel value
static uint16_t const MAX_CHANNEL_VALUE = 500; // max 2 ms pulsewidth
// adt data for the linear mapper
static linear_mapper map_ch1_fwd, map_ch1_bwd, map_ch2_fwd, map_ch2_bwd;
	
/**
 * @brief this class is called when new data has arrived - its job is to calculate new data and transmit it to the motor drivers
 */
void control_update();

/**
 * @brief callback function called when new data on channel 1 arrived
 */
void control_ch1_data_callback(uint16_t const pulse_duration) {
	if(pulse_duration <= MAX_CHANNEL_VALUE) {
		// subtract the 1 ms minimum pulsewidth which we always have, equals pulse_duration - 250, the max value of pulse_duration is thereby 250
		m_ch_value[CH1] = pulse_duration - (MAX_CHANNEL_VALUE >> 1);
		// call the update function
		control_update();
	}		
}	
	
/**
 * @brief callback function called when new data on channel 2 arrived
 */
void control_ch2_data_callback(uint16_t const pulse_duration) {
	if(pulse_duration <= MAX_CHANNEL_VALUE) {
		m_ch_value[CH2] = pulse_duration - (MAX_CHANNEL_VALUE >> 1); 
		control_update();
	}
}

/**
 * @brief this class is called when new data has arrived - its job is to calculate new data and transmit it to the motor drivers
 */
void control_update() {
	// initial middle value of the channels
	static int16_t MIDDLE_VALUE_CH1 = 125;
	static int16_t MIDDLE_VALUE_CH2 = 125;
	
	// do the calibration of the neutral position
	if(do_calibration_of_neutral_position) {
		MIDDLE_VALUE_CH1 = m_ch_value[CH1];
		MIDDLE_VALUE_CH2 = m_ch_value[CH2];
		// configuration done
		do_calibration_of_neutral_position = false;	
		// adjust the linear mapping modules
		init_linear_mapper(&map_ch1_bwd, configuration.remote_control_min_value_ch_1, MIDDLE_VALUE_CH1, MAX_MOTOR_VALUE, 0);
		init_linear_mapper(&map_ch1_fwd, MIDDLE_VALUE_CH1, configuration.remote_control_max_value_ch_1, 0, MAX_MOTOR_VALUE);
		init_linear_mapper(&map_ch2_bwd, configuration.remote_control_min_value_ch_2, MIDDLE_VALUE_CH2, MAX_MOTOR_VALUE, 0);
		init_linear_mapper(&map_ch2_fwd, MIDDLE_VALUE_CH2, configuration.remote_control_max_value_ch_2, 0, MAX_MOTOR_VALUE);
		// exit function here
		return;
	}

	/************************************************************************/
	/* TANK DRIVE                                                           */
	/************************************************************************/
	if(configuration.control == TANK) {

		// CHANNEL 1
		if(m_ch_value[CH1] > MIDDLE_VALUE_CH1) { // drive forward
			uint16_t const speed = (uint16_t)(linear_map(&map_ch1_fwd, m_ch_value[CH1]));
			if((speed<<5) > configuration.deadzone) set_pwm_motor_left(FWD, speed);
			else set_pwm_motor_left(FWD, 0);
		} else {
			int16_t const speed = (uint16_t)(linear_map(&map_ch1_bwd, m_ch_value[CH1]));
			if((speed<<5) > configuration.deadzone) set_pwm_motor_left(BWD, speed);
			else set_pwm_motor_left(BWD, 0);
		}
		// CHANNEL 2
		set_pwm_motor_right(FWD, 0);
		if(m_ch_value[CH2] > MIDDLE_VALUE_CH2) { // drive forward
			uint16_t const speed = (uint16_t)(linear_map(&map_ch2_fwd, m_ch_value[CH2]));
			if((speed<<5) > configuration.deadzone) set_pwm_motor_right(FWD, speed);
			else set_pwm_motor_right(FWD, 0);
		} else {
			uint16_t const speed = (uint16_t)(linear_map(&map_ch2_bwd, m_ch_value[CH2]));
			if((speed<<5) > configuration.deadzone) set_pwm_motor_right(BWD, speed);
			else set_pwm_motor_right(BWD, 0);
		}			
			
	/************************************************************************/
	/* DELTA DRIVE                                                          */
	/************************************************************************/
	} else if(configuration.control == DELTA) {
		
	}
}
