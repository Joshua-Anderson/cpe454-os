#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "drivers/display/Display.h"

class Platform {
 public:
  static Display* GetDflConsole();
  static Display* GetDflDisplay();
};

#endif
