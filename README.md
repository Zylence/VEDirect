# [UNDER CONSTRUCTION - TESTING ONGOING]
# VEDirect Library for Arduino

This library handles reading VE.Direct TEXT frames from a hardware or software serial connection assuring data integrity with checksum calculation. 

## Features
- **Checksum Calculation**: Automatically calculates and validates checksums for received frames.
- **Event-Driven**: Utilizes convenient callback functions to handle incoming data frames and logging messages.
- **Configurable**: Checksum calculation can be opted out, buffer size can be set, callbacks can be changed at runtime.
- **Flexibility**: You get to choose how you parse your value readings on a per label basis.

## Usage Example

Here’s a simple example of how to use the VEDirect library:

```cpp
#include "VEDirect.h"

void frame_handler(const char* label, const char* value) {
    // When the checksum is correct, every entry in the frame is passed to this handler.
}

void log_handler(const char* message) {
    // Receives diagnostic information from the library which you may or may not use.
}

// Instanciate and configure the library.
VEDirect ve(&Serial1, frame_handler, log_handler, /*256, true*/);

void setup() {
    Serial1.begin(19200); // Initialize your serial connection as you see fit
}

void loop() {
    ve.read(); // Call to read VE.Direct
}
```

## Contributing

Contributions are welcome, **especially testing on different architectures**. For code changes, please create an issue for discussion before making a PR.

## Supports
- ESP32
- Potentially others
