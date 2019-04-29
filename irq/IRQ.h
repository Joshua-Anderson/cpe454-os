#ifndef _IRQ_H__
#define _IRQ_H__

class IRQ {
 public:
  static void Init();
  static void Enable();
  static void Disable();
  static char SetMask(int);
  static char ClearMask(int);
};

#endif
