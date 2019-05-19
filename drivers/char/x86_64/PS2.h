#ifndef _PS2_H__
#define _PS2_H__

#include "../CharIn.h"

class PS2 : public CharIn {
 public:
  PS2();
  char GetChar();
  char GetCharIRQ();

 private:
  bool shift = false;

  char GetLetter(char c);
};

#endif
