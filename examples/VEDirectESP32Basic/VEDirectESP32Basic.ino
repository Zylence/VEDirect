/*
* This example reads VE.Direct Protocol from a SoftwareSerial (VEDirectSerial) connection
* does parse some values and enables logging to HardwareSerial (Serial)
*/

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <VEDirect.h>

float voltage;
float current;

inline float convert(const char* value) {
  return (float) strtol(value, NULL, 10)/1000;
}

// Handle VE.Direct labels and values 
void frame_handler(const char* label, const char* value) {
  if (strcmp(label, "V") == 0) {
    voltage = convert(value);
  } else if (strcmp(label, "I") == 0) {
    current = convert(value);
  }
  // ...
}

// Write all library logs to Serial
void log_handler(const char* message) {
  Serial.println(message);
}

SoftwareSerial VEDirectSerial(18,19); //RX , TX
VEDirect ve(&VEDirectSerial, frame_handler, log_handler);

void setup() {
  VEDirectSerial.begin(19200);
  VEDirectSerial.flush();
  Serial.begin(9600);
}

void loop() {
  ve.read();
}
