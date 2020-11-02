/*********************************************************************************************************
*
* File                : gp2y1010au.c
* Hardware Environment: 
* Build Environment   : RealView MDK-ARM  Version: 4.74
* Version             : V1.0
* By                  : 
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/
#include "stm32f4xx.h"
#include "arm_cortex_m.h"


#define            COV_RATIO                            0.2            // ug/mmm / mv
#define            NO_DUST_VOLTAGE                      500            //mv              

/*
Private Function
*/
static void _GP2Y_HardInit(void);
static void _GP2Y_Wait(void);
static u32 _GP2Y_GetADCValue(void);
static float _GP2Y_ADCValue2Voltage(vu32 Value);
static float _GP2Y_DataProcess(float Voltage);
static int _filter(int m);



/*******************************************************************************
* Function Name  : static void _GP2Y_HardInit(void)
* Description    : Initialize Hardware
                   1. PA.6    ADC
									 2. PA.7    ILED
* Input          : None
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/
static void _GP2Y_HardInit(void)
{
  ADC_InitTypeDef       ADC_InitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;

	/*Clock enable*/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  /* Configure PA.06 (ADC Channel6) as analog input -------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Configure PA.07 as digital output */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 	
	GPIO_ResetBits(GPIOA, GPIO_Pin_7);                                             //default low, ILED closed

  /* ADC Common Init **********************************************************/
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* ADC1 Init ****************************************************************/
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channe6 configuration *************************************/
  ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_3Cycles);
	
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
}
/*******************************************************************************
* Function Name  : static void _GP2Y_Wait(void)
* Description    : wait 0.28ms
* Input          : 
* Output         : None
* Return         : ADC value
* Attention		   : None
*******************************************************************************/
static void _GP2Y_Wait(void)
{
	CM3_SysTickDelayUS(280);
}
/*******************************************************************************
* Function Name  : static u32 _GP2Y_GetADCValue(void)
* Description    : 
* Input          : 
* Output         : None
* Return         : ADC value
* Attention		   : None
*******************************************************************************/
static u32 _GP2Y_GetADCValue(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_7);
	_GP2Y_Wait();
	
	/*Start once cov*/
	ADC_SoftwareStartConv(ADC1);
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != SET)
		;
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	
  GPIO_ResetBits(GPIOA, GPIO_Pin_7);                                             //default low, ILED closed
	
	return _filter(ADC_GetConversionValue(ADC1));
}
/*******************************************************************************
* Function Name  : static float _GP2Y_ADCValue2Voltage(vu32 Value)
* Description    : value -> Voltage
* Input          : ADC value
* Output         : None
* Return         : voltage, unit: v
* Attention		   : None
*******************************************************************************/
static float _GP2Y_ADCValue2Voltage(vu32 Value)
{
	float Temp;
	
	Temp = (3300 / 4096.0) * Value * 11;
	
	return Temp;
}
/*******************************************************************************
* Function Name  : static float _GP2Y_DataProcess(float Voltage)
* Description    : Voltage -> Dust density
* Input          : Voltage: Sensor output voltage, unit: v
* Output         : None
* Return         : Dust density, unit: ug/m*m*m
* Attention		   : None
*******************************************************************************/
static float _GP2Y_DataProcess(float Voltage)
{
	if(Voltage >= NO_DUST_VOLTAGE)
	{
		Voltage -= NO_DUST_VOLTAGE;
		
		return (Voltage * COV_RATIO);
	}
	else
		return 0;
}
/*******************************************************************************
* Function Name  : static int _filter(int m)
* Description    : 
* Input          : 
* Output         : None
* Return         : 
* Attention		   : None
*******************************************************************************/
static int _filter(int m)
{
  static int flag_first = 0, _buff[10], sum;
  const int _buff_max = 10;
  int i;
  
  if(flag_first == 0)
  {
    flag_first = 1;

    for(i = 0, sum = 0; i < _buff_max; i++)
    {
      _buff[i] = m;
      sum += _buff[i];
    }
    return m;
  }
  else
  {
    sum -= _buff[0];
    for(i = 0; i < (_buff_max - 1); i++)
    {
      _buff[i] = _buff[i + 1];
    }
    _buff[9] = m;
    sum += _buff[9];
    
    i = sum / 10.0;
    return i;
  }
}




/*******************************************************************************
* Function Name  : void GP2Y_Init(void)
* Description    : 1. Initialize Hardware
* Input          : 
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/
void GP2Y_Init(void)
{
	_GP2Y_HardInit();
}
/*******************************************************************************
* Function Name  : float GP2Y_GetDensity(void)
* Description    : get density
* Input          : 
* Output         : None
* Return         : density
* Attention		   : None
*******************************************************************************/
float GP2Y_GetDensity(void)
{
	float Voltage;
	
	Voltage = _GP2Y_ADCValue2Voltage(_GP2Y_GetADCValue());
	
	Voltage = _GP2Y_DataProcess(Voltage);
	
	return Voltage;
}


















