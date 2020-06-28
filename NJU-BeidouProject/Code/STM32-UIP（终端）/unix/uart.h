#ifndef _UART_H_
#define _UART_H_
#include "stm32f10x_conf.h"
#include "stm32f10x_it.h"

#define RX_MAX 128

extern int TX_CONT;
extern int UART4_RX_STA;
extern unsigned char UART4_RX_BUF[RX_MAX];

void UART_INIT(u32 bound);
unsigned int UART_WRITE(unsigned int len,unsigned char *data);
unsigned int UART_READ(unsigned int len,unsigned char *data);
void sendstring4(unsigned int len,char str[]);
void senddata(unsigned int len,unsigned char *data);
#endif
