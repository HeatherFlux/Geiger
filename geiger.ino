#include <FreqCounter.h>  //FREQ
#include <Wire.h>         //For RTC
#include "RTClib.h"        //For RTC
RTC_DS1307 RTC;            //For RTC
#include <SD.h>            //For log
#include <SPI.h>
const int chipSelect = 4;
File myFile;

void setup() {

//Start Serial
 Serial.begin(57600);                    // connect to the serial port
 Serial.println("Frequency Counter");
 
//Start RTC
 Wire.begin();              
 RTC.begin();
 if (! RTC.isrunning()) {
   Serial.println("RTC is NOT running!");}
 if (RTC.isrunning()){
   Serial.println("RTC is running");}
   
//Check SD
 Serial.print("Initializing SD card...");
 
 // see if the card is present and can be initialized:
 if (!SD.begin(chipSelect)) {
   Serial.println("Card failed, or not present");
   digitalWrite(2, HIGH);   // set the LED on
   digitalWrite(3, LOW);   // set the LED off
   return;
 }
 
 Serial.println("card initialized.");
 
//Prep File
 //file open
 myFile = SD.open("test.txt", FILE_WRITE);
 //write to it:
 if (myFile) {
   Serial.print("Writing to test.txt...");
   myFile.println("timestamp,date,time, cps,");
   //note consistant data structure here
   //timestamp, date, time, cps<frq>
  // close the file:
   myFile.close();
   Serial.println("ready.");
 }
}



long int cpm;
long int frq;



void loop() {

///FREQ Counter components
   FreqCounter::f_comp= 10;    // Set compensation to 12
//////////////////////IMPORTANT CHANGE INTERVAL FOR READINGS HERE///////////////  
   FreqCounter::start(60000);   // Start counting with gatetime of 1000ms
   while (FreqCounter::f_ready == 0)         // wait until counter ready

   frq=FreqCounter::f_freq;            // read result
   Serial.print("cps:");
   Serial.println(frq);                // print result
 
///RTC components
   DateTime now = RTC.now();

   Serial.print(now.year(), DEC);  
   Serial.print('/');
   Serial.print(now.month(), DEC);
   Serial.print('/');
   Serial.print(now.day(), DEC);
   Serial.print(' ');
   Serial.print(now.hour(), DEC);
   Serial.print(':');
   Serial.print(now.minute(), DEC);
   Serial.print(':');
   Serial.print(now.second(), DEC);
   Serial.print(':');
   Serial.print("gmt");
   Serial.println();
   Serial.println();

///SAFETY
// use the LEDS for warning !!
//2CPS=117CPM=1.02µSv/hr=24.65µSv/dy=9000µSv/yr 9mSv/yr Airline safe dose

if (frq <= 11)  //11Cps=649cpm=5.70µSv/hr=136.98µSv/dy=50000µSv/yr 50mSv/yr Nuclear Worker Limit
 {
  digitalWrite(3, HIGH);
  digitalWrite(2, LOW);
 }
 else if (frq >11 && frq <= 22) //22Cps=1305cpm=11.415µSv/hr=273.96µSv/dy=100000µSv/yr 100000mSv/yr Threshold for detectable cancer risk
 {
  digitalWrite(3, HIGH);
  digitalWrite(2, HIGH);
 }
 else if (frq > 22) //
 {
  digitalWrite(3, LOW);
  digitalWrite(2, HIGH);
 }
///2=RED 3=GREEN

/// Logging

//file open
 myFile = SD.open("test.txt", FILE_WRITE);
 
 //write to it:
  if (myFile) {
    Serial.print("logging...");
    //note consistant data structure here
    //timestamp, date, time, cps<frq>
    myFile.print(now.unixtime());
    myFile.print(',');
    myFile.print(now.year(), DEC); 
    myFile.print('/');
    myFile.print(now.month(), DEC);
    myFile.print('/');
    myFile.print(now.day(), DEC);
    myFile.print(',');
    myFile.print(now.hour(), DEC);
    myFile.print(':');
    myFile.print(now.minute(), DEC);
    myFile.print(':');
    myFile.print(now.second(), DEC);
    myFile.print(':');
    myFile.print("gmt");
    myFile.print(',');
    myFile.println(frq);
   // close the file:
    myFile.close();
    Serial.println("done");
  }

}
