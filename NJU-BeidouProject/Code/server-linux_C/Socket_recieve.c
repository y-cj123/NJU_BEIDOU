/*********客户机接受服务器数据线程*************/
#include "Socket_recieve.h"

extern int download_signal;
extern int command_lenth;
extern unsigned char download_command[1024];

extern time_t BD_last_sendtimer;
int bd_send_notReady[1];

void socket_recieve(int sockfd)
{
	unsigned char buffer[1024];
	int nread = 0;
	time_t curr_timer;
	bzero(buffer,sizeof(buffer));
	while(1)
	{
		sleep(2);
		nread=read(sockfd,buffer,sizeof(buffer));
		time_t curr_timer;
		time(&curr_timer);
		printf("BD_last_sendtimer is %d\n",BD_last_sendtimer);
		printf("curr_timer is %d\n",curr_timer);
		if (curr_timer-BD_last_sendtimer>60)
		{
			if(nread > 0)
			{
				command_lenth = nread;
				int cnt = 0;
				for(cnt = 0; cnt != nread; ++cnt)
					download_command[cnt] = buffer[cnt];		
				printf("recieve command from server\n");
				bzero(buffer,sizeof(buffer));
				nread = 0;
				download_signal = 1;
				BD_last_sendtimer=curr_timer;
			}
		}
		else {
			int wait_time=60-(curr_timer-BD_last_sendtimer);
			bd_send_notReady[0]=wait_time;
			send(sockfd, bd_send_notReady, 1, 0);	
		}

	}
}
