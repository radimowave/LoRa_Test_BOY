#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
//#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR 0x27 // กำหนดตำแหน่ง Address ของ I2C
// #define I2C_ADDR 0x3F // ในบางกรณี Address ของ I2C เป็น 0x3f
//ให้เปลี่ยน 0x27 เป็น 0x3F

//#define BACKLIGHT_PIN 13


//LiquidCrystal_I2C lcd(I2C_ADDR, 2, 1, 0, 4, 5, 6, 7);
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SS      15         //D8
#define RST     10         //SD3
//#define DI0     9          //SD2 ตัวรับไม่ต้องต่อ ต่อแล้วจะ upload ไม่ได้
#define BAND    433E6      // set frequency 433 MHz

//SPI ESP8266
//CLK CLK/ 6
//MISO SD0 /7
//MOSI SD1 /8

int txPower = 17;         // set txPower 5-17 ,default 17
int spreadingFactor = 12; // set spreadingFactor ranges from 6-12,default 7
unsigned int SBW = 125E3; // set Bandwidth

String valu ;

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
  LoRa.setSpreadingFactor(spreadingFactor);
  LoRa.setSignalBandwidth(SBW);
  Serial.println("LoRa Initial OK!");

  // lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  // lcd.setBacklight(HIGH);
  // initialize the LCD
	lcd.begin();
  	// Turn on the blacklight and print a message.
	lcd.backlight();
	//lcd.home (); // ไปที่ตัวอักษรที่ 0 แถวที่ 1
  lcd.setCursor(0, 0); // ไปทตัวอักษรที่ 1 แถวที่ 1
  lcd.print("MeTaRa LoRa Test");
  lcd.setCursor(1, 1); // ไปที่ตัวอักษรที่ 5 แถวที่ 2
  lcd.print("Pleases Wait...");

  Serial.println();

  //lcd.clear();

}

void loop() {

  int packetSize = LoRa.parsePacket();
  char packetrecriver [packetSize]; // [0],[1],[2],[3],[4],[5]
  if (packetSize) {

    Serial.print("packetSize =");
    Serial.println(packetSize);

    while (LoRa.available()) {


      for (int i = 0; i < packetSize; i++)
      {
        packetrecriver[i] = ((char)LoRa.read()); // [0],[1],[2],[3],[4],[5]
      }



      valu = "";
      for (int j = 0; j < packetSize ; j++) { // array [0],[1],[2],[3],[4],[5]
        valu += packetrecriver[j];
      }

    }

    lcd.clear();
    lcd.setCursor(0, 0);            // Start at top-left corner
    lcd.print("RSSI>>");
    lcd.setCursor(7, 0);
    lcd.print(LoRa.packetRssi());
    Serial.println(LoRa.packetRssi());

    lcd.setCursor(0, 1);
    lcd.print("SNR>");
    lcd.setCursor(5, 1);
    lcd.print(LoRa.packetSnr());

    lcd.setCursor(11, 1);
    lcd.print("A");
    lcd.setCursor(12, 22);
    lcd.print(valu);


    // print RSSI of packet
    Serial.print("with RSSI => ");
    Serial.println(LoRa.packetRssi()); //RSSI packet
    Serial.print("with SNR => ");
    Serial.println(LoRa.packetSnr());   //SNR packet
    Serial.println(valu);

  }
}
