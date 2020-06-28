/* SystemFrequency / 1000    1ms????
 * SystemFrequency / 100000     10us????
 * SystemFrequency / 1000000 1us????
 */

#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	
#include "sys.h"
#define SYSTICKPERIOD                    0.000001
#define SYSTICKFREQUENCY            (1/SYSTICKPERIOD)

/**
  * @brief  ???2????,,????1ms
  * @param  ?
  * @retval ?
  */
void delay_init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    /*AHB = 72MHz,RCC_CFGR?PPRE1 = 2,??APB1 = 36MHz,TIM2CLK = APB1*2 = 72MHz */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    /* Time base configuration */         
    TIM_TimeBaseStructure.TIM_Period = 999;
    TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock/SYSTICKFREQUENCY -1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    
    /* ?????????????????????? */
    TIM_UpdateRequestConfig(TIM2,TIM_UpdateSource_Global); 
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
}

/**
  * @brief   ms????,1ms?????
  * @param  
  *        @arg nTime: Delay_ms( 10 ) ??????? 10 * 1ms = 10ms
  * @retval  ?
  */
void delay_ms(__IO uint32_t nTime)
{     
    /* ????????????? */  
    TIM2->CNT   = 0;  
    TIM_Cmd(TIM2, ENABLE);     

    for( ; nTime > 0 ; nTime--)
    {
     /* ??????????? */
     while(TIM_GetFlagStatus(TIM2, TIM_FLAG_Update) != SET);
     TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    }

    TIM_Cmd(TIM2, DISABLE);
}