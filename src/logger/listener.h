#pragma once
#include "buffer.h"

#define LOG_BUFFER_SIZE 256

class LoggedFrame: public Printable {
public:
  uint32_t time;
  uint8_t port;
  CAN_message_t frame;

  size_t printTo(Print& p) const;
};

size_t LoggedFrame::printTo(Print& p) const {
  size_t s = 0;

  s += p.print(time);
  s += p.write('\t');

  s += p.print(port);
  s += p.write('\t');

  s += p.print(frame.id, HEX);
  s += p.write('\t');

  // Length is implicitly defined by number of chars in data field
  // s += p.print(frame.len, HEX);
  // s += p.write('\t');

  for (size_t c = 0; c < frame.len; ++c) {
    if (frame.buf[c] < 16) s += p.write('0');
    s += p.print(frame.buf[c], HEX);
  }

  return s;
}

class CommonListener: public CANListener {
public:
  const uint8_t port;
  FIFOCircBuffer<LoggedFrame, LOG_BUFFER_SIZE> buffer;

  CommonListener(uint8_t port): port(port) {}
  void gotFrame(CAN_message_t &frame, int mailbox);
};

void CommonListener::gotFrame(CAN_message_t &frame, int mailbox) {
  LoggedFrame loggedframe;

  loggedframe.time = millis();
  loggedframe.port = port;
  loggedframe.frame = frame;

  buffer.add(loggedframe);
}
