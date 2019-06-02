#ifndef __FRAME_H__
#define __FRAME_H__

#include <stdint.h>

#define MAX_MEM_REGIONS 8
#define MAX_EXCLUDE_REGIONS 4

class Frame {
 public:
  static const uint32_t FRAME_SIZE = 4096;

  static int AddMemRegion(uint64_t start, uint64_t end);
  static int AddMemExcludeRegion(uint64_t start, uint64_t end);
  static void PrintMemRegions();
  static void* Alloc();
  static void Free(void *);
 private:
   static int TotalRegions;
   static uint64_t MemReg[MAX_MEM_REGIONS][3];
   static int TotalEx;
   static uint64_t MemEx[MAX_EXCLUDE_REGIONS][2];
};

#endif
