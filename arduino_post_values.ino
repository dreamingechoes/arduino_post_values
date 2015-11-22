/*
 * arduino_post_values.ino
 *
 * Program that sends sensors values from an Arduino board to an RESTful API.
 * Author: Ivan Gonzalez (a.k.a dreamingechoes)
 */

#include <EtherCard.h>
#include "Arduino.h"
#include "Dht11.h"

// RESTful API token for authentication.
// Change it to the one you need.
#define TOKEN  "ac4bd425935c350a54aabb362906283f"

// Ethernet interface mac address, must be unique on the LAN
byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

// Global variables
const char website[] PROGMEM = "sensors-admin-panel.herokuapp.com";
static byte session;
int next = 1;
static uint32_t timer;
enum {
  DHT_DATA_PIN = 2,
  SERIAL_BAUD  = 9600,
  POLL_DELAY   = 2000,
};

byte Ethernet::buffer[700];
Stash stash;

/*
* function sendToAPI
*
*   int id: ID of the sensor on the final RESTful API application
*   int value: value of the sensor measure that will be send
*/
static void sendToAPI (int id, int value) {
  byte sd = stash.create();

  // Assign the values to the request params.
  stash.print("token=");
  stash.print(TOKEN);
  stash.print("&value=");
  stash.print(value);
  stash.print("&sensor_id=");
  stash.print(id);
  stash.save();
  int stash_size = stash.size();

  // Compose the http POST request, taking the headers below and appending
  // previously created stash in the sd holder.
  Stash::prepare(PSTR("POST https://$F$F HTTP/1.0" "\r\n"
    "Host: $F" "\r\n"
    "Content-Length: $D" "\r\n"
    "\r\n"
    "$H"),
  website, PSTR("/api/v1/measures"), website, stash_size, sd);

  // Send the packet. This also releases all stash buffers once done.
  // Save the session ID so we can watch for it in the main loop.
  session = ether.tcpSend();
}

void setup () {
  // Setup the Ethernet chip
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
  int temp_val = 0;
  int hum_val = 0;
  static Dht11 sensor(DHT_DATA_PIN);

  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);

  // Control when we could execute the next HTTP request
  if (millis() > timer)
    switch (sensor.read()) {
      case Dht11::OK:
        switch (next) {
          case 1:
            // Read humidity level
            Serial.print("Humidity (%): ");
            hum_val = sensor.getHumidity();
            Serial.println(hum_val);
            if (millis() > timer) {
              sendToAPI(1, hum_val);
              timer = millis() + 5000;
            }
            next = 2;
            // Added delay to allow the sensor to obtain measurements correctly
            delay(2000);
            break;

          case 2:
            // Read temperature level
            Serial.print("Temperature (C): ");
            temp_val = sensor.getTemperature();
            Serial.println(temp_val);
            if (millis() > timer) {
              sendToAPI(2, temp_val);
              timer = millis() + 5000;
            }
            next = 1;
            // Added delay to allow the sensor to obtain measurements correctly
            delay(2000);
            break;
        }
        break;

      // Manage ERROR_CHECKSUM DHT11 error
      case Dht11::ERROR_CHECKSUM:
        Serial.println("Checksum error");
        break;

      // Manage ERROR_TIMEOUT DHT11 error
      case Dht11::ERROR_TIMEOUT:
        Serial.println("Timeout error");
        break;

      default:
        Serial.println("Unknown error");
        break;
    }
  }
}
