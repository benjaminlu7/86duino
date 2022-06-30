/*
================================================================================================
Mac Address
================================================================================================
A simple text that shows the value using 86Duino Zero or 86Duino Educake to display the outcome
to Serial Monitor.
  
@package   Mac Address
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
#include <SPI.h>
#include <Ethernet.h>

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

  // Here, we are going to make sure check if we are connected.
  if ( Ethernet.begin() == 0 ) {
    Serial.println( "Failed to Retrieve MAC Address" );
  }

  // Display Hello World to Serial Monitor.
  delay( 1000 );
  Serial.println( "Retrieving Mac Address" );
  delay( 2000 );
  byte *mac = Ethernet.localMAC();
  
  Serial.print(mac[0], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.println(mac[5], HEX);
}

/*
================================================================================================
 3.0 - Loop
================================================================================================
*/
void loop() {
  // put your main code here, to run repeatedly:

}
