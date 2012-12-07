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
 * @file configuration.h
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <iostream>
#include <stdlib.h>
#include "dim3.h"

enum E_CONTROL{TANK, DELTA};

typedef struct {
	E_CONTROL control;
	size_t deadzone;
	size_t remote_control_min_value_ch1;
	size_t remote_control_max_value_ch1;
	size_t remote_control_min_value_ch2;
	size_t remote_control_max_value_ch2;
	int r1, s1, t1;
	int r2, s2, t2;
} s_configuration;

class configuration {
public:
	/**
	 * @brief Constructor
	 */
	configuration();

	/**
	 * @brief Destructor
	 */
	~configuration() { }

	/**
	 * @brief writes the configuration
	 */
	void write();

	/**
	 * @brief writes the configuration in a output stream for displaying it to the user
	 */
	friend std::ostream &operator<<(std::ostream& os, configuration &c);

	/**
	 * @brief providing external access to the configuration for setting parameters
	 */
	inline s_configuration *get() { return &m_conf; }

	/**
	 * @brief update function for updating the r-s-t values for left and right channel delta control
	 */
	void update();

private:
	s_configuration m_conf;

	/**
	 * @brief reads the configuration from the device and stores it in m_conf
	 */
	void read();

	/**
	 * @brief calculates the r-s-t parameters out of 3 points for the delta mixer parameters
	 */
	void calc_RST(int &r, int &s, int &t, dim3 const &P1, dim3 const &P2, dim3 const &P3);
};


#endif /* CONFIGURATION_H_ */
