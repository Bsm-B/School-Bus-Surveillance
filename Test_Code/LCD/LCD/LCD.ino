#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup()
{
	// initialize the LCD
	lcd.begin();
	// Turn on the blacklight and print a message.
	lcd.backlight();
	lcd.print("Hello, world! 0");
  lcd.setCursor(0,1);
  lcd.print("Hello, world! 1");
  lcd.setCursor(0,2);
  lcd.print("Hello, world! 2");
  lcd.setCursor(0,3);
  lcd.print("Hello, world! 3");


}

void loop()
{
	// Do nothing here...
}
