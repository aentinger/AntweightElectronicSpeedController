/*
             LUFA Library
     Copyright (C) Dean Camera, 2013.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2013  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Header file for VirtualSerial.c.
 */

#ifndef _VIRTUALSERIAL_H_
#define _VIRTUALSERIAL_H_

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "Descriptors.h"

#include <LUFA/Drivers/USB/USB.h>

/**
 * @brief LUFA provided function prototypes
 */
void init_virtual_serial(void);
void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);

/** 
 * @brief task that needs to be called periodically to check on the usb stuff
 */
void virtual_serial_task();

/** 
 * @brief returns the number of bytes available in the usb buffer
 * @return num_of_bytes is the number of bytes available to be read
 */		
uint16_t virtual_serial_bytes_available();

/** 
 * @brief receives one byte from the virtual serial device
 * @param data pointer to where the received data byte has to be stored
 * @return true in case of success, false in case of failure
 */
bool virtual_serial_receive_byte(uint8_t *data);

/**
 * @brief send data to the host
 * @param data pointer to the data array to be sent
 * @param size number of bytes to be sent
 * @return true in case of success, false in case of failure
 */
bool virtual_serial_send_data(uint8_t const *data, uint8_t const size);

#endif

