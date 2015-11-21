#include <EtherCard.h>
#include "Arduino.h"
#include "Dht11.h"

#define TOKEN  "ac4bd425935c350a54aabb362906283f"

byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

const char website[] PROGMEM = "sensors-admin-panel.herokuapp.com";
static byte session;
byte Ethernet::buffer[700];
int next = 1;
static uint32_t timer;

Stash stash;
enum {
  DHT_DATA_PIN = 2,
  SERIAL_BAUD  = 9600,
  POLL_DELAY   = 2000,
};

static void sendToAPI (int id, int value) {
  byte sd = stash.create();

  stash.print("token=");
  stash.print(TOKEN);
  stash.print("&value=");
  stash.print(value);
  stash.print("&sensor_id=");
  stash.print(id);
  stash.save();
  int stash_size = stash.size();

  Stash::prepare(PSTR("POST https://$F$F HTTP/1.0" "\r\n"
    "Host: $F" "\r\n"
    "Content-Length: $D" "\r\n"
    "\r\n"
    "$H"),
  website, PSTR("/api/v1/measures"), website, stash_size, sd);

  session = ether.tcpSend();
}

void setup () {
  Serial.begin(57600);
  Serial.println("\n[API Client]");

  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
    Serial.println(F("Failed to access Ethernet controller"));
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);

  if (!ether.dnsLookup(website))
    Serial.println(F("DNS failed"));

  ether.printIp("SRV: ", ether.hisip);
}

void loop () {
  static Dht11 sensor(DHT_DATA_PIN);
  int temp_val = 0;
  int hum_val = 0;

  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);

  if (millis() > timer) {
    switch (sensor.read()) {
      case Dht11::OK:
        switch (next) {
          case 1:
            Serial.print("Humidity (%): ");
            hum_val = sensor.getHumidity();
            Serial.println(hum_val);
            if (millis() > timer) {
              sendToAPI(1, hum_val);
              timer = millis() + 5000;
            }
            next = 2;
            delay(2000);
            break;

          case 2:
            Serial.print("Temperature (C): ");
            temp_val = sensor.getTemperature();
            Serial.println(temp_val);
            if (millis() > timer) {
              sendToAPI(2, temp_val);
              timer = millis() + 5000;
            }
            next = 1;
            delay(2000);
            break;
        }
        break;

      case Dht11::ERROR_CHECKSUM:
        Serial.println("Checksum error");
        break;

      case Dht11::ERROR_TIMEOUT:
        Serial.println("Timeout error");
        break;

      default:
        Serial.println("Unknown error");
        break;
    }
  }
}
