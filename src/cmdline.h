// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 Bootlin
 *
 * Author: Herve Codina <herve.codina@bootlin.com>
 *
 * Overview:
 *   Parse cmdline args.
 */
#ifndef _CMDLINE_H_
#define _CMDLINE_H_

/* cmdline args */
typedef struct cmdline_s {
	int is_scan_only;
	const char *ddr_driver;
	const char *firmware;
} cmdline_t;

/* Parse given cmdline args */
void cmdline_argsparse(cmdline_t *cmdline,int argc, char* argv[]);

#endif /*_CMDLINE_H_*/
