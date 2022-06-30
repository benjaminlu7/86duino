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

File configFile;

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

  // re-open the file for reading:
  configFile = SD.open("config.txt");
  if (configFile) {
    Serial.println("config.txt:");
    
    // read from the file until there's nothing else in it:
    while (configFile.available()) {
      Serial.write(configFile.read());
    }
    // close the file:
    configFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

/*
================================================================================================
 3.0 - Loop
================================================================================================
*/
void loop() {
  // put your main code here, to run repeatedly:

}
