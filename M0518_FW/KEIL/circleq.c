#include <stdio.h>
#include <string.h>

#include "M0518.h"
#include "circleq.h"

/*
extern void init_uart_q(Queue_t* target_q_init);
extern void push(Queue_t* target_q_push ,uint8_t data);
extern uint8_t pop(Queue_t* target_q_pop);
extern uint8_t isEmpty(Queue_t* targeet_q_check);
*/

void init_q(volatile queue_t* target_q_init)
{
	target_q_init->head = 0;
	target_q_init->tail = 0;
	memset(&(target_q_init->buffer),0,sizeof(target_q_init->buffer));
}

void push(volatile queue_t* target_q_push ,uint8_t data)
{
	target_q_push->buffer[target_q_push->head] = data;
	
	target_q_push->head++;
	
	if(target_q_push->head>= MAX_BUF_SIZE){
		target_q_push->head = 0;
	}
}

uint32_t pop(volatile queue_t* target_q_pop)
{
	uint32_t pop_data;
	
	pop_data = target_q_pop->buffer[target_q_pop->tail];
	
	target_q_pop->tail++;
	
	if (target_q_pop->tail >= MAX_BUF_SIZE) {
    target_q_pop->tail = 0;
  }
	
	return pop_data;
}

uint32_t isEmpty(volatile queue_t* target_q_check)
{
	return target_q_check->head == target_q_check->tail;
}
