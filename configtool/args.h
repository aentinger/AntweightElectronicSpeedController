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
 * @file args.h
 */

#ifndef ARGS_H_
#define ARGS_H_

#include <queue>
#include <string>

class args {
public:
	/**
	 * @brief Constructor
	 */
	args(int argc, char **argv);

	/**
	 * @brief Destructor
	 */
	~args() { }

	/**
	 * @brief returns the device node on which the node is connected
	 */
	std::string get_dev_node() const { return m_dev_node; }

	/**
	 * @brief returns true if no arguments are left
	 */
	bool empty() const { return m_args.empty(); }

	/**
	 * @brief returns the next argument
	 */
	std::string get_next_argument();

	/**
	 * @brief returns true, if arg = -help
	 */
	static bool is_help(std::string const &arg);
	/**
	 * @brief returns true if arg is -display, false otherwise
	 */
	static bool is_display_configuration(std::string const &arg);
	/**
	 * @brief determines the control method
	 */
	static bool is_control_tank(std::string const &arg);
	static bool is_control_delta(std::string const &arg);
	/**
	 * @brief determines if a deadzone is to be set and if which value
	 */
	static bool is_deadzone(std::string const &arg, size_t *deadzone);
	/**
	 * @brief determines if a minimum/maximum channel value for ch x is to be set and if which value
	 */
	static bool is_rc_ch1_min(std::string const &arg, size_t *value);
	static bool is_rc_ch1_max(std::string const &arg, size_t *value);
	static bool is_rc_ch2_min(std::string const &arg, size_t *value);
	static bool is_rc_ch2_max(std::string const &arg, size_t *value);

private:
	std::queue<std::string> m_args;
	std::string m_dev_node;

	/**
	 * @brief converts the float value provided in the -ch1-min-value arguments to size_t which we need for configuration
	 */
	static size_t util_convert_from_ms_rc_min_max(float const value);
};


#endif /* ARGS_H_ */
