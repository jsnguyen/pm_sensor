/*********************************************************************************************************
*
* File                : DustSensor
* Hardware Environment: 
* Build Environment   : Keil MDK
* Version             : V4.74
* By                  : WaveShare
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                       http://www.waveshare.com
*                                          All Rights Reserved
*
*********************************************************************************************************/
#include "mbed.h"


#define        COV_RATIO                       0.2            //ug/mmm / mv
#define        NO_DUST_VOLTAGE                 500            //mv   

/*
variable
*/
Serial uart(USBTX, USBRX);
DigitalOut myled(LED1), iled(D7);
AnalogIn analog_value(A0); 

float density, voltage;
 
 
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
 
int main(void) 
{
	uart.baud(115200);
	iled = 0;
	uart.printf("*********************************** WaveShare ***********************************\n");
  while(1) 
	{ 
	  iled = 1;
    wait_us(280);
		voltage = analog_value.read() * 3300 * 11;
		iled = 0;
		
		voltage = _filter(voltage);
		
		/*
		
		*/
		if(voltage >= NO_DUST_VOLTAGE)
		{
			voltage -= NO_DUST_VOLTAGE;
			
			density = voltage * COV_RATIO;
		}
		else
			density = 0;
		
		uart.printf("The current dust concentration is: %4.1f ug/m3\n", density);
		
    myled = !myled;
		wait(1);
  }
}





 

