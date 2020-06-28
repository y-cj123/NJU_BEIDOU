/********************************
解析北斗通信报文
********************************/

#include "BD_split.h"

/********************************
函数： BD_read
输入参数： port_f 串口设备； from_addr 发信地址； recieve_data 接收数据
输出参数： 接收数据长度
********************************/
int acked;
int BD_read(int port_f, unsigned char from_addr[], unsigned char recieve_data[])
{
	unsigned char origin[1024];
	unsigned char hol_len[2],my_addr[3],info_cate,send_addr[3];
	unsigned char send_time[2],len[2],crc,check;
	int lenth;
	int i=0,n=0,k=0;
	int flag=0,new_data=0;
	int nread=0;
	unsigned char buff[512];
	bzero(origin,sizeof(origin));
	bzero(buff,sizeof(buff));
	while((nread = read(port_f,buff,1))>0)
	{
		//printf("%x ", buff[0]);
		if(new_data==0)
		{
			if(buff[0]=='$') 
			{
				new_data=1;//判断是否为新数据的标志位
				origin[5]=0xff;
				origin[6]=0xff;
			}
		}
		if(new_data==1)
		{
			//printf("buff = %x", buff[0]);
			origin[k]=buff[0];
			k++;
		}
	}
	printf("\nk is %d\n", k);
	if(k == 0)
	{	
		printf("未读到串口数据\n");
		bzero(origin,sizeof(origin));
		bzero(buff,sizeof(buff));
		new_data=0;
	}
	else
	{
		printf("\nbuff: ");
		for(i=0; i<k; i++)
		{
			printf("%x ",origin[i]);
		}
		k=0;
		new_data=0;
	}
	sleep(2);
/*
	while(buffer[i] != '$')
		++i;
	for(n = 0; n < strlen(buffer); ++n)
		origin[n] = buffer[i + n];
*/		
	if(origin[0]!=0&&origin[1]!=0)
	{//printf("%x, %x, %x, %x, %x",origin[0],origin[1],origin[2],origin[3],origin[4]);
		
		if(origin[0]=='$'&&origin[1]=='F'&&origin[2]=='K')//修改 查看是否有北斗返回信息以确保数据已经通过北斗发送出去
		{
			printf("北斗已发送数据并有返回数据\n");
			acked=1;
		}
		else
		{
			acked=0;
		}
		
		if(flag==0&&origin[0]=='$') 
		{
			flag++;  //用来判断是什么指令【flag=1是命令】
		}
		if(flag==1&&origin[1]=='T')
		{
			flag++;//flag=2;  $TXXX 通信信息
		}
		else
		{
			flag=0;
		}
		if(flag==2&&origin[2]=='X')
		{
			flag++; //flag=3
		}
		else
		{
			flag=0;
		}
		if(flag==3&&origin[3]=='X')
        {
            flag++;
    	}
        else
        {
        	flag=0;
		}
		if(flag==4&&origin[4]=='X')
		{
			flag=0;
			hol_len[0]=origin[5];          //报文长度高位
			hol_len[1]=origin[6];          //报文长度低位
			my_addr[0]=origin[7];          //用户地址
			my_addr[1]=origin[8];
			my_addr[2]=origin[9];
			info_cate=origin[10];          //信息类别
			send_addr[0]=origin[11];       //发信地址
			send_addr[1]=origin[12];
			send_addr[2]=origin[13];
			send_time[0]=origin[14];       //发信时间h
			send_time[1]=origin[15];       //发信时间m
			len[0]=origin[16];             //电文长度高位
			len[1]=origin[17];             //电文长度低位
			lenth=((len[0]&0x00ff)<<8)|(len[1]&0x00ff);//电文长度bit
			lenth=lenth/8;		//电文长度字节
			unsigned char content[lenth];
			for(n=0;n<lenth;n++)
			{
				content[n]=origin[18+n];     //电文内容
			}
			crc=origin[18+lenth];          //CRC标志
			check=origin[18+lenth+1];      //校验和
			
			for(n=0;n<3;n++)		//返回地址和数据
			{
				from_addr[n]=send_addr[n];
			}
			for(n=0;n<lenth;n++)
			{
				recieve_data[n]=content[n];
			}

			return lenth; //函数返回值为接收到电文的数据长度
			bzero(origin,sizeof(origin));
			bzero(buff,sizeof(buff));
		}
		else
		{
			flag=0;
			return -1;
			bzero(origin,sizeof(origin));
			bzero(buff,sizeof(buff));
		}
	}		
}
