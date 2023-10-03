#include <SPI.h>
#include <LoRa.h>

#define SS      10
#define RST     8
#define DI0     2
#define BAND    433E6   // set frequency 410-525 MHz

int counter = 0;
int txPower = 14;         // Supported values are 0 to 14 for PA_OUTPUT_RFO_PIN. ** Ra-02 Aithinker
int spreadingFactor = 12; // set spreadingFactor ranges from 6-12,default 7
unsigned int SBW = 125E3; // set Bandwidth


void setup() {

  Serial.begin(9600);
  while (!Serial); //If just the the basic function, must connect to a computer
  delay (1000);



  LoRa.setPins(SS, RST, DI0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }


  //LoRa.setTxPower(txPower);

  LoRa.setTxPower(17, 20); //Supported values are 2 to 20 for PA_OUTPUT_PA_BOOST_PIN, and 0 to 14 for PA_OUTPUT_RFO_PIN.

  LoRa.setSpreadingFactor(spreadingFactor);
  LoRa.setSignalBandwidth(SBW);
  Serial.println("LoRa Initial OK!");

  pinMode(7, OUTPUT);

  digitalWrite(7, HIGH);
  delay(500);
  digitalWrite(7, LOW);;

}

void loop() {

  
  LoRa.beginPacket();
  LoRa.print(counter); // send data
  LoRa.endPacket();

  Serial.print(counter);
  Serial.println(" Send OK!!");

  counter++;
  
  digitalWrite(7, HIGH);
  delay(500);
  digitalWrite(7, LOW);

  delay(5000); //************************************ interval

}
