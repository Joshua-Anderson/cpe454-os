#ifndef _SERIAL_CONSOLE_H__
#define _SERIAL_CONSOLE_H__

#include "../Display.h"

class SerialConsole : public Display {
 public:
  void PrintChar(char);
  SerialConsole();
};

#endif
