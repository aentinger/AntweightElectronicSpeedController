/*
	Copyright 2013 by Alexander Entinger, BSc

    This file is part of escconfig.

    escconfig is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    escconfig is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with escconfig.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @author Alexander Entinger, BSc
 * @brief main file for antweight esc configuration tool
 * @file main.cpp
 */

#include <iostream>
#include <stdlib.h>
#include <string>
#include <stdexcept>
#include "serial.h"
#include "args.h"
#include "configuration.h"

void print_help() {
	std::cout << "Usage: escconfig DEVICE_NODE [ARGS]" << std::endl;
	std::cout << "\tDEVICE_NODE\tname of the serial port occupied by the device,\n\t\t\te.g. COM3 in Windows or /dev/ttyACM0 in Linux" << std::endl;
	std::cout << "\t-help\t\tget this help file" << std::endl;
	std::cout << "\t-display\tshows the current configuration of the speed controller" << std::endl;
	std::cout << "\t-control-tank\tset control method to tank steering" << std::endl;
	std::cout << "\t-control-delta\tset control method to delta steering" << std::endl;
	std::cout << "\t-deadzone-VALUE\tset the deadzone value to the value VALUE (around 0.1 ms)" << std::endl;
	std::cout << "\t-ch1-min-value-VALUE\tset the minimum value of the remote control of ch 1 (around 1.0 ms)" << std::endl;
	std::cout << "\t-ch1-max-value-VALUE\tset the maximum value of the remote control of ch 1 (around 2.0 ms)" << std::endl;
	std::cout << "\t-ch2-min-value-VALUE\tset the minimum value of the remote control of ch 2 (around 1.0 ms)" << std::endl;
	std::cout << "\t-ch2-max-value-VALUE\tset the maximum value of the remote control of ch 2 (around 2.0 ms)" << std::endl;
}


void checked_main(int const argc, char **argv) {

	bool display_configuration = false;

	args arg_cont(argc, argv);

	serial::get_instance().init(arg_cont.get_dev_node(), 115200);

	configuration conf;

	while(!arg_cont.empty()) {
		std::string const arg = arg_cont.get_next_argument();

		size_t deadzone = 0;
		size_t rc_val = 0;
		if(args::is_help(arg)) {
			print_help();
			throw std::runtime_error("Hopefully the help helped you. Exiting program.");
		}
		else if(args::is_control_tank(arg)) conf.get()->control = TANK;
		else if(args::is_control_delta(arg)) conf.get()->control = DELTA;
		else if(args::is_deadzone(arg, &deadzone)) conf.get()->deadzone = deadzone;
		else if(args::is_rc_ch1_min(arg, &rc_val)) conf.get()->remote_control_min_value_ch1 = rc_val;
		else if(args::is_rc_ch1_max(arg, &rc_val)) conf.get()->remote_control_max_value_ch1 = rc_val;
		else if(args::is_rc_ch2_min(arg, &rc_val)) conf.get()->remote_control_min_value_ch2 = rc_val;
		else if(args::is_rc_ch2_max(arg, &rc_val)) conf.get()->remote_control_max_value_ch2 = rc_val;
		else if(args::is_display_configuration(arg)) display_configuration = true;
		else throw std::runtime_error("Argument not valid. Exiting program.");
	}

	if(display_configuration) { // no writing when we read, otherwise the interface gets to confusing
		std::cout << conf;
	} else {
		conf.write();
	}

	// cleanup
	serial::destroy();
}

int main(int argc, char **argv) {

	try {
		checked_main(argc, argv);
	} catch(std::exception &e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	} catch(...) {
		std::cerr << "Unhandled exception caught, exiting programm." << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
