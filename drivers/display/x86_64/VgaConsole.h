#ifndef _VGA_CONSOLE_H__
#define _VGA_CONSOLE_H__

#include "../Display.h"

class VgaConsole : public Display {
public:
  void PrintChar(char);

private:
  bool Scroll();

  static const int rows = 25;
  static const int cols = 80;
  int pos = 0;
};

#endif
