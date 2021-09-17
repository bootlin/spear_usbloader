// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 Bootlin
 *
 * Author: Herve Codina <herve.codina@bootlin.com>
 *
 * Overview:
 *   Manage SPEAr USB boot protocol.
 */
#include "spear_usb.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "usb.h"

struct dev_supported {
	uint16_t idVendor;
	uint16_t idProduct;
	const char *name;
};

static const struct dev_supported tab_dev_supported[] = {
	{ .idVendor = 0x0483, .idProduct = 0x3801, .name = "SPEAr3xx" },
	{ 0 },
};

static int is_supported_dev(struct usb_device *dev)
{
	const struct dev_supported *dev_supported;

	dev_supported = tab_dev_supported;
	do {
		if ((dev_supported->idVendor == dev->descriptor.idVendor) &&
		    (dev_supported->idProduct == dev->descriptor.idProduct) ) {
			printf("Found %s (VID=%04X, PID=0x%04X)\n",
				dev_supported->name,
				dev_supported->idVendor,
				dev_supported->idProduct);
			return 1;
		}
	} while ((++dev_supported)->name);
	return 0;
}


/**
 * Init spear usb
 * @return Error code
 */
int spear_usb_init(void)
{
	/* Initialize USB library */
	usb_init();

	/* find all busses */
	usb_find_busses();

	/* find all connected devices */
	usb_find_devices();

	return 0;
}


void spear_usb_exit(void)
{
	return;
}


static struct usb_device *spear_usb_search(void)
{
	struct usb_bus *bus;
	struct usb_device *dev;

	/* Search all busses, all devices for a spear */
	bus = usb_get_busses();
	while (bus) {
		dev = bus->devices;
		while (dev) {
			if (is_supported_dev(dev)) {
				return dev;
			}
			dev = dev->next;
		}
		bus = bus->next;
	}
	return NULL;
}


int spear_usb_open(struct spear_usb_dev *dev)
{
	struct usb_device *usb_dev;
	usb_dev_handle *handle;

	/* Raz data */
	memset(dev, 0, sizeof(*dev));

	/* Search SPEAr device ...*/
	usb_dev = spear_usb_search();
	if (!usb_dev) {
		fprintf(stderr, "No USB SPEAr device found!\n");
		return -1;
	}

	/* ... open it ...*/
	handle = usb_open(usb_dev);
	if (!handle) {
		fprintf(stderr, "usb_open failed.\n");
		return -1;
	}

	/* ... set the active configuration ... */
	if(usb_set_configuration(handle, 1) < 0) {
		fprintf(stderr, "usb_set_configuration failed.\n");
		goto failed;
	}

	/* ... claim interface */
	if(usb_claim_interface(handle, 0) < 0) {
		fprintf(stderr, "usb_claim_interface failed.\n");
		goto failed;
	}

	/* Ok, everything is done -> save usb_dev_handle */
	dev->handle = handle;
	return 0;

failed:
	usb_close(handle);
	return -1;
}

void spear_usb_close(struct spear_usb_dev *dev)
{
	if (dev->handle)
		usb_close(dev->handle);
}


ssize_t spear_usb_send_buffer(struct spear_usb_dev *dev, void *buffer, size_t size)
{
	size_t left;
	size_t s;
	void *tmp;

	left = size;
	tmp = buffer;
	while (left) {
		s = left;
		if (s > 1024)
			s = 1024;

		if(usb_bulk_write(dev->handle, 0x02, tmp, s, 1000) != s) {
			fprintf(stderr, "usb_bulk_write failed.\n");
			return -1;
		}
		tmp += s;
		left -= s;
	}
	return size;
}

ssize_t spear_usb_send_file(struct spear_usb_dev *dev, FILE *file, size_t size)
{
	size_t left;
	size_t s;
	uint8_t buffer[1024];
	size_t ret;

	left = size;
	while (left) {
		s = left;
		if (s > sizeof(buffer))
			s = sizeof(buffer);

		/* Read to a buffer */
		ret = fread(buffer, 1, s, file);
		if (ret != s) {
			fprintf(stderr, "fread failed.\n");
			return -1;
		}

		/* Send buffer */
		ret = spear_usb_send_buffer(dev, buffer, s);
		if (ret != s) {
			fprintf(stderr, "spear_usb_send_buffer failed.\n");
			return -1;
		}

		left -= s;
	}
	return size;
}

int spear_usb_send_command(struct spear_usb_dev *dev,
		enum spear_usb_type type, uint32_t size, uint32_t load_addr)
{
	uint8_t buf[12] = {0};
	ssize_t ssize;

	/* Set Type of data */
	switch(type) {
	case DDR_DRIVER: buf[0] = 0x11; break;
	case FIRMWARE:   buf[0] = 0x22; break;
	default:
		fprintf(stderr, "type %d unsupported.\n", type);
		return -1;
	}

	/* Set size */
	buf[4] = size & 0xff;
	buf[5] = (size >> 8)  & 0xff;
	buf[6] = (size >> 16) & 0xff;
	buf[7] = (size >> 24) & 0xff;

	/* Set load address */
	buf[8] = load_addr & 0xff;
	buf[9] = (load_addr >> 8)  & 0xff;
	buf[10] = (load_addr >> 16) & 0xff;
	buf[11] = (load_addr >> 24) & 0xff;

	/* Send buffer */
	ssize = spear_usb_send_buffer(dev, buf, sizeof(buf));
	if (ssize != sizeof(buf)) {
		fprintf(stderr, "command send failed.\n");
		return -1;
	}

	return 0;
}