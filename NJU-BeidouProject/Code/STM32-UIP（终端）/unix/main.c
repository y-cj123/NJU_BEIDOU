/*
 * Copyright (c) 2001, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Adam Dunkels.
 * 4. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: main.c,v 1.16 2006/06/11 21:55:03 adam Exp $
 *
 */


#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "packet.h"
#include "string.h"
#include "usart.h"
#include "stm32f10x_conf.h"
#include "spi.h"
#include "enc28j60.h"
#include "timer.h"
#include "uart.h"
#include "uart5.h"
#include "BD_1.h"
#include "usart.h"
#include "sim900a.h"
#include "delay.h"

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */

struct timer BD_send_timer;//修改 BD_send_timer为全局变量

unsigned char recieve_data[100];
unsigned char test_data[10] = "hello\r\n";
int online_flag;
u8 gprs_state[]={0x0D,0x0A,0x5B,0x55,0x53,0x52,0x2D,0x37,0x53,0x34,0x20,0x56,0x32,0x5D,0x0D,0x0A};
int gprs_on=1;
int bd_on=0;
u8 ip[20]="114.212.112.36";
u8 por[5]="8086";
int ask_count;
//#ifdef CLIENT
/*
* Here is a simple example of how to use the fifo
* 串口接收数据处理函数
*/
void UART_DATA_PROCESS()
{

//将串口读到的数据放到临时缓存temp中
	unsigned char temp[300];
	int temp_len;
	int momo;
	temp_len=UART4_RX_STA;
	if(temp_len>0)
	{
		printf("收到报文长度为%d的数据:\r\n",temp_len);
		for(momo=0;momo<temp_len;momo++)
		{
			temp[momo]=UART4_RX_BUF[momo];
			printf("%02X ",temp[momo]);
		}
		bd_on=1;
		printf("\r\n------------------------------------------------\r\n");
		UART4_RX_STA=0; 
    //对收到的数据进行解析，去北斗封装，将电文内容放在recieve_data中，并返回电文长度
		temp_len = BD_read(temp,recieve_data);
    //将电文转交给网口部分处理
		if(temp_len>0){
		packet_send(temp_len,recieve_data);
		}
		
	}
}

int uart5_DATA_PROCESS()
{
	unsigned char temp[300];
	int len;	
	int i;
	int cnt=0;
	if(UART5_RX_STA&0X8000)		//接收到一次数据了
	{ 
		UART5_RX_BUF[UART5_RX_STA&0X7FFF]=0;//添加结束符
		len=UART5_RX_STA&0X7FFF;
		gprs_on=1;
		for(i=0;i<len;i++)
		{
			temp[i]=UART5_RX_BUF[i];
			printf("%02X ",temp[i]);
			
			if(temp[i]==gprs_state[i]){
					++cnt;
					if(cnt==16&&bd_on==0){
						printf("\r\n------------------------\r\n");
						printf("|   4G通信模块已接入    |");
						printf("\r\n------------------------\r\n");
						UART5_RX_STA=0;
						for(i=0;i<len;i++)
						{
							UART5_RX_BUF[i]=0;					
						}
						return 1;
					}
			}
		}
		printf("\r\n------------------------------------------------\r\n");
		packet_send(len,temp);
		UART5_RX_STA=0;
		return 0;
	} 
}
//#endif
/*---------------------------------------------------------------------------*/
int main(void)
{
  int i;
  uip_ipaddr_t ipaddr;
  struct timer periodic_timer, arp_timer,event_timer,gprs_timer;
	//struct timer periodic_timer, arp_timer,event_timer,BD_send_timer,gprs_timer;
	unsigned char buf[16];
	int online_judge_flag=0;
	u8 ADDR[3] = {0x04, 0xE3, 0xC9};//目的北斗终端机地址
  online_flag=0;
	BD_process_STA=0;
	ask_count=0;


//	GPIO_InitTypeDef GPIO_InitStructure;
	UART_INIT(9600);
	usart_init(9600);
	UART5_Init(9600);	
	printf("--------------------------------\r\n");
	printf("|---北斗终端模块正在准备启动---|\r\n");
	printf("|                              |\r\n");
	printf("|     -串口初始化完成-         |\r\n");
	delay_init();
	SPI1_Init();
	printf("|     -SPI初始化完成-          |\r\n");
	SysTick_Config(72000);
	packet_init();
	//定时器设置
  timer_set(&periodic_timer, CLOCK_SECOND/10);
  timer_set(&arp_timer, CLOCK_SECOND*10);
	timer_set(&event_timer, CLOCK_SECOND*10);//
	timer_set(&BD_send_timer, CLOCK_SECOND*61);
  timer_set(&gprs_timer, CLOCK_SECOND*300);
	
  tapdev_init();	
  uip_init();
	printf("|    -网卡和uip初始化完成-     |\r\n");
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);// 设置中断优先级分组2 
	printf("|-配置本机地址并开始连接服务器-|\r\n");
	printf("|本机IP：  114.212.117.53      |\r\n");
	printf("|子网掩码：114,212,112,1       |\r\n");
	printf("|网关：    255,255,240,0       |\r\n");
	uip_ipaddr(ipaddr, 114,212,117,53);  //设置本机的IP地址
  uip_sethostaddr(ipaddr);
  uip_ipaddr(ipaddr, 114,212,112,1);
  uip_setdraddr(ipaddr);
  uip_ipaddr(ipaddr, 255,255,240,0);
  uip_setnetmask(ipaddr);
	printf("-------------------------------\r\n");		
	UIP_APPINIT();
  while (1)  
    {  
        uip_len = tapdev_read();  
        if(uip_len > 0)  
        {  
            /* ??IP??? */
            if(BUF->type == htons(UIP_ETHTYPE_IP))  
            {  						
                uip_arp_ipin();  
                uip_input();							
                if (uip_len > 0)  
                {  
                    uip_arp_out();  
                    tapdev_send();
	
                }  
            }  
            else if (BUF->type == htons(UIP_ETHTYPE_ARP))  
            {  
                uip_arp_arpin();
                					
                if (uip_len > 0)  
                {  
                    tapdev_send(); 
									  
                }  
            }  
        }   
        if(timer_expired(&periodic_timer))  
        {  
            timer_reset(&periodic_timer);  
            GPIOD->ODR ^= GPIO_Pin_13;
						
            for(i = 0; i < UIP_CONNS; i++)  
            {  
                uip_periodic(i);  
                if(uip_len > 0)  
                {  
                    uip_arp_out();  
                    tapdev_send();  
									  
                }  
            }   
						if (timer_expired(&arp_timer))  
						{  
								timer_reset(&arp_timer);  
								uip_arp_timer();  
								printf("arp_time...\r\n");
						}  		
						
						uart5_DATA_PROCESS();
						UART_DATA_PROCESS();
        }  
//				if (timer_expired(&gprs_timer)&&gprs_on==1)  
//            {  
//                timer_reset(&gprs_timer);
//								printf("发送4G心跳包...\r\n");							
//								UART5_WRITE(16,gprs_state);
//								//delay_ms(50);
//							  if(uart5_DATA_PROCESS()==0)
//								{ 
//									gprs_on=0;
//									printf("4G连接已中断……");
//								}
//            } 
						
				if (timer_expired(&BD_send_timer)&&BD_process_STA==1)  
            {  
              //timer_reset(&BD_send_timer);  
							timer_restart(&BD_send_timer);
							printf("北斗正在工作，请勿再次发送数据\r\n");
							if(BD_BUF_1_LEN>0)
							{
								int i;
								BD_TXSQ_SEND(ADDR,BD_BUF_1,BD_BUF_1_LEN);
								
								BD_BUF_1_LEN=0;
								
							}
							else if(BD_BUF_2_LEN>0)
							{
								BD_TXSQ_SEND(ADDR,BD_BUF_2,BD_BUF_2_LEN);
							
								BD_BUF_2_LEN=0;
							}
							else if(BD_BUF_3_LEN>0)
							{
								BD_TXSQ_SEND(ADDR,BD_BUF_3,BD_BUF_3_LEN);
								
								BD_BUF_3_LEN=0;
							}
							else if(BD_BUF_ASK_LEN>0)  //1min内未收到ACK将发起ASK，3次之后仍未收到则认定本次通信失败
							{
								printf("子包已全部发送完毕，正在封装ASK报文……\r\n");
								BD_TXSQ_SEND(ADDR,BD_BUF_ASK,BD_BUF_ASK_LEN);
								
								ask_count++;
								if(ask_count>3){
									printf("ASK次数超过三次,该数据传输过程失败\r\n");
									BD_BUF_ASK_LEN=0;
									BD_process_STA=0;
									bd_on=0;
									ask_count=0;
								}
							}		
							
            }    		
    }  

  return 0;
}
/*---------------------------------------------------------------------------*/

#ifdef __DHCPC_H__
void
dhcpc_configured(const struct dhcpc_state *s)
{
  uip_sethostaddr(s->ipaddr);
  uip_setnetmask(s->netmask);
  uip_setdraddr(s->default_router);
  resolv_conf(s->dnsaddr);
}
#endif /* __DHCPC_H__ */
//*-------------------------------------------------------------------*/
