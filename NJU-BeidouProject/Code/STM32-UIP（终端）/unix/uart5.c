#include "delay.h"
#include "uart5.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	   
//////////////////////////////////////////////////////////////////////////////////	 
							  
////////////////////////////////////////////////////////////////////////////////// 	   

//串口发送缓存区 	
__align(8) u8 UART5_TX_BUF[UART5_MAX_SEND_LEN]; 	//发送缓冲,最大UART4_MAX_SEND_LEN字节
#ifdef UART5_RX_EN   								//如果使能了接收   	  
//串口接收缓存区 	
u8 UART5_RX_BUF[UART5_MAX_RECV_LEN]; 				//接收缓冲,最大UART4_MAX_RECV_LEN个字节.
unsigned char *TXP;
int TX_len=0;


//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
u16 UART5_RX_STA=0;   	 
void UART5_IRQHandler(void)
{
	u8 res;	    
if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)//接收到数据
	{	 
		res =USART_ReceiveData(UART5);				 
		if(UART5_RX_STA<UART5_MAX_RECV_LEN)		//还可以接收数据
		{
		  TIM_SetCounter(TIM4,0);//计数器清空      
			if(UART5_RX_STA==0)TIM4_Set(1);	 	//使能定时器4的中断 
			UART5_RX_BUF[UART5_RX_STA++]=res;		//记录接收到的值	 
		}else 
		{
			UART5_RX_STA|=1<<15;					//强制标记接收完成
		} 
	}  	

if(USART_GetITStatus(UART5,USART_IT_TXE)==SET)
	{		 
		 if(TX_len!=0)
		{
			USART_SendData(UART5,*TXP);
			TX_len--;
			TXP++;
		}
			else
		{
			USART_ClearITPendingBit(UART5,USART_IT_TXE);
			USART_ITConfig(UART5,USART_IT_TXE,DISABLE);
		}
	}	
}  
 
unsigned int UART5_WRITE(unsigned int len,unsigned char *data)
{
	if(TX_len!=0)
	{
		return -1;
	}
	TX_len=len;
	TXP=data;
	
	USART_ClearITPendingBit(UART5,USART_IT_TXE);
	USART_ITConfig(UART5,USART_IT_TXE,ENABLE);
	return 0;
}
//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void UART5_Init(u32 bound)
{ 	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);	// GPIOA时钟
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
	
 	USART_DeInit(UART5);  //复位串口1
		 //UART4_TX   PA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PA.2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化PA2
   
    //UART4_RX	  PA.3
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOD, &GPIO_InitStructure);  //初始化PA3
	
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  
	USART_Init(UART5, &USART_InitStructure); //初始化串口	2
  
	//波特率设置
 	//UART4->BRR=(pclk1*1000000)/(bound);// 波特率设置	 
	//UART4->CR1|=0X200C;  	//1位停止,无校验位.
	//USART_DMACmd(UART4,USART_DMAReq_Tx,ENABLE);  	//使能串口2的DMA发送
	//UART_DMA_Config(DMA1_Channel7,(u32)&UART4->DR,(u32)UART4_TX_BUF);//DMA1通道7,外设为串口2,存储器为UART4_TX_BUF 
	USART_Cmd(UART5, ENABLE);                    //使能串口 
	
#ifdef UART5_RX_EN		  	//如果使能了接收
	//使能接收中断
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启中断   
	
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	TIM4_Init(99,7199);		//10ms中断
	UART5_RX_STA=0;		//清零
	TIM4_Set(0);			//关闭定时器4
#endif	 									  	
}

//////////////////////////////////////////////////////////////////////////
//串口printf函数
/*
 * 函数名：itoa
 * 描述  ：将整型数据转换成字符串
 * 输入  :-radix =10 表示10进制,其他结果为0
 *         -value 要转换的整型数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  :无
 * 返回  :无
 * 调用  :被USART_printf()调用
 */
 static char *itoa(int value, char *string, int radix)
 {
  int  i, d;
  int  flag = 0;
  char *ptr = string;

 /* This implementation only works for decimal numbers. */
  if (radix != 10)
  {
      *ptr = 0;
      return string;
  }
  if (!value)
  {
      *ptr++ = 0x30;
      *ptr = 0;
      return string;
  }
  /* if this is a negative value insert the minus sign. */
  if (value < 0)
  {
      *ptr++ = '-';
      /* Make the value positive. */
      value *= -1;
  }
  for (i = 10000; i > 0; i /= 10)
  {
      d = value / i;
      if (d || flag)
      {
          *ptr++ = (char)(d + 0x30);
          value -= (d * i);
          flag = 1;
      }
  }
     /* Null terminate the string. */
  *ptr = 0;
  return string;
 } /* NCL_Itoa */

 /*
 * 函数名:USART_printf
 * 描述  :格式化输出,类似C库中的printf,但这里没有用到C库
 * 输入  :-USARTx 串口通道
 *                     -Data   要发送到串口的内容的指针
 *                           -...    其他参数
 * 输出  :无
 * 返回  :无 
 * 调用  :外部调用
 *         典型应用   USART_printf( USART1, "\r\n this is a demo \r\n" );
 *                             USART_printf( UART4, "\r\n %d \r\n", i );
 *                             USART_printf( USART3, "\r\n %s \r\n", j );
 */
void u2_printf(char* fmt,...)
 {
   const char *s;
   int d;   
   char buf[16];

   va_list ap;
   va_start(ap, fmt);

      while ( * fmt != 0)     // 判断是否达到字符串结束符
      {                                                          
       if ( * fmt == 0x5c )   //'\'
       {                                                                          
        switch ( *++ fmt )
        {
         case 'r':            //回车
         USART_SendData(UART5, 0x0d);
          fmt ++;
         break;
         case 'n':             //换行
         USART_SendData(UART5, 0x0a);        
          fmt ++;
         break;                      
         default:
          fmt ++;
         break;
        }                         
      }
       else if ( * fmt == '%')
        {                       //
         switch ( *++ fmt )
         {                                
          case 's':             //字符串
          s = va_arg(ap, const char *);
          for ( ; *s; s++) 
          {
					 UART5->SR;
           USART_SendData(UART5,*s);
           while( USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET );
           }
            fmt++;
           break;

          case 'd':               //十进制
          d = va_arg(ap, int);
          itoa(d, buf, 10);
          for (s = buf; *s; s++) 
           {
						UART5->SR;
            USART_SendData(UART5,*s);
            while( USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET );
           }
            fmt++;
           break;
           default:
           fmt++;
           break;
           }                 
       } /* end of else if */
        else UART5->SR;
			  USART_SendData(UART5, * fmt++);
        while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET );
      }
			
 }

//定时器4中断服务程序		    
void TIM4_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//是更新中断
	{	 			   
		UART5_RX_STA|=1<<15;	//标记接收完成
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //清除TIMx更新中断标志    
		TIM4_Set(0);			//关闭TIM4  
	}   
}
//设置TIM4的开关
//sta:0，关闭;1,开启;
void TIM4_Set(u8 sta)
{
	if(sta)
	{
       
		TIM_SetCounter(TIM4,0);//计数器清空
		TIM_Cmd(TIM4, ENABLE);  //使能TIMx	
	}else TIM_Cmd(TIM4, DISABLE);//关闭定时器4
}
//通用定时器中断初始化
//这里始终选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数		 
void TIM4_Init(u16 arr,u16 psc)
{	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能//TIM4时钟使能    
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM4中断,允许更新中断

	 	  
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
}
#endif		 
									 





















