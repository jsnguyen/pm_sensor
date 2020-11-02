/*********************************************************************************************************
*
* File                : arm_cortex_m.c
* Hardware Environment: 
* Build Environment   : RealView MDK-ARM  Version: 4.74
* Version             : V1.0
* By                  : V
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/
#include "arm_cortex_m.h"

/*
Private var
*/
static vu32 _CountPerUS;
static vu32 _CountPerMS;



/*******************************************************************************
* Function Name  : void CM3_SysTickInit(vu32 SysClk)
* Description    : SysTick init
                   1. Clock source HCLK
									 2. Interrupt disable
									 3. Counter disable
* Input          : SysClk: current system clock, unit: Hz
* Output         : None
* Return         : ADC value
* Attention		   : None
*******************************************************************************/
void CM3_SysTickInit(vu32 SysClk)
{
	SysTick->CTRL = 0x00;
	SysTick->CTRL |= 0x01 << 2;                                                //clock source HCLK
	
	CM3_SysTickCalculate(SysClk);
}
/*******************************************************************************
* Function Name  : void CM3_SysTickCalculate(vu32 SysClk)
* Description    : SysTick init
* Input          : SysClk: current system clock, unit: Hz
* Output         : None
* Return         : ADC value
* Attention		   : None
*******************************************************************************/
void CM3_SysTickCalculate(vu32 SysClk)
{
	_CountPerUS = SysClk / 1000000.0;
	_CountPerMS = SysClk / 1000.0;	
}
/*******************************************************************************
* Function Name  : void CM3_SysTickDelayUS(vu32 Count)
* Description    : delay
* Input          : xus
* Output         : None
* Return         : ADC value
* Attention		   : None
*******************************************************************************/
void CM3_SysTickDelayUS(vu32 Count)
{
	SysTick->LOAD = Count * _CountPerUS;
	SysTick->VAL = 0;
	
	SysTick->CTRL  |= 0x01;
	
	while( !(SysTick->CTRL & (0x01 << 16)) )
		;
	SysTick->CTRL &= ~(0x01 << 16);
	SysTick->CTRL &= ~0x01;
}
/*******************************************************************************
* Function Name  : void CM3_SysTickDelayMS(vu32 Count)
* Description    : delay xms
* Input          : xms
* Output         : None
* Return         : ADC value
* Attention		   : None
*******************************************************************************/
void CM3_SysTickDelayMS(vu32 Count)
{
	vu32 i;
	
	Count *= 2;
	
	for(i = 0; i < Count; i++)
	{
		CM3_SysTickDelayUS(500);
	}
}

	





































