#include "HX711.h"

#define calibration_factor 26374 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define DT  6
#define SCK  5

HX711 scale(SCK, DT);

int hallsensor = 7;                // Hall sensor at pin 2
volatile byte counter;
unsigned int rpm;
unsigned long passedtime;
int gearNum = 10;

void setup() {
  Serial.begin(9600);
  
  pinMode(hallsensor, INPUT); //Sets hallsensor as input might want input_pullup
  attachInterrupt(digitalPinToInterrupt(hallsensor), isr, RISING); //Interrupts are called on Rise of Input
  counter = 0;
  rpm = 0;
  passedtime = 0; //Initialise the values
 
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
  Serial.println("HX711 scale demo");
  Serial.println("Readings:");
}

void loop() {
  delay(500);//Update RPM 10 times a second
  detachInterrupt(0); //Interrupts are disabled
  rpm = (60.0*(1000.0/(millis() - passedtime))*counter)/gearNum;
  passedtime = millis();
  Serial.print("Counter= ");
  Serial.println(counter);
  counter = 0;
  Serial.print("RPM=");
  Serial.println(rpm); //Print out result to monitor

  attachInterrupt(digitalPinToInterrupt(hallsensor), isr, RISING);   //Restart the interrupt processing
 
  Serial.print("Reading: ");
  Serial.print(-1*(scale.get_units()), 1); //scale.get_units() returns a float
  Serial.print(" lbs"); //You can change this to kg but you'll need to refactor the calibration_factor
  Serial.println();
}

void isr(){
   //Each rotation, this interrupt function is run twice, so take that into consideration for
   //calculating RPM
   //Update count
      counter++;
}
