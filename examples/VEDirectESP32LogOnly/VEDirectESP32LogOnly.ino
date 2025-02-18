/*
* This example reads VE.Direct Protocol from a SoftwareSerial (VEDirectSerial) connection
* and dumps all library logs to HardwareSerial (Serial) giving you an overview of the data
*/

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <VEDirect.h>

// Write all library logs to Serial
void log_handler(const char* message) {
  Serial.println(message);
}

SoftwareSerial VEDirectSerial(18,19); //RX , TX
VEDirect ve(&VEDirectSerial, nullptr, log_handler);

void setup() {
  VEDirectSerial.begin(19200);
  Serial.begin(9600);
}

void loop() {
  ve.read();
}
