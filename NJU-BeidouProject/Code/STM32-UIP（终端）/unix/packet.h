//ะฃั้ฮป
#ifndef _PACKET_H_
#define _PACKET_H_

#define PACKET_PREAMBLE 0xAA

extern unsigned char packet_data[64];
extern unsigned char packet_len;

void packet_init();
int packet_in(unsigned char *data);
int packet_out(unsigned char* data,unsigned char len);

typedef struct packet_head_s
{
	unsigned char len;
	unsigned char parity;
	unsigned char preamble;
}packet_head;

#endif




