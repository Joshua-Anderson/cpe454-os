#ifndef __PAGE_H__
#define __PAGE_H__

#include <stdint.h>

#include "Frame.h"

class Page {
 public:
  static const uint32_t PAGE_SIZE = Frame::FRAME_SIZE;
  static const uint64_t KHEAP_START_ADDR = 0x20000000000;
  static const uint64_t KSTACK_START_ADDR = 0x10000000000;
  static const uint32_t KTHREAD_STACK_SIZE = 0x200000;  // 2 MB

  static uint64_t KernStackPos;
  static uint64_t KernHeapPos;
  static void* AllocKernStackMem();
  static void FreeKernStackMem(void*);
  static void* AllocKernHeapPage();
  static void* AllocKernHeap(uint32_t);
  static void FreeKernHeapPage(void*);
  static void FreeKernHeap(void*, uint32_t);
  static void InitIdentityMap();

  Page();
  void Load();
  void* Alloc(int region, uint64_t size);

 private:
  void* PTableLoc;
};

#endif
