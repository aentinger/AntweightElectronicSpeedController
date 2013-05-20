/*
	Copyright 2013 by Alexander Entinger, BSc

    This file is part of antweight_esc_firmware.

    antweight_esc_firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    antweight_esc_firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with antweight_esc_firmware.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @author Alexander Entinger, BSc
 * @brief this file implements a filter for the input signals
 * @file filter.h
 */

#ifndef FILTER_H_
#define FILTER_H_

#include <stdint.h>

// definition of adt filter
typedef struct filter {
	uint16_t *p_data;
	uint8_t size;
	uint8_t pos;
} filter;

/** 
 * @brief create a filter with the the size 'size' and initializes the values with the value 'init_value'
 */
void init_filter(filter *f, uint8_t const size, uint16_t const init_value);

/**
 * @brief adds the value val to the filter
 */
void filter_add_value(filter *f, uint16_t const val);

/**
 * @brief returns the filtered value
 */
uint16_t filter_get_value(filter *f);


#endif /* FILTER_H_ */