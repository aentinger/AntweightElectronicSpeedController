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
* @file linear_mapper.c
*/

#include "linear_mapper.h"

/**
 * @brief initializes the linear mapper
 * @param lm the linear mapper ADT (abstract data type)
 * @param input_min minimum value of the input
 * @param input_max maximum value of the input
 * @param output_min minimum output value
 * @param output_max maximum output value
 */
void init_linear_mapper(linear_mapper *lm, int16_t const input_min, int16_t const input_max, int16_t const output_min, int16_t const output_max) {
	int32_t const delta_input = ((int32_t)(input_max) - (int32_t)(input_min)) << 8;
	int32_t const delta_output = ((int32_t)(output_max) - (int32_t)(output_min)) << 8;
	
	lm->k = (int32_t)
		(
			delta_output
			/ 
			delta_input
		); // multiplication by 16 for increasing accuracy in the mapping function
	lm->d = (int32_t)
		(
			(int32_t)(output_max)
			- 
			(
				lm->k * (int32_t)(input_max)
			)
		);
}

/**
 * @brief performs the linear mapping 
 * @param value input value to be mapped to output value
 * @return mapped output value
 */
int16_t linear_map(linear_mapper *lm, int16_t const value) {
	return (int16_t)
			(
			(lm->k * (int32_t)(value))
			+
			lm->d
			);
}