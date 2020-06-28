#include "uart.h"
#include "usart.h"

int UART4_RX_STA=0;
unsigned char UART4_RX_BUF[RX_MAX];

unsigned char *TX_P;
int TX_CONT=0;


void UART_INIT(u32 bound)
{
	GPIO_InitTypeDef GPIO_INIT_S;
	USART_InitTypeDef USART_INIT_S;
	USART_ClockInitTypeDef USART_CLOCK_S;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO|RCC_APB2Periph_USART1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_INIT_S.GPIO_Mode= GPIO_Mode_AF_PP;
	GPIO_INIT_S.GPIO_Pin=GPIO_Pin_10;
	GPIO_INIT_S.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_INIT_S);
	
	GPIO_INIT_S.GPIO_Mode= GPIO_Mode_IN_FLOATING;;
	GPIO_INIT_S.GPIO_Pin=GPIO_Pin_11;
	GPIO_INIT_S.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_INIT_S); 
  
	USART_INIT_S.USART_BaudRate=bound;
	USART_INIT_S.USART_WordLength=USART_WordLength_8b;
	USART_INIT_S.USART_StopBits=USART_StopBits_1;
	USART_INIT_S.USART_Parity=USART_Parity_No;
	USART_INIT_S.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_INIT_S.USART_Mode= USART_Mode_Rx | USART_Mode_Tx;
	
	
	USART_CLOCK_S.USART_Clock=USART_Clock_Disable;
	USART_CLOCK_S.USART_CPHA=USART_CPHA_1Edge;
	USART_CLOCK_S.USART_CPOL=USART_CPOL_Low;

	USART_ClockInit(UART4,&USART_CLOCK_S);
	USART_Init(UART4,&USART_INIT_S);
	USART_Cmd(UART4,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_ClearITPendingBit(UART4,0xffff);
	USART_ITConfig(UART4,USART_IT_RXNE,ENABLE);

}
	
unsigned int UART_WRITE(unsigned int len,unsigned char *data)
{
	if(TX_CONT!=0)
	{
		return -1;
	}
	TX_CONT=len;
	TX_P=data;
	
	USART_ClearITPendingBit(UART4,USART_IT_TXE);
	USART_ITConfig(UART4,USART_IT_TXE,ENABLE);
	return 0;
}
void UART4_IRQHandler(void)
{
	 if(USART_GetITStatus(UART4,USART_IT_RXNE)==SET)
	 {
		
		UART4_RX_BUF[UART4_RX_STA]=USART_ReceiveData(UART4);
		UART4_RX_STA++;
		//printf("UART4_RX_STA+1\r\n");
		USART_ClearITPendingBit(UART4,USART_IT_RXNE);
	 }
	 
	 if(USART_GetITStatus(UART4,USART_IT_TXE)==SET)
	 {		 
		 if(TX_CONT!=0)
		 {
				USART_SendData(UART4,*TX_P);
				TX_CONT--;
			  TX_P++;
		 }
		 else
		 {
			 USART_ClearITPendingBit(UART4,USART_IT_TXE);
			 USART_ITConfig(UART4,USART_IT_TXE,DISABLE);
		 }
	 }
	 USART_ClearFlag(UART4,0XFF);
}

//void senddata(unsigned int len,unsigned char *data)
//{
//	u8 i;
//	for(i=0;i<len;i++)
//			{
//				UART4->DR=data[i];
//				while((UART4->SR&0X40)==0);//等待发送结束
//			}
//}
//void sendstring4(unsigned int len,char str[])
//{
//	u8 i=0;
//	i=0;
//	while(i<len)
//	{
//		USART_SendData(UART4, str[i]);         //向串口1发送数据
//		while(USART_GetFlagStatus(UART4,USART_FLAG_TC)!=SET);//等待发送结束
//		i++;
//	}
//}
