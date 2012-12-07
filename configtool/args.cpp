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
 * @brief class for holding the arguments with what which the function was called
 * @file args.cpp
 */

#include "args.h"
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include <iostream>

/**
 * @brief Constructor
 */
args::args(int argc, char **argv) {
	if(argc < 2) throw std::runtime_error("Usage: escconfig /dev/ttyACM0 [args]");

	m_dev_node = std::string(argv[1]);

	for(size_t i=2; i<static_cast<size_t>(argc); i++) {
		m_args.push(std::string(argv[i]));
	}
}

/**
 * @brief returns the next argument
 */
std::string args::get_next_argument() {
	if(!m_args.empty()) {
		std::string arg = m_args.front();
		m_args.pop();
		return arg;
	}
	return "";
}

/**
 * @brief returns true, if arg = -help
 */
bool args::is_help(std::string const &arg) {
	std::string const help_arg = "-help";
	return (arg == help_arg);
}

/**
 * @brief determines the control method
 */
bool args::is_control_tank(std::string const &arg) {
	std::string const control_tank_arg = "-control-tank";
	return (arg == control_tank_arg);
}
bool args::is_control_delta(std::string const &arg) {
	std::string const control_delta_arg = "-control-delta";
	return (arg == control_delta_arg);
}

/**
 * @brief determines if a deadzone is to be set
 */
bool args::is_deadzone(std::string const &arg, size_t *deadzone) {
	std::string const deadzone_arg = "-deadzone"; // -deadzone-0.1 => deadzone of 0.1 ms of the remote signal
	size_t const pos_last_minus = arg.rfind("-");
	if(pos_last_minus == std::string::npos) return false;
	if(deadzone_arg != arg.substr(0, pos_last_minus)) return false;
	std::string deadzone_value = arg.substr(pos_last_minus + 1);
	float tmp_val = 0.0f;
	try {
		tmp_val = boost::lexical_cast<float>(deadzone_value);
	} catch(boost::bad_lexical_cast &e) {
		throw std::runtime_error("Could not convert number of -deadzone argument from string to number");
	}
	*deadzone = static_cast<size_t>(tmp_val * 250.0f);
	return true;
}

/**
 * @brief determines if a minimum channel value for ch 1 is to be set and if which value
 */
bool args::is_rc_ch1_min(std::string const &arg, size_t *value) {
	std::string const rc_ch1_min_arg = "-ch1-min-value"; // -ch1_min_value-1.112 => deadzone of 1.112 ms
	size_t const pos_last_minus = arg.rfind("-");
	if(pos_last_minus == std::string::npos) return false;
	if(rc_ch1_min_arg != arg.substr(0, pos_last_minus)) return false;
	std::string rc_ch1_min_value = arg.substr(pos_last_minus + 1);
	float tmp_val = 0.0f;
	try {
		tmp_val = boost::lexical_cast<float>(rc_ch1_min_value);
	} catch(boost::bad_lexical_cast &e) {
		throw std::runtime_error("Could not convert number of -ch1-min-value argument from string to number");
	}
	*value = args::util_convert_from_ms_rc_min_max(tmp_val);
	return true;
}
bool args::is_rc_ch1_max(std::string const &arg, size_t *value) {
	std::string const rc_ch1_max_arg = "-ch1-max-value";
	size_t const pos_last_minus = arg.rfind("-");
	if(pos_last_minus == std::string::npos) return false;
	if(rc_ch1_max_arg != arg.substr(0, pos_last_minus)) return false;
	std::string rc_ch1_max_value = arg.substr(pos_last_minus + 1);
	float tmp_val = 0.0f;
	try {
		tmp_val = boost::lexical_cast<float>(rc_ch1_max_value);
	} catch(boost::bad_lexical_cast &e) {
		throw std::runtime_error("Could not convert number of -ch1-max-value argument from string to number");
	}
	*value = args::util_convert_from_ms_rc_min_max(tmp_val);
	return true;
}
bool args::is_rc_ch2_min(std::string const &arg, size_t *value) {
	std::string const rc_ch2_min_arg = "-ch2-min-value";
	size_t const pos_last_minus = arg.rfind("-");
	if(pos_last_minus == std::string::npos) return false;
	if(rc_ch2_min_arg != arg.substr(0, pos_last_minus)) return false;
	std::string rc_ch2_min_value = arg.substr(pos_last_minus + 1);
	float tmp_val = 0.0f;
	try {
		tmp_val = boost::lexical_cast<float>(rc_ch2_min_value);
	} catch(boost::bad_lexical_cast &e) {
		throw std::runtime_error("Could not convert number of -ch2-min-value argument from string to number");
	}
	*value = args::util_convert_from_ms_rc_min_max(tmp_val);
	return true;
}
bool args::is_rc_ch2_max(std::string const &arg, size_t *value) {
	std::string const rc_ch2_max_arg = "-ch2-max-value";
	size_t const pos_last_minus = arg.rfind("-");
	if(pos_last_minus == std::string::npos) return false;
	if(rc_ch2_max_arg != arg.substr(0, pos_last_minus)) return false;
	std::string rc_ch2_max_value = arg.substr(pos_last_minus + 1);
	float tmp_val = 0.0f;
	try {
		tmp_val = boost::lexical_cast<float>(rc_ch2_max_value);
	} catch(boost::bad_lexical_cast &e) {
		throw std::runtime_error("Could not convert number of -ch2-max-value argument from string to number");
	}
	*value = args::util_convert_from_ms_rc_min_max(tmp_val);
	return true;
}

/**
 * @brief returns true if arg is -display, false otherwise
 */
bool args::is_display_configuration(std::string const &arg) {
	std::string const read_arg = "-display";
	return (arg == read_arg);
}

/**
 * @brief converts the float value provided in the -ch1-min-value arguments to size_t which we need for configuration
 */
size_t args::util_convert_from_ms_rc_min_max(float const value) {

	if(value >= 0.9f && value <= 2.1f) return static_cast<size_t>((value - 1.0f) * 250.0f);
	else throw std::runtime_error("Value provided for chx_min/max_value is out of allowed boundaries (0.9 - 2.1 ms)");
}
