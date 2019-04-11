#ifndef _VGA_CONSOLE_H__
#define _VGA_CONSOLE_H__

#include "../Display.h"

class VgaConsole : public Display {
public:
  void PrintChar(char);

private:
  void Scroll();

  static const int rows = 25;
  static const int cols = 25;
  int pos = 0;
};

#endif
