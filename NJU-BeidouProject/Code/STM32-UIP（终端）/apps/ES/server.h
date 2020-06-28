
#ifndef _SERVER_H_
#define _SERVER_H_

#include "uipopt.h"
#include "psock.h"
#include "packet.h"

typedef struct server_app_state {
  struct psock p;
  char inputbuffer[10];
  char name[40];
} uip_tcp_appstate_t;

int packet_send(int len,unsigned char *data);

void server_appcall(void);
#ifndef UIP_APPCALL
#define UIP_APPCALL server_appcall
#endif /* UIP_APPCALL */


void server_init(void);
#ifndef UIP_APPINIT
#define UIP_APPINIT server_init
#endif
#endif /* __HELLO_WORLD_H__ */
/** @} */
/** @} */
