/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>

/** 2 bits communication
 * 00 : train comming
 * 01 : gate closing
 * 10 : gate opening
 * 11 : drive carefully
 */
#define PIN_COM_1 13
#define PIN_COM_2 10

bool bit1, bit2;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  pinMode(PIN_COM_1, INPUT);
  pinMode(PIN_COM_2, INPUT);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);Serial.begin(9600);
}

void loop() {
  lcd.clear();
  bit1 = digitalRead(PIN_COM_1);
  bit2 = digitalRead(PIN_COM_2);Serial.print(bit2);Serial.print(bit1);
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(6, 0);
  (bit1 && bit2)?lcd.print(" GO "):lcd.print("STOP");
  lcd.setCursor(0, 1);
  if(bit1 && bit2)
    lcd.print("Drive  carefully");
  else if(!bit1 && bit2)
    lcd.print("  Gate closing  ");
  else if(bit1 && !bit2)
    lcd.print("  Gate opening  ");
  else
    lcd.print(" Train  comming ");
  // delay a bit
  delay(500);
}
 