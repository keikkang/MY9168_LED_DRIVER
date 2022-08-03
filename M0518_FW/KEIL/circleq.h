#ifndef __CIRCLEQ_H__
#define __CIRCLEQ_H__

#define MAX_BUF_SIZE   32

typedef struct _queue_t_
{
		uint8_t head;
	  uint8_t tail;
	  uint8_t buffer[MAX_BUF_SIZE];
}queue_t;

typedef struct _uart_t_
{
		uint8_t rx_data[MAX_BUF_SIZE];
    uint8_t tx_data[MAX_BUF_SIZE];
}uart_t;

extern void init_q(volatile queue_t* target_q_init);
extern void push(volatile queue_t* target_q_push ,uint8_t data);
extern uint32_t pop(volatile queue_t* target_q_pop);
extern uint32_t isEmpty(volatile queue_t* target_q_check);



#endif
