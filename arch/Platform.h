#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "drivers/char/CharIn.h"
#include "drivers/display/Display.h"
#include "mm/Page.h"

class Platform {
 public:
  static Display* GetDflConsole();
  static Display* GetDflDisplay();
  static CharIn* GetDflInput();
  static Page* GetDflPageTable();
};

#endif
