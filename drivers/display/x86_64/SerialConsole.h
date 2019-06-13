#ifndef _SERIAL_CONSOLE_H__
#define _SERIAL_CONSOLE_H__

#include "../Display.h"

class SerialConsole : public Display {
 public:
  SerialConsole();
  void Clear();
  void PrintChar(char);
  void PrintChar(char, uint8_t, uint8_t) {}
  void PrintChar(char, uint8_t, uint8_t, unsigned, unsigned) {}
  unsigned Rows() { return -1; };
  unsigned Cols() { return -1; };
};

#endif
