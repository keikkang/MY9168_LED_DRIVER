#include <stdio.h>
#include "M0518.h"
#include "my9168.h"
#include "circleq.h"

void my9168_init(SPI_T* f_spi_ch )
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init SPI                                                                                                */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Configure SPI0 as a master, clock idle low, 32-bit transaction, drive output on falling clock edge and latch input on rising edge. */
    /* Set IP clock divider. SPI clock rate = 2MHz */

		SPI_Open(f_spi_ch, SPI_MASTER, SPI_MODE_0, 16, 2000000);

    if(f_spi_ch == SPI0)
		{
			NVIC_DisableIRQ(SPI0_IRQn);
			
		  GPIO_SetMode(PC, BIT0, GPIO_PMD_OUTPUT);
		  GPIO_SetMode(PC, BIT2, GPIO_PMD_OUTPUT);
	    GPIO_SetMode(PA, BIT14, GPIO_PMD_OUTPUT); //for debug io led
	
	    CHIP_ENABLE = 1; //ENABLE
	    LATCH = 0; //LATCH
    }
		
	  SPI_EnableFIFO(f_spi_ch, 1, 1);

    /* Disable TX FIFO threshold interrupt and RX FIFO time-out interrupt */
    SPI_DisableInt(f_spi_ch, SPI_FIFO_TX_INT_MASK | SPI_FIFO_TIMEOUT_INT_MASK);

}

void set_led_data(uint16_t f_data)
{
			CHIP_ENABLE = 1;
			DEBUG_LED ^= 1;
			
			SPI_WRITE_TX(SPI0, f_data);
			SPI_TRIGGER(SPI0);
			while(SPI_IS_BUSY(SPI0));

			__NOP();
			__NOP();
			__NOP();
			
			LATCH = 1; //FOR LATCH
	
			__NOP();
			__NOP();
			__NOP();
			
			LATCH = 0;
		
			CHIP_ENABLE = 0;
}

void set_brigtness(uint32_t timer_val, MY9168_t* my9168_channel)
{
	if(timer_val%10 <= my9168_channel->brightness){
		CHIP_ENABLE = 0;
	}
	else{
		CHIP_ENABLE = 1;
	}
}



