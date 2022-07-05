/*
========================================================================================================================================
LED Lights (Web Control)
========================================================================================================================================
This tagline deals with LED Lights which is controlled by GPIO at pins 3, 4, and 5. It uses the 
web server to control the LED from Off to On.
@package        LED Light (Web Control)
@copyright      Copyright (C) 2018. Benjamin Lu
@license        GNU General Public License v2 or later (http://www.gnu.org/licenses/gpl-2.0.html)
@author         Benjamin Lu (https://www.benjlu.com/)
========================================================================================================================================
*/

/*
========================================================================================================================================
Table of Content
========================================================================================================================================
 1.0 - Required Files
 2.0 - Variables
 3.0 - Setup
 4.0 - Loop
 5.0 - redLED Checkbox
 6.0 - yellowLED Checkbox
 7.0 - greenLED Checkbox
========================================================================================================================================
*/

/*
========================================================================================================================================
 1.0 - Required Files
========================================================================================================================================
*/
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <Arduino.h>
#include <SensirionI2CSdp.h>
#include <Wire.h>



/*
========================================================================================================================================
 2.0 - Variables
========================================================================================================================================
*/
byte *mac = Ethernet.localMAC();                      // Enter MAC Address of 86Duino (Educake)
IPAddress ip(192, 168, 100, 49);                      // Enter Static IP Address of 86Duino (Educake)
EthernetServer web_server(80);                        // Creating a Web Server Using Port 80

String http_request;                                  // This will stored the http request

#define CONFIG_FILE "config.txt"
#define KEY_MAX_LENGTH 30
#define VALUE_MAX_LENGTH 30

SensirionI2CSdp sdp;

String name;
String tagline;





/*
========================================================================================================================================
 3.0 - Setup
========================================================================================================================================
*/
void setup() {
  Ethernet.begin(mac, ip);                            // Initializing Ethernet Communications
  web_server.begin();                                 // Start to Lisen for clients
  Serial.begin(9600);                                 // Open Serial Communications for Diagnostics

  name    = keyString( F( "name" ) );
  tagline = getTagline( F( "tagline" ) );


   Wire.begin();

    uint16_t error;
    char errorMessage[256];

    sdp.begin(Wire, SDP8XX_I2C_ADDRESS_0);

    uint32_t productNumber;
    uint8_t serialNumber[8];
    uint8_t serialNumberSize = 8;

    sdp.stopContinuousMeasurement();

    error = sdp.readProductIdentifier(productNumber, serialNumber,
                                      serialNumberSize);
    if (error) {
        Serial.print("Error trying to execute readProductIdentifier(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.print("ProductNumber:");
        Serial.print(productNumber);
        Serial.print("\t");
        Serial.print("SerialNumber:");
        Serial.print("0x");
        for (size_t i = 0; i < serialNumberSize; i++) {
            Serial.print(serialNumber[i], HEX);
        }
        Serial.println();
    }

    error = sdp.startContinuousMeasurementWithDiffPressureTCompAndAveraging();

    if (error) {
        Serial.print(
            "Error trying to execute "
            "startContinuousMeasurementWithDiffPressureTCompAndAveraging(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }
}

/*
========================================================================================================================================
 4.0 - Loop
========================================================================================================================================
*/
void loop() { 
  EthernetClient client = web_server.available();       // Try to get a client

  if (client) {                                         // Got Client
    boolean currentLineIsBlank = true;

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        http_request += c;

        if (c == '\n' && currentLineIsBlank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: Closed");
          client.println();

          /*
          ==============================================================================================================================
          Start the Web Page that contains all information for the LED Lights to display.
          ==============================================================================================================================
          */
          client.println("<!DOCTYPE html");
          client.println("<html lang=\"en-US\">");
          client.println("<head>");
          client.println("<title>LED Lights (Web Control)</title>");
          client.println("<meta charset=\"UTF-8\" />");
          client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
          client.println("<link href=\"https://gmpg.org/xfn/11\" rel=\"profile\">");
          client.println("<style type=\"text/css\">");
          client.println("body { background: #eeeeee; font-family: Trebuchet MS; margin: 1.125em 0; padding: 0; }");
          client.println("h1 { display: block; font-size: 2em; font-weight: 700; margin: 0.67em 0; }");
          client.println("h2 { display: block; font-size: 1.5em; font-weight: 700; margin: 0.83em 0; }");
          client.println("h3 { display: block; font-size: 1.17em; font-weight: 700; margin: 1em 0; }");
          client.println("h4 { display: block; font-size: 1em; font-weight: 700; margin: 1.33em 0; }");
          client.println("h5 { didplay: block; font-size: 0.83em; font-weight: 700; margin: 1.67em; }");
          client.println("h6 { display: block; font-size: 0.67em; font-weight: 700; margin: 2.33em 0; }");
          client.println(".site-container { background: #ffffff; border: 0.063em solid #cccccc; clear: both; content: ""; display: table; margin: 0 auto; padding: 1.125em; width: 70.875em }");
          client.println(".site-header { margin-bottom: 1.125em; padding: 1.125em; }");
          client.println(".site-title, .site-description { margin: 0; padding: 0; text-align: center; }");
          client.println(".site-address { margin: 0; padding: 0; text-align: center; }");
          client.println(".led-grid-system { text-align: center; }");
          client.println(".led-grid-system ul { float: left; list-style: none; margin: 0; padding: 0; width: 100%; }");
          client.println(".led-grid-system li { float: left; list-style: none; margin: 0; padding: 0; width: 33.33333333333333%; }");
          client.println(".led-lights { margin: 0;  padding: 0; }");
          client.println("</style>");
          client.println("</head>");
          client.println("<body>");
          client.println("<section class=\"site-container\">");
          client.println("<header class=\"site-header\">");
          client.println("<h1 class=\"site-title\">");
          client.println( name );
          client.println("</h1>");
          client.println("<h3 class=\"site-description\">");
          client.println( tagline );  
          client.println("</h3>");
          client.println("<h4 class=\"site-address\">");
          client.println(Ethernet.localIP());
          client.println("</h4>");
          client.println("</header>");
          client.println("<div class=\"led-grid-system\">");
          client.println("<ul>");
          client.println("<li>");
          client.println("<h2 class=\"led-lights\">System Information</h2>");
          client.println("<p>Click to turn on and off the Red LED Light.</p>");
          client.println("<div class=\"system-information\">");
          client.println("<table>");
          client.println("<tbody>");
          client.println("<tr>");
          client.println("<th>Model</th>");
          client.println("<td>");
          client.println("86Duino Zero");
          client.println("</td>");
          client.println("</tr>");
          client.println("<tr>");
          client.println("<th>MAC Address</th>");
          client.println("<td>");
          macAddress( client );
          client.println("</td>");
          client.println("</tr>");
          client.println("</tbody>");
          client.println("</table>");
          client.println("</div>");
          client.println("</li>");
          client.println("<li>");
          client.println("<h2 class=\"led-lights\">Differiential Pressure</h2>");
          sda(client);
          client.println("</li>");
          client.println("<li>");
          client.println("<h2 class=\"led-lights\">Green LED Light</h2>");
          client.println("<p>Click to turn on and off the Green LED Light.</p>");
          client.println("<form method=\"get\">");
          greenLED_Checkbox(client);          
          client.println("</form>");
          client.println("</li>");
          client.println("</ul>");
          client.println("</div>");
          client.println("</section>");
          client.println("</body>");
          client.println("</html>");
          Serial.print(http_request);
          http_request = "";
          break;
        }

        if (c == '\n') {
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    client.stop();
  }
}

/*
========================================================================================================================================
 5.0 - redLED Checkbox
========================================================================================================================================
*/

void macAddress( EthernetClient cl ) {
  cl.print(mac[0], HEX);
  cl.print(":");
  cl.print(mac[1], HEX);
  cl.print(":");
  cl.print(mac[2], HEX);
  cl.print(":");
  cl.print(mac[3], HEX);
  cl.print(":");
  cl.print(mac[4], HEX);
  cl.print(":");
  cl.println(mac[5], HEX);
}
void sda(EthernetClient cl) {
    uint16_t error;
    char errorMessage[256];

    delay(1000);

    // Read Measurement
    float differentialPressure;
    float temperature;

    error = sdp.readMeasurement(differentialPressure, temperature);

    if (error) {
        cl.print("Error trying to execute readMeasurement(): ");
        errorToString(error, errorMessage, 256);
        cl.println(errorMessage);
    } else {
        cl.print("DifferentialPressure[Pa]:");
        cl.print(differentialPressure);
        cl.print("\t");
        cl.print("Temperature[°C]:");
        cl.print(temperature);
        cl.println();
    }
}

/*
========================================================================================================================================
 6.0 - yellowLED Checkbox
========================================================================================================================================
*/
void yellowLED_Checkbox(EthernetClient cl) {
  
}

/*
========================================================================================================================================
 7.0 - greenLED Checkbox
========================================================================================================================================
*/
void greenLED_Checkbox(EthernetClient cl) {
 
}

String keyString(const __FlashStringHelper * key) {
  char value_string[VALUE_MAX_LENGTH];
  int value_length = SD_findKey(key, value_string);
  return HELPER_ascii2String(value_string, value_length);
}
String getTagline(const __FlashStringHelper * key) {
  char value_string[VALUE_MAX_LENGTH];
  int value_length = SD_findKey(key, value_string);
  return HELPER_ascii2String(value_string, value_length);
}

int SD_findKey(const __FlashStringHelper * key, char * value) {
  File configFile = SD.open(CONFIG_FILE);

  if (!configFile) {
    Serial.print(F("SD Card: error on opening file "));
    Serial.println(CONFIG_FILE);
    return 0;
  }

  char key_string[KEY_MAX_LENGTH];
  char SD_buffer[KEY_MAX_LENGTH + VALUE_MAX_LENGTH + 1]; // 1 is = character
  int key_length = 0;
  int value_length = 0;

  // Flash string to string
  PGM_P keyPoiter;
  keyPoiter = reinterpret_cast<PGM_P>(key);
  byte ch;
  do {
    ch = pgm_read_byte(keyPoiter++);
    if (ch != 0)
      key_string[key_length++] = ch;
  } while (ch != 0);

  // check line by line
  while (configFile.available()) {
    int buffer_length = configFile.readBytesUntil('\n', SD_buffer, 100);
    if (SD_buffer[buffer_length - 1] == '\r')
      buffer_length--; // trim the \r

    if (buffer_length > (key_length + 1)) { // 1 is = character
      if (memcmp(SD_buffer, key_string, key_length) == 0) { // equal
        if (SD_buffer[key_length] == '=') {
          value_length = buffer_length - key_length - 1;
          memcpy(value, SD_buffer + key_length + 1, value_length);
          break;
        }
      }
    }
  }

  configFile.close();  // close the file
  return value_length;
}

String HELPER_ascii2String(char *ascii, int length) {
  String str;
  str.reserve(length);
  str = "";

  for (int i = 0; i < length; i++) {
    char c = *(ascii + i);
    str += String(c);
  }

  return str;
}
