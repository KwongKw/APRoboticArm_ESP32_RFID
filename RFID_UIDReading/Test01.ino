/*
  06/01/2016
  Author: Makerbro
  Platforms: ESP8266
  Language: C++
  File: HelloOLED.ino
  ------------------------------------------------------------------------
  Description: 
  Demo for OLED display showcasing writing text to the screen.
  ------------------------------------------------------------------------
  Please consider buying products from ACROBOTIC to help fund future
  Open-Source projects like this! We'll always put our best effort in every
  project, and release all our design files and code for you to use. 
  https://acrobotic.com/
  ------------------------------------------------------------------------
  License:
  Released under the MIT license. Please check LICENSE.txt for more
  information.  All text above must be included in any redistribution. 
*/
#include <Wire.h>
#include <ACROBOTIC_SSD1306.h>

void display_init(void);

//Inf to be read from RFID
static unsigned int bottle_total = 5;
static char bottle_type[5] = {'A', 'B', 'C', 'D', 'E'};
static unsigned int bottle_remain[5] = {100, 120, 200, 120, 150};
static char bottle_expiry[5][10] = {
    "01/07/24",
    "08/07/24",
    "08/07/24",
    "15/04/24",
    "03/07/23",
};

void setup()
{
  Wire.begin();	
  display_init();                      // Initialze SSD1306 OLED display  
}

void loop()
{
  for (int i = 1; i < 6; i++){  
        oled.clearDisplay();              // Clear screen
        oled.setTextXY(0,0);              // Set cursor position, start of line 0
        oled.putString("Type         : "+String(bottle_type[i]));
        oled.setTextXY(1,0);              // Set cursor position, start of line 1
        oled.putString("Remaining    : "+String(bottle_remain[i]));
        oled.setTextXY(2,0);              // Set cursor position, start of line 2
        oled.putString("Expiry Date  : "+String(bottle_expiry[i]));
  }
  delay(10000);
}

void display_init(void){
  oled.init();                      // Initialze SSD1306 OLED display
  oled.setFont(font5x7);
  oled.clearDisplay();              // Clear screen
  oled.setTextXY(0,0);              // Set cursor position, start of line 0
  oled.putString("Type         : ");
  oled.setTextXY(1,0);              // Set cursor position, start of line 1
  oled.putString("Remaining    : ");
  oled.setTextXY(2,0);              // Set cursor position, start of line 2
  oled.putString("Expiry Date  : ");         
}