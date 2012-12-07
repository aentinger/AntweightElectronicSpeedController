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
#include <arpa/inet.h>
#include <stdio.h>

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
<<<<<<< HEAD
	size_t const write_request_size = 7 + 3 * sizeof(int); // sizeof(int) = 4; 7 + 3 * 4 = 19
	unsigned char wire_request_buf[write_request_size] = {0x01, 0x00,
=======
	size_t const write_request_size = 7;
	unsigned char write_request_buf[write_request_size] = {0x01, 0x00,
>>>>>>> d6695cba4097f67bc2857d6e746d338d4d1c3dc0
			static_cast<unsigned char>(m_conf.deadzone),
			static_cast<unsigned char>(m_conf.remote_control_min_value_ch1),
			static_cast<unsigned char>(m_conf.remote_control_max_value_ch1),
			static_cast<unsigned char>(m_conf.remote_control_min_value_ch2),
			static_cast<unsigned char>(m_conf.remote_control_max_value_ch2),
			0,0,0,0,0,0,0,0,0,0,0,0};

<<<<<<< HEAD
	if(m_conf.control == TANK) wire_request_buf[1] |= 0x02;

	// we transmit the r-s-t values in the order R1 R2 S1, since
	// T1 = S1
	// S2 = S1
	// T2 = -S2

	// convert r-s-t values to network-byte-order to provide guaranteed big-endianess
	unsigned int const R1 = htonl(m_conf.r1);
	memcpy(wire_request_buf + 7, &R1, sizeof(int));
	unsigned int const R2 = htonl(m_conf.r2);
	memcpy(wire_request_buf + 7 + sizeof(int), &R2, sizeof(int));
	unsigned int const S1 = htonl(m_conf.s1);
	memcpy(wire_request_buf + 7 + 2 * sizeof(int), &S1, sizeof(int));

	// transmit the data
	serial::get_instance().writeToSerial(wire_request_buf, write_request_size);
=======
	if(m_conf.control == TANK) write_request_buf[1] |= 0x02;
	serial::get_instance().writeToSerial(write_request_buf, write_request_size);
>>>>>>> d6695cba4097f67bc2857d6e746d338d4d1c3dc0

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

	// calculate the r-s-t values in case we write them dowm to the device again if this is a write command
	update();
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

/**
 * @brief update function for updating the r-s-t values for left and right channel delta control
 */
void configuration::update() {

	float const neutral_pos = 125.0f;
	float const min_motor_value = -255.0f * (1<<5);
	float const max_motor_value = 255.0f * (1<<5);

	// left channel
	{
		dim3 const P1(static_cast<float>(m_conf.remote_control_min_value_ch1), neutral_pos, min_motor_value); // CH1, CH2, Motorvalue
		dim3 const P2(static_cast<float>(m_conf.remote_control_max_value_ch1), neutral_pos, max_motor_value);
		dim3 const P3(neutral_pos, static_cast<float>(m_conf.remote_control_min_value_ch2), min_motor_value);

		std::cout << "P1 = " << P1 << std::endl;

		calc_RST(m_conf.r1, m_conf.s1, m_conf.t1, P1, P2, P3);
	}
	// right channel
	{
		dim3 const P1(static_cast<float>(m_conf.remote_control_min_value_ch1), neutral_pos, min_motor_value); // CH1, CH2, Motorvalue
		dim3 const P2(static_cast<float>(m_conf.remote_control_max_value_ch1), neutral_pos, max_motor_value);
		dim3 const P3(neutral_pos, static_cast<float>(m_conf.remote_control_max_value_ch2), min_motor_value);

		calc_RST(m_conf.r2, m_conf.s2, m_conf.t2, P1, P2, P3);
	}

	// debug output
//	std::cout << "Left Channel:" << std::endl;
//	std::cout << "r = " << m_conf.r1 << std::endl;
//	std::cout << "s = " << m_conf.s1 << std::endl;
//	std::cout << "t = " << m_conf.t1 << std::endl;
//	std::cout << "Right Channel:" << std::endl;
//	std::cout << "r = " << m_conf.r2 << std::endl;
//	std::cout << "s = " << m_conf.s2 << std::endl;
//	std::cout << "t = " << m_conf.t2 << std::endl;
}

/**
 * @brief calculates the r-s-t parameters out of 3 points for the delta mixer parameters
 */
void configuration::calc_RST(int &r, int &s, int &t, dim3 const &P1, dim3 const &P2, dim3 const &P3) {
	dim3 const P1P2 = P1 - P2;
	dim3 const P1P3 = P1 - P3;

	dim3 const n = dim3::cross(P1P2, P1P3);

	float const d = n.x() * P1.x() + n.y() * P1.y() + n.z() * P1.z();

	r = static_cast<int>(d/n.z());
	s = static_cast<int>(n.x()/n.z());
	t = static_cast<int>(n.y()/n.z());
}
