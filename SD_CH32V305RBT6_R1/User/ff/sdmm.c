/*------------------------------------------------------------------------/
/  Foolproof MMCv3/SDv1/SDv2 (in SPI mode) control module
/-------------------------------------------------------------------------/
/
/  Copyright (C) 2019, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-------------------------------------------------------------------------/
  Features and Limitations:

  * Easy to Port Bit-banging SPI
    It uses only four GPIO pins. No complex peripheral needs to be used.

  * Platform Independent
    You need to modify only a few macros to control the GPIO port.

  * Low Speed
    The data transfer rate will be several times slower than hardware SPI.

  * No Media Change Detection
    Application program needs to perform a f_mount() after media change.

/-------------------------------------------------------------------------*/


//#include "ff.h"		/* Obtains integer types for FatFs */
#include "ff/diskio.h"	/* Common include file for FatFs and disk I/O layer */


/*-------------------------------------------------------------------------*/
/* Platform dependent macros and functions needed to be modified           */
/*-------------------------------------------------------------------------*/

#include "ch32v30x.h"			/* Include device specific declareation file here */
#include "sdio.h"

//#include "hw_spi\hw_spi.h"

#define dly_us(n)	Delay_Us(n)	/* Delay n microseconds */
#define	FORWARD(d)		/* Data in-time processing function (depends on the project) */

//#define	CS_H()		SD_CS_GPIO->BSHR = ( 1 << SD_CS_PIN )
//#define CS_L()		SD_CS_GPIO->BSHR = ( 1 << ( 16 + SD_CS_PIN ) )


static DSTATUS Stat = STA_NOINIT;	/* Disk status */

static BYTE CardType;			/* b0:MMC, b1:SDv1, b2:SDv2, b3:Block addressing */



/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE drv			/* Drive number (always 0) */
)
{
	if (drv) return STA_NOINIT;

	return Stat;
}



/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv		/* Physical drive nmuber (0) */
)
{

	if (drv) return RES_NOTRDY;

	if(SD_Init() != SD_OK) return STA_NOINIT;

	switch(SDCardInfo.CardType)
		{
		case SDIO_STD_CAPACITY_SD_CARD_V1_1:
			//printf("Card Type:SDSC V1.1\r\n");
			CardType = 1;
			break;
		case SDIO_STD_CAPACITY_SD_CARD_V2_0:
			CardType = 2;
			//printf("Card Type:SDSC V2.0\r\n");
			break;
		case SDIO_HIGH_CAPACITY_SD_CARD:
			CardType = 3;
			//printf("Card Type:SDHC V2.0\r\n");
			break;
		case SDIO_MULTIMEDIA_CARD:
			CardType = 0;
			//printf("Card Type:MMC Card\r\n");
			break;
		}
	Stat = RES_OK;
	return RES_OK;

}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE drv,			/* Physical drive nmuber (0) */
	BYTE *buff,			/* Pointer to the data buffer to store read data */
	LBA_t sector,		/* Start sector number (LBA) */
	UINT count			/* Sector count (1..128) */
)
{
	DWORD sect = (DWORD)sector;


	if (disk_status(drv) & STA_NOINIT) return RES_NOTRDY;
	//if (!(CardType & CT_BLOCK)) sect *= 512;	/* Convert LBA to byte address if needed */

	if (SD_ReadDisk((u8 *)buff,sect,count) == SD_OK ) {
		return RES_OK;
	}
	return RES_ERROR;


}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0) */
	const BYTE *buff,	/* Pointer to the data to be written */
	LBA_t sector,		/* Start sector number (LBA) */
	UINT count			/* Sector count (1..128) */
)
{
	DWORD sect = (DWORD)sector;


	if (disk_status(drv) & STA_NOINIT) return RES_NOTRDY;
	//if (!(CardType & CT_BLOCK)) sect *= 512;	/* Convert LBA to byte address if needed */

	if (SD_WriteDisk((u8 *)buff,sect,count) == SD_OK) {
		return RES_OK;
	}
	return RES_ERROR;

	return count ? RES_ERROR : RES_OK;
}


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;

	if (disk_status(drv) & STA_NOINIT) return RES_NOTRDY;	/* Check if card is in the socket */

	res = RES_ERROR;
	switch (ctrl) {
		case CTRL_SYNC :		/* Make sure that no pending write process */
			res = RES_OK;
			break;
		case GET_SECTOR_SIZE:
			//*(DWORD*)buff = 512;
			*(DWORD*)buff = SDCardInfo.CardBlockSize;
			break;

		case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
			*(LBA_t*)buff = ((u32)(SDCardInfo.CardCapacity>>20))/2;
			res = RES_OK;
			break;

		case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
			*(DWORD*)buff = 128;

			res = RES_OK;
			break;

		default:
			res = RES_PARERR;
	}

	//sd_deselect();

	return res;
}


