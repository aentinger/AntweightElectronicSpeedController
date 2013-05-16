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
* @brief this file implements the reading from the receiver channels
* @file input.cpp
*/

#include "input.h"

#include <avr/io.h>
#include <avr/interrupt.h>

typedef enum {CH1 = 0, CH2 = 1} E_CHANNEL_SELECT;

static volatile bool m_input_good = false; // signals if we have sufficient signals or not
static volatile E_EDGE_STATE m_ch_edge_state[2] = {RISING, RISING};
static volatile callback_func m_ch_callback[2] = {0, 0};
static volatile uint8_t m_pulse_cnt = 0; // pulse counter for determining signal loss

/**
 * @brief initializes the input module
 */
void init_input(callback_func cb_ch1, callback_func cb_ch2) {
	// register the callbacks
	m_ch_callback[CH1] = cb_ch1;
	m_ch_callback[CH2] = cb_ch2;
	
	// ch1 input pin = PD0 (INT0)
	// ch2 input pin = PD1 (INT1)
	DDRD &=  ~((1<<PORTD0) | (1<<PORTD1)); // set to input
	PORTD |= (1<<PORTD0) | (1<<PORTD1); // activate pullup
	
	// first we go for the rising edge
	EICRA |= (1<<ISC01) | (1<<ISC00) | (1<<ISC11) | (1<<ISC10);
	
	// enable the external interrupts
	EIMSK |= (1<<INT0) | (1<<INT1);

	// set timer to zero
	TCNT1 = 0;
	
	// enable timer 0 overflow interrupt
	TIMSK1 = (1<<TOIE1);
	
	// Prescaler = 64, tTimerStep = 4 us
	TCCR1B = (1<<CS11) | (1<<CS10);
}

/** 
 * @brief returns if there were valid pulses in valid periods received
 */
bool input_good() {
	return m_input_good;
}

/** 
 * @brief timer 1 overflow interrupt, occurs every 262 ms
 */
ISR(TIMER1_OVF_vect) {
	// in 260 ms on one channel we should have 13 pulses
	// so with two channels we should have 26 pulses, if we have significant less (lets say half)
	// its fair to assume, that we have a signal loss,
	uint8_t const MIN_PULSES = 20;
	
	if(m_pulse_cnt < MIN_PULSES) {
		// set the flag that symbolizes good data to false
		m_input_good = false;
		// reset the states to wait for a rising edge again (when new correct data starts to arrive)
		m_ch_edge_state[CH1] = RISING;
		m_ch_edge_state[CH2] = RISING;
		EICRA |= (1<<ISC01) | (1<<ISC00) | (1<<ISC11) | (1<<ISC10);
	} else {
		m_input_good = true;
	}
	// reset the pulse cnt
	m_pulse_cnt = 0;
}

/**
 * @brief externe interrupt for ch 1
 */
ISR(INT0_vect) {
	static uint16_t start = 0, stop = 0;
	
	if(m_ch_edge_state[CH1] == RISING) {
		start = TCNT1; // measure the time
		EICRA &= ~(1<<ISC00); // now wait for falling edge
		m_ch_edge_state[CH1] = FALLING; // switch state
	} else if(m_ch_edge_state[CH1] == FALLING) {
		stop = TCNT1; // measure the time
		uint16_t const pulse_duration = stop - start; // calculate the difference
		(*(m_ch_callback[CH1]))(pulse_duration);
		m_pulse_cnt++;
		EICRA |= (1<<ISC00); // now wait for falling edge
		m_ch_edge_state[CH1] = RISING; // switch state
	}
}

/**
 * @brief externe interrupt for ch 2
 */
ISR(INT1_vect) {
	static uint16_t start = 0, stop = 0;
		
	if(m_ch_edge_state[CH2] == RISING) {
		start = TCNT1; // measure the time
		EICRA &= ~(1<<ISC10); // now wait for falling edge
		m_ch_edge_state[CH2] = FALLING; // switch state
	} else if(m_ch_edge_state[CH2] == FALLING) {
		stop = TCNT1; // measure the time
		uint16_t const pulse_duration = stop - start; // calculate the difference
		(*(m_ch_callback[CH2]))(pulse_duration);
		m_pulse_cnt++;
		EICRA |= (1<<ISC10); // now wait for falling edge
		m_ch_edge_state[CH2] = RISING; // switch state
	}
}
