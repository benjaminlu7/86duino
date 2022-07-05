/*
================================================================================================
Read SD Card
================================================================================================
A simple readable script using 86Duino Zero or 86Duino Educake to display the outcome to Serial 
Monitor.
  
@package   Read SD Card
@author    ICOP Technology Inc.
@copyright Copyright 2022. ICOP Technology Inc.
@license   GNU General Public License v2 or later
@link      https://www.gnu.org/licenses/gpl-2.0.html
================================================================================================
*/

/*
================================================================================================
Table of Content
================================================================================================
 1.0 - Required Files
 2.0 - Setup
 3.0 - Loop
================================================================================================
*/

/*
================================================================================================
 1.0 - Required Files
================================================================================================
*/
#include <SD.h>

#define CONFIG_FILE "config.txt"
#define KEY_MAX_LENGTH    30 // change it if key is longer
#define VALUE_MAX_LENGTH  30 // change it if value is longer

String name;
int ip;

/*
================================================================================================
 2.0 - Setup
================================================================================================
*/
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin( 9600 );

  while( ! Serial ) {
    // Wait for serial port to connect. Needed for 86Duino Zero or 86Duino Educake only.  
  }

  name = keyString( F( "name" ) );
}

/*
================================================================================================
 3.0 - Loop
================================================================================================
*/
void loop() {
  // put your main code here, to run repeatedly:

}

String keyString(const __FlashStringHelper * key) {
  char value_string[VALUE_MAX_LENGTH];
  int value_length = SD_findKey(key, value_string);
  return HELPER_ascii2String(value_string, value_length);
}

float keyNumber(const __FlashStringHelper * key) {
  char value_string[VALUE_MAX_LENGTH];
  int value_length = SD_findKey(key, value_string);
  // return HELPER_ascii2Float(value_string, value_length);
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

float HELPER_ascii2Float(char *ascii, int length) {
  int sign = 1;
  int decimalPlace = 0;
  float number  = 0;
  float decimal = 0;

  for (int i = 0; i < length; i++) {
    char c = *(ascii + i);
    if (i == 0 && c == '-')
      sign = -1;
    else {
      if (c == '.')
        decimalPlace = 1;
      else if (c >= '0' && c <= '9') {
        if (!decimalPlace)
          number = number * 10 + (c - '0');
        else {
          decimal += ((float)(c - '0') / pow(10.0, decimalPlace));
          decimalPlace++;
        }
      }
    }
  }

  return (number + decimal) * sign;
}
