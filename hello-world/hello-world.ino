/*
================================================================================================
Hello World
================================================================================================
A simple text that shows the value using 86Duino Zero or 86Duino Educake to display the outcome
to Serial Monitor.
  
@package   Hello World
@author    ICOP Technology Inc.
@copyright Copyright 2022. ICOP Technology Inc.
@license   GNU General Public License v2 or later
@link      https://www.gnu.org/licenses/gpl-2.0.html
================================================================================================
*/

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin( 9600 );

  while( ! Serial ) {
    // Wait for serial port to connect. Needed for 86Duino Zero or 86Duino Educake only.  
  }

  // Display Hello World to Serial Monitor.
  delay( 1000 );
  Serial.println( "Hello World!" );
}

void loop() {
  // put your main code here, to run repeatedly:

}
