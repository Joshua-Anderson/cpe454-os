#include "stdlib.h"

#include "../Page.h"

// struct CR3_REG {
//   uint8_t res_1 : 3;
//   uint8_t pwt : 1;
//   uint8_t pcd : 1;
//   uint8_t res_2 : 7;
//   uint64_t base_1 : 40;
//   uint16_t res_3 : 12;
// } __attribute__((packed));

struct PTEntry {
  uint8_t p : 1;
  uint8_t rw : 1;
  uint8_t us_mode : 1;
  uint8_t pwt : 1;
  uint8_t pcd : 1;
  uint8_t a : 1;
  uint8_t res_1 : 1;
  uint8_t ps : 1;
  uint8_t res_2 : 1;
  uint8_t avl_1 : 3;
  uint64_t base : 40;
  uint16_t avl_2 : 11;
  uint8_t nx : 1;
} __attribute__((packed));

#define PTR_TO_PTABLE_BASE(x) (((uint64_t) x) >> 12)

#define PAGE_TABLE_SIZE 512

struct PTEntry identity_p3[PAGE_TABLE_SIZE] __attribute__((aligned(0x1000))) = {};
struct PTEntry identity_p2[PAGE_TABLE_SIZE][PAGE_TABLE_SIZE] __attribute__((aligned(0x1000))) = {{}};
void Page::InitIdentityMap() {
  uint64_t addr = 0;

  for(int i = 0; i < PAGE_TABLE_SIZE; i++) {
    identity_p3[i].base = PTR_TO_PTABLE_BASE(&identity_p2[i]);
    identity_p3[i].p = 1;
    identity_p3[i].rw = 1;
    for(int j = 0; j < PAGE_TABLE_SIZE; j++) {
      identity_p2[i][j].base = PTR_TO_PTABLE_BASE(addr);
      identity_p2[i][j].p = 1;
      identity_p2[i][j].rw = 1;
      identity_p2[i][j].ps = 1; // Identity table has 2MB entries
      addr += 0x200000;
    }
  }
}

Page::Page() {
  Page::PTableLoc = Frame::Alloc();
  memset(Page::PTableLoc, 0, Frame::FRAME_SIZE);
  struct PTEntry* p4 = (struct PTEntry*) Page::PTableLoc;

  // Identity Map
  p4[0].base = PTR_TO_PTABLE_BASE(&identity_p3[0]);
  p4[0].p = 1;
  p4[0].rw = 1;
}

void Page::Load() {
  asm ( "movq %0, %%cr3" : : "r" ((uint64_t) Page::PTableLoc));
}
