

#include <Arduino.h>
#include <VEDirect.h>

float voltage;
float current;

inline float convert(char* value) {
  return (float) strtol(value, NULL, 10)/1000;
}

// Handle VE.Direct labels and values 
void frame_handler(char* label, char* value) {
  if (strcmp(label, "V") == 0) {
    voltage = convert(value);
  } else if (strcmp(label, "I") == 0) {
    current = convert(value);
  }
  // ...
}

// Write all library logs to Serial
void log_handler(char* message) {
  Serial.println(message);
}

VEDirect ve(&Serial, frame_handler, log_handler);

void setup() {
  Serial.begin(9600);
}

void loop() {
  ve.read();
}
