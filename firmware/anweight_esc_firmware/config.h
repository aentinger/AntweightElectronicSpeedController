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
	E_CONTROL_SELECT control; // select the control method
	uint8_t deadzone; // if a speed is lower than that value, do not drive -> this is used for preventing movements of motors if the input signal has small variations around the neutral position
	uint8_t remote_control_min_value_ch_1; // minimum pulse with of the remote control ch 1
	uint8_t remote_control_max_value_ch_1; // maximum pulse width of the remote control ch 1
	uint8_t remote_control_min_value_ch_2; // minimum pulse with of the remote control ch 2
	uint8_t remote_control_max_value_ch_2; // maximum pulse width of the remote control ch 2
	int32_t r1, s1, t1; // channel 1
	int32_t r2, s2, t2; // channel 2
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
