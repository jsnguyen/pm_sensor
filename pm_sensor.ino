#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define COV_RATIO 0.2            //ug/mmm / mv
#define NO_DUST_VOLTAGE 400            //mv
#define SYS_VOLTAGE 5000           
#define PIX_PER_CHAR 8

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);


/*
I/O define
*/
const int iled = 13;                                            //drive the led of sensor
const int vout = 14;                                            //analog input

/*
variable
*/
float density, voltage;
int   adcvalue;

/*
private function
*/
int Filter(int m)
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


void setup(void)
{
  pinMode(iled, OUTPUT);
  digitalWrite(iled, LOW); //iled default closed

  delay(200);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done
  display.clearDisplay();   // clears the screen and buffer

  
  Serial.begin(9600); //send and receive at 9600 baud
  Serial.print("Startup\n");
}

void loop(void)
{
  /*
  get adcvalue
  */
  digitalWrite(iled, HIGH);
  delayMicroseconds(280);
  adcvalue = analogRead(vout);
  digitalWrite(iled, LOW);
  
  adcvalue = Filter(adcvalue);
  
  /*
   * convert voltage (mv)
   */
  voltage = (SYS_VOLTAGE / 1024.0) * adcvalue * 11;
  
  /*
   * voltage to density
   */
  if(voltage >= NO_DUST_VOLTAGE) {
    voltage -= NO_DUST_VOLTAGE;
    
    density = voltage * COV_RATIO;
  } else {
    density = 0;
  }

  uint8_t n_circles = 6;
  uint8_t circle_r = 8;
  uint8_t spc_btwn = 4;

  uint8_t offset = ( display.width() - 2 * circle_r * n_circles - spc_btwn * (n_circles-1) )/2 ;
  
  for(int i=0; i<n_circles; i++){
    display.drawCircle(offset + circle_r + circle_r*2*i + i*spc_btwn, 2*circle_r, circle_r, WHITE);
  }

  uint8_t level = 0;
  float to_aqi = 0;
  float aqi = 0;
  float pm_offset = 0;
  float aqi_offset = 0;
  if (density <= 35){
    level = 1;
    to_aqi = 50.0/35.0;
    pm_offset = 0.0;
    aqi_offset = 0.0;
  }
  
  else if (35 < density && density <= 75){
    level = 2;
    to_aqi = 50.0/40.0;
    pm_offset = 35.0;
    aqi_offset = 50;
  }
  
  else if (75 < density && density <= 115){
    level = 3;
    to_aqi = 50.0/40.0;
    pm_offset = 75.0;
    aqi_offset = 100;
  }
  
  else if (115 < density && density <= 150){
    level = 4;
    to_aqi = 50.0/35.0;
    pm_offset = 115.0;
    aqi_offset = 150;
  }
  
  else if (150 < density && density <= 250){
    level = 5;
    to_aqi = 1;
    pm_offset = 150.0;
    aqi_offset = 200;
  }
  else if (250 < density){
    level = 6;
    to_aqi = 1;
    pm_offset = 250.0;
    aqi_offset = 300;
  }

  aqi = (density-pm_offset)*to_aqi + aqi_offset;

  Serial.print(density);
  Serial.print(" ug/m3\n");
  Serial.print(aqi);  
  Serial.print(" AQI\n");
  Serial.print(to_aqi);  
  Serial.print(" to_aqi\n");
  Serial.print(level);  
  Serial.print(" level\n\n");

  // text display tests
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(display.width()/2-PIX_PER_CHAR*4,display.height()/2);
  display.println(String(aqi));
  display.setCursor(display.width()/2-PIX_PER_CHAR*2,display.height()*3/4);
  display.println(String("AQI"));

  for(int i=0; i<level; i++){
    display.fillCircle(offset + circle_r + circle_r*2*i + i*spc_btwn, 2*circle_r, circle_r, WHITE);
  }
  
  display.display();
  
  delay(1000);
  display.clearDisplay();

}
