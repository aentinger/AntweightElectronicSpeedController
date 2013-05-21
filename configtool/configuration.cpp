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
 * @brief this class reads and writes the configuration from/to the device
 * @file configuration.cpp
 */

#include "configuration.h"
#include "serial.h"
#include <boost/shared_array.hpp>
#include <stdexcept>
#include <iostream>
#include <iomanip>

/**
 * @brief Constructor
 */
configuration::configuration() {
	read();
}

/**
 * @brief writes the configuration
 */
void configuration::write() {
	// send the configuration data to the device
	size_t const write_request_size = 7;
	unsigned char write_request_buf[write_request_size] = {0x01, 0x00,
			static_cast<unsigned char>(m_conf.deadzone),
			static_cast<unsigned char>(m_conf.remote_control_min_value_ch1),
			static_cast<unsigned char>(m_conf.remote_control_max_value_ch1),
			static_cast<unsigned char>(m_conf.remote_control_min_value_ch2),
			static_cast<unsigned char>(m_conf.remote_control_max_value_ch2)};

	if(m_conf.control == TANK) write_request_buf[1] |= 0x02;
	serial::get_instance().writeToSerial(write_request_buf, write_request_size);

	// check if it was written successfully
	size_t const write_reply_size = 1;
	boost::shared_array<unsigned char> write_reply_buf = serial::get_instance().readFromSerial(write_reply_size);
	if(write_reply_buf.get()[0] != 0x01) throw std::runtime_error("Error, could not write to the device.");
}

/**
 * @brief reads the configuration from the device and stores it in m_conf
 */
void configuration::read() {
	// request the data from the device
	size_t const read_request_size = 1;
	unsigned char read_request_buf[read_request_size] = {0x00};
	serial::get_instance().writeToSerial(read_request_buf, read_request_size);

	// read the answer from the device
	size_t const read_reply_size = 7;
	boost::shared_array<unsigned char> read_reply_buf = serial::get_instance().readFromSerial(read_reply_size);

	// interpret the answer
	if(read_reply_buf.get()[1] & 0x02) m_conf.control = TANK;
	else m_conf.control = DELTA;

	m_conf.deadzone = static_cast<size_t>(read_reply_buf.get()[2]);
	m_conf.remote_control_min_value_ch1 = static_cast<size_t>(read_reply_buf.get()[3]);
	m_conf.remote_control_max_value_ch1 = static_cast<size_t>(read_reply_buf.get()[4]);
	m_conf.remote_control_min_value_ch2 = static_cast<size_t>(read_reply_buf.get()[5]);
	m_conf.remote_control_max_value_ch2 = static_cast<size_t>(read_reply_buf.get()[6]);
}

/**
 * @brief writes the configuration in a output stream for displaying it to the user
 */
std::ostream &operator<<(std::ostream& os, configuration &c) {
	os << "LXRobotics Antweight Electronic Speed Controller Configuration:" << std::endl;
	os << "Control Method = ";
	if(c.m_conf.control == TANK) os << "TANK" << std::endl;
	else os << "DELTA" << std::endl;
	os << "Deadzone = " << static_cast<float>(c.m_conf.deadzone) / 250.0f << std::endl;
	os << "CH1:" << std::endl << std::setprecision(2) << std::fixed;
	os << "Remote Control Min Value = " << static_cast<float>(c.m_conf.remote_control_min_value_ch1) / 250.0f + 1.0f << std::endl;
	os << "Remote Control Max Value = " << static_cast<float>(c.m_conf.remote_control_max_value_ch1) / 250.0f + 1.0f << std::endl;
	os << "CH2:" << std::endl;
	os << "Remote Control Min Value = " << static_cast<float>(c.m_conf.remote_control_min_value_ch2) / 250.0f + 1.0f << std::endl;
	os << "Remote Control Max Value = " << static_cast<float>(c.m_conf.remote_control_max_value_ch2) / 250.0f + 1.0f << std::endl;
	return os;
}
