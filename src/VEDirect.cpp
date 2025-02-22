/*
 * License: See LICENSE file in the repository
 * Repository: https://github.com/Zylence/VEDirect
 * Author: Julian Kirsch
 * Date: 18.02.2025
 */

#include "VEDirect.h"

const char* _frame_delimiter = "Checksum"; // +2 bytes
const int8_t _frame_delimiter_length = 8;

VEDirect::VEDirect(Stream* serial, FrameHandler fh, LogHandler lh, int buffer_size, bool use_checksum) : 
  _serial(serial), frame_handler(fh), log_handler(lh), _buffer_size(buffer_size), _use_checksum(use_checksum) {
  
  _buffer = new char[buffer_size];
  if (buffer_size < 256) {
    log("Buffer size is smaller than recommended - This may cause problems");
  }
}

VEDirect::~VEDirect() {
  delete[] _buffer;
}

void VEDirect::read() {
  int unread_checksum_bytes = 2;
  bool ending_frame = false;

  while (_serial->available() > 0) {
    char incomingChar = _serial->read(); // Reads one char

    // Prevent overflow (should never trigger)
    if (_buffer_pos < _buffer_size - 1) {
      // Add char to buffer and null terminate
      _buffer[_buffer_pos++] = incomingChar;
      _buffer[_buffer_pos] = '\0';

      // Check if the incoming message ends with the checksum
      // todo this is a bit expensive
      if (_buffer_pos >= _frame_delimiter_length && 
          strcmp(&_buffer[_buffer_pos - _frame_delimiter_length], _frame_delimiter) == 0) {
        ending_frame = true;
        //delay(10);
      } else if (ending_frame) {
        // countdown until all expected ending bytes are read
        unread_checksum_bytes -= 1;

        // frame ended
        if (unread_checksum_bytes == 0) {
          // parse only if checksum is valid
          if (_use_checksum) {
            if (is_checksum_valid()) {
              log("Valid Checksum");
              parse();
            } else {
              log("Invalid Checksum");
            }
          }
          // parse blindly
          else {
            parse();
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
  char* buffer_ptr = _buffer + 2;
  
  char* line;
  char* line_saveptr = NULL;
  char* key_val_saveptr = NULL;

  char* label;
  char *value;

  log(buffer_ptr);
  line = strtok_r(buffer_ptr, "\n", &line_saveptr);
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

inline void VEDirect::log(const char* message) {
  if (log_handler != nullptr) {
    log_handler(message);
  }
}

inline void VEDirect::notify(const char* key, const char* value) {
  if (frame_handler != nullptr) {
    frame_handler(key, value);
  }
}

//https://www.victronenergy.com/live/vedirect_protocol:faq#q8how_do_i_calculate_the_text_checksum
//Mod sum of all chars in frame should equal 0
inline bool VEDirect::is_checksum_valid() 
{
  int checksum = 0;
  for (int i = 0; i < strlen(_buffer); i++) {
    checksum = (checksum + _buffer[i]) & 255;
  }
  return checksum == 0;
}

/* todo test
inline bool is_frame_ending() {
  if (_buffer_pos >= _fame_delimiter_length) {
    for (int8_t i = 0; i < _fame_delimiter_length; ++i) {
        if (_buffer[_buffer_pos - _fame_delimiter_length + i] != _fame_delimiter[i]) {
            return false;
        }
    }
    return true;
  }
}
*/
