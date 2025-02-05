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

/* Global define */
#define N_DAC_BUF 128    //Tamanho do Buffer do DAC

/* Global Variable */
uint32_t Dual_DAC_Value[N_DAC_BUF];
uint8_t buff_msg[2*N_DAC_BUF];

volatile int f_buf_half_0 = 0 ,f_buf_half_1 = 0;

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


/*********************************************************************
 * @fn      Dac_Init
 *
 * @brief   Initializes DAC collection.
 *
 * @return  none
 */
void Dual_Dac_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure={0};
	DAC_InitTypeDef DAC_InitType={0};

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	DAC_InitType.DAC_Trigger=DAC_Trigger_T4_TRGO;
	DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;
	DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;
	DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1,&DAC_InitType);
	DAC_Init(DAC_Channel_2,&DAC_InitType);

	DAC_Cmd(DAC_Channel_1, ENABLE);
	DAC_Cmd(DAC_Channel_2, ENABLE);

	DAC_DMACmd(DAC_Channel_1,ENABLE);
	DAC_DMACmd(DAC_Channel_2,ENABLE);

	DAC_SetDualChannelData(DAC_Align_12b_R, 0x00,0x00);
}

/*********************************************************************
 * @fn      DAC1_DMA_INIT
 *
 * @brief   Initializes DMA of DAC1 collection.
 *
 * @return  none
 */
void Dac_Dma_Init(void)
{
	DMA_InitTypeDef DMA_InitStructure={0};
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

	DMA_StructInit( &DMA_InitStructure);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(DAC->RD12BDHR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Dual_DAC_Value[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = N_DAC_BUF;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA2_Channel3, &DMA_InitStructure);

	//habilita iterrupções para transferência de DMA
	DMA_ITConfig(DMA2_Channel3,DMA_IT_TC,ENABLE);
	DMA_ITConfig(DMA2_Channel3,DMA_IT_HT,ENABLE);
	NVIC_EnableIRQ(DMA2_Channel3_IRQn);

	DMA_Cmd(DMA2_Channel3, ENABLE);
}

/*********************************************************************
 * @fn      Timer4_Init
 * @brief   Initializes TIM4
 * @return  none
 */
void Timer4_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure={0};
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = 3000-1;
	TIM_TimeBaseStructure.TIM_Prescaler =1-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_SelectOutputTrigger(TIM4, TIM_TRGOSource_Update);
	TIM_Cmd(TIM4, ENABLE);
}


//interrupção para DMA canal 3, Metade do buffer completo
void DMA2_Channel3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel3_IRQHandler(void) {


	//interrupção para metade do buffer
	if(DMA_GetITStatus(DMA2_IT_HT3)) {

		//marca a primeira metade do buffer completa
		f_buf_half_0 = 1;

		//limpa flag de interrupção
		DMA_ClearITPendingBit(DMA2_IT_HT3);
	}

	if(DMA_GetITStatus(DMA2_IT_TC3)) {

		//marca a segunda metade do buffer completa
		f_buf_half_1 = 1;

		//limpa flag de interrupção
		DMA_ClearITPendingBit(DMA2_IT_TC3);

	}

}




/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
	FRESULT fr;
	UINT bw;
	uint32_t index_b, kb;
	uint32_t s1,s2;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	SystemCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(115200);	
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
	printf("Exemplo com Cartao SD\r\n");
	GPIO_LEDB_Init();

	//configura e inicia DAC por DMA
	for(int i=0;i<N_DAC_BUF;i++)
	{
		Dual_DAC_Value[i]=0;
		//printf("0x%08x\r\n",Dual_DAC_Value[i]);
	}
	Dual_Dac_Init();
	Dac_Dma_Init();
	Timer4_Init();  //configura timer para frequencia de amostragem de 48kHz

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

	// abre o arquivo para lersinais do DAC
	fr = f_open(&fil, "sig.bin", FA_READ );	/* Read file */
	if (fr != FR_OK) {
		while(1) {
			LEDB_toggle();
			printf(" >Erro ao abrir arquivo\n\r");
			Delay_Ms(100);
		}
	}

	while(1)
	{
		//primeira metade
		if (f_buf_half_0) {
			LEDB_set(0);
			fr = f_read(&fil,buff_msg,N_DAC_BUF*2,&bw);
			index_b = 0;
			for (kb = 0; kb < bw; kb+=4) {
				s1 = (uint32_t)(buff_msg[kb+1]<<8)+(uint32_t)buff_msg[kb];
				s2 = (uint32_t)(buff_msg[kb+3]<<8)+(uint32_t)buff_msg[kb+2];
				Dual_DAC_Value[index_b++] = (s1<<16) | s2;
			}

			if (bw < N_DAC_BUF*2) {
				//volta arquivo para inicio do arquivo para completar metade do buffer
				f_lseek (&fil, 0);
				fr = f_read(&fil,buff_msg,N_DAC_BUF*2 - bw,&bw);
				for (kb = 0; kb < bw; kb+=4) {
					s1 = (uint32_t)(buff_msg[kb+1]<<8)+(uint32_t)buff_msg[kb];
					s2 = (uint32_t)(buff_msg[kb+3]<<8)+(uint32_t)buff_msg[kb+2];
					Dual_DAC_Value[index_b++] = (s1<<16) | s2;
				}
			}
			f_buf_half_0  = 0;
			LEDB_set(1);
		}

		//segunda metade
		if (f_buf_half_1) {
			LEDB_set(0);
			fr = f_read(&fil,buff_msg,N_DAC_BUF*2,&bw);
			index_b = 0;
			for (kb = 0; kb < bw; kb+=4) {
				s1 = (uint32_t)(buff_msg[kb+1]<<8)+(uint32_t)buff_msg[kb];
				s2 = (uint32_t)(buff_msg[kb+3]<<8)+(uint32_t)buff_msg[kb+2];
				Dual_DAC_Value[N_DAC_BUF/2+index_b++] = (s1<<16) | s2;
			}

			if (bw < N_DAC_BUF*2) {
				//volta arquivo para inicio do arquivo para completar metade do buffer
				f_lseek (&fil, 0);
				fr = f_read(&fil,buff_msg,N_DAC_BUF*2 - bw,&bw);
				for (kb = 0; kb < bw; kb+=4) {
					s1 = (uint32_t)(buff_msg[kb+1]<<8)+(uint32_t)buff_msg[kb];
					s2 = (uint32_t)(buff_msg[kb+3]<<8)+(uint32_t)buff_msg[kb+2];
					Dual_DAC_Value[N_DAC_BUF/2+index_b++] = (s1<<16) | s2;
				}

			}

			f_buf_half_1  = 0;
			LEDB_set(1);
		}

	}
}



