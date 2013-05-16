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
* @brief this file represents a motor control for the two channels of the esc
* @file motor_control.cpp
*/

#include "motor_control.h"

#include <avr/io.h>
#include <avr/interrupt.h>

static volatile E_MOTOR_STATE m_motor_state = DISABLED;
static volatile E_MOTOR_DIRECTION m_motor_left_dir = FWD;
static volatile E_MOTOR_DIRECTION m_motor_right_dir = FWD;

#define MOTOR_LEFT_A_PIN	(6)
#define MOTOR_LEFT_A_DDR	(DDRC)
#define MOTOR_LEFT_A_PORT	(PORTC)
#define MOTOR_LEFT_B_PIN	(7)
#define MOTOR_LEFT_B_DDR	(DDRC)
#define MOTOR_LEFT_B_PORT	(PORTC)

#define MOTOR_RIGHT_A_PIN	(5)
#define MOTOR_RIGHT_A_DDR	(DDRB)
#define MOTOR_RIGHT_A_PORT	(PORTB)
#define MOTOR_RIGHT_B_PIN	(4)
#define MOTOR_RIGHT_B_DDR	(DDRB)
#define MOTOR_RIGHT_B_PORT	(PORTB)

/**
* @brief initializes the motor control object
*/
void init_motor_control() {
	// set value of als Pins to '0'
	MOTOR_LEFT_A_PORT  &= ~(1<<MOTOR_LEFT_A_PIN);
	MOTOR_LEFT_B_PORT  &= ~(1<<MOTOR_LEFT_B_PIN);
	MOTOR_RIGHT_A_PORT &= ~(1<<MOTOR_RIGHT_A_PIN);
	MOTOR_RIGHT_B_PORT &= ~(1<<MOTOR_RIGHT_B_PIN);
	
	// set all pins to output
	MOTOR_LEFT_A_DDR  |= (1<<MOTOR_LEFT_A_PIN);
	MOTOR_LEFT_B_DDR  |= (1<<MOTOR_LEFT_B_PIN);
	MOTOR_RIGHT_A_DDR |= (1<<MOTOR_RIGHT_A_PIN);
	MOTOR_RIGHT_B_DDR |= (1<<MOTOR_RIGHT_B_PIN);
	
	// enable output compare interrupt a and b and timer 1 overflow interrupt
	TIMSK0 = (1<<OCIE0A) | (1<<OCIE0B) | (1<<TOIE0);
	
	// Timer 0: f = 16 MHz
	// Prescaler = 64 => tTimerStep = 4 us
	// fPWM = 1 kHz, Reload Value = 250
	TCCR0B = (1<<CS01) | (1<<CS00);
}

/**
* @brief enables the motor outputs
*/
void enable_motors() {
	m_motor_state = ENABLED;
}

/**
* @brief disables the motor outputs (both Motor + and - are at zero)
*/
void disable_motors() {
	m_motor_state = DISABLED;
	// disable the motors
	MOTOR_LEFT_A_PORT  &= ~(1<<MOTOR_LEFT_A_PIN);
	MOTOR_LEFT_B_PORT  &= ~(1<<MOTOR_LEFT_B_PIN);
	MOTOR_RIGHT_A_PORT &= ~(1<<MOTOR_RIGHT_A_PIN);
	MOTOR_RIGHT_B_PORT &= ~(1<<MOTOR_RIGHT_B_PIN);
}

/**
* @brief sets the pwm value of the left motor
* @param dir movement direction of the motor: either forward of backward
* @param s speed value between 0 and 255 (0 % to 100 %), speed of 0 = BRAKE
*/
void set_pwm_motor_left(E_MOTOR_DIRECTION const dir, uint8_t const s) {
	uint16_t speed = s;
	if(s == 0) {
		TIMSK0 &= ~(1<<OCIE0A);
	}
	else {
		OCR0A = 255 - speed;
		TIMSK0 |= (1<<OCIE0A);
	}
	m_motor_left_dir = dir;
}

/**
* @brief sets the pwm value of the right motor
* @param dir movement direction of the motor: either forward of backward
* @param s speed value between 0 and 255 (0 % to 100 %), speed of 0 = BRAKE
*/
void set_pwm_motor_right(E_MOTOR_DIRECTION const dir, uint8_t const s) {
	uint16_t speed = s;
	if(s == 0) {
		TIMSK0 &= ~(1<<OCIE0B);
	}
	else {
		OCR0B = 255 - speed;
		TIMSK0 |= (1<<OCIE0B);
	}
	m_motor_right_dir = dir;
}

/**
* @brief ISR for Timer 0 overflow vector
*/
ISR(TIMER0_OVF_vect) {
	// switch the motors off
	MOTOR_LEFT_A_PORT  &= ~(1<<MOTOR_LEFT_A_PIN);
	MOTOR_LEFT_B_PORT  &= ~(1<<MOTOR_LEFT_B_PIN);
	MOTOR_RIGHT_A_PORT &= ~(1<<MOTOR_RIGHT_A_PIN);
	MOTOR_RIGHT_B_PORT &= ~(1<<MOTOR_RIGHT_B_PIN);
}

/**
* @brief ISR for output compare channel A (left motor)
*/
ISR(TIMER0_COMPA_vect) {
	if(m_motor_state == ENABLED) {
		if(m_motor_left_dir == FWD) {
			MOTOR_LEFT_A_PORT  |= (1<<MOTOR_LEFT_A_PIN);
		} else if(m_motor_left_dir == BWD) {
			MOTOR_LEFT_B_PORT  |= (1<<MOTOR_LEFT_B_PIN);
		}
	}
}

/**
* @brief ISR for output compare channel B (right motor)
*/
ISR(TIMER0_COMPB_vect) {
	if(m_motor_state == ENABLED) {
		if(m_motor_right_dir == FWD) {
			MOTOR_RIGHT_A_PORT  |= (1<<MOTOR_RIGHT_A_PIN);
		} else if(m_motor_right_dir == BWD) {
			MOTOR_RIGHT_B_PORT  |= (1<<MOTOR_RIGHT_B_PIN);
		}
	}
}
