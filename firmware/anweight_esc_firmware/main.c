/**
* @author Alexander Entinger, BSc
* @brief main file for antweight electronic speed control firmware
* @file main.cpp
*/

#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>
#include "motor_control.h"
#include "input.h"
#include "control.h"
#include "config.h"
#include "VirtualSerial/VirtualSerial.h"

// configuration structure
volatile s_config_data configuration;
// flag to tell control module to perform calibration of neutral position
volatile bool do_calibration_of_neutral_position = false;

/**
* @brief initializes the whole application
*/
void init_application();

typedef enum {INIT = 0, ACTIVE = 1, FAILSAFE = 2, CONFIG = 3, ERROR = 4} E_FIRMWARE_STATE;

int main(void) {
	
	init_application();

	uint8_t firmware_state = INIT;
	
	for(;;) {
	
		switch(firmware_state) {
			case INIT: {
				while(!input_good()) {
					// wait until we have a good signal
					// do the usb task necessary for working the usb
					virtual_serial_task();
					// if we have data available, switch the firmware state to go to config mode
					if(virtual_serial_bytes_available()) {
						firmware_state = CONFIG;
						break;
					}
				} 
				if(firmware_state == INIT) {
					// now do the calibration, set the config flag
					do_calibration_of_neutral_position = true;
					while(do_calibration_of_neutral_position) {
						// wait for calibration to be done
					}
					// and switch over to avtive state
					firmware_state = ACTIVE;
				} // else we switch over to the config mode
			}	break;
			case ACTIVE: {
				// the input signals are switch to the output signals depending on the driving mode (tank or v mixer)
				// monitor the signals, if there are no pulses on both channels for 5 periods switch to failsafe mode
				enable_motors();
				while(input_good()) {
					// do the control stuff here, since its interrupt controlled nothing to do here anymore
				}
				firmware_state = FAILSAFE; // input channels are bad, switch to failsafe
			} break;
			case FAILSAFE: {
				disable_motors();
				while(!input_good()) {
					// wait until the signals are back up, if thats the case switch back to active
				}
				firmware_state = ACTIVE;
			} break;
			case CONFIG: {
				// do the config, no disabling of the motors needed, since they are not yet enabled
				bool config_done = false;
				do {
					// do the usb task necessary for working the usb
					virtual_serial_task();
					// read from usb, change the settings according to that and send the requests answers
					if(virtual_serial_bytes_available()) {
						uint8_t data_byte = 0;
						if(virtual_serial_receive_byte(&data_byte)) {
							config_parse_data(data_byte, &config_done);
						}
					}
				} while(!config_done);				
				// then go back to init
				firmware_state = INIT; 
			} break;
			case ERROR: {
				// if we should land hear, whatever the reason, switch all output off
				disable_motors();
				// no way leads out of here
			} break;
			default: {
				firmware_state = ERROR;
			} break;
		}
	}
}

/**
* @brief initializes the whole application
*/
void init_application() {

	// load parameters from EEPROM
	init_config();
	
	// initialize motor control
	init_motor_control();
	
	// initialize the input module and register the callbacks
	init_input(control_ch1_data_callback, control_ch2_data_callback);
	
	// initialize the virtual serial
	init_virtual_serial();
	
	// enable globally interrupts
	sei();
}