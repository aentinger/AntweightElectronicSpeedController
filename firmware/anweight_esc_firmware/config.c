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
		configuration.enable_boost = true;
		configuration.control = TANK;
		configuration.deadzone = 5;
		configuration.remote_control_min_value_ch_1 = 0;
		configuration.remote_control_max_value_ch_1 = 250;
		configuration.remote_control_min_value_ch_2 = 0;
		configuration.remote_control_max_value_ch_2 = 250;
		eeprom_write_block((void*)(&configuration), CONFIG_EEPROM_ADDRESS, sizeof(configuration));
	}
}

#define S_REQUEST_KIND		(0x00)
#define S_WRITE_CONFIG		(0x01)
#define S_WRITE_DEADZONE	(0x02)

#define	S_REQUEST_KIND_READ			(0x00)
#define	S_REQUEST_KIND_WRITE		(0x01)

#define S_CONFIG_ENABLE_BOOST_MASK	(1<<0)
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
	
	static volatile uint8_t msg[3];
	
	switch(config_parse_state) {
		case S_REQUEST_KIND: {
			msg[S_REQUEST_KIND] = data_byte;
			if(msg[S_REQUEST_KIND] == S_REQUEST_KIND_READ) {				
				// configuration is now done here
				*config_done_ptr = true;
				// generate read reply message
				uint8_t msg_reply[3] = {0x00};
				msg_reply[0] = MSG_OK;
				if(configuration.enable_boost) msg_reply[1] |= S_CONFIG_ENABLE_BOOST_MASK;
				if(configuration.control == TANK) msg_reply[1] |= S_CONFIG_CONTROL_MASK;
				msg_reply[2] = configuration.deadzone;
				// send read reply message
				virtual_serial_send_data(&msg_reply, 3);					
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
			// configuration byte
			if(msg[S_WRITE_CONFIG] & S_CONFIG_ENABLE_BOOST_MASK) configuration.enable_boost = true;
			else configuration.enable_boost = false;
			if(msg[S_WRITE_CONFIG] & S_CONFIG_CONTROL_MASK) configuration.control = TANK;
			else configuration.control = DELTA;
			// deadzone byte
			configuration.deadzone = msg[S_WRITE_DEADZONE];
			// write data to eeprom
			eeprom_write_block((void*)(&configuration), CONFIG_EEPROM_ADDRESS, sizeof(configuration));
			// configuration is now done here
			config_parse_state = S_REQUEST_KIND;
			*config_done_ptr = true;
			// send answer
			uint8_t msg_reply = MSG_OK;
			virtual_serial_send_data(&msg_reply, 1);
		} break;
		
		default: {
			config_parse_state = S_REQUEST_KIND;
		} break;
	}
}