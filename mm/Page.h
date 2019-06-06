#ifndef __PAGE_H__
#define __PAGE_H__

#include <stdint.h>

#include "Frame.h"

class Page {
 public:
  static const uint32_t PAGE_SIZE = Frame::FRAME_SIZE;
  static const uint64_t KSTACK_START_ADDR = 0x8000000000; // 2 MB
  static const uint32_t KTHREAD_STACK_SIZE = 0x200000; // 2 MB

  static uint64_t KernStackOffSet;
  static void* AllocKernStackMem();
  static void InitIdentityMap();
  static Page* GetCurrentPage();

  Page();
  void Load();
  void* Alloc(int region, uint64_t size);
  void* GetPageTableLocation();

 private:
    static Page* CurrentPage;
    void* PTableLoc;
};

#endif
