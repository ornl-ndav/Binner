#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "vcbdebug.h"

int vcbDebugLevel = 10;

void vcbDebugLevelSet(int newlevel)
{
	VCB_VERBOSE("Trying to change vcbDebugLevel from %d to %d\n", vcbDebugLevel, newlevel);
	vcbDebugLevel = newlevel;
	VCB_INFO("new vcbDebugLevel is %d\n", vcbDebugLevel);
}

void _db_e_win(int errorcode, char *fmt, ...) {
	va_list ap;
	if(vcbDebugLevel>=VCB_DEBUG_ERROR) {
		fprintf(stderr, "     ERROR: ");
		va_start(ap, fmt);
		vfprintf(stderr, fmt, ap);
		va_end(ap);
	}
	if(errorcode) exit(errorcode);
}

void _db_w_win(char *fmt, ...) {
	va_list ap;
	if(vcbDebugLevel>=VCB_DEBUG_WARNING) {
		fprintf(stderr, "   WARNING: ");
		va_start(ap, fmt);
		vfprintf(stderr, fmt, ap);
		va_end(ap);
	}
}

void _db_i_win(char *fmt, ...) {
	va_list ap;
	if(vcbDebugLevel>=VCB_DEBUG_INFO) {
		fprintf(stderr, "DEBUG INFO: ");
		va_start(ap, fmt);
		vfprintf(stderr, fmt, ap);
		va_end(ap);
	}
}

void _db_v_win(char *fmt, ...) {
	va_list ap;
	if(vcbDebugLevel>=VCB_DEBUG_INFO) {
		fprintf(stderr, "DEBUG INFO: ");
		va_start(ap, fmt);
		vfprintf(stderr, fmt, ap);
		va_end(ap);
	}
}

void _db_d_win(char *fmt, ...) {
	va_list ap;
	if(vcbDebugLevel>=VCB_DEBUG_INFO) {
		fprintf(stderr, "DEBUG INFO: ");
		va_start(ap, fmt);
		vfprintf(stderr, fmt, ap);
		va_end(ap);
	}
}

void _db_dl_win(int level, char *fmt, ...) {
	va_list ap;
	if(vcbDebugLevel>=level) {
		fprintf(stderr, "DEBUG INFO: ");
		va_start(ap, fmt);
		vfprintf(stderr, fmt, ap);
		va_end(ap);
	}
}
