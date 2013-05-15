/**
* @author Alexander Entinger, BSc
* @brief this file implements the reading from the receiver channels
* @file input.h
*/

#ifndef INPUT_H_
#define INPUT_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum {RISING = 0, FALLING = 1} E_EDGE_STATE;
typedef void (*callback_func)(uint16_t);

/**
 * @brief initializes the input module
 */
void init_input(callback_func cb_ch1, callback_func cb_ch2);
	
/** 
 * @brief returns if there were valid pulses in valid periods received
 */
bool input_good();	

#endif /* INPUT_H_ */