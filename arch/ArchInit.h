#ifndef __ARCHINIT_H__
#define __ARCHINIT_H__

#include <stdint.h>
#include "drivers/display/Display.h"

class ArchInit {
 public:
  static void Init(uint32_t mb_magic, void *mb_header);
};

#endif
