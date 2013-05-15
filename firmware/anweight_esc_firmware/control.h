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
 * @brief callback function called when new data on channel 1 arrived
 */
void control_ch1_data_callback(uint16_t const pulse_duration);
	
/**
 * @brief callback function called when new data on channel 2 arrived
 */
void control_ch2_data_callback(uint16_t const pulse_duration);

#endif /* CONTROL_H_ */