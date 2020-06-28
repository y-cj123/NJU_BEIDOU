#include "BD_write.h"
#include "BD_combine.h"
#include "Init.h"
#include "BD_split.h"

extern unsigned char my_addr[3];
extern int download_signal;
extern int command_lenth;
extern unsigned char download_command[1024];
extern time_t BD_last_sendtimer;
extern int acked;
time_t curr_timer;
int receive_length=0;

void BD_write(int port_f)
{
	unsigned char send_buffer[1024];
	bzero(send_buffer,sizeof(send_buffer));
	
	while(1)
	{
		if(download_signal == 1)
		{
			command_lenth --;	//由于服务端命令是shell终端打印的，所以命令最后会多出一个回车字符
			//printf("command length is : %d\n", command_lenth);

			download_command[command_lenth+1] = '\0';
			//printf("strlen = %d\n", strlen(download_command));
			unsigned char from_addr[3];
			unsigned char receive_data[512];
			int len1=0;
			unsigned char to_addr[3];
			unsigned char command[1024];
			bzero(to_addr, sizeof(to_addr));
			bzero(command, sizeof(command));
			int i;

			//to_addr[0] = char2xchar(download_command,2);
			//to_addr[1] = char2xchar(download_command + 2,2);
			//to_addr[2] = char2xchar(download_command + 4,2);
			to_addr[0] = 0x04;
			to_addr[1] = 0xE3;
			to_addr[2] = 0xC8;

			for(i = 0; i != strlen(download_command); ++i)
				printf("%x ", download_command[i]);
			printf("\n");
			unsigned char hour,min;
			struct tm *tim;
			time_t timep;
			time(&timep);	//获取UTC时间（1900年1月1日起）单位秒
			tim=gmtime(&timep);

			hour=tim->tm_hour;	//UTC
			min=tim->tm_min;

			command[0] = hour;
			command[1] = min;
			command[2] = 0x01;	//子包号
			command[3] = 0x01;	//子包总数
			
			for(i = 4; i != command_lenth+5; ++i)		//命令
				command[i] = download_command[i - 4];
			
			command_lenth = command_lenth  + 5;	//减去6个字节地址数据和1个字节':'；加上4个字节报头
			//printf("command length is : %d\n", command_lenth);

			printf("目的地址为： %x %x %x\n", to_addr[0], to_addr[1], to_addr[2]);
			BD_send(command, command_lenth, my_addr, to_addr, send_buffer);
			printf("封装待发送的命令为：\n");
			
			command_lenth +=19;	//加上19个字节北斗报文封装
			//printf("command length is : %d\n", command_lenth);
			
			for(i = 0; i != command_lenth; ++i)
			printf("%x ",send_buffer[i]);
			printf("\n");

			//写入串口
			//write(port_f, send_buffer, send_buffer);		

			//写入串口改写
			while(1){
					sleep(2);
					time(&curr_timer);
					if(write(port_f,send_buffer,command_lenth)!=-1)
					{
						receive_length=BD_read(port_f,from_addr,receive_data);
						if(receive_length==-1&&acked==1)
						{
							printf("already send data to Beidou\n");
							BD_last_sendtimer=curr_timer;
							acked=0;
							printf("BD_last_sendtimer is %d\n",BD_last_sendtimer);
							break;
						}

					}
					else printf("串口写入失败\n");
				}
			bzero(send_buffer,sizeof(send_buffer));			
			bzero(download_command, sizeof(download_command));
			command_lenth = 0;
			download_signal = 0;
		}
	}
}
