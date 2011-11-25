/*
 *
 *  Copyright (c) 2004-2005  Warren Jasper <wjasper@tx.ncsu.edu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/*
 * your kernel needs to be configured with /dev/usb/hiddev support
 * I think most distros are already
 * mikee@signalscape.com
 *
 */

#ifndef PMD_H
#define PMD_H

#ifdef __cplusplus
extern "C" { 
#endif 

#define MAX_HIDDEV 128
#define CBUSB_SUCCESS 0;

#define MCC_VID (0x09db)

#include <hid.h>

/* These definitions are used to build the request type in usb_control_msg */

/* USB Request */
enum {
  GET_REPORT             = 1,
  GET_IDLE               = 2,
  GET_PROTOCOL           = 3,
  SET_REPORT             = 9,
  SET_IDLE               = 0x0A,
  SET_PROTOCOL           = 0x0B
};

#define  INPUT_REPORT      (1 << 8)
#define  OUTPUT_REPORT     (2 << 8)
#define  FEATURE_REPORT    (3 << 8)

/* For the HID devices */
int PMD_SendOutputReport(HIDInterface* hid, __u8 reportID, __u8* vals, int num_vals, int delay);
int PMD_GetInputReport(HIDInterface* hid, __u8 reportID, __u8 *vals, int num_vals, int delay);
int PMD_GetFeatureReport(HIDInterface* hid, __u8 reportID, __u8 *vals, int num_vals, int delay);
int PMD_Find_Interface(HIDInterface** hid, int interface, int productID);
char* PMD_GetSerialNumber(HIDInterface* hid);

/* For USB devices */
usb_dev_handle* usb_device_find_USB_MCC(int productId);

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif 

#endif  //PMD_H
