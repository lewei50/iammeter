/**
 * IotWebConf01Minimal.ino -- IotWebConf is an ESP8266/ESP32
 *   non blocking WiFi/AP web configuration library for Arduino.
 *   https://github.com/prampec/IotWebConf 
 *
 * Copyright (C) 2020 Balazs Kelemen <prampec+arduino@gmail.com>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

/**
 * Example: Minimal
 * Description:
 *   This example will shows the bare minimum required for IotWebConf to start up.
 *   After starting up the thing, please search for WiFi access points e.g. with
 *   your phone. Use password provided in the code!
 *   After connecting to the access point the root page will automatically appears.
 *   We call this "captive portal".
 *   
 *   Please set a new password for the Thing (for the access point) as well as
 *   the SSID and password of your local WiFi. You cannot move on without these steps.
 *   
 *   You have to leave the access point before to let the Thing continue operation
 *   with connecting to configured WiFi.
 *
 *   Note that you can find detailed debug information in the serial console depending
 *   on the settings IOTWEBCONF_DEBUG_TO_SERIAL, IOTWEBCONF_DEBUG_PWD_TO_SERIAL set
 *   in the IotWebConf.h .
 */

#include <IotWebConf.h>
#include <IotWebConfUsing.h> // This loads aliases for easier class names.
#include <IotWebConfOptionalGroup.h>

// UpdateServer includes
// For ESP32 IotWebConf provides a drop-in replacement for UpdateServer.
# include <IotWebConfESP32HTTPUpdateServer.h>

// -- Initial name of the Thing. Used e.g. as SSID of the own Access Point.
const char thingName[] = "WDP";

// -- Initial password to connect to the Thing, when it creates an own Access Point.
const char wifiInitialApPassword[] = "12345678";

#define STRING_LEN 128
#define NUMBER_LEN 32

// -- Configuration specific key. The value should be modified if config structure was changed.
#define CONFIG_VERSION "1.01"

// -- When CONFIG_PIN is pulled to ground on startup, the Thing will use the initial
//      password to buld an AP. (E.g. in case of lost password)
#define CONFIG_PIN 35

#define LED_BUILTIN  (4)    //led

// -- Status indicator pin.
//      First it will light up (kept LOW), on Wifi connection it will blink,
//      when connected to the Wifi it will turn off (kept HIGH).
#define STATUS_PIN LED_BUILTIN

// -- Maximal length the input-range attributes can have.
#define RANGE_ATTR_LENGTH 60


// -- Method declarations.
void handleRoot();
// -- Callback methods.
void wifiConnected();
void configSaved();
void parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base);

DNSServer dnsServer;
WebServer server(80);
HTTPUpdateServer httpUpdater;

byte ip[4];


char MeterIp[STRING_LEN];
char metertypeParamValue[NUMBER_LEN];

IotWebConf iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword, CONFIG_VERSION);

iotwebconf::OptionalParameterGroup MeterGroup = iotwebconf::OptionalParameterGroup("IAMMETER DSP", "IAMMETER", false);
iotwebconf::TextParameter MeterIpParam = iotwebconf::TextParameter("IP address", "MeterIp", MeterIp, STRING_LEN, "192.168.1.6");
iotwebconf::NumberParameter metertypeParam = iotwebconf::NumberParameter("MeterType 1:Single 3:Three", "MeterType", metertypeParamValue, NUMBER_LEN, "3", "1..3", "min='1' max='3' step='1'");

// -- An instance must be created from the class defined above.
iotwebconf::OptionalGroupHtmlFormatProvider optionalGroupHtmlFormatProvider;
