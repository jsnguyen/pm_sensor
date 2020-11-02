/*********************************************************************************************************
*
* File                : main.c
* Hardware Environment: 
* Build Environment   : RealView MDK-ARM  Version: 4.20
* Version             : V1.0
* By                  : 
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "SysTick\\systick.h"
#include <stdio.h>
#include "USART\\usart.h"
#include "gp2y1010au.h"
#include "arm_cortex_m.h"


/* Private function prototypes -----------------------------------------------*/
void USART_Configuration(void);

/* Private variables ---------------------------------------------------------*/
float density;

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int main(void)
{
	CM3_SysTickInit(SystemCoreClock);
	USART_Configuration();
	GP2Y_Init();
  printf("\r\n****************************************************************\r\n");
  /* Infinite loop */
  while (1)
  {
    /* Printf message with dust density to serial port every 1 second */
		
		density = GP2Y_GetDensity();
    printf("The current dust concentration is: %4.1f ug/m3\r\n", density);
	  CM3_SysTickDelayMS(1000);   /* delay 1000ms */
	}
}
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
