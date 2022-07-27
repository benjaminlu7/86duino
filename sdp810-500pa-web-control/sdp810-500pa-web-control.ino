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
#include <Ethernet.h>
#include <SD.h>
#include <Arduino.h>
#include <ArduinoMDNS.h>
#include <SensirionI2CSdp.h>
#include <SPI.h>
#include <Wire.h>

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
#define REQ_BUF_SZ   30


EthernetServer server(80);
FILE *fp;

char HTTP_req[REQ_BUF_SZ] = {0}; 
char req_index = 0;
char spf[50];

char * commands[] = { "getSensors", "getTitle", "getTagline"};
enum {getSensors, getTitle, getTagline};
const int commands_count=sizeof(commands)/sizeof(commands[0]);

String name;
String tagline;

void setup() {
    Serial.begin(115200);   
    pinMode(A0, INPUT) ;
    pinMode(A1, INPUT) ;
    
    
    #if debug    
        // if debug mode waiting for Serial console 
        delay( 1000 );
        while( ! Serial ) {
          
        }
        delay( 1000 );
    #endif
    
    #if spi_flash
        // show whether it's boot from SD or internal SPI flash
        Serial.println("Run from SPI Flash");
        #else
        Serial.println("Run from SD card");
    #endif
    
    #if dhcp
      ;
    #else
        byte ip[] = { 192, 168, 100, 147 };    
    #endif
    
    #if dhcp
        Ethernet.begin();
    #else
        Ethernet.begin(Ethernet.localMAC(), ip);
    #endif
    
    server.begin();
    
    Serial.print("Server is running at ip: ");       
    Serial.println(Ethernet.localIP());     
    
    name = my_findString("name");
    
    Serial.println( name );
}

void loop()
{
    int cmd;
    name    = my_findString( "name" );
    tagline = my_findString( "tagline" );
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

                        case  getSensors :
                                        sprintf(spf, "{\"analog0\":\"%d\",\"analog1\":\"%d\"}\n", random(0,100), random(0,100) );
                                        client.print(spf);
                                        break;

                        case getTitle :
                                        
                                        sprintf(spf, "{\"value\":\"%s\"}\n", name.c_str()); 
                                        client.println(spf);
                                        break;
                                     
                        case getTagline :
                                        sprintf( spf, "{\"value\":\"%s\"}\n", tagline.c_str() );  
                                        client.print(spf);
                                        break;

                          default :
                                  if (StrContains(HTTP_req, "GET / ")  || StrContains(HTTP_req, "GET /web/index.htm")) {
                                        http200ok(client);
                                        fp = fopen("/web/index.htm", "r");
                                        char cc = fgetc(fp);
                                        while (cc != EOF) {
                                            client.write(cc);
                                            cc = fgetc(fp);
                                        }  
                                        fclose(fp);
                                        break;  
                                  }
                    }         // switch end

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
