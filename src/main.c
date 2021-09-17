// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 Bootlin
 *
 * Author: Herve Codina <herve.codina@bootlin.com>
 *
 * Overview:
 *   Main entry point.
 */
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <endian.h>
#include "cmdline.h"
#include "spear_usb.h"


#define IH_MAGIC	0x27051956	/* Image Magic Number		*/
#define IH_NMLEN		32	/* Image Name Length		*/

typedef struct image_header {
	uint32_t	ih_magic;	/* Image Header Magic Number	*/
	uint32_t	ih_hcrc;	/* Image Header CRC Checksum	*/
	uint32_t	ih_time;	/* Image Creation Timestamp	*/
	uint32_t	ih_size;	/* Image Data Size		*/
	uint32_t	ih_load;	/* Data	 Load  Address		*/
	uint32_t	ih_ep;		/* Entry Point Address		*/
	uint32_t	ih_dcrc;	/* Image Data CRC Checksum	*/
	uint8_t		ih_os;		/* Operating System		*/
	uint8_t		ih_arch;	/* CPU architecture		*/
	uint8_t		ih_type;	/* Image Type			*/
	uint8_t		ih_comp;	/* Compression Type		*/
	uint8_t		ih_name[IH_NMLEN];	/* Image Name		*/
} image_header_t;


static int read_header(FILE *file, image_header_t *header)
{
	uint32_t tmp;
	size_t ret;

	ret = fread(&tmp, 4, 1, file);
	if (ret < 1) {
		fprintf(stderr, "read header.ih_magic failed\n");
		return -1;
	}
	header->ih_magic = be32toh(tmp);

	ret = fread(&tmp, 4, 1, file);
	if (ret < 1) {
		fprintf(stderr, "read header.ih_hcrc failed\n");
		return -1;
	}
	header->ih_hcrc = be32toh(tmp);

	ret = fread(&tmp, 4, 1, file);
	if (ret < 1) {
		fprintf(stderr, "read header.ih_time failed\n");
		return -1;
	}
	header->ih_time = be32toh(tmp);

	ret = fread(&tmp, 4, 1, file);
	if (ret < 1) {
		fprintf(stderr, "read header.ih_size failed\n");
		return -1;
	}
	header->ih_size = be32toh(tmp);

	ret = fread(&tmp, 4, 1, file);
	if (ret < 1) {
		fprintf(stderr, "read header.ih_load failed\n");
		return -1;
	}
	header->ih_load = be32toh(tmp);

	ret = fread(&tmp, 4, 1, file);
	if (ret < 1) {
		fprintf(stderr, "read header.ih_ep failed\n");
		return -1;
	}
	header->ih_ep = be32toh(tmp);

	ret = fread(&tmp, 4, 1, file);
	if (ret < 1) {
		fprintf(stderr, "read header.ih_dcrc failed\n");
		return -1;
	}
	header->ih_dcrc = be32toh(tmp);

	ret = fread(&header->ih_os, 1, 1, file);
	if (ret < 1) {
		fprintf(stderr, "read header.ih_os failed\n");
		return -1;
	}

	ret = fread(&header->ih_arch, 1, 1, file);
	if (ret < 1) {
		fprintf(stderr, "read header.ih_arch failed\n");
		return -1;
	}

	ret = fread(&header->ih_type, 1, 1, file);
	if (ret < 1) {
		fprintf(stderr, "read header.ih_type failed\n");
		return -1;
	}

	ret = fread(&header->ih_comp, 1, 1, file);
	if (ret < 1) {
		fprintf(stderr, "read header.ih_comp failed\n");
		return -1;
	}

	ret = fread(&header->ih_name, 1, IH_NMLEN, file);
	if (ret < IH_NMLEN) {
		fprintf(stderr, "read header.ih_name failed\n");
		return -1;
	}

	return 0;
}

static int send_file(struct spear_usb_dev *dev, enum spear_usb_type type, const char *filename)
{
	FILE *file;
	image_header_t header;
	int ret;
	ssize_t ssize;

	file = fopen(filename, "r");
	if (!file) {
		fprintf(stderr,"open %s failed (%d)\n", filename, errno);
		return -1;
	}

	ret = read_header(file, &header);
	if (ret < 0)
		goto end;

	if (header.ih_magic != IH_MAGIC) {
		fprintf(stderr,"unsupported file %s\n", filename);
		ret = -1;
		goto end;
	}

	ret = spear_usb_send_command(dev, type, header.ih_size, header.ih_load);
	if (ret < 0)
		goto end;

	ssize = spear_usb_send_file(dev, file, header.ih_size);
	if (ssize < 0) {
		ret = ssize;
		goto end;
	}

	ret = 0;

end:
	fclose(file);
	return ret;
}


/**
 * Entry point
 * @param argc standard argc
 * @param argv standard argv
 * @return Error code
 */
int main(int argc, char* argv[])
{
	cmdline_t cmdline;
	struct spear_usb_dev dev;
	int exit_code;

	/* Parse cmdline */
	cmdline_argsparse(&cmdline,argc,argv);

	/* Init spear usb */
	if (spear_usb_init() != 0) {
		return EXIT_FAILURE;
	}

	/* Open spear device */
	if (spear_usb_open(&dev) != 0) {
		exit_code = EXIT_FAILURE;
		goto end_usb_exit;
	}

	/* In case of scan only, everything is done */
	if (cmdline.is_scan_only) {
		exit_code = 0;
		goto end_usb_close;
	}

	if (!cmdline.ddr_driver) {
		fprintf(stderr, "ddr driver file is not given.\n");
		exit_code = EXIT_FAILURE;
		goto end_usb_close;
	}

	if (!cmdline.firmware) {
		fprintf(stderr, "firmware file is not given.\n");
		exit_code = EXIT_FAILURE;
		goto end_usb_close;
	}

	printf("Send ddr_driver %s\n",cmdline.ddr_driver);
	if ( send_file(&dev, DDR_DRIVER, cmdline.ddr_driver) < 0) {
		fprintf(stderr, "failed to send ddr driver file %s\n",cmdline.ddr_driver);
		exit_code = EXIT_FAILURE;
		goto end_usb_close;
	}

	printf("Send firmware %s\n",cmdline.ddr_driver);
	if ( send_file(&dev, FIRMWARE, cmdline.firmware) < 0) {
		fprintf(stderr, "failed to send firmware file %s\n",cmdline.firmware);
		exit_code = EXIT_FAILURE;
		goto end_usb_close;
	}

	exit_code = 0;

end_usb_close:
	/* Close spear device */
	spear_usb_close(&dev);
end_usb_exit:
	/* Exit spear usb */
	spear_usb_exit();
	return exit_code;
}
