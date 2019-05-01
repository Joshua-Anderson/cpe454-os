#ifndef _IRQ_H__
#define _IRQ_H__

class IRQ {
 public:
  static void Init();
  static void Enable();
  static void Disable();
  static void SetMask(int);
  static void ClearMask(int);
};

#endif
