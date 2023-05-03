
#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
       #define DEBUG_ESP_PORT Serial
       #define NODEBUG_WEBSOCKETS
       #define NDEBUG
#endif 

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>
#define RELAYPIN_1 D1
#define RELAYPIN_2 D2
#define RELAYPIN_3 D3
struct RelayInfo {
  String deviceId;
  String name;
  int pin;
};

std::vector<RelayInfo> relays = {
    {"643a53ac312d40edc3f0ce20", "Relay 1", RELAYPIN_1},
    {"643a5359312d40edc3f0cd8a", "Relay 2", RELAYPIN_2},
    {"644d70a8918a3c911c8ce22b", "Relay 3", RELAYPIN_3}};

#define WIFI_SSID  "5G"
#define WIFI_PASS  "Souravboss"
#define APP_KEY    "2f42f35b-39c3-4a71-b6ed-ddeaa481804e"    
#define APP_SECRET "44080d54-472c-4c22-ab5b-0001a4630673-72fe8f59-83bd-4b28-a164-628e4a913522" 

#define BAUD_RATE  9600        

bool onPowerState(const String &deviceId, bool &state) {
  for (auto &relay : relays) {                                                            // for each relay configuration
    if (deviceId == relay.deviceId) {                                                       // check if deviceId matches
      Serial.printf("Device %s turned %s\r\n", relay.name.c_str(), state ? "on" : "off");     // print relay name and state to serial
      digitalWrite(relay.pin, state);                                                         // set state to digital pin / gpio
      return true;                                                                            // return with success true
    }
  }
  return false; // if no relay configuration was found, return false
}

void setupRelayPins() {
  for (auto &relay : relays) {    // for each relay configuration
    pinMode(relay.pin, OUTPUT);     // set pinMode to OUTPUT
  }
}

void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

void setupSinricPro() {
  for (auto &relay : relays) {                             // for each relay configuration
    SinricProSwitch &mySwitch = SinricPro[relay.deviceId];   // create a new device with deviceId from relay configuration
    mySwitch.onPowerState(onPowerState);                     // attach onPowerState callback to the new device
  }

  SinricPro.onConnected([]() { Serial.printf("Connected to SinricPro\r\n"); });
  SinricPro.onDisconnected([]() { Serial.printf("Disconnected from SinricPro\r\n"); });

  SinricPro.begin(APP_KEY, APP_SECRET);
}

void setup() {
  Serial.begin(BAUD_RATE);
  setupRelayPins();
  setupWiFi();
  setupSinricPro();
}

void loop() {
  SinricPro.handle();
}
