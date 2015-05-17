/*
  AT command mode with Serial Monitor for HC-05
 
 Simplifies setup of the serial port Bluetooth module HC-05.
 
 Serial Monitor settings:
 * Both NL & CR
 * 9600 baud
 
 created 15 Dec 2014
 by Tobias Johansson
 
 http://ecgbiozholter.johansson.xyz
 
 */
#include <SoftwareSerial.h>

SoftwareSerial BTSerial(10, 11); // RX, TX

char c;
boolean first_c = true;

void setup()
{
  // Open serial communication
  Serial.begin(9600);
  Serial.println("Enter AT commands: ");

  // Set the data rate of the Software Serial port
  BTSerial.begin(38400); // HC-05 baud rate in the AT command mode
}

void loop()
{
  // Arduino Serial Monitor >> HC-05
  if (Serial.available()) {
    c = Serial.read(); // receives
    if (first_c) {Serial.print("sent > "); first_c = false;} // indicates the direction of data
    if (c == '\n') first_c = true;
    BTSerial.write(c); // transmits
    Serial.print(c);   // and displays in Serial Monitor
  }
  
  // Arduino Serial Monitor << HC-05
  if (BTSerial.available()) {
    c = BTSerial.read(); // receives
    if (first_c) {Serial.print("received > "); first_c = false;} // indicates the direction of data
    if (c == '\n') first_c = true;
    Serial.write(c); // transmits and displays in Serial Monitor
  }
}
