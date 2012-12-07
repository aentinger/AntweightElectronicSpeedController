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
 * @file serial.h
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#include <string>
#include <boost/asio.hpp>
#include <boost/shared_array.hpp>

class serial {
public:
	/**
	 * @brief creates a singleton instance and returns it to the caller (once created)
	 */
	static serial &get_instance() {
		if(!m_ptr_instance) {
			m_ptr_instance = new serial;
		}
		return (*m_ptr_instance);
	}

	/**
	 * @brief destroys a serial singleton instance
	 */
	static void destroy() {
		if(!m_ptr_instance) {
			delete m_ptr_instance;
			m_ptr_instance = 0;
		}
	}

	/**
	 * @brief initializes the serial
	 */
	void init(std::string const &devNode, unsigned int const baudRate);

	/**
	 * @brief write data to the serial port
	 */
	void writeToSerial(unsigned char const *buf, unsigned int const size);

	/**
	 * @brief read data from the serial port
	 */
	boost::shared_array<unsigned char> readFromSerial(unsigned int const size);

private:
	static serial *m_ptr_instance;

	boost::asio::io_service m_io_service;
	boost::asio::serial_port m_serial_port;

	/**
	 * @brief Constructor
	 */
	serial() : m_io_service(), m_serial_port(m_io_service) { }

	/**
	 * @brief Destructor
	 */
	~serial() {
		m_serial_port.close();
	}
};

#endif /* SERIAL_H_ */
