#ifndef BD_1_H
#define BD_1_H

typedef enum { FALSE = 0, TRUE = 1} Boolean;
typedef unsigned char u8;
typedef unsigned short u16;

extern Boolean BD_1_RX_FLAG;
extern Boolean BD_1_DATA_FLAG;
extern u8 BD_1_ADDR[3];
extern u8 BD_Count;
extern u8 ADDR[];
extern int Freq_commu;
extern int BD_process_STA;
extern u8 BD_BUF_1[100];
extern int BD_BUF_1_LEN;
extern u8 BD_BUF_2[100];
extern int BD_BUF_2_LEN;
extern u8 BD_BUF_3[100];
extern int BD_BUF_3_LEN;
extern u8 BD_BUF_ASK[50];
extern int BD_BUF_ASK_LEN;

int BD_CMD_ZJ(void);
void BD_CMD_IC(void);
void BD_TXSQ_SEND(u8* ADDR, u8* Data, u16 len);
void file_deal(u8 *origin, u8 lenth, u8 n);
void BD_retransmission(unsigned char len,unsigned char *data);
int BD_read(u8* origin, u8* recieve_data);
void UART_DATA_GET();
//void IP_ADDR_GET(u8* origin, u8* ip_data);
void file_deal(u8 *origin, u8 lenth, u8 n);
#endif
