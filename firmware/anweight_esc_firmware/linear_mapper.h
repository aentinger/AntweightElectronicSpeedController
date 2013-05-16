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
* @brief this file implements a linear mapper for adapting signal ranges between various functions
* @file linear_mapper.h
*/

#ifndef LINEAR_MAPPER_H_
#define LINEAR_MAPPER_H_

#include <stdint.h>

// definition of adt linear_mapper
typedef struct linear_mapper {
	int32_t k;
	int32_t d;
} linear_mapper;

/**
 * @brief initializes the linear mapper
 * @param lm the linear mapper ADT (abstract data type)
 * @param input_min minimum value of the input
 * @param input_max maximum value of the input
 * @param output_min minimum output value
 * @param output_max maximum output value
 */
void init_linear_mapper(linear_mapper *lm, int16_t const input_min, int16_t const input_max, int16_t const output_min, int16_t const output_max);

/**
 * @brief performs the linear mapping 
 * @param value input value to be mapped to output value
 * @return mapped output value
 */
int16_t linear_map(linear_mapper *lm, int16_t const value);


#endif /* LINEAR_MAPPER_H_ */