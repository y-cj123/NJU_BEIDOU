#ifdef TARGET_SERVER
#include "server.h"
#include "uart.h"
#include "uip.h"
#include "BD_1.h"
#include "usart.h"
#include "delay.h"
#include "uart5.h"
#include <string.h>

u8 confirm[] = {0x68, 0x32, 0, 0x32, 0, 0x68, 0, 0x21, 0x42, 0x08, 0x93, 0x02, 0, 0x60, 0, 0, 0x01, 0, 0x61, 0x16};   //确认报文，针对登录、心跳等报文
	
//查询电量指令
u8 check[] = {0x68,0x32,0x00,0x32,0x00,0x68,0x4B,0x21,0x42,0x08,0x93,0x02,0x0C,0x60,0x01,0x01,0x01,0x04,0xBE,0x16};	//共20个字节
u8 check1[] = {0x68,0x32,0x00,0x32,0x00,0x68,0x4B,0x21,0x42,0x08,0x93,0x02,0x0C,0x60,0x02,0x01,0x01,0x04,0xBF,0x16};	//共20个字节
u8 check2[] = {0x68,0x32,0x00,0x32,0x00,0x68,0x4B,0x21,0x42,0x08,0x93,0x02,0x0C,0x60,0x04,0x01,0x01,0x04,0xC1,0x16};	//共20个字节
u8 check3[] = {0x68,0x32,0x00,0x32,0x00,0x68,0x4B,0x21,0x42,0x08,0x93,0x02,0x0C,0x60,0x08,0x01,0x01,0x04,0x7C,0x16};	//共20个字节
u8 check4[] = {0x68,0x32,0x00,0x32,0x00,0x68,0x4B,0x21,0x42,0x08,0x93,0x02,0x0C,0x60,0x10,0x01,0x01,0x04,0x84,0x16};	//共20个字节
u8 check5[] = {0x68,0x32,0x00,0x32,0x00,0x68,0x4B,0x21,0x42,0x08,0x93,0x02,0x0C,0x60,0x20,0x01,0x01,0x04,0x94,0x16};	//共20个字节
u8 check6[] = {0x68,0x32,0x00,0x32,0x00,0x68,0x4B,0x21,0x42,0x08,0x93,0x02,0x0C,0x60,0x40,0x01,0x01,0x04,0xB4,0x16};	//共20个字节
u8 check7[] = {0x68,0x32,0x00,0x32,0x00,0x68,0x4B,0x21,0x42,0x08,0x93,0x02,0x0C,0x60,0x80,0x01,0x01,0x04,0xF4,0x16};	//共20个字节
u8 check8[] = {0x68,0x32,0x00,0x32,0x00,0x68,0x4B,0x21,0x42,0x08,0x93,0x02,0x0C,0x60,0x01,0x02,0x01,0x04,0x76,0x16};	//共20个字节
u8 check9[] = {0x68,0x32,0x00,0x32,0x00,0x68,0x4B,0x21,0x42,0x08,0x93,0x02,0x0C,0x60,0x02,0x02,0x01,0x04,0x77,0x16};	//共20个字节

u8 SEND_ADDR[3] = {0x04, 0xE3, 0xC9};   //目的北斗终端地址
u8 buff[256];
unsigned char *data_send;
unsigned char data_len;
unsigned int LEN0;
extern int gprs_on;
extern int bd_on;
extern struct timer BD_send_timer;
//Boolean BD_1_ZJ_FLAG = FALSE;
//Boolean BD_1_IC_FLAG = FALSE;

/*
*网口接收数据处理函数
网口发给串口
*/
void 
DATA_PROCESS(u16 len,unsigned char *data)
{
	unsigned int i;
	
	for(i=0;i<len;i++)
		{
			printf("%02X ",data[i]);
		}
	printf("\r\n");
	if(data[6] == 0xC9)
	{
		printf("心跳包数据\r\n");
		data_len = 20;
		data_send = confirm;
	}
	else if(data[6] == 0xD4 && data[12] == 0x0C)
	{
//		uip_send(confirm, 20);
		BD_process_STA=1;
		printf("待分发数据包\r\n");
		for(i=0; i<len; i++)
		{
			buff[i] = data[i];
		}
		file_deal(buff, len, 60);	
	}
	else if(data[6] == 0x88)
	{
		BD_process_STA=1;
		printf("待分发数据包\r\n");
		for(i=0; i<len; i++)
		{
			buff[i] = data[i];
		}
		
		file_deal(buff, len, 60);
	}
	else if(data[6] == 0x88 && len <70)
	{
		printf("时间信息数据\r\n");
		//sendstring1	("send to uart4\r\n");	
		BD_TXSQ_SEND(SEND_ADDR, data, len);
	}
	else
	{
		printf("无法识别数据\r\n");
	}
}                                                                                                                                              
/*
*网口发送函数
*/
int packet_send(int len,unsigned char *data)
{
//	uip_ipaddr_t ipaddr;
	
	
//	if(data[1] != 0xff)
//	{
//		data_len = len-5;
//		data_send = &data[5];
//	}
//	else 
//	{
//		BD_retransmission(len, data);//判断是否需要丢包重传
//	}
	//修改
	int i;
	//接受到主站的查询指令
	if(data[1] != 0xff && data[5]==0x0C)
	{
		data_len=20;
		data_send=check;
		printf("即将发送的指令为：\n");
		for(i=0;i<data_len;i++)
		{
			printf("%02X ",data_send[i]);
		}
		printf("\r\n");
	}
	
	else if(data[1] != 0xff && data[5]==0x01 && data[6]==0x0C)
	{
		data_len=20;
		data_send=check1;
		printf("即将发送的指令为：\n");
		for(i=0;i<data_len;i++)
		{
			printf("%02X ",data_send[i]);
		}
		printf("\r\n");
	}
	
	else if(data[1] != 0xff && data[5]==0x02 && data[6]==0x0C)
	{
		data_len=20;
		data_send=check2;
		printf("即将发送的指令为：\n");
		for(i=0;i<data_len;i++)
		{
			printf("%02X ",data_send[i]);
		}
		printf("\r\n");
	}
	
	else if(data[1] != 0xff && data[5]==0x03 && data[6]==0x0C)
	{
		data_len=20;
		data_send=check3;
		printf("即将发送的指令为：\n");
		for(i=0;i<data_len;i++)
		{
			printf("%02X ",data_send[i]);
		}
		printf("\r\n");
	}
	
	else if(data[1] != 0xff && data[5]==0x04 && data[6]==0x0C)
	{
		data_len=20;
		data_send=check4;
		printf("即将发送的指令为：\n");
		for(i=0;i<data_len;i++)
		{
			printf("%02X ",data_send[i]);
		}
		printf("\r\n");
	}
	
	else if(data[1] != 0xff && data[5]==0x05 && data[6]==0x0C)
	{
		data_len=20;
		data_send=check5;
		printf("即将发送的指令为：\n");
		for(i=0;i<data_len;i++)
		{
			printf("%02X ",data_send[i]);
		}
		printf("\r\n");
	}
	
	else if(data[1] != 0xff && data[5]==0x06 && data[6]==0x0C)
	{
		data_len=20;
		data_send=check6;
		printf("即将发送的指令为：\n");
		for(i=0;i<data_len;i++)
		{
			printf("%02X ",data_send[i]);
		}
		printf("\r\n");
	}
	
	else if(data[1] != 0xff && data[5]==0x07 && data[6]==0x0C)
	{
		data_len=20;
		data_send=check7;
		printf("即将发送的指令为：\n");
		for(i=0;i<data_len;i++)
		{
			printf("%02X ",data_send[i]);
		}
		printf("\r\n");
	}
	
	else if(data[1] != 0xff && data[5]==0x08 && data[6]==0x0C)
	{
		data_len=20;
		data_send=check8;
		printf("即将发送的指令为：\n");
		for(i=0;i<data_len;i++)
		{
			printf("%02X ",data_send[i]);
		}
		printf("\r\n");
	}
	
	else if(data[1] != 0xff && data[5]==0x09 && data[6]==0x0C)
	{
		data_len=20;
		data_send=check9;
		printf("即将发送的指令为：\n");
		for(i=0;i<data_len;i++)
		{
			printf("%02X ",data_send[i]);
		}
		printf("\r\n");
	}


	//接收到服务器的ACK信息
	else 
	{
		BD_retransmission(len, data);//判断是否需要丢包重传
	}
	//data_len = len;
	//data_send = data;

	return 0;
}

void
server_init(void)
{
  uip_listen(HTONS(8080));    //服务端监听端口号
}

void
server_appcall(void)
{
	static unsigned char buf[16]="Hello";
  uip_tcp_appstate_t *s = &(uip_conn->appstate);


	if(uip_aborted()) 
	{ 
	
	}
	
	if(uip_timedout()) 
	{  
		
	}
	
	if(uip_closed()) 
	{ 
//		UART_WRITE(sizeof("Closed"),"Closed");
		printf("关闭\r\n");
	}
	
	if(uip_connected()) 
	{ 
//		UART_WRITE(sizeof("Connected"),"Connected");
		printf("连接成功\r\n");

	}

	if(uip_acked()) 
	{    
		
	}
	
	if(uip_newdata()) 
	{ 
		
		int i;
		printf("UIP接收到新数据\r\n");
		printf("BD_process_STA=%d\r\n",BD_process_STA);
		printf("bd_on=%d\r\n",bd_on);
		printf("gprs_on=%d\r\n",gprs_on);
		//修改
		if(bd_on==0&&BD_process_STA==0) 
		{
			unsigned char *data=uip_appdata;
			for(i=0;i<uip_len;i++)
		{
			printf("%02X ",data[i]);
		}
	    printf("\r\n");
		  UART5_WRITE(uip_len,uip_appdata);
		
			if(data[6] == 0xC9)
		{
			printf("心跳包数据\r\n");
			data_len = 20;
			data_send = confirm;
		}
	  }
		
		else{
			if(BD_process_STA==0)
			{
				DATA_PROCESS(uip_len,uip_appdata);  
			}
			else printf("北斗进程繁忙\r\n");
		} 

	}
	
	if(uip_rexmit())
	{
		
	} 
	
	if(uip_poll()) 
	{
		int i=0;
		uip_send(data_send,data_len);//网口发送数据
		data_len=0;
		
		//uip_close();
	}
}
#endif
