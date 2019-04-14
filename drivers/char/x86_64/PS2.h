#ifndef _PS2_H__
#define _PS2_H__

#include "../CharDev.h"

class PS2 : public CharDev {
public:
  void Init();
  char GetChar();
};

#endif
