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
 * @file config.cpp
 */

#include "config.h"
#include "VirtualSerial.h"
#include <avr/eeprom.h>

#define CONFIG_EEPROM_ADDRESS	(const void*)(0)
#define EEPROM_WRITTEN			(0x00)
#define EEPROM_NOT_WRITTEN		(0xFF)

/**
 * @brief initializes the configuration data
 */
void init_config() {
	// load from EEPROM
	eeprom_read_block((void*)(&configuration), CONFIG_EEPROM_ADDRESS, sizeof(configuration));
	if(configuration.eeprom_written == EEPROM_NOT_WRITTEN) { // device was previously not configured, setting to standard values
		configuration.eeprom_written = EEPROM_WRITTEN;
		configuration.control = TANK;
		configuration.deadzone = 20;
		configuration.remote_control_min_value_ch_1 = 0;
		configuration.remote_control_max_value_ch_1 = 250;
		configuration.remote_control_min_value_ch_2 = 0;
		configuration.remote_control_max_value_ch_2 = 250;
		eeprom_write_block((void*)(&configuration), CONFIG_EEPROM_ADDRESS, sizeof(configuration));
	}
}

#define S_REQUEST_KIND		(0)
#define S_WRITE_CONFIG		(1)
#define S_WRITE_DEADZONE	(2)
#define S_WRITE_CH1_MIN		(3)
#define S_WRITE_CH1_MAX		(4)
#define S_WRITE_CH2_MIN		(5)
#define S_WRITE_CH2_MAX		(6)
#define S_WRITE_R1			(7)
#define S_WRITE_R2			(8)
#define S_WRITE_S1			(9)

#define	S_REQUEST_KIND_READ			(0x00)
#define	S_REQUEST_KIND_WRITE		(0x01)

#define S_CONFIG_CONTROL_MASK		(1<<1)

#define MSG_OK			(0x01)
#define MSG_NOK			(0x00)

static uint8_t config_parse_state = S_REQUEST_KIND;
/** 
 * @brief parses the incoming data on the serial usb device
 * @param data_byte received byte from the serial usb device
 * @param config_done_ptr used for telling the firmware when the configuration is done and it can proceed to INIT
 */
void config_parse_data(uint8_t const data_byte, bool *config_done_ptr) {
	
	static volatile uint8_t msg[7 + 3 * sizeof(uint32_t)];
	static uint8_t r1[4] = {0}, r2[4] = {0}, s1[4] = {0};
	static uint8_t byte_cnt = 0; // used for receiving the 4 byte integers for R-S-T
	
	switch(config_parse_state) {
		case S_REQUEST_KIND: {
			msg[S_REQUEST_KIND] = data_byte;
			if(msg[S_REQUEST_KIND] == S_REQUEST_KIND_READ) {				
				// configuration is now done here
				*config_done_ptr = true;
				// generate read reply message
				uint8_t msg_reply[7] = {0x00};
				msg_reply[0] = MSG_OK;
				if(configuration.control == TANK) msg_reply[1] |= S_CONFIG_CONTROL_MASK;
				msg_reply[2] = configuration.deadzone;
				msg_reply[3] = configuration.remote_control_min_value_ch_1;
				msg_reply[4] = configuration.remote_control_max_value_ch_1;
				msg_reply[5] = configuration.remote_control_min_value_ch_2;
				msg_reply[6] = configuration.remote_control_max_value_ch_2;
				// send read reply message
				virtual_serial_send_data(&msg_reply, 7);					
			} else if(msg[S_REQUEST_KIND] == S_REQUEST_KIND_WRITE) {
				config_parse_state = S_WRITE_CONFIG;
			}			
		} break;
		
		case S_WRITE_CONFIG: {
			msg[S_WRITE_CONFIG] = data_byte;
			config_parse_state = S_WRITE_DEADZONE;
		} break;
		case S_WRITE_DEADZONE: {
			msg[S_WRITE_DEADZONE] = data_byte;
			config_parse_state = S_WRITE_CH1_MIN;
		} break;
		case S_WRITE_CH1_MIN: {
			msg[S_WRITE_CH1_MIN] = data_byte;
			config_parse_state = S_WRITE_CH1_MAX;
		} break;
		case S_WRITE_CH1_MAX: {
			msg[S_WRITE_CH1_MAX] = data_byte;
			config_parse_state = S_WRITE_CH2_MIN;
		} break;
		case S_WRITE_CH2_MIN: {
			msg[S_WRITE_CH2_MIN] = data_byte;
			config_parse_state = S_WRITE_CH2_MAX;
		} break;
		case S_WRITE_CH2_MAX: {
			msg[S_WRITE_CH2_MAX] = data_byte;
			config_parse_state = S_WRITE_R1;
		} break;
		case S_WRITE_R1: {
			r1[byte_cnt] = data_byte;
			byte_cnt++;
			if(byte_cnt == 4) {
				byte_cnt = 0;
				config_parse_state = S_WRITE_R2;
			}
		} break;
		case S_WRITE_R2: {
			r2[byte_cnt] = data_byte;
			byte_cnt++;
			if(byte_cnt == 4) {
				byte_cnt = 0;
				config_parse_state = S_WRITE_S1;
			}
		} break;
		case S_WRITE_S1: {
			s1[byte_cnt] = data_byte;
			byte_cnt++;
			if(byte_cnt == 4) {
				byte_cnt = 0;
				
				config_parse_state = S_REQUEST_KIND;
				
				// configuration byte
				if(msg[S_WRITE_CONFIG] & S_CONFIG_CONTROL_MASK) configuration.control = TANK;
				else configuration.control = DELTA;
				// deadzone byte
				configuration.deadzone = msg[S_WRITE_DEADZONE];
				// channel 1 min and maximum values
				configuration.remote_control_min_value_ch_1 = msg[S_WRITE_CH1_MIN];
				configuration.remote_control_max_value_ch_1 = msg[S_WRITE_CH1_MAX];
				// channel 2 min and maximum values
				configuration.remote_control_min_value_ch_2 = msg[S_WRITE_CH2_MIN];
				configuration.remote_control_max_value_ch_2 = msg[S_WRITE_CH2_MAX];
				// r - s - t values
				// channel 1
				configuration.r1 = (int32_t)(((uint32_t)(r1[0])<<24) + ((uint32_t)(r1[1])<<16) + ((uint32_t)(r1[2])<<8) + ((uint32_t)(r1[3])));
				configuration.s1 = (int32_t)(((uint32_t)(s1[0])<<24) + ((uint32_t)(s1[1])<<16) + ((uint32_t)(s1[2])<<8) + ((uint32_t)(s1[3])));
				configuration.t1 = configuration.s1;
				// channel 2
				configuration.r2 = (int32_t)(((uint32_t)(r2[0])<<24) + ((uint32_t)(r2[1])<<16) + ((uint32_t)(r2[2])<<8) + ((uint32_t)(r2[3])));
				configuration.s2 = configuration.s1;
				configuration.t2 = 0 - configuration.s1; // *(-1)
				// update the linear mapper 2d
				update_linear_mapper_2d();
				// write data to eeprom
				eeprom_write_block((void*)(&configuration), CONFIG_EEPROM_ADDRESS, sizeof(configuration));
				// configuration is now done here
				*config_done_ptr = true;
				// send answer
				uint8_t msg_reply = MSG_OK;
				virtual_serial_send_data(&msg_reply, 1);
			}
		} break;
		default: {
			config_parse_state = S_REQUEST_KIND;
		} break;
	}
}