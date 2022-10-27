/*
========================================================================================================================================
SDP810-500pa (Web Control) 
========================================================================================================================================
This SDP810-500pa (Web Control) is used as a sensor for outputing differiential pressure and temperature. This project also allows users
to pre-define variables using config.txt file which can be modify.

@package   SDP810-500pas (Web Control)
@author    ICOP Technology Inc. <https://www.icoptech.com>
@copyright Copyright 2022, ICOP Technology Inc.
@license   GNU General Public License v2 or later
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
========================================================================================================================================
*/

/*
========================================================================================================================================
 1.0 - Required Files
========================================================================================================================================
*/
#include <Arduino.h>
#include <SensirionI2CSdp.h>
#include <Wire.h>
#include <Ethernet.h>
#include <SD.h>
#include <SPI.h>



/*
========================================================================================================================================
 2.0 - Variables
========================================================================================================================================
*/
#define debug 0
#define spi_flash 0
#define dhcp 0

#define CONFIG_FILE "config.txt"
#define KEY_MAX_LENGTH 30
#define VALUE_MAX_LENGTH 30

// size of buffer used to capture HTTP requests
#define REQ_BUF_SZ   50


EthernetServer server(80);
FILE *fp;
float differentialPressure;
float temperature;

char HTTP_req[REQ_BUF_SZ] = {0}; 
char req_index = 0;
char spf[50];

char * commands[] = { "getTitle", "getTagline", "getMacAddress", "getIPAddress", "getPrimarySensor", "getSecondarySensor" };
enum { getTitle, getTagline, getMacAddress, getIPAddress, getPrimarySensor, getSecondarySensor };
const int commands_count=sizeof(commands)/sizeof(commands[0]);

byte *mac = Ethernet.localMAC();

String name    = my_findString( "name" );
String tagline    = my_findString( "tagline" );

int ip1 = my_findInt( "ip1" );
int ip2 = my_findInt( "ip2" );
int ip3 =  my_findInt( "ip3" );
int ip4 = my_findInt( "ip4" );

IPAddress ip(ip1, ip2, ip3, ip4);

SensirionI2CSdp sdp;
SensirionI2CSdp sdp1;

void setup() {
    Serial.begin(115200);
   
    #if debug    
        // if debug mode waiting for Serial console 
//        delay( 3000 );
        while( ! Serial ) { }
    #endif
//=============== SDP800 init =========================================
    Wire.begin();
    sdp.begin(Wire, SDP8XX_I2C_ADDRESS_0);
    sdp1.begin(Wire, SDP8XX_I2C_ADDRESS_1);
    sdp.startContinuousMeasurementWithDiffPressureTCompAndAveraging();
    sdp1.startContinuousMeasurementWithDiffPressureTCompAndAveraging();
//====================================================================      
    
    #if spi_flash
        // show whether it's boot from SD or internal SPI flash
        Serial.println("Run from SPI Flash");
        #else
        Serial.println("Run from SD card");
    #endif
    
    #if dhcp
        Ethernet.begin();
    #else
        Ethernet.begin(mac, ip);
    #endif
    
    server.begin();
    
    Serial.print("Server is running at ip: ");       
    Serial.println(Ethernet.localIP());     
}

void loop()
{
    int cmd;
    
    EthernetClient client = server.available();

    if (client) {
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {
                char c = client.read(); 

                if (req_index < (REQ_BUF_SZ - 1)) {
                    HTTP_req[req_index] = c;   
                    req_index++;
                }

                if (c == '\n' && currentLineIsBlank) {
                    
                  if (StrContains(HTTP_req, "GET /")) {
                    cmd=findCommand (HTTP_req);
                    switch(cmd) {

                        case getTitle :
                                        
                                        sprintf(spf, "{\"value\":\"%s\"}\n", name.c_str()); 
                                        client.print(spf);
                                        break;
                                     
                        case getTagline :
                                        sprintf( spf, "{\"value\":\"%s\"}\n", tagline.c_str() );  
                                        client.print(spf);
                                        break;

                        case getMacAddress :
                            sprintf( spf, "{\"value\":\"%s\"}\n", displayMacAddress(Ethernet.localMAC()).c_str() );  
                            client.print(spf);
                            break;

                        case getIPAddress :
                            sprintf( spf, "{\"value\":\"%s\"}\n", displayIPAddress(Ethernet.localIP()).c_str() );  
                            client.print(spf);
                            break;
                            
                        case getPrimarySensor:
                                        readPrimarySensor(&differentialPressure, &temperature);
                                        sprintf(spf, "{\"pressure\":\"%d.%02d\",\"temperature\":\"%d.%02d\"}\n",(int)differentialPressure, (int)(fabsf(differentialPressure)*100)%100, (int)temperature, (int)(fabsf(temperature)*100)%100 );
                                        client.print(spf);
                                        break;

                        case getSecondarySensor:
                                        readSecondarySensor(&differentialPressure, &temperature);
                                        sprintf(spf, "{\"pressure\":\"%d.%02d\",\"temperature\":\"%d.%02d\"}\n",(int)differentialPressure, (int)(fabsf(differentialPressure)*100)%100, (int)temperature, (int)(fabsf(temperature)*100)%100 );
                                        client.print(spf);
                                        break;

                          default :
                                  if (StrContains(HTTP_req, "GET / ")  || StrContains(HTTP_req, "GET /index.htm")) {

                                        http200ok(client);
                                        fp = fopen("/index.htm", "r");
                                        char cc = fgetc(fp);
                                        while (cc != EOF) {
                                            client.write(cc);
                                            cc = fgetc(fp);
                                        }  
                                        fclose(fp);
                                        break;  
                                  }
                    }         // swistch end

                      req_index = 0;
                      StrClear(HTTP_req, REQ_BUF_SZ);
                      break;
                  }
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

void readPrimarySensor(float* differentialPressure, float* temperature) {
  float dp, tmp;
  sdp.readMeasurement(dp, tmp);
  *differentialPressure=dp;
  *temperature=tmp;
}

void readSecondarySensor(float* differentialPressure, float* temperature) {
  float dp, tmp;
  sdp1.readMeasurement(dp, tmp);
  *differentialPressure=dp;
  *temperature=tmp;
}

String displayMacAddress(byte *address) {
    return  String(address[0], HEX) + "0:" + 
            String(address[1], HEX) + ":" + 
            String(address[2], HEX) + ":" + 
            String(address[3], HEX) + ":" +
            String(address[4], HEX);
}

String displayIPAddress(IPAddress address) {
    return  String(address[0]) + "." + 
            String(address[1]) + "." + 
            String(address[2]) + "." + 
            String(address[3]);
}

void StrClear(char *str, char length) {
    for (int i = 0; i < length; i++) {
        str[i] = 0;
    }
}

char StrContains(char *str, char *sfind) {
    char found = 0;
    char index = 0;
    char len;

    len = strlen(str);
    if (strlen(sfind) > len) {
        return 0;
    }
    while (index < len) {
        if (str[index] == sfind[found]) {
            found++;
            if (strlen(sfind) == found) {
                return 1;
            }
        }
        else {
            found = 0;
        }
        index++;
    }
    return 0;
}

void http200ok(EthernetClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connnection: close");
  client.println();
}



int findCommand (char* searchText) {
    int count = 0;
    int foundIndex = -1;  // -1 = not found
    while (count < commands_count)  {
        if (StrContains (searchText, commands[count] ))  {
            foundIndex = count;
            break;
        }
        count++;
    }
    return foundIndex;
}

String my_findString(char * key) {
  char value_string[VALUE_MAX_LENGTH];
  int value_length = my_findKey(key, value_string);
  return HELPER_ascii2String(value_string, value_length);
}

int my_findInt(char * key) {
  char value_string[VALUE_MAX_LENGTH];
  int value_length = my_findKey(key, value_string);
  return HELPER_ascii2Int(value_string, value_length);
}

int my_findKey(char * key, char * value) {
  FILE *fp=fopen(CONFIG_FILE, "r");
  if(fp == NULL) {
    Serial.print("Error on opening file ");
    Serial.println(CONFIG_FILE);
    return 0;
  }
 
  char my_buffer[KEY_MAX_LENGTH + VALUE_MAX_LENGTH + 1];
  int key_length = strlen(key);
  int value_length = 0;
  while (fgets(my_buffer, 255, (FILE*)fp) != NULL) {
    int buffer_length=strlen(my_buffer)-1;
    my_buffer[buffer_length]='\0';

    if (buffer_length > (key_length + 1)) {
    if (memcmp(my_buffer, key, key_length) == 0) { 
      if (my_buffer[key_length] == '=') {
        value_length = buffer_length - key_length - 1;
        memcpy(value, my_buffer + key_length + 1, value_length);
        value[value_length]='\0';
        break;
        }
      }
    }

  }
  fclose(fp);  
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

int HELPER_ascii2Int(char *ascii, int length) {
  int sign = 1;
  int number = 0;

  for (int i = 0; i < length; i++) {
    char c = *(ascii + i);
    if (i == 0 && c == '-')
      sign = -1;
    else {
      if (c >= '0' && c <= '9')
        number = number * 10 + (c - '0');
    }
  }

  return number * sign;
}
