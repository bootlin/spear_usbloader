// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 Bootlin
 *
 * Author: Herve Codina <herve.codina@bootlin.com>
 *
 * Overview:
 *   Manage SPEAr USB boot protocol.
 */
#ifndef _SPEAR_USB_H_
#define _SPEAR_USB_H_


/*
 * spear_usb.c: Manage SPEAr USB boot protocol
 * 
 * Copyright (C) Bootlin 2021. All Rights Reserved.
 * Author: Herve Codina <herve.codina@bootlin.com>
 *
 */
//#include <stdlib.h>
#include <stdio.h>
#include "usb.h"


int spear_usb_init(void);
void spear_usb_exit(void);

struct spear_usb_dev {
	struct usb_dev_handle *handle;
};

int spear_usb_open(struct spear_usb_dev *dev);
void spear_usb_close(struct spear_usb_dev *dev);

enum spear_usb_type {
	DDR_DRIVER,
	FIRMWARE,
};

int spear_usb_send_command(struct spear_usb_dev *dev,
	enum spear_usb_type type, uint32_t size, uint32_t load_addr);

ssize_t spear_usb_send_buffer(struct spear_usb_dev *dev, void *buffer, size_t size);
ssize_t spear_usb_send_file(struct spear_usb_dev *dev, FILE *file, size_t size);

#endif /*_SPEAR_USB_H_*/
