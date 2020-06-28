#ifdef TARGET_CLIENT
#include "client.h"
#include "uart.h"
#include "uip.h"

#include <string.h>

unsigned char *data_send;
int data_len;
int tcp_closed_flag=0;
int tcp_timeout_flag=0;
unsigned char connect=0;
struct uip_conn *conn_port;
uip_ipaddr_t ipaddr;
//char buffer[512]="111111111111the salary cut is actually less than that of Paul.unsigned char data_len;unsigned char data_len;222222222222222unsigned char data_len;unsigned char data_len;unsigned char data_len;333333333333333unsigned char data_len;unsigned char data_len;unsigned char data_len;4444444444444444444444444unsigned char data_len;unsigned char data_len;unsigned char data_len;unsigned char data_len;unsigned char data_len;unsigned char data_len;unsigned char data_len;555555555555555555555555555";

int packet_send(int len,unsigned char *data)
{
	
	
	//write  GDW_answer_write函数 here
	
	                                                                           
	data_len=len;
	data_send=data;
	//connect to the host

//	uip_ipaddr(ipaddr, 192,168,1,101);
//	uip_connect(&ipaddr,HTONS(234));

		return 0;
}

void
client_init(void)
{
  //uip_listen(HTONS(1000));
	uip_listen(HTONS(8081));
}

void
client_appcall(void)
{
	static unsigned char buf[16]="Hello";
  uip_tcp_appstate_t *s = &(uip_conn->appstate);
	int i=0;
  printf("正在检查TCP连接状态……\r\n");
	if(uip_aborted()) 
	{
	printf("---------------------->uip_aborted……\r\n");
	//uip_ipaddr(ipaddr, 114,212,112,36);
	//uip_connect(&ipaddr,HTONS(8888));	
	
  uip_ipaddr(ipaddr, 114,212,118,220);
	uip_connect(&ipaddr,HTONS(8081));
	}
	
	if(uip_timedout()) 
	{  
  printf("---------------------->uip_timeout……\r\n");
	tcp_timeout_flag++;
	printf("timeout第%d次……\r\n",tcp_timeout_flag);
//	if(tcp_timeout_flag>=3)
//	{
	printf("---------------------->即将abort……\r\n");
	uip_abort();
	printf("---------------------->abort后正在重新连接……\r\n",i);
  uip_ipaddr(ipaddr, 114,212,112,36);
	uip_connect(&ipaddr,HTONS(8888));
  printf("lport=%d;rport=%d\r\n",conn_port->lport,conn_port->lport);	
	}
	
	if(uip_closed()) 
	{ 
	printf("---------------------->uip_closed……\r\n");
	tcp_closed_flag++;
	if(tcp_closed_flag>=2)
	{
	printf("---------------------->closed后正在重新连接……\r\n");
  uip_ipaddr(ipaddr, 114,212,112,36);
	uip_connect(&ipaddr,HTONS(8888));
	printf("lport=%d;rport=%d\r\n",conn_port->lport,conn_port->lport);
	}
	}
	
	if(uip_connected()) 
	{ 
	tcp_timeout_flag=0;
	tcp_closed_flag=0;
		//let send message
  printf("---------------------->uip_connected……\r\n");
	}

	if(uip_acked()) 
	{    
		printf("---------------------->recieved uip_ack……\r\n");
		//acked received ,close the connection
		//uip_close();
	}
	
	if(uip_newdata()) 
	{  
   UART_WRITE(uip_len, uip_appdata);
	}
	
	if(uip_rexmit())
	{
		printf("---------------------->uip_rexmit……\r\n");
//		uip_rexmit_flag++;
//		if(uip_rexmit_flag>=5)
//		{
//		uip_ipaddr(ipaddr, 192,168,1,101);
//	  uip_connect(&ipaddr,HTONS(8081));
//    }	
  } 
	
	if(uip_poll()) 
	{
		if(data_len>0)
		{
		printf("uip_send…\r\n");
		uip_send(data_send,data_len);
		data_len = 0;
		}
		//DATA_PROCESS(uip_len,uip_appdata);
		//uip_close();
	}
}
#endif
