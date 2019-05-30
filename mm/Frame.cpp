#include "Frame.h"
#include "printk.h"

#define ROUND_UP(num, mult) ((num + mult - 1) / mult) * mult

int Frame::TotalRegions = 0;
int Frame::TotalEx = 0;
uint64_t Frame::MemReg[MAX_MEM_REGIONS][2] = {{0}};
uint64_t Frame::MemEx[MAX_EXCLUDE_REGIONS][2] = {{0}};

int Frame::AddMemRegion(uint64_t start, uint64_t end) {
  if(start % Frame::FRAME_SIZE) {
    return 1;
  }

  end = ROUND_UP(end, Frame::FRAME_SIZE);

  // Don't use the 0ith frame to avoid NULL confusion
  if(start == 0) {
    start = Frame::FRAME_SIZE;
  }

  Frame::MemReg[Frame::TotalRegions][0] = start;
  Frame::MemReg[Frame::TotalRegions][1] = end;
  Frame::TotalRegions++;

  return 0;
}

int Frame::AddMemExcludeRegion(uint64_t start, uint64_t end) {
  if(start % Frame::FRAME_SIZE) {
    return 1;
  }

  end = ROUND_UP(end, Frame::FRAME_SIZE);

  Frame::MemEx[Frame::TotalEx][0] = start;
  Frame::MemEx[Frame::TotalEx][1] = end;
  Frame::TotalEx++;

  return 0;
}

void Frame::PrintMemRegions() {
  printk("\tDetected Memory Regions:\n");
  for(int i = 0; i < Frame::TotalRegions; i++) {
    printk("\t\t%d: 0x%lx - 0x%lx\n", i, Frame::MemReg[i][0], Frame::MemReg[i][1]);
  }
  printk("\tExcluded Memory Regions:\n");
  for(int i = 0; i < Frame::TotalEx; i++) {
    printk("\t\t%d: 0x%lx - 0x%lx\n", i, Frame::MemEx[i][0], Frame::MemEx[i][1]);
  }
}
