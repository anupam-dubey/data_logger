#include <SPI.h>
#include <SD.h>
#include <DS3231.h>
#include <LiquidCrystal.h> // includes the LiquidCrystal Library 
#include <dht.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define dht1_pin A0
#define dht2_pin A1
#define dht3_pin A2

// Data wire is plugged into digital pin 2 on the Arduino
#define ONE_WIRE_BUS 8
int deviceCount = 0;
unsigned long interval=60000; // the time we need to wait
unsigned long previousMillis=0; // millis() returns an unsigned long.
// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);  

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

dht DHT1;
dht DHT2;
dht DHT3;
DS3231  rtc(SDA, SCL);
long id = 1;              //Store the id# of our reading
const int chipSelect = 10;
float t1,t2,t3,h1,h2,h3;
float temp[9];
LiquidCrystal lcd(2, 3, 4, 5, 6, 7); // Creates an LC object. Parameters: (rs, enable, d4, d5, d6, d7) 
void read_all_dht(void);
void read_all_temp(void);
void compress_save(void);
void setup() { 
  Serial.begin(115200);
 rtc.begin(); // Initialize the rtc object
 sensors.begin(); // Start up the library
 lcd.begin(16,2); // Initializes the interface to the LCD screen, and specifies the dimensions (width and height) of the display } 
//  rtc.setDOW(FRIDAY);     // Set Day-of-Week to SUNDAY
//  rtc.setTime(12, 58, 0);     // Set the time to 12:00:00 (24hr format)
//  rtc.setDate(20, 3, 2020);   // Set the date to January 1st, 2014
  pinMode(10, OUTPUT);
  delay(100);
   // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
   // Serial.println("Card failed, or not present");
     lcd.setCursor(0,1);
     lcd.print("Card Fail");delay(1350);
    // don't do anything more:
    return;
  }
  lcd.setCursor(0,1);
  lcd.print("Card Success");delay(1350);
  //Write Log.csv File Header
  File logFile = SD.open("LOG.csv", FILE_WRITE);
  if (logFile)
  {
    logFile.println(", , , , , , , , , , , , , , , , ,"); //Just a leading blank line, incase there was previous data
    logFile.println("ID, Date, Time, T1, H1, T2, H2, T3, H3, T4, T5, T6, T7, T8, T9, T10, T11, T12");
    logFile.close();
  }
  read_all_dht();
  lcd.clear();
  delay(1000);
  lcd.print(t1,1);
  lcd.print(" ");
  lcd.print(t2,1);
  lcd.print(" ");
  lcd.print(t3,1);
  lcd.print(" ");
  lcd.setCursor(0,1);
  lcd.print(h1,1);
  lcd.print(" ");
  lcd.print(h2,1);
  lcd.print(" ");
  lcd.print(h3,1);
  lcd.print(" ");
  delay(2000);
  lcd.clear();
  deviceCount = sensors.getDeviceCount();
  lcd.print(deviceCount, DEC);
  lcd.setCursor(0,1);
  lcd.print("sensors. Found");
  delay(2000);
  lcd.clear();
}
void loop() { 
 unsigned long currentMillis = millis(); // grab current time
 
 // check if "interval" time has passed (1000 milliseconds)
 if ((unsigned long)(currentMillis - previousMillis) >= interval) {
  
   read_all_dht();
   read_all_temp();
   compress_save();
   // save the "current" time
   previousMillis = millis();
 }
 lcd.setCursor(0,0);
 lcd.print("Time:  ");
 lcd.print(rtc.getTimeStr());
 
 lcd.setCursor(0,1);
 lcd.print("Date: ");
 lcd.print(rtc.getDateStr());
  delay(1000); 
}
void read_all_dht()
{
  DHT1.read11(dht1_pin);
  DHT2.read11(dht2_pin);
  DHT3.read11(dht3_pin);
  h1=DHT1.humidity;
  t1=DHT1.temperature;
  h2=DHT2.humidity;
  t2=DHT2.temperature;
  h3=DHT3.humidity;
  t3=DHT3.temperature;
}
void read_all_temp()
{
 sensors.requestTemperatures(); 
  
  // Display temperature from each sensor
  lcd.clear();
  for (int i = 0;  i < deviceCount;  i++)
  { lcd.setCursor(0,0);
    lcd.print(i+1);
    lcd.print(" ");
    temp[i] = sensors.getTempCByIndex(i);
    lcd.print(temp[i],1);
    delay(500);
  }  
}
void compress_save()
{
   //Open a file to write to
  //Only one file can be open at a time
  String dataString = String(id) + ", " + rtc.getDateStr()+", " + rtc.getTimeStr() ;
  dataString += ", " + String(t1) + ", " + String(h1);
  dataString += ", " + String(t2) + ", " + String(h2);
  dataString += ", " + String(t3) + ", " + String(h3);
  dataString += ", " + String(temp[0]) + ", " + String(temp[1]);
  dataString += ", " + String(temp[2]) + ", " + String(temp[3]);
  dataString += ", " + String(temp[4]) + ", " + String(temp[5]);
  dataString += ", " + String(temp[6]) + ", " + String(temp[7]);
  dataString += ", " + String(temp[8]);
  Serial.println(dataString);
  File logFile = SD.open("LOG.csv", FILE_WRITE);
  if(logFile)
  {
    logFile.println(dataString);
    logFile.close();
  //  Serial.println(dataString);
  }
}

