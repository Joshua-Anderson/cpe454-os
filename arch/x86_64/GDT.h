#ifndef __GDT_H__
#define __GDT_H__

#include <stdint.h>

class GDT {
 public:
  static uint16_t CS_SEG;
  static uint16_t TSS_SEG;

  static void InitGDT();
  static void InitTSS();
  static void LoadIST(int num, void* sp);
};

#endif
