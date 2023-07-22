#include <Wire.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <list>


#define SS_PIN  5  // ESP32 pin GIOP5 
#define RST_PIN 27 // ESP32 pin GIOP27 

// Set the LCD address to 0x27 for a 20 chars and 4 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);
// Set Pin to RFID
MFRC522 rfid(SS_PIN, RST_PIN);

const char* ssid = "TT_1380";
const char* password = "nm8ho731d3";
// Global Varibles

String          Tag;
String          Tag_Driver;
static char     Final_Duration[9]; 
unsigned int    Status;
unsigned int    counter       = 0 ;
unsigned long   startTime     = 0;
unsigned long   elapsedTime   = 0;


std::list<String> tagsList;


// WIFI Functions

void connectToWiFi() {
  lcd.clear();
  lcd.print("Connecting to");
  lcd.setCursor(0, 1);
  lcd.print("Wi-Fi...");
  Serial.print("\n[WIFI]  Connecting to Wi-Fi...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\n[WIFI]  Connected");
  Serial.print("[WIFI] IP Address: ");
  Serial.println(WiFi.localIP());
}

// RFID Functions

int read_rfid(String *Tag) {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {

    *Tag = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      *Tag += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
      *Tag += String(rfid.uid.uidByte[i], HEX);
    }
    Serial.println("[RFID] New Tag Detected");
    Serial.print("[RFID] TAG : ");
    Serial.println(*Tag);
    rfid.PICC_HaltA();
    
    return 1;
    
   } else {
    
    return 0;
    
  }
}

void verfiy_tag(const String& element){
  
  auto it = std::find(tagsList.begin(), tagsList.end(), element);

  if (it == tagsList.end()) {
    // Element not found in the list, add it
    tagsList.push_back(element);
    counter++;
  } else {
    // Element found in the list, delete it
    tagsList.erase(it);
    counter--;
  }
  
}

int verfiy_driver(String Tag) {

  return 1;


}


// LCD Functions

void lcd_welcome_screen() {
  
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("Welcome to ");
  lcd.setCursor(5, 2);
  lcd.print("School-Bus");
  lcd.setCursor(4, 3);
  lcd.print("Surveillance");
  delay(3000);
  
}

void lcd_diplay_tag(String Tag) {
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RFID Tag:");
  lcd.setCursor(0, 1);
  lcd.print(Tag);
  delay(1000);
  lcd.clear();
  
}


void lcd_display_driver_msg() {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Please Enter ");
  lcd.setCursor(0, 1);
  lcd.print("Driver Card  ");
  lcd.setCursor(0, 2);
  lcd.print("To Start ... ");
  
}

void lcd_display_error(){
  
  lcd.clear(); 
  lcd.setCursor(0, 0);
  lcd.print("Attention");
  lcd.setCursor(0, 1);
  lcd.print("someone on a bus");

  for (int i = 0; i < 6; i++){
        lcd.noBacklight();
        delay(500);
        lcd.backlight();
        delay(500);
  }
  delay(1000);
  lcd.clear();
  
}


void lcd_display_terminate(){
  Serial.println("\n[Main] Final Duration ");
  Serial.println(Final_Duration);
  lcd.clear();
  lcd.print("Trip Terminate ");
  lcd.setCursor(0, 2);
  lcd.print("Duration :");
  lcd.setCursor(0, 3);
  lcd.print(Final_Duration);
  delay(5000);
  lcd.clear();
  
  }

void lcd_display_time_counter(int counter) {

  elapsedTime = millis() - startTime;

  // Calculate hours, minutes, and seconds from milliseconds
  unsigned long hours = elapsedTime / 3600000;
  unsigned long minutes = (elapsedTime / 60000) % 60;
  unsigned long seconds = (elapsedTime / 1000) % 60;
  lcd.setCursor(0, 0);
  lcd.print("Number of Kids:");
  lcd.setCursor(0, 1);
  lcd.print(String(counter));
  lcd.setCursor(0, 2);
  lcd.print("Trip Duration :");
  lcd.setCursor(0, 3);
  lcd.print(hours < 10 ? "0" : "");
  lcd.print(hours);
  lcd.print(":");
  lcd.print(minutes < 10 ? "0" : "");
  lcd.print(minutes);
  lcd.print(":");
  lcd.print(seconds < 10 ? "0" : "");
  lcd.print(seconds);

  sprintf(Final_Duration, "%02lu:%02lu:%02lu", hours, minutes, seconds);

}

void setup() {

  Serial.begin(115200);

  Serial.println("System Start");

  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522

  Serial.println("[RFID] Init RFID OK");
  // initialize the LCD
  lcd.begin();
  // Turn on the blacklight and print a message.
  lcd.backlight();
  
  Serial.println("[LCD] Init LCD OK");
  lcd_welcome_screen();

  connectToWiFi();

  Serial.println("[Main] State 1");

}


void loop() {
  switch (Status)
  {
    // Check WIFI
    case 0:
      
      lcd_display_driver_msg();
      Status = 1;

      break;

    // RFID Read
    case 1:
    
      if ( read_rfid(&Tag)) {
        lcd_diplay_tag(Tag);
        if (verfiy_driver(Tag)) {
          
   
          startTime = millis();
          Tag_Driver = Tag; // Save Tag_Driver
          Serial.println("[Main] State 2");
          Status = 2;
          
        } else {
          Serial.println("[Main] State 0");
          Status = 0;
        
        }

      }
      break;

    case 2:

      if (read_rfid(&Tag)) {

        if (Tag == Tag_Driver){
          Serial.println("[Main] State 3");
          Status = 3;
          
        }else{
          
         verfiy_tag(Tag);
       
        }
      }

      lcd_display_time_counter(counter);

      break;

    case 3:
      if (counter == 0){
         
         lcd_display_terminate();
         Serial.println("[Main] State 0");
         Status = 0;
   
      }else{
         
        lcd_display_error();
        Serial.println("[Main] State 2");
        Status = 2;
      
      }
      
      break;

  }

}
