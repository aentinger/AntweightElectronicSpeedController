/**
 * @author Alexander Entinger, BSc
 * @brief this file represents the configuration data handling of the esc
 * @file config.h
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdint.h>
#include <stdbool.h>
#include "control.h"

typedef struct {
	uint8_t eeprom_written; // status flag, for intial writing of the eeprom
	bool enable_boost; // obsolet, needs to removed
	E_CONTROL_SELECT control; // select the control method
	uint8_t deadzone; // if a speed is lower than that value, do not drive -> this is used for preventing movements of motors if the input signal has small variations around the neutral position
	uint8_t remote_control_min_value_ch_1; // minimum pulse with of the remote control ch 1
	uint8_t remote_control_max_value_ch_1; // maximum pulse width of the remote control ch 1
	uint8_t remote_control_min_value_ch_2; // minimum pulse with of the remote control ch 2
	uint8_t remote_control_max_value_ch_2; // maximum pulse width of the remote control ch 2
} s_config_data;

extern volatile s_config_data configuration;

/**
 * @brief initializes the configuration data
 */
void init_config();

/** 
 * @brief parses the incoming data on the serial usb device
 * @param data_byte received byte from the serial usb device
 * @param config_done_ptr used for telling the firmware when the configuration is done and it can proceed to INIT
 */
void config_parse_data(uint8_t const data_byte, bool *config_done_ptr);

#endif
