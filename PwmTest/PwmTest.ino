/*
  Analog input, analog output, serial output
 
 Reads an analog input pin, maps the result to a range from 0 to 255
 and uses the result to set the pulsewidth modulation (PWM) of an output pin.
 Also prints the results to the serial monitor.
 
 The circuit:
 * potentiometer connected to analog pin 0.
   Center pin of the potentiometer goes to the analog pin.
   side pins of the potentiometer go to +5V and ground
 * LED connected from digital pin 9 to ground
 
 created 29 Dec. 2008
 modified 9 Apr 2012
 by Tom Igoe
 
 This example code is in the public domain.
 
 */

// These constants won't change.  They're used to give names
// to the pins used:
const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int analogOutPin = 9; // Analog output pin that the LED is attached to

int sensorValue = 0;        // value read from the pot
int RampDir = 10;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
 
  pinMode(11, OUTPUT);
  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS21);
  OCR2A = 80;
 
  
}

void loop() {
  // read the analog in value:
  
  if(sensorValue > 200) RampDir = -5;            
  if(sensorValue <= 0 ) RampDir = 5;
  
  //Adjust Sensor Value
  sensorValue = sensorValue + RampDir;
  OCR2A = sensorValue;
  //Set output Value 
  //analogWrite(analogOutPin, sensorValue);

  // wait 2 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  delay(300);                 
}
