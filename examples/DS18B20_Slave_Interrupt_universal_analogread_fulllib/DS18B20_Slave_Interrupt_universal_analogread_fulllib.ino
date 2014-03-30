#define dsslavepin 12

#include <OneWireSlave.h>

//                     {Fami, <---, ----, ----, ID--, ----, --->,  CRC} 
unsigned char rom[8] = {0x28, 0xAD, 0xDA, 0xCE, 0x0F, 0x00, 0x11, 0x00};
//                            {TLSB, TMSB, THRH, TLRL, Conf, 0xFF, Rese, 0x10,  CRC}
unsigned char scratchpad[9] = {0x7F, 0x09, 0x4B, 0x46, 0x7F, 0xFF, 0x00, 0x10, 0x00};
//                                       {TLSB, TMSB}
unsigned char scratchpadtemperature[2] = {0x7F, 0x09};

OneWireSlave ds(dsslavepin);

//Blinking
const int ledPin =  13;
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated
long interval = 250;            // interval at which to blink (milliseconds)

String serialFeedcomplete;
float value, oldvalue;
boolean expose = true;

void setup() {
  Serial.begin(9600);
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for Leonardo only
//  }
  pinMode(ledPin, OUTPUT);
  ds.init(rom);
  ds.setScratchpad(scratchpad);
  ds.setPower(PARASITE);
  ds.setResolution(9);
  value = -55;
  ds.attach44h(temper);
}

void loop() {
  blinking();
  
  serialRead();
  //Serial.println(value);
  //ds.waitForRequest(false);
}

void temper() {
  int16_t degree = value * 16;
  scratchpadtemperature[0] = degree & 0xFF;
  scratchpadtemperature[1] = degree >> 8;
  //scratchpadtemperature[0] = scratchpadtemperature[0] + 1;
  ds.setTemperature(scratchpadtemperature);
}

void blinking() {
  unsigned long currentMillis = millis(); 
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
  }
}

void serialRead() {
  if (expose) {
    Serial.println("Vorgabe Temperatur: ");
    expose = false;
  }
  
  int analog = analogRead(A5);
  analog = map(analog, 1, 1023, -1023, 1023);  
  
  value = (float)analog/10;
  
  if (value != oldvalue){
    //Serial.print(value);
    //Serial.println(" C");
    oldvalue = value;
  }
}
