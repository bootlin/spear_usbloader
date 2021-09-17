// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 Bootlin
 *
 * Author: Herve Codina <herve.codina@bootlin.com>
 *
 * Overview:
 *   Parse cmdline args.
 */
#include "cmdline.h"
#include "config.h"
#include <argp.h>
#include <stdlib.h>


static int cmdline_argscb(int key, char *arg, struct argp_state *state);

/* Version du programme */
const char *argp_program_version = PACKAGE_VERSION " - " __DATE__ " " __TIME__;

/* Documentation. */
static char cmdline_doc[]="spear_usbloader\n"
	"USB loader for SPEAr3xx and SPEAr600 SOC";

/* Options */
static struct argp_option cmdline_options[] = {
	{"scan"     , 's', NULL                ,0 ,"Scan only (do not load images)"          ,0 },
	{"ddr"      , 'd', "<ddr_driver_file>" ,0 ,"Use given ddr_driver_file as ddr driver" ,0 },
	{"firmware" , 'f', "<firmware_file>"   ,0 ,"Use given firmware_file as firmware"     ,0 },
	{0}
};

/* The parser itself */
static struct argp cmdline_argp = { 
	cmdline_options, 
	cmdline_argscb, 
	NULL,/*cmdline_argsdoc,*/ 
	cmdline_doc,
	NULL,NULL,NULL
};


/**
 * Parser callback
 * @param key parameter key
 * @param arg argument related to the key
 * @param state parser state
 * @return error code
 */
static int cmdline_argscb(int key, char *arg, struct argp_state *state)
{
	cmdline_t *cmdline = state->input;

	switch (key) {
	case 's':
		cmdline->is_scan_only = 1;
		return 0;
	case 'd':
		cmdline->ddr_driver = arg;
		return 0;
	case 'f':
		cmdline->firmware = arg;
		return 0;

	case ARGP_KEY_ARG:
		if(state->arg_num >= 0) {
			/* Too mutch args */
			argp_usage(state);
			break;
		}
		return 0;

	case ARGP_KEY_END:
		if (state->arg_num != 0) {
			/* Invalid args */
			argp_usage(state);
			break;
		}

	default:
		break;
	}
	return ARGP_ERR_UNKNOWN;
}


/**
 * Parse given cmdline args
 * @param cmdline cmdline
 * @param argc standard argc
 * @param argv standard argv
 * @return void
 */
void cmdline_argsparse(cmdline_t *cmdline,int argc, char* argv[])
{
	/* Set default values */
	cmdline->is_scan_only = 0;
	cmdline->ddr_driver = NULL;
	cmdline->firmware = NULL;

	/* Parse */
	argp_parse(&cmdline_argp, argc, argv, 0, 0, cmdline);  
}

