/*
 Example using the SparkFun HX711 breakout board with a scale
 By: Nathan Seidle
 SparkFun Electronics
 Date: November 19th, 2014
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

 This example demonstrates basic scale output. See the calibration sketch to get the calibration_factor for your
 specific load cell setup.

 This example code uses bogde's excellent library: https://github.com/bogde/HX711
 bogde's library is released under a GNU GENERAL PUBLIC LICENSE

 The HX711 does one thing well: read load cells. The breakout board is compatible with any wheat-stone bridge
 based load cell which should allow a user to measure everything from a few grams to tens of tons.
 Arduino pin 2 -> HX711 CLK
 3 -> DAT
 5V -> VCC
 GND -> GND

 The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.

*/

#include "HX711.h"
#include <SPI.h>
#include <SD.h>

#define calibration_factor 26374 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define DT  6
#define SCK  5

HX711 scale(SCK, DT);

float reading;

File myFile; //txt
File dataFile; //csv
String dataString = "";
String fileName = "022122-1";

// change this to match your SD shield or module;
const int chipSelect = 10;

int hallsensor = 7;                // Hall sensor at pin 7
volatile byte counter;
unsigned int rpm;
unsigned long passedtime;
double horsepower;
double armLength = 1.3958333333; //in feet
int gearNum = 10;

void setup() {
  Serial.begin(9600);

  pinMode(hallsensor, INPUT); //Sets hallsensor as input might want input_pullup
  attachInterrupt(digitalPinToInterrupt(hallsensor), isr, RISING); //Interrupts are called on Rise of Input
  counter = 0;
  rpm = 0;
  horsepower = 0;
  passedtime = 0;

  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  myFile = SD.open(""+fileName+".txt", FILE_WRITE);
  if (myFile) {
    myFile.println("Load Cell Readings:");
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening txt");
  }

  dataFile = SD.open(""+fileName+".csv", FILE_WRITE);
  if (dataFile){
    dataFile.println("Load Cell Readings");
    dataFile.close(); // close the file
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening csv");
  }
  
  //scale.begin(DOUT, CLK);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0

  //myFile.println("Readings:");
}

void loop() {
  delay(50);//Update RPM 20 times a second
  detachInterrupt(0); //Interrupts are disabled
  rpm = 60.0*(1000.0/(millis() - passedtime))*counter/gearNum;
  passedtime = millis();
  counter = 0;
  attachInterrupt(digitalPinToInterrupt(hallsensor), isr, RISING);   //Restart the interrupt processing

  horsepower = 1.0*scale.get_units()*armLength*rpm/5252; //5252 is constant conversion for rpm in horsepower
  
  Serial.print("RPM=");
  Serial.println(rpm); //Print out result to monitor

  reading = scale.get_units();
  dataString = String(millis()) + "," + String(horsepower) + "," + String(reading) + "," + String(rpm);
  saveDataCSV();
  saveDataTXT();
//  if(Serial.available()){
//    char temp = Serial.read();
//    if(temp == 'e')
//      myFile.close();
//      Serial.println("Stopping");
//      return;
//  }
}

void saveDataTXT(){
  myFile = SD.open(""+fileName+".txt", FILE_WRITE);
  if (myFile) {
    myFile.print("Reading: ");
    myFile.print(reading, 1); //scale.get_units() returns a float
    myFile.print(" lbs "); //You can change this to kg but you'll need to refactor the calibration_factor
    myFile.print(rpm);
    myFile.print(" rpm");
    myFile.println();
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

void saveDataCSV(){
  dataFile = SD.open(""+fileName+".csv", FILE_WRITE);
  if (dataFile){
    dataFile.println(dataString);
    dataFile.close(); // close the file
  }else{
    Serial.println("Error writing to file !");
  }
}

void isr(){
   //Each rotation, this interrupt function is run twice, so take that into consideration for
   //calculating RPM
   //Update count
      counter++;
}
