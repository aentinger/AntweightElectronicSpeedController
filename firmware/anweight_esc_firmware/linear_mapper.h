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