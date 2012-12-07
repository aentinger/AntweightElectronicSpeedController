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
 * @brief this file implements a linear mapper for two dimensions representing a plane for the delta mixer functionaliaty
 * @file linear_mapper_2d.h
 */

#ifndef LINEAR_MAPPER_2D_H_
#define LINEAR_MAPPER_2D_H_

#include <stdint.h>

// definition of adt linear_mapper_2d
typedef struct linear_mapper_2d {
	int32_t r;
	int32_t s;
	int32_t t;
} linear_mapper_2d;

/**
 * @brief initializes the linear mapper 2d
 * @param lm the linear mapper ADT (abstract data type)
 * @param r,s,t parameters of the mapping function
 */
void init_linear_mapper_2d(linear_mapper_2d *lm, int32_t const r, int32_t const s, int32_t const t);

/**
 * @brief performs the linear mapping in 2d
 * @param ch1 input value for channel 1
 * @param ch1 input value for channel 2
 * @return mapped output value
 */
int32_t linear_map_2d(linear_mapper_2d const *const lm, int16_t const ch1, int16_t const ch2);





#endif /* LINEAR_MAPPER_2D_H_ */