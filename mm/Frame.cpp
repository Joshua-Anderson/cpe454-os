#include "Frame.h"
#include <stddef.h>
#include "printk.h"
#include "stdlib.h"

struct FreeFrame {
  struct FreeFrame* nxt;
  struct FreeFrame* prev;
};
struct FreeFrame* free_tail = NULL;

int Frame::TotalRegions = 0;
int Frame::TotalEx = 0;
uint64_t Frame::MemReg[MAX_MEM_REGIONS][3] = {{0}};
uint64_t Frame::MemEx[MAX_EXCLUDE_REGIONS][2] = {{0}};

int Frame::AddMemRegion(uint64_t start, uint64_t end) {
  if (start % Frame::FRAME_SIZE) {
    return 1;
  }

  end = ROUND_UP(end, Frame::FRAME_SIZE);

  // Don't use the 0ith frame to avoid NULL confusion
  if (start == 0) {
    Frame::MemReg[Frame::TotalRegions][2] = Frame::FRAME_SIZE;
  } else {
    Frame::MemReg[Frame::TotalRegions][2] = start;
  }

  Frame::MemReg[Frame::TotalRegions][0] = start;
  Frame::MemReg[Frame::TotalRegions][1] = end;
  Frame::TotalRegions++;

  return 0;
}

int Frame::AddMemExcludeRegion(uint64_t start, uint64_t end) {
  if (start % Frame::FRAME_SIZE) {
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
  for (int i = 0; i < Frame::TotalRegions; i++) {
    printk("\t\t%d: 0x%lx - 0x%lx\n", i, Frame::MemReg[i][0],
           Frame::MemReg[i][1]);
  }
  printk("\tExcluded Memory Regions:\n");
  for (int i = 0; i < Frame::TotalEx; i++) {
    printk("\t\t%d: 0x%lx - 0x%lx\n", i, Frame::MemEx[i][0],
           Frame::MemEx[i][1]);
  }
}

void* Frame::Alloc() {
  void* frame = NULL;

  for (int i = 0; i < Frame::TotalRegions; i++) {
    // If the alloced address is equal to the end, region is full
    if (Frame::MemReg[i][2] == Frame::MemReg[i][1]) {
      continue;
    }

    for (int j = 0; j < Frame::TotalEx; j++) {
      if (Frame::MemReg[i][0] > Frame::MemEx[j][1] ||
          Frame::MemReg[i][1] < Frame::MemEx[j][0]) {
        continue;
      }

      uint64_t new_size = Frame::MemEx[j][1] + Frame::FRAME_SIZE;
      if (new_size > Frame::MemReg[i][2]) {
        // Skip the excluded portion of the region
        Frame::MemReg[i][2] = new_size;
      }
    }

    if (Frame::MemReg[i][2] >= Frame::MemReg[i][1]) {
      Frame::MemReg[i][2] = Frame::MemReg[i][1];
      continue;
    }

    frame = (void*)Frame::MemReg[i][2];
    Frame::MemReg[i][2] += Frame::FRAME_SIZE;
    break;
  }

  if (frame) {
    return frame;
  }

  if (free_tail == NULL) {
    return NULL;
  }

  frame = (void*)free_tail;
  free_tail = free_tail->prev;

  return frame;
}

void* Frame::AllocZeroed() {
  void* addr = Frame::Alloc();
  if (addr) {
    memset(addr, 0, Frame::FRAME_SIZE);
  }

  return addr;
}

void Frame::Free(void* frame) {
  if ((intptr_t)frame % Frame::FRAME_SIZE) {
    ERROR("Attempted to free invalid frame address 0x%p", frame);
    return;
  }

  struct FreeFrame* f = (struct FreeFrame*)frame;
  f->prev = free_tail;
  free_tail = f;
}
