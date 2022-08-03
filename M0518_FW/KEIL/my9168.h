#ifndef __MY9168_H__
#define __MY9168_H__

#include "circleq.h"

#define CHIP_ENABLE PC0
#define LATCH       PC2
#define DEBUG_LED   PA14
#define LED_DATA_SIZE 16
#define LED_DRVIER_NUM  2

typedef struct _MY9168_{
	SPI_T* spi_ch;
	uint16_t data; //get data from uart channel
}MY9168_t;

extern void my9168_init(SPI_T* f_spi_ch );
extern void set_led_data(uint16_t f_data);


#endif
