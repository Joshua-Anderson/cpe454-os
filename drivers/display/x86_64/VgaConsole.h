#ifndef _VGA_CONSOLE_H__
#define _VGA_CONSOLE_H__

#include "../Color.h"
#include "../Display.h"

class VgaConsole : public Display {
 public:
  void PrintChar(char);
  void PrintChar(char, uint8_t fg, uint8_t bg);
  void PrintChar(char, uint8_t fg, uint8_t bg, unsigned row, unsigned col);
  void Clear();
  unsigned Rows();
  unsigned Cols();

 private:
  bool Scroll();

  static const unsigned rows = 25;
  static const unsigned cols = 80;
  uint8_t bg = Color::Black;
  uint8_t fg = Color::White;
  unsigned pos = 0;
};

#endif
