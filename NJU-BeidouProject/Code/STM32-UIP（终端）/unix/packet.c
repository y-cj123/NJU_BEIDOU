#include "packet.h"

unsigned char packet_len;
unsigned char packet_data[64];

char packet_check(packet_head* head)
{
	if( head->preamble == PACKET_PREAMBLE )
	{
		if( head->preamble + head->len == head->parity)
		{
			if(head->len<=252)
			{
					return 1;
			}
		}
	}
	return 0;
}

void packet_init()
{
	packet_len=0;
}

int packet_in(unsigned char *data)
{
	packet_head head;
	unsigned char i;
	
	head.preamble=data[0];
	head.len=data[1];
	head.parity=data[2];
	
	if(packet_check(&head))
	{
		packet_len=head.len;
		data+=3;
		for(i=0;i<packet_len;++i)
		{
			packet_data[i]=*data;
			++data;
		}
		return 1;
	}
	else
	{
		packet_len=0;
		return 0;
	}
}

int packet_out(unsigned char* data,unsigned char len)
{
	packet_head head;
	unsigned char i;
	

	
  head.preamble=PACKET_PREAMBLE;
	head.len=len;
	head.parity=len+PACKET_PREAMBLE;
	packet_len=len+3;
	
	packet_data[0]=head.preamble;
	packet_data[1]=head.len;
	packet_data[2]=head.parity;
	
	for(i=0;i<len;++i)
	{
		packet_data[3+i]=*data;
		data++;
	}
	return 0;
}