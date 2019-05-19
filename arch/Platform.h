#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "drivers/display/Display.h"
#include "drivers/char/CharIn.h"

class Platform {
 public:
  static Display* GetDflConsole();
  static Display* GetDflDisplay();
  static CharIn* GetDflInput();
};

#endif
