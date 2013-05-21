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
* @brief this module controls the status led of the esc
* @file status_led.c
*/

#include <avr/io.h>
#include "status_led.h"

#define LED_DDR		(DDRC)
#define LED_PORT	(PORTC)
#define LED_NR		(2)

/** 
 * @brief initialize the status led
 */
void init_status_led() {
	LED_DDR  |= (1<<LED_NR);
	LED_PORT |= (1<<LED_NR);
}

/** 
 * @brief turn status led on
 */
void status_led_turn_on() {
	LED_PORT &= ~(1<<LED_NR);
}

/** 
 * @brief turn status led off
 */
void status_led_turn_off() {
	LED_PORT |= (1<<LED_NR);
}