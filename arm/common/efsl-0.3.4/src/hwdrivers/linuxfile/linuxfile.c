/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : linuxfile.c                                                      *
* Description : This file contains the functions needed to use efs for        *
*               accessing files under linux. This interface is meant          *
*               to be used for debugging purposes.                            *
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
*                                                    (c)2004 Lennart Yseboodt *
*                                                    (c)2005 Michael De Nil   *
\*****************************************************************************/

/*****************************************************************************/
#include "linuxfile.h"
/*****************************************************************************/

/* ****************************************************************************  
 * short if_initInterface(hwInterface* file, char* fileName)
 * Description: This function should bring the hardware described in file in a
 * ready state to receive and retrieve data.
 * Return value: Return 0 on succes and -1 on failure.
*/
esint8 lf_init(void* LFI)
{
	eint32 sc;
	
	Fopen(&(((linuxFileInterface*)LFI)->imageFile),(eint8*)((linuxFileInterface*)LFI)->fileName);
	sc=getFileSize(((linuxFileInterface*)LFI)->imageFile);
	((linuxFileInterface*)LFI)->sectorCount=sc/512;
	return(0);
}
/*****************************************************************************/ 

/* ****************************************************************************  
 * short if_readBuf(hwInterface* file,unsigned long address,unsigned char* buf)
 * Description: This function should fill the characterpointer buf with 512 
 * bytes, offset by address*512 bytes. Adress is thus a LBA address.
 * Return value: Return 0 on success and -1 on failure.
*/
esint8 lf_readBuf(void* LFI,euint32 address,euint8* buf)
{
	/*printf("READ  %li\n",address);*/
	if(lf_setPos(LFI,address))return(-1);
	if( fread((void*)buf,512,1,((linuxFileInterface*)LFI)->imageFile) != 1) return(-1);
	return(0);
}
/*****************************************************************************/ 

/* ****************************************************************************  
 * short if_writeBuf(hwInterface* file,unsigned long address,unsigned char* buf)
 * Description: This function writes 512 bytes from uchar* buf to the hardware
 * disc described in file. The write offset should be address sectors of 512 bytes.
 * Return value: Return 0 on success and -1 on failure.
*/
esint8 lf_writeBuf(void* LFI,euint32 address,euint8* buf)
{
	/*printf("WRITE %li\n",address);*/
	if(lf_setPos(LFI,address))return(-1);
	if((fwrite((void*)buf,512,1,((linuxFileInterface*)LFI)->imageFile))!=1){
		perror("cf_writeBuf:");
		exit(-1);
	}
	fflush(((linuxFileInterface*)LFI)->imageFile);
	return(0);
}
/*****************************************************************************/ 

esint8 lf_ioctl(void* LFI,euint16 ctl,void* data)
{
	switch(ctl){
		case IOCTL_NOP:
			return(0);
			break;
		case IOCTL_SECTORCOUNT:
			*((euint32*)data)=getFileSize(((linuxFileInterface*)LFI)->imageFile)/512;
			return(0);
			break;
		default:
			return(-1);
			break;
	}
}

/* ****************************************************************************  
 * short if_setPos(hwInterface* file,unsigned long address)
 * Description: This function may or may not be required. It would set the write
 * or read buffer offset by 512*address bytes from the beginning of the disc.
 * Return value: Return 0 on success and -1 on failure.
*/
esint8 lf_setPos(void* LFI,euint32 address)
{
	if(address>(((linuxFileInterface*)LFI)->sectorCount-1)){
		DBG((TXT("Illegal address\n")));
		exit(-1);
	}
	if((fseek(((linuxFileInterface*)LFI)->imageFile,512*address,SEEK_SET))!=0){
		perror("cf_setPos:");
		exit(-1);
	}
	return(0);
}
/*****************************************************************************/ 

