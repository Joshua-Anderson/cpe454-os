#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <stdint.h>

class Display {
 public:
  virtual void PrintChar(char) = 0;
  virtual void PrintChar(char, uint8_t fg, uint8_t bg) = 0;
  virtual void PrintChar(char, uint8_t fg, uint8_t bg, unsigned row,
                         unsigned col) = 0;
  virtual void Clear() = 0;
  virtual unsigned Rows() = 0;
  virtual unsigned Cols() = 0;
};

#endif
