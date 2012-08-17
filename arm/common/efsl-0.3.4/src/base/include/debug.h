/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          ---------------------------------------------------------          *
*                                                                             *
* Filename : debug.h                                                          *
* Description : Headerfile for debug.c                                        *
*                                                                             *
* This library is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU Lesser General Public                  *
* License as published by the Free Software Foundation; either                *
* version 2.1 of the License, or (at your option) any later version.          *
*                                                                             *
* This library is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           *
* Lesser General Public License for more details.                             *
*                                                                             *
*                                                    (c)2005 Lennart Yseboody *
*                                                    (c)2005 Michael De Nil   *
******************************************************************************/

#ifndef __DEBUG_H__
#define __DEBUG_H__

/*****************************************************************************/
#include "types.h"
#include "config.h"
/*****************************************************************************/

#ifndef DEBUG
	#define TXT(x) ;
	#define DBG(x) ;
	#define FUNC_IN(x) ;
	#define FUNC_OUT(x) ;
#endif

#ifdef DEBUG

	#ifdef HW_ENDPOINT_ATMEGA128_SD
		#include <avr/io.h>
		#include <compat/ina90.h>
		#include <avr/pgmspace.h>
		#include <stdio.h>

		#define TXT(x) PSTR(x)
		#define DBG(x) debug x
		#define FUNC_IN(x) ;
		#define FUNC_OUT(x) ;
	#endif

        #ifdef HW_ENDPOINT_LINUX
		#include <stdio.h>
		#include <stdarg.h>
	
		#define TXT(x) x
		#define DBG(x) debug x
		#define FUNC_IN(x) debug_funcin(x)
		#define FUNC_OUT(x) debug_funcout(x)
	#endif

	#ifdef HW_ENDPOINT_DSP_TI6713_SD
		#include <stdio.h>
		#include <stdarg.h>

		#define TXT(x) x
		#define DBG(x) printf x
		#define FUNC_IN(x) ;
		#define FUNC_OUT(x) ;
	#endif

	#ifdef HW_ENDPOINT_IAR_ARM_2K
		#include <stdio.h>
		#include <stdarg.h>
	
		#define TXT(x) x
		#define DBG(x) printf x
		#define FUNC_IN(x) ;
		#define FUNC_OUT(x) ;
        #endif

        void debug(const eint8 *format, ...);
	void debug_init();
	void debug_end();

	#ifdef HW_ENDPOINT_LINUX
	FILE* debugfile;
	volatile euint8 tw;
	void debug_funcin(const eint8 *format, ...);
	void debug_funcout(const eint8 *format, ...);
	euint8 debug_getByte();
	euint8 debug_getString(euint8 *data,euint16 length);
	#endif

	#ifdef HW_ENDPOINT_ATMEGA128_SD
	void debug_initUART(euint16 baudrate );
	void debug_sendByte(euint8 data );
	#endif

        #ifdef HW_ENDPOINT_IAR_ARM_2K
	void debug_initUART(euint16 baudrate );
	void debug_sendByte(euint8 data );
        #endif
#endif

#endif
