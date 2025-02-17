/*
 * License: See LICENSE file in the repository
 * Repository: https://github.com/username/repository
 * Author: Julian Kirsch
 * Date: 18.02.2025
 */

#include "VEDirect.h"

VEDirect::VEDirect(Stream* serial, FrameHandler fh, LogHandler lh, int bufferSize, bool useChecksum) : 
  _serial(serial), frame_handler(fh), log_handler(lh), _buffer_size(bufferSize), _use_checksum(useChecksum) {
  
  _buffer = new char[bufferSize];
}

VEDirect::~VEDirect() {
  delete[] _buffer;
}

void VEDirect::scan() {
  int unreadChecksumBytes = 2;
  bool endingFrame = false;

  while (_serial->available() > 0) {
    char incomingChar = _serial->read(); // Reads one char

    // Prevent overflow (should never trigger)
    if (_buffer_pos < _buffer_size - 1) {
      // Add char to buffer and null terminate
      _buffer[_buffer_pos++] = incomingChar;
      _buffer[_buffer_pos] = '\0';

      // Check if the incoming message ends with the checksum
      // todo this is a bit expensive
      if (_buffer_pos >= _fame_delimiter_length && 
          strcmp(&_buffer[_buffer_pos - _fame_delimiter_length], _fame_delimiter) == 0) {
        endingFrame = true;
        //delay(10);
      } else if (endingFrame) {
        // countdown until all expected ending bytes are read
        unreadChecksumBytes -= 1;

        // frame ended
        if (unreadChecksumBytes == 0) {
          if (!_use_checksum || is_checksum_valid()) {
            log("Valid Checksum");
            parse();
          } else {
            log("Invalid Checksum");
          }

          // Reset the buffer for the next iteration
          memset(_buffer, 0, _buffer_size);
          _buffer_pos = 0;
        }
      }
    } else {
      // Buffer overflow, reset the index
      _buffer_pos = 0;
      log("Buffer Overflow - choose larger buffersize");
    }
  }
}

// https://www.victronenergy.com/live/vedirect_protocol:faq
inline void VEDirect::parse() {
  // skip first two bytes
  _buffer = _buffer + 2;
  
  char* line;
  char* line_saveptr = NULL;
  char* key_val_saveptr = NULL;

  char* label;
  char *value;

  log(_buffer);
  line = strtok_r(_buffer, "\n", &line_saveptr);
  // iterate lines
  while (line != NULL) {

    // extract label and value
    label = strtok_r(line, "\t\r", &key_val_saveptr);
    if (label != NULL) {
      value = strtok_r(NULL, "\t\r", &key_val_saveptr);
    }

    if (value != NULL) {
      notify(label, value);
    }
    
    line = strtok_r(NULL, "\n", &line_saveptr);
  }
}

inline void VEDirect::log(char* message) {
  if (log_handler != nullptr) {
    log_handler(message);
  }
}

inline void VEDirect::notify(char* key, char* value) {
  if (frame_handler != nullptr) {
    frame_handler(key, value);
  }
}

inline bool VEDirect::is_checksum_valid() 
/*
Um die Prüfsumme zu berechnen, müssen Sie alle ASCII-Werte addieren. 
Das Ergebnis sollte ein Vielfaches von 256 sein. (Modulo % 256;)
Wenn Sie alles in einer Bytevariablen zusammenfassen, sollte das Ergebnis 0 sein.
*/
{
  int checksum = 0;
  for (int i = 0; i < strlen(_buffer); i++) {
    checksum = (checksum + _buffer[i]) & 255;
  }

  return checksum == 0;
}
