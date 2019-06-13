#ifndef __IRQ_H__
#define __IRQ_H__

#include <stdint.h>

class IRQ {
 public:
  static void Init();
  static void Enable();
  static void Disable();
  static void SetMask(int);
  static void ClearMask(int);
  static void Register(int, void (*hndlr)(uint32_t, uint32_t));
  static void BlockForIRQ();
};

#endif
