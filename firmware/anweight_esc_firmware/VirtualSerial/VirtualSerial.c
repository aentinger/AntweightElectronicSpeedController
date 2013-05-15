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
 *  Main source file for the VirtualSerial demo. This file contains the main tasks of
 *  the demo and is responsible for the initial application hardware configuration.
 */

#include "VirtualSerial.h"

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber   = 0,
				.DataINEndpoint           =
					{
						.Address          = CDC_TX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.DataOUTEndpoint =
					{
						.Address          = CDC_RX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.NotificationEndpoint =
					{
						.Address          = CDC_NOTIFICATION_EPADDR,
						.Size             = CDC_NOTIFICATION_EPSIZE,
						.Banks            = 1,
					},
			},
	};

/**
 * @brief Configures the board hardware and chip peripherals for the demo's functionality. 
 */
void init_virtual_serial(void) {
	// Disable watchdog if enabled by bootloader/fuses 
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	// Disable clock division 
	clock_prescale_set(clock_div_1);

	// Hardware Initialization
	USB_Init();
}

/** 
 * @brief task that needs to be called periodically to check on the usb stuff
 */
void virtual_serial_task() {
	CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
	USB_USBTask();
}

/** 
 * @brief returns the number of bytes available in the usb buffer
 * @param num_of_bytes is the number of bytes available to be read
 */		
uint16_t virtual_serial_bytes_available() {
	if (USB_DeviceState == DEVICE_STATE_Configured) {
		return CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
	} else {
		return 0;
	}		
}

/** 
 * @brief receives one byte from the virtual serial device
 * @param data pointer to where the received data byte has to be stored
 * @return true in case of success, false in case of failure
 */
bool virtual_serial_receive_byte(uint8_t *data) {
	bool success = false;
	
	if(USB_DeviceState == DEVICE_STATE_Configured) {
		int16_t const recv_byte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
		if(recv_byte >= 0) {
			*data = (uint8_t)(recv_byte);
			success = true;
		}
	}	
	return success;
}

/**
 * @brief send data to the host
 * @param data pointer to the data array to be sent
 * @param size number of bytes to be sent
 * @return true in case of success, false in case of failure
 */
bool virtual_serial_send_data(uint8_t const *data, uint8_t const size) {
	bool success = false;
	
	if(USB_DeviceState == DEVICE_STATE_Configured) {
		if(CDC_Device_SendData(&VirtualSerial_CDC_Interface, (void*)(data), (uint16_t)(size)) == ENDPOINT_RWSTREAM_NoError ) {
			CDC_Device_Flush(&VirtualSerial_CDC_Interface); // guarantee that the device buffers are emptied
			success = true;
		} 
	}		
	
	return success;
}

/**
 * @brief Event handler for the library USB Connection event. 
 */
void EVENT_USB_Device_Connect(void) {

}

/**
 * @brief Event handler for the library USB Disconnection event. 
 */
void EVENT_USB_Device_Disconnect(void) {

}

/**
 * @brief Event handler for the library USB Configuration Changed event. 
 */
void EVENT_USB_Device_ConfigurationChanged(void) {
	bool ConfigSuccess = true;

	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);
}

/**
 * @brief Event handler for the library USB Control Request reception event. 
 */
void EVENT_USB_Device_ControlRequest(void) {
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}

