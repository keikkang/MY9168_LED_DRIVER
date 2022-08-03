/****************************************************************************
 * @file     main.c
 * @version  V3.00
 * $Revision: 3 $
 * $Date: 15/06/04 11:19a $
 * @brief    Transmit and receive data from PC terminal through RS232 interface.
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "M0518.h"
#include "circleq.h"
#include "my9168.h"


#define PLL_CLOCK   50000000

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

uint8_t test_string0[] = "THIS_IS_UART0\n";
uint8_t test_string1[] = "THIS_IS_UART1\n";
uint8_t test_string2[] = "THIS_IS_UART2\n";

volatile queue_t uart0_q;

volatile queue_t uart1_q;
volatile queue_t uart2_q;

volatile uart_t  uart0_d = {0};
volatile uart_t  uart1_d = {0};
volatile uart_t  uart2_d = {0};

void UART02_IRQHandler(void);
void UART1_IRQHandler(void);
void check_uart_buffer(volatile queue_t* q_channel, volatile uart_t* uart_data_channel, uint8_t* handler, MY9168_t* my9168_channel);

/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    
    /* Enable external XTAL 12MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for external XTAL clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);
	
	  /* Select HXT as the clock source of HCLK. HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HXT, CLK_CLKDIV_HCLK(1));
	
	  CLK_SetModuleClock(SPI0_MODULE, CLK_CLKSEL1_SPI0_S_HCLK, MODULE_NoMsk);

    /* Set core clock as PLL_CLOCK from PLL */
    //CLK_SetCoreClock(PLL_CLOCK);

    /* Enable UART module clock */
		CLK_EnableModuleClock(SPI0_MODULE);
    CLK_EnableModuleClock(UART0_MODULE);
		CLK_EnableModuleClock(UART1_MODULE);
		CLK_EnableModuleClock(UART2_MODULE);

    /* Select UART module clock source */
	
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));
		CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));
		CLK_SetModuleClock(UART2_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));
		
		SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set GPB multi-function pins for UART0 RXD(PB.0) and TXD(PB.1) */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk| SYS_GPB_MFP_PB4_Msk | SYS_GPB_MFP_PB5_Msk );
    SYS->GPB_MFP |= SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD | SYS_GPB_MFP_PB4_UART1_RXD | SYS_GPB_MFP_PB5_UART1_TXD;
										
		SYS->ALT_MFP &= ~(SYS_ALT_MFP3_PD14_Msk  | SYS_ALT_MFP3_PD15_Msk );
		SYS->GPD_MFP |= SYS_GPD_MFP_PD14_UART2_RXD | SYS_GPD_MFP_PD15_UART2_TXD ;
		
		/* Setup SPI0 multi-function pins */
    SYS->GPC_MFP &= ~( SYS_GPC_MFP_PC1_Msk | SYS_GPC_MFP_PC3_Msk);
		SYS->GPC_MFP |= ( SYS_GPC_MFP_PC1_SPI0_CLK | SYS_GPC_MFP_PC3_SPI0_MOSI0);
    SYS->ALT_MFP &= ~( SYS_ALT_MFP_PC1_Msk |  SYS_ALT_MFP_PC3_Msk);
		SYS->ALT_MFP |= ( SYS_ALT_MFP_PC1_SPI0_CLK | SYS_ALT_MFP_PC3_SPI0_MOSI0);
}

void UART_Init(uint8_t channel)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    switch(channel)
		{
			case 0: 
				 SYS_ResetModule(UART0_RST);
				 UART_Open(UART0, 115200);
			break;
			
			case 1:
				 SYS_ResetModule(UART1_RST);
				 UART_Open(UART1, 115200);
			break;
			
			case 2:
				 SYS_ResetModule(UART2_RST);
				 UART_Open(UART2, 115200);
			break;
			
			default:
				break;
		}		

}

/*---------------------------------------------------------------------------------------------------------*/
/* UART Test Sample                                                                                        */
/* Test Item                                                                                               */
/* It sends the received data to HyperTerminal.                                                            */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int main(void)
{
		uint32_t i;
	
		MY9168_t my9168_1;
	
	  my9168_1.data = 0;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();
	
	  /*call back funtion init*/
    UART_Init(0); //UART0 INIT
	  UART_Init(1); //UART1 INIT
 
    init_q(&uart0_q); //UART0 CIRCLEQ INIT
	  init_q(&uart1_q); //UART1 CIRCLEQ INIT
	  printf("\n*********************************\n");   
	  printf("*********MY9168_DRIVER_**********\n");
    printf("*********************************\n");
		
		
	  UART_Write(UART0, test_string0, sizeof(test_string0));
		UART_Write(UART1, test_string1, sizeof(test_string1));
	  UART_EnableInt(UART0, (UART_IER_RDA_IEN_Msk));
		UART_EnableInt(UART1, (UART_IER_RDA_IEN_Msk));
		
		
		#ifdef UART_CH_DEBUG_MODE
		UART_Init(2); //UART2 INIT
		init_q(&uart2_q); //UART2 CIRCLEQ INIT
		UART_Write(UART2, test_string2, sizeof(test_string2));
	  UART_EnableInt(UART2, (UART_IER_RDA_IEN_Msk));
		#endif
		
		
    /* LED DRIVE INITALIZE */


    while(1)
		{
			/*check uart_q buf*/
			check_uart_buffer(&uart0_q, &uart0_d, "UART0", &my9168_1);
			
			if(my9168_1.data){
				set_led_data(my9168_1.data);
				my9168_1.data = 0;
			}
			/*
			check_uart_buffer(&uart1_q, &uart1_d, "UART1");
			check_uart_buffer(&uart2_q, &uart2_d, "UART2");
			*/
			
			DEBUG_LED ^= 1;
		
			for(i=0;i<1000000;i++) //DEFAULT 100000
			{
   			__NOP();
			}	

		}

}

/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle UART Channel 0 interrupt event                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void UART02_IRQHandler(void)
{
	while(UART_IS_RX_READY(UART0))
	{
			push(&uart0_q, UART0->RBR);
	}
	
	while(UART_IS_RX_READY(UART2))
	{
			push(&uart2_q, UART2->RBR);
	}
}

void UART1_IRQHandler(void)
{
	while(UART_IS_RX_READY(UART1))
	{
			push(&uart1_q, UART1->RBR);
	}
}

void check_uart_buffer(volatile queue_t* q_channel, volatile uart_t* uart_data_channel, uint8_t* handler, MY9168_t* my9168_channel)
{
	static uint8_t num = 0;
	uint16_t data_buf;
	
	if(isEmpty(q_channel)== 0)
	{
		//uart1_d.rx_data[j] = pop(&uart1_q);
		uart_data_channel->rx_data[num] = pop(q_channel);
		num++;		

		if(num>1)
		{
				while(!(num==1)){
					uart_data_channel->rx_data[num--] = 0; //trim dummy data;
				}
			
			data_buf = (uint16_t)( (uart_data_channel->rx_data[1]<<8) | uart_data_channel->rx_data[0] );
			my9168_channel->data = data_buf;
			printf("led_data : %x \n", data_buf );
			num=0;
		}
		
		#ifdef UART_CH_DEBUG_MODE
		if(i >= MAX_BUF_SIZE)
		{
			printf("%s_q is full!\n", handler);
			
			
			for(num=0;num<MAX_BUF_SIZE;num++)
			{
				uart_data_channel->rx_data[num] = 0;
			}
			
			num = 0;
		}	
		#endif
	}
}



