#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"
#endif

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

// WiFi AP SSID
#define WIFI_SSID "b"
// WiFi password
#define WIFI_PASSWORD "0"

#define INFLUXDB_URL "https://us-east-1-1.aws.cloud2.influxdata.com/"
#define INFLUXDB_TOKEN "rl3Vg=="
#define INFLUXDB_ORG "d"
#define INFLUXDB_BUCKET "LA"

// Time zone info
#define TZ_INFO "UTC7"

#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET D3
Adafruit_SSD1306 display(OLED_RESET);


#define SS      15
#define RST     10
#define DI0     9
#define BAND    433E6      // set frequency 920-925 MHz

int txPower = 17;         // set txPower 5-17 ,default 17
int spreadingFactor = 12; // set spreadingFactor ranges from 6-12,default 7
unsigned int SBW = 125E3; // set Bandwidth

String value ;

// Declare InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Declare Data point
Point sensor("LoRa_Status"); /////////////////////////////////////////////////////////////////////////////////////////////////////Measurement

void setup() {

  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3c); //initialize I2C addr 0x3c

  while (!Serial); //If just the the basic function, must connect to a computer
  delay (1000);

  LoRa.setPins(SS, RST, DI0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setTxPower(txPower);
  LoRa.setSpreadingFactor(spreadingFactor);
  LoRa.setSignalBandwidth(SBW);
  Serial.println("LoRa Initial OK!");


  display.clearDisplay(); // clears the screen and buffer
  display.drawPixel(127, 63, WHITE);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 13);
  display.println("LoRa Test Signal!!");
  display.setTextColor(WHITE, BLACK);
  display.display();

  // Setup wifi
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Accurate time is necessary for certificate validation and writing in batches
  // We use the NTP servers in your area as provided by: https://www.pool.ntp.org/zone/
  // Syncing progress and the time will be printed to Serial.
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");


  // Check server connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }

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



      value = "";
      for (int j = 0; j < packetSize ; j++) { // array [0],[1],[2],[3],[4],[5]
        value += packetrecriver[j];
      }

    }

    display.clearDisplay();
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(WHITE);        // Draw white text

    display.setCursor(0, 0);            // Start at top-left corner
    display.print("Data >>");
    display.setCursor(55, 0);
    display.print(value);
    display.setCursor(100, 0);
    display.print("Int.");

    display.setCursor(0, 12);
    display.print("RSSI >>");
    display.setCursor(55, 12);
    display.print(LoRa.packetRssi());
    display.setCursor(100, 12);
    display.print("dBm.");

    display.setCursor(0, 22);
    display.print("SNR  >>");
    display.setCursor(55, 22);
    display.print(LoRa.packetSnr());
    display.setCursor(100, 22);
    display.print("dB.");

    display.setTextColor(WHITE, BLACK);
    display.display();

    // print RSSI of packet
    Serial.print("with RSSI => ");
    Serial.println(LoRa.packetRssi()); //RSSI packet
    Serial.print("with SNR => ");
    Serial.println(LoRa.packetSnr());   //SNR packet
    Serial.println(value);

    /////////////////////////////////////////////////////////////////////////////////////////*********

    // Clear fields for reusing the point. Tags will remain the same as set above.
    sensor.clearFields();

    int val = value.toInt();
    int snr = ((LoRa.packetSnr())*100);
   

    // Store measured value into point
    // Report RSSI of currently connected network
    sensor.addField("WiFi_Rssi", WiFi.RSSI()); /////////////////////////////////////////////////////////////////////////////////////////////////////field
    sensor.addField("LoRa_Rssi", LoRa.packetRssi());
    sensor.addField("LoRa_SNR", snr);
    sensor.addField("LoRa_Value", val);


    // Print what are we exactly writing
    Serial.print("Writing: ");
    Serial.println(sensor.toLineProtocol());

    // Check WiFi connection and reconnect if needed
    if (wifiMulti.run() != WL_CONNECTED) {
      Serial.println("Wifi connection lost");
    }

    // Write point
    if (!client.writePoint(sensor)) {
      Serial.print("InfluxDB write failed: ");
      Serial.println(client.getLastErrorMessage());
    }

    Serial.println("Waiting 1 second");
    delay(1000);

    //////////////////////////////////////////////////////////////////////////////////////////********

  }
}
