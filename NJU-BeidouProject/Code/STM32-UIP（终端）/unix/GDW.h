#ifndef GDW_H
#define GDW_H

typedef unsigned char u8;
typedef unsigned short u16;



//报文解析,正确返回值是透明转发报文长度，错误返回值是-1
int GDW_read(u8* origin_data, u8* recieve_data);

int GDW_answer_read(u8* origin_data, u8* recieve_data);
//透明转发报文封装
void GDW_forward_write(u8* addr, u8* origin_data, u8* send_data, u16 len);
//透明转发应答报文封装
void GDW_answer_write(u8* addr, u8* origin_data, u8* send_data, u16 len);

void LOG_ON(u8* addr, u8* send_data);
	
void HEART(u8* addr, u8* send_data);

void CONFIRM(u8* addr, u8* send_data);

#endif
