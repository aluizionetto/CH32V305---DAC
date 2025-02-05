/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 USART Print debugging routine:
 USART1_Tx(PA9).
 This example demonstrates using USART1(PA9) as a print debug port output.

 */

#include "debug.h"

#include "string.h"
#include "ff/ff.h"		/* Declarations of FatFs API */
#include "ff/diskio.h"
#include "ff/sdio.h"
#include <stdio.h>


/* Global typedef */

/* Global define */

/* Global Variable */

/*********************************************************************
 * @fn      show_sdcard_info
 *
 * @brief   SD Card information.
 *
 * @return  none
 */
void show_sdcard_info(void)
{
	switch(SDCardInfo.CardType)
	{
	case SDIO_STD_CAPACITY_SD_CARD_V1_1:printf("Card Type:SDSC V1.1\r\n");break;
	case SDIO_STD_CAPACITY_SD_CARD_V2_0:printf("Card Type:SDSC V2.0\r\n");break;
	case SDIO_HIGH_CAPACITY_SD_CARD:printf("Card Type:SDHC V2.0\r\n");break;
	case SDIO_MULTIMEDIA_CARD:printf("Card Type:MMC Card\r\n");break;
	}
	printf("Card ManufacturerID:%d\r\n",SDCardInfo.SD_cid.ManufacturerID);
	printf("Card RCA:%d\r\n",SDCardInfo.RCA);
	printf("Card Capacity:%d MB\r\n",(u32)(SDCardInfo.CardCapacity>>20));
	printf("Card BlockSize:%d\r\n\r\n",SDCardInfo.CardBlockSize);
}



u8 buf[512];
u8 Readbuf[512];


void GPIO_LEDB_Init(void) {
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//configura pino A3 como Saida
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

}

static int led_s = 0;
void LEDB_set (int s) {
	if (s){
		GPIO_WriteBit(GPIOA,GPIO_Pin_3,Bit_SET);
		led_s = 1;
	}
	else{
		GPIO_WriteBit(GPIOA,GPIO_Pin_3,Bit_RESET);
		led_s =0;
	}
}
void LEDB_toggle(void) {
	if(led_s) {
		GPIO_WriteBit(GPIOA,GPIO_Pin_3,Bit_RESET);
		led_s = 0;
	}
	else {
		GPIO_WriteBit(GPIOA,GPIO_Pin_3,Bit_SET);
		led_s = 1;
	}
}

char buff_msg[128];

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
	uint16_t count = 0;
	FRESULT fr;
	UINT bw;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	SystemCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(115200);	
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
	printf("Exemplo com Cartao SD\r\n");
	GPIO_LEDB_Init();


	//Fatfs object
	FATFS FatFs;
	//File object
	FIL fil;

	//tenta montar unda SD
	if (f_mount(&FatFs, "", 1) == FR_OK) {
		//Mounted OK, turn on RED LED
		printf("Mount OK SD\r\n");
		show_sdcard_info();
	}else {
		while(1) {
			LEDB_toggle();
			printf(" >Erro ao montar Unidade\n\r");
			Delay_Ms(1000);
		}
	}

	while(1)
	{
		LEDB_set(0);

		printf("Teste Led, %d\r\n", count++);
#if 0 //1 -> escrita, 0-> leitura
		sprintf(buff_msg,"Registro %d \n",count);
		fr = f_open(&fil, "REGISTRO.txt", FA_WRITE | FA_OPEN_APPEND);	/* Create a file */
		if (fr == FR_OK) {
			f_write(&fil, buff_msg, strlen(buff_msg), &bw);	/* Write data to the file */
			fr = f_close(&fil);							/* Close the file */
			printf("erro: %d, lm: %d, lg: %d\n\r",fr,strlen(buff_msg),bw);
			if (fr == FR_OK && bw == strlen(buff_msg)) {		/* Arquivo Escrito com sucesso */
				printf(" >OK\n\r");
			}
			else
			{
				printf(" >ERRO \n\r");
			}
		}
		else
		{
			printf(" >Erro ao abrir arquivo\n\r");
		}
#else
		fr = f_open(&fil, "REGISTRO.txt", FA_READ );	/* Read file */
		if (fr == FR_OK) {
			do {
				fr = f_read(&fil,buff_msg,64,&bw);
				for (int kb = 0; kb< bw; kb++) {

					while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
					USART_SendData(USART1, buff_msg[kb]);
				}

			}while (bw == 64 && (fr == FR_OK) );

			f_close(&fil);
		}
		else
		{
			printf(" >Erro ao abrir arquivo\n\r");
		}


#endif
		LEDB_set(1);
		Delay_Ms(1000);
	}


}

