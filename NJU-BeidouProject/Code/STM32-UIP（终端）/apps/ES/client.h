
#ifndef _CLIENT_H_
#define _CLIENT_H_

#define CLIENT
#include "uipopt.h"
#include "psock.h"
#include "usart.h"

extern unsigned char *data_send;
extern int data_len;
extern int tcp_timeout_flag;
extern int tcp_closed_flag;


typedef enum CON_STATE_E
{
	PACKET_SEND,
	IDLE,
}CON_STATE;

typedef struct client_state {
  CON_STATE STATE;
	unsigned char send_len;
	unsigned char *send_data;
} uip_tcp_appstate_t;


void client_appcall(void);
#ifndef UIP_APPCALL
#define UIP_APPCALL client_appcall
#endif /* UIP_APPCALL */

void client_init(void);
#ifndef UIP_APPINIT
#define UIP_APPINIT client_appcall

int packet_send(int len,unsigned char *data);

#endif /* UIP_APPCALL */
#endif /* __client_H__ */

