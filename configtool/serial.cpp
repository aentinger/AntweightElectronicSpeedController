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
 * @brief this file implements a class for serial communication
 * @file serial.cpp
 */

#include "serial.h"

// Instantiation of static member
serial *serial::m_ptr_instance = 0;

/**
 * @brief initializes the serial
 */
void serial::init(std::string const &devNode, unsigned int const baudRate) {

	m_serial_port.open(devNode);
	m_serial_port.set_option(boost::asio::serial_port_base::baud_rate(baudRate));
	m_serial_port.set_option(boost::asio::serial_port_base::character_size(8));
	m_serial_port.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
	m_serial_port.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
	m_serial_port.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
}

/**
 * @brief write data to the serial port
 */
void serial::writeToSerial(unsigned char const *buf, unsigned int const size) {
	boost::asio::write(m_serial_port, boost::asio::buffer(buf, size));
}

/**
 * @brief read data from the serial port
 */
boost::shared_array<unsigned char> serial::readFromSerial(unsigned int const size) {
	boost::shared_array<unsigned char> buf(new unsigned char[size]);

	boost::asio::read(m_serial_port, boost::asio::buffer(buf.get(), size));

	return buf;
}
