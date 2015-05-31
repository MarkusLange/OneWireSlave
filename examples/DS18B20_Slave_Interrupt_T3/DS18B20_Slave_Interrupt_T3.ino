#include <OneWireSlave.h>

//                     {Fami, <---, ----, ----, ID--, ----, --->,  CRC} 
unsigned char rom[8] = {0x28, 0xAD, 0xDA, 0xCE, 0x0F, 0x00, 0x11, 0x00};
//                            {TLSB, TMSB, THRH, TLRL, Conf, 0xFF, Rese, 0x10,  CRC}
unsigned char scratchpad[9] = {0x7F, 0x09, 0x4B, 0x46, 0x7F, 0xFF, 0x00, 0x10, 0x00};
//                                       {TLSB, TMSB}
unsigned char scratchpadtemperature[2] = {0x7F, 0x09};

OneWireSlave ds(12);

//Interrupt
volatile long previousmicros = 0;
volatile long old_previousmicros = 0;
volatile long difference = 0;

//Blinking
const int ledPin =  13;
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated
long interval = 750;            // interval at which to blink (milliseconds)

String serialFeedcomplete;
float value;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  //pinMode(12, INPUT);
  ds.init(rom);
  ds.setScratchpad(scratchpad);
  ds.setPower(PARASITE);
  ds.setResolution(9);
  attachInterrupt(12, DS18B20_1, CHANGE);
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
  int16_t degree = (int16_t)value * 16;
  scratchpadtemperature[0] = degree & 0xFF;
  scratchpadtemperature[1] = degree >> 8;
  //scratchpadtemperature[0] = scratchpadtemperature[0] + 1;
  ds.setTemperature(scratchpadtemperature);
}

void DS18B20_1() {
  //Serial.println("1st Step");
  old_previousmicros = previousmicros;
  previousmicros = micros();
  difference = previousmicros - old_previousmicros;
  //if (difference >= 380 && difference <= 800) { //on Leonardo
  if (difference >= 325 && difference <= 500) {
    ds.waitForRequestInterrupt(false);
//    Serial.println("2nd Step");
//    Serial.println(difference);
  }
}

void blinking() {
  unsigned long currentMillis = millis(); 
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
  }
}

void serialRead(){
  Serial.println("Vorgabe Temperatur: ");
  if (Serial.available() > 0) {
    String serialFeed;
    while(Serial.available()){          // Lese Daten ein
      delay(100);
      char c = Serial.read() ;
      if (c == ';')
      {
        break;
      }
     serialFeed += c;                   // Daten eingelesen in String gespeichert
    }
    if (serialFeedcomplete != serialFeed){
      serialFeedcomplete = serialFeed;
    }
  }
  Serial.print(serialFeedcomplete);
  Serial.println(" C");

  // Überführung von Serialfeed zu float
  int feedlaenge = serialFeedcomplete.length();
  feedlaenge = feedlaenge + 1;  
  char buffer[feedlaenge];
  serialFeedcomplete.toCharArray(buffer,feedlaenge);
  value=atof(buffer);
}
