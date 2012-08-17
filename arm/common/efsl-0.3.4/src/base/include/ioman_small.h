/*****************************************************************************\
*                     libfat - General purpose FAT library                    *
*                      ----------------------------------                     *
*                                                                             *
* Filename : ioman.h                                                          *
* Description : The IO Manager receives all requests for sectors in a central *
*               allowing it to make smart decision regarding caching.         *
*               The IOMAN_NUMBUFFER parameter determines how many sectors     *
*               ioman can cache. ioman also supports overallocating and       *
*               backtracking sectors.                                         *
*               This is the small version of IOMan, for systems with limited  *
*               resources. It features only one fixed buffer, and has         *
*               simplified operation                                          *
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
*                                                    (c)2005 Lennart Yseboodt *
*                                                    (c)2004 Michael De Nil   *
\*****************************************************************************/

#include "interface.h"
#include "error.h"
#include "plibc.h"
#include "debug.h"
#include "types.h"
#include "config.h"

#define IOMAN_STATUS_ATTR_VALIDDATA  0
#define IOMAN_STATUS_ATTR_USERBUFFER 1
#define IOMAN_STATUS_ATTR_WRITE      2

#define IOM_MODE_READONLY  1
#define IOM_MODE_READWRITE 2
#define IOM_MODE_EXP_REQ   4

struct IOManStack{
	euint32 sector;
	euint8  status;
};
typedef struct IOManStack IOManStack;

struct IOManager{
	Interface *iface;
	
	euint8 bufptr[512];
		
	IOMAN_ERR_EUINT8
		
	euint32 sector;
	euint8  status;
	euint8  itptr;
	IOManStack stack;
};
typedef struct IOManager IOManager;

#define IOBJ ioman

#define ioman_isValid() ioman_getAttr(IOBJ,IOMAN_STATUS_ATTR_VALIDDATA)
#define ioman_isUserBuf() ioman_getAttr(IOBJ,IOMAN_STATUS_ATTR_USERBUFFER)
#define ioman_isWritable() ioman_getAttr(IOBJ,IOMAN_STATUS_ATTR_WRITE)

#define ioman_setValid() ioman_setAttr(IOBJ,IOMAN_STATUS_ATTR_VALIDDATA,1)
#define ioman_setUserBuf() ioman_setAttr(IOBJ,IOMAN_STATUS_ATTR_USERBUFFER,1)
#define ioman_setWritable() ioman_setAttr(IOBJ,IOMAN_STATUS_ATTR_WRITE,1)

#define ioman_setNotValid() ioman_setAttr(IOBJ,IOMAN_STATUS_ATTR_VALIDDATA,0)
#define ioman_setNotUserBuf() ioman_setAttr(IOBJ,IOMAN_STATUS_ATTR_USERBUFFER,0)
#define ioman_setNotWritable() ioman_setAttr(IOBJ,IOMAN_STATUS_ATTR_WRITE,0)

#define ioman_isReqRo(mode)  ((mode)&(IOM_MODE_READONLY))
#define ioman_isReqRw(mode)  ((mode)&(IOM_MODE_READWRITE))
#define ioman_isReqExp(mode) ((mode)&(IOM_MODE_EXP_REQ))

esint8 ioman_init(IOManager *ioman, Interface *iface, euint8* bufferarea);
void ioman_reset(IOManager *ioman);
void ioman_setAttr(IOManager *ioman,euint8 attribute,euint8 val);
euint8 ioman_getAttr(IOManager *ioman,euint8 attribute);
