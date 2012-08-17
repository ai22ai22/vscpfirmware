/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : sd.c                                                             *
* Revision : Initial developement                                             *
* Description : This file contains the functions needed to use efs for        *
*               accessing files on an SD-card.                                *
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
*                                                    (c)2005 Michael De Nil   *
*                                                    (c)2005 Lennart Yseboodt *
\*****************************************************************************/

/*****************************************************************************/
#include "sd.h"
/*****************************************************************************/

esint8  sd_Init(SdSpiProtocol *ssp)
{
	esint16 i;
	euint8 resp;
	
	/* Init HW-Layer */
	ssp->spiHwInit(ssp->spiHwInterface);
	
	/* Try to send reset command up to 512 times */
	i=512;
	do{
		sd_Command(ssp,0, 0, 0);
		resp=sd_Resp8b(ssp);
	}
	while(resp!=1 && i--);
	
	/* If response was not one, find out what went wrong & quit */
	if(resp!=1){
		if(resp==0xff){
			return(-1);
		}
		else{
			sd_Resp8bError(ssp,resp);
			return(-2);
		}
	}

	/* Wait till card is ready initializing (returns 0 on CMD1) */
	/* Try up to 32000 times. */
	i=32000;
	do{
		sd_Command(ssp,1, 0, 0);
		
		resp=sd_Resp8b(ssp);
		if(resp!=0)
			sd_Resp8bError(ssp,resp);
	}
	while(resp==1 && i--);
	
	/* If the card is not ready by now, return an error */
	if(resp!=0){
		sd_Resp8bError(ssp,resp);
		return(-3);
	}
	
	/* Request state, should be 0 */
	if(sd_State(ssp)<0){
		DBG((TXT("Card didn't return the ready state, breaking up...\n")));
		return(-4);
	}
	
	return(0);
}
/*****************************************************************************/

void sd_Command(SdSpiProtocol *ssp,euint8 cmd, euint16 paramx, euint16 paramy)
{
        ssp->spiSendByte(ssp->spiHwInterface,0xff);

	ssp->spiSendByte(ssp->spiHwInterface,0x40 | cmd);
	ssp->spiSendByte(ssp->spiHwInterface,(euint8) (paramx >> 8));/* MSB of x */
	ssp->spiSendByte(ssp->spiHwInterface,(euint8) (paramx)); 	 /* LSB of x */
	ssp->spiSendByte(ssp->spiHwInterface,(euint8) (paramy >> 8));/* MSB of y */
	ssp->spiSendByte(ssp->spiHwInterface,(euint8) (paramy)); 	 /* LSB of y */

	/* Checksum (should be only valid for first command (0) */
	ssp->spiSendByte(ssp->spiHwInterface,0x95);

	/* Eat response, we don't care for now */
	ssp->spiSendByte(ssp->spiHwInterface,0xff);
}
/*****************************************************************************/

euint8 sd_Resp8b(SdSpiProtocol *ssp)
{
	euint8 i;
	euint8 resp;
	
	/* Respone will come after 1 - 8 pings */
	for(i=0;i<8;i++){
		resp = ssp->spiSendByte(ssp->spiHwInterface,0xff);
		if(resp != 0xff)
			return(resp);
	}
		
	return(resp);
}
/*****************************************************************************/

euint16 sd_Resp16b(SdSpiProtocol *ssp)
{
	euint16 resp;
	
	resp = ( sd_Resp8b(ssp) << 8 ) & 0xff00;
	resp |= ssp->spiSendByte(ssp->spiHwInterface,0xff);
	
	return(resp);
}
/*****************************************************************************/

void sd_Resp8bError(SdSpiProtocol *ssp,euint8 value)
{
	switch(value)
	{
		case 0x40:
			DBG((TXT("Argument out of bounds.\n\r")));
			break;
		case 0x20:
			DBG((TXT("Address out of bounds.\n\r")));
			break;
		case 0x10:
			DBG((TXT("Error during erase sequence.\n\r")));
			break;
		case 0x08:
			DBG((TXT("CRC failed.\n\r")));
			break;
		case 0x04:
			DBG((TXT("Illegal command.\n\r")));
			break;
		case 0x02:
			DBG((TXT("Erase reset (see SanDisk docs p5-13).\n\r")));
			break;
		case 0x01:
			DBG((TXT("Card is initialising.\n\r")));
			break;
		default:
			DBG((TXT("Unknown error 0x%x (see SanDisk docs p5-13).\n\r"),value));
			break;
	}
}
/*****************************************************************************/

esint8 sd_State(SdSpiProtocol *ssp)
{
	eint16 value;
	
	sd_Command(ssp,13, 0, 0);
	value=sd_Resp16b(ssp);
	
	switch(value)
	{
		case 0x0000:
			return(1);
			break;
		case 0x0001:
			DBG((TXT("Card is Locked.\n")));
			break;
		case 0x0002:
			DBG((TXT("WP Erase Skip, Lock/Unlock Cmd Failed.\n")));
			break;
		case 0x0004:
			DBG((TXT("General / Unknown error -- card broken?.\n")));
			break;
		case 0x0008:
			DBG((TXT("Internal card controller error.\n")));
			break;
		case 0x0010:
			DBG((TXT("Card internal ECC was applied, but failed to correct the data.\n")));
			break;
		case 0x0020:
			DBG((TXT("Write protect violation.\n")));
			break;
		case 0x0040:
			DBG((TXT("An invalid selection, sectors for erase.\n")));
			break;
		case 0x0080:
			DBG((TXT("Out of Range, CSD_Overwrite.\n")));
			break;
		default:
			if(value>0x00FF)
				sd_Resp8bError(ssp,(euint8) (value>>8));
			else
				DBG((TXT("Unknown error: 0x%x (see SanDisk docs p5-14).\n"),value));
			break;
	}
	return(-1);
}
/*****************************************************************************/

/* ****************************************************************************
 * WAIT ?? -- FIXME
 * CMDWRITE
 * WAIT
 * CARD RESP
 * WAIT
 * DATA BLOCK OUT
 *      START BLOCK
 *      DATA
 *      CHKS (2B)
 * BUSY...
 */

esint8 sd_writeSector(SdSpiProtocol *ssp,euint32 address, euint8* buf)
{
	euint32 place;
	euint16 i;
	euint16 t=0;
	
	/*DBG((TXT("Trying to write %u to sector %u.\n"),(void *)&buf,address));*/
	place=512*address;
	sd_Command(ssp,CMDWRITE, (euint16) (place >> 16), (euint16) place);

	sd_Resp8b(ssp); /* Card response */

	ssp->spiSendByte(ssp->spiHwInterface,0xfe); /* Start block */
	for(i=0;i<512;i++)
		ssp->spiSendByte(ssp->spiHwInterface,buf[i]); /* Send data */
	ssp->spiSendByte(ssp->spiHwInterface,0xff); /* Checksum part 1 */
	ssp->spiSendByte(ssp->spiHwInterface,0xff); /* Checksum part 2 */

	ssp->spiSendByte(ssp->spiHwInterface,0xff);

	while(ssp->spiSendByte(ssp->spiHwInterface,0xff)!=0xff){
		t++;
		/* Removed NOP */
	}
	/*DBG((TXT("Nopp'ed %u times.\n"),t));*/

	return(0);
}
/*****************************************************************************/

/* ****************************************************************************
 * WAIT ?? -- FIXME
 * CMDCMD
 * WAIT
 * CARD RESP
 * WAIT
 * DATA BLOCK IN
 * 		START BLOCK
 * 		DATA
 * 		CHKS (2B)
 */

esint8 sd_readSector(SdSpiProtocol *ssp,euint32 address, euint8* buf)
{
	euint8 cardresp;
	euint8 firstblock;
	euint8 c;
	euint16 fb_timeout=0xffff;
	euint32 i;
	euint32 place;
	const euint16 len=512; /* FIXME */

	/*DBG((TXT("sd_readSector::Trying to read sector %u and store it at %p.\n"),address,&buf[0]));*/
	place=512*address;
	sd_Command(ssp,CMDREAD, (euint16) (place >> 16), (euint16) place);
	
	cardresp=sd_Resp8b(ssp); /* Card response */

	/* Wait for startblock */
	do
		firstblock=sd_Resp8b(ssp);
	while(firstblock==0xff && fb_timeout--);

	if(cardresp!=0x00 || firstblock!=0xfe){
		sd_Resp8bError(ssp,firstblock);
		return(-1);
	}
	
	for(i=0;i<512;i++){
		c = ssp->spiSendByte(ssp->spiHwInterface,0xff);
		if(i<len)
			buf[i] = c;
	}

	/* Checksum (2 byte) - ignore for now */
	ssp->spiSendByte(ssp->spiHwInterface,0xff);
	ssp->spiSendByte(ssp->spiHwInterface,0xff);

	return(0);
}
/*****************************************************************************/

esint8 sd_ioctl(SdSpiProtocol* ssp,euint16 ctl,void* data)
{
	return(0);
}
