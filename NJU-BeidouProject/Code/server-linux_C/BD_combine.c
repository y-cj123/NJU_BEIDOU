/*******************************
封装北斗包头
*******************************/

#include "BD_combine.h"

/*******************************
函数：BD_send
输入参数： data 需要封装的数据【已有】； lenth 数据长度【已有】； my_addr 发送方地址；
           send_addr 接收方地址； send_data 待发送数据
*******************************/
void BD_send(unsigned char data[],int lenth,unsigned char my_addr[],unsigned char send_addr[],unsigned char send_data[])
{

	int i;
	unsigned char check;

	send_data[0]='$';
	send_data[1]='T';
	send_data[2]='X';
	send_data[3]='S';
	send_data[4]='Q';

	send_data[5]=((lenth+19)&0xff00)>>8;     //报文长度高八位
	send_data[6]=((lenth+19)&0x00ff);        //报文长度低八位

	send_data[7]=my_addr[0];		//发送地址
	send_data[8]=my_addr[1];
	send_data[9]=my_addr[2];

	send_data[10]=0x46;		//信息类别

	send_data[11]=send_addr[0];		//目的地址
	send_data[12]=send_addr[1];
	send_data[13]=send_addr[2];

	send_data[14]=(((lenth+1)<<3)&0xff00)>>8;		//数据长度
	send_data[15]=(((lenth+1)<<3)&0x00ff);

	send_data[16]=0x00;

	send_data[17]=0xA4;

	for(i=0;i<lenth;i++)		//写入数据
	{
		send_data[i+18]=data[i];
	}

	check=0;		//校验和计算
	for(i=0;i<lenth+18;i++)
	{
		check ^=send_data[i];
	}

	send_data[lenth+18]=check;		//校验和

	return;
}

