#include "sim900a.h"
#include "usart.h"		
#include "delay.h"		 
//#include "malloc.h"
#include "string.h"    		
#include "uart.h" 


//////////////////////////////////////////////////////////////////////////////////	
extern u8 ip[20];
extern u8 por[5];
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//usmart支持部分 
//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零UART4_RX_STA;
//     1,清零UART4_RX_STA;
void sim_at_response(u8 mode)
{
	if(UART4_RX_STA&0X8000)		//接收到一次数据了
	{ 
		UART4_RX_BUF[UART4_RX_STA&0X7FFF]=0;//添加结束符
		printf("%s",UART4_RX_BUF);	//发送到串口
		if(mode)UART4_RX_STA=0;
	} 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//ATK-SIM900A 各项测试(拨号测试、短信测试、GPRS测试)共用代码

//sim900a发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* sim900a_check_cmd(u8 *str)
{
	char *strx=0;
	if(UART4_RX_STA&0X8000)		//接收到一次数据了
	{ 
		UART4_RX_BUF[UART4_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)UART4_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;   
	
}
//向sim900a发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	UART4_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while((UART4->SR&0X40)==0);	//等待上一次数据传输完成   
		UART4->DR=(u32)cmd;
	}else u2_printf("%s\r\n",cmd);//发送命令
	printf("%s\r\n",cmd);
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(UART4_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(sim900a_check_cmd(ack))break;//得到有效数据 
				UART4_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 
//将1个字符转换为16进制数字
//chr:字符,0~9/A~F/a~F
//返回值:chr对应的16进制数值
u8 sim900a_chr2hex(u8 chr)
{
	if(chr>='0'&&chr<='9')return chr-'0';
	if(chr>='A'&&chr<='F')return (chr-'A'+10);
	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
	return 0;
}
//将1个16进制数字转换为字符
//hex:16进制数字,0~15;
//返回值:字符
u8 sim900a_hex2chr(u8 hex)
{
	if(hex<=9)return hex+'0';
	if(hex>=10&&hex<=15)return (hex-10+'A'); 
	return '0';
}


int GPRS_init(const u8 *port,u8 * ipaddr,int ctr )
{
	//port="8086";	//端口固定为8086,当你的电脑8086端口被其他程序占用的时候,请修改为其他空闲端口
//	u8 mode=0;			//0,TCP连接;1,UDP连接
  u8 p[100];
//	u8 timex=0;
  int TstRslt=0;	//通信测试结果
	int i;
	//u8 ipbuf[16]="114.212.112.36"; 		//IP缓存
  for(i=ctr;i>0;i--)
	{
	while(sim900a_send_cmd("AT","OK",100))//检测是否应答AT指令 
	{
		sim_at_response(1);
		printf("未检测到模块!!!\n"); 
		delay_ms(1000);
		printf("尝试连接模块...\n"); 	
	 	delay_ms(1000);  
	}

  printf("服务器IP地址:%s 端口:%s 本机端口：8888 \r\n",ipaddr,port);
	
//进行GPRS连接
	printf("GPRS连接正在进行配置...\r\n");
  sim900a_send_cmd("AT+CIPSHUT","SHUT OK",100);	sim_at_response(1);	//关闭移动场景 
	if(sim900a_send_cmd("AT+CGCLASS=\"B\"","OK",1000))continue;	sim_at_response(1);		//设置GPRS移动台类别为B,支持包交换和数据交换 
	if(sim900a_send_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",1000))continue; sim_at_response(1); //设置PDP上下文,互联网接协议,接入点等信息
	if(sim900a_send_cmd("AT+CGATT=1","OK",1000))continue;sim_at_response(1);					//附着GPRS业务
	if(sim900a_send_cmd("AT+CIPCSGP=1,\"CMNET\"","OK",1000))continue;sim_at_response(1);//设置为GPRS连接模式
	if(sim900a_send_cmd("AT+CIPHEAD=1","OK",1000))continue;sim_at_response(1); 				//设置接收数据显示IP头(方便判断数据来源)
  if(sim900a_send_cmd("AT+CLPORT=\"TCP\",\"8888\"","OK",1000))continue;sim_at_response(1);//设置TCP连接本地端口号为8888
////向主机发起连接
	printf("正在向服务器发起连接...\r\n");
	sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",\"%s\"",ipaddr,port);
	//printf("%s\r\n",p);
	while (sim900a_send_cmd(p,"OK",1000)) sim_at_response(1);		//发起连接
  sim_at_response(1);
  printf("连接成功，发送测试中....\r\n");      
	while (sim900a_send_cmd("AT+CIPSEND",">",1000));		//发送数据
	
	sim_at_response(1);
	u2_printf("Hello,I'm from GPRS!");
	delay_ms(10);
	if(sim900a_send_cmd((u8*)0X1A,"SEND OK",1000)==0)
		TstRslt=1;//最长等待5s
	else
		TstRslt=0; 
	sim_at_response(1);
	delay_ms(1000); 
	
//	  sim900a_send_cmd((u8*)0X1B,0,0);	//ESC,取消发送 
	if(TstRslt)
	{
		printf("init ok!\r\n");
		ctr=0;
//		sim900a_send_cmd("AT+CIPSHUT","SHUT OK",100);	sim_at_response(1);	//关闭移动场景 
		return 1;

	}
	UART4_RX_STA=0;	
	}	
	printf("init failed!\r\n");
	return 0;	
}

int GPRS_send(u8 *data,u16 len )
{
	
	u8 *p;
	u16 timex=0;
	u8 count=0;
	u8 connectsta=0;			//0,正在连接;1,连接成功;2,连接关闭; 
	u8 hbeaterrcnt=0;			//心跳错误计数器,连续5次心跳信号无应答,则重新连接

	UART4_RX_STA=0;

	printf("数据发送中....\r\n");
		  
			if(sim900a_send_cmd("AT+CIPSEND",">",500)==0)		//发送数据
			{ 
 				printf("CIPSEND DATA:%s\r\n",data);	 			//发送数据打印到串口
				u2_printf("%s\r\n",data);
				printf("UART4_TX_BUF:%s\r\n",UART4_TX_BUF);
				delay_ms(10);
				if(sim900a_send_cmd((u8*)0X1A,"SEND OK",5000)==0)
				{
					printf("数据发送成功\r\n");//最长等待10s
				  return 1;
				}
				else printf("数据发送失败\r\n"); 
				delay_ms(500); 
			}else sim900a_send_cmd((u8*)0X1B,0,0);	//ESC,取消发送 

	
}

/////////////////////////////
//这边需要加一个fifo函数，最终可以把接收到的数据存到Rx_GPRS中
void GPRS_rec()
{
	
}





void GPRS_close( )
{
	sim900a_send_cmd("AT+CIPCLOSE=1","CLOSE OK",500);	//关闭连接
	sim900a_send_cmd("AT+CIPSHUT","SHUT OK",500);		//关闭移动场景 
	if(strstr((const char*)UART4_RX_BUF,"SHUT OK"))printf("GPRS closed!");
}


