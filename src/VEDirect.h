/*
 * License: See LICENSE file in the repository
 * Repository: https://github.com/username/repository
 * Author: Julian Kirsch
 * Date: 18.02.2025
 */

#ifndef VEDirect_h
#define VEDirect_h

#include <Arduino.h>
#include <Stream.h>

/**
 * @typedef FrameHandler
 * @brief Callback function passing label (metric) / value (measurement) pairs.
 *
 * This callback is executed multiple times per frame, once per read line
 * with the label and value VEDirect writes. If checksum checking is enabled this will only be
 * called when the checksum of the entire frame was correct. 
 *
 * @param label (V, I, P ...)
 * @param value Value, may be string or integer
 */
typedef void (*FrameHandler)(char* label, char* value);

/**
 * @typedef LogHandler
 * @brief Callback function passing debug information to the consumer
 *
 * This optional callback is executed to pass debug infromation from the library to the consumer.
 * The callback may be logged to console.
 *
 * @param message Message logged by the library
 */
typedef void (*LogHandler)(char* message);

/**
 * @class VE
 * @brief Enables reading VEDirect messages from a serial connection.
 */
class VEDirect {
public:
  VEDirect(Stream* serial, FrameHandler fh, LogHandler lh = nullptr, int bufferSize = 256, bool useChecksum = true);
  virtual ~VEDirect();

  /**
    * @var LogHandler log_handler
    * @brief Callback for logs from the library.
    *
    * May be changed at runtime.
    * May be set null. 
    */
  LogHandler log_handler;

  /**
    * @var FrameHandler frame_handler
    * @brief Callback for individual lines from VEDirect frames.
    *
    * May be changed at runtime.
    * May be set null. 
    */
  FrameHandler frame_handler;

  /**
  * @brief Scans the serial connection for VEDirect frames.
  * 
  * Should be called in loop().
  * May need to be delayed.
  */
  void scan();

private:
  Stream* _serial;

  int _buffer_size;
  char* _buffer;
  bool _use_checksum;
  int _buffer_pos = 0;
  const char* _fame_delimiter = "Checksum"; // +2 bytes
  const int _fame_delimiter_length = 8;
  
  void log(char* message);
  void notify(char* key, char* value);
  void parse();
  bool is_checksum_valid();
};

#endif