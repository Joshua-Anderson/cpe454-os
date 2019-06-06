#include "stdlib.h"
#include "arch/x86_64/arch.h"
#include "printk.h"
#include "irq/IRQ.h"
#include "../Page.h"

// struct CR3_REG {
//   uint8_t res_1 : 3;
//   uint8_t pwt : 1;
//   uint8_t pcd : 1;
//   uint8_t res_2 : 7;
//   uint64_t base_1 : 40;
//   uint16_t res_3 : 12;
// } __attribute__((packed));

Page* Page::CurrentPage = NULL;

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
#define LOOKUP_PAGE(x) ((struct PTEntry *) ((uint64_t) x << 12))

#define PAGE_TABLE_SIZE 512

struct PTEntry identity_p3[PAGE_TABLE_SIZE] __attribute__((aligned(0x1000))) = {};
struct PTEntry kern_stack_p3[PAGE_TABLE_SIZE] __attribute__((aligned(0x1000))) = {};

void pf_irq_handler(unsigned int, unsigned int err) {
  uint64_t fault_addr;
  get_reg("cr2", fault_addr);

  if(err) {
    printk("Fatal Page Fault: CR2 is 0x%lx, err is %u\n", fault_addr, err);
    hlt();
  }

  Page* p = Page::GetCurrentPage();
  if(p == NULL) {
    printk("Fatal Page Fault: No Page Table Loaded\n");
    hlt();
  }

  struct PTEntry* ptl4 = (struct PTEntry*) p->GetPageTableLocation();
  uint16_t p1_entry = (fault_addr >> 12) & 0x1FF;
  uint16_t p2_entry = (fault_addr >> (12+9)) & 0x1FF;
  uint16_t p3_entry = (fault_addr >> (12+9+9)) & 0x1FF;
  uint16_t p4_entry = (fault_addr >> (12+9+9+9)) & 0x1FF;
  printk("Page Offset: ptable[%u][%u][%u][%u]\n", p4_entry, p3_entry, p2_entry, p1_entry);

  if(ptl4[p4_entry].p == 0) {
    printk("Fatal Page Fault %lx: VMem segment %u nonexistent (Invalid L4)\n", fault_addr, p4_entry);
    hlt();
  }

  struct PTEntry *ptl3 = LOOKUP_PAGE(ptl4[p4_entry].base);
  if(ptl3[p3_entry].p == 0) {
    printk("Fatal Page Fault %lx: VMem segment %u->%u nonexistent (Invalid L3)\n", fault_addr, p4_entry, p3_entry);
    hlt();
  }

  struct PTEntry *ptl2 = LOOKUP_PAGE(ptl3[p3_entry].base);
  if(ptl2[p2_entry].p == 0) {
    printk("Fatal Page Fault %lx: VMem segment %u->%u->%u nonexistent (Invalid L2)\n", fault_addr, p4_entry, p3_entry, p2_entry);
    hlt();
  }

  struct PTEntry *ptl1 = LOOKUP_PAGE(ptl2[p2_entry].base);
  if(ptl1[p1_entry].avl_1 == 0) {
    printk("Fatal Page Fault %lx: VMem segment %u->%u->%u->%u nonexistent (Invalid L1)\n", fault_addr, p4_entry, p3_entry, p2_entry, p1_entry);
    hlt();
  }

  void * new_page = Frame::Alloc();
  ptl1[p1_entry].p = 1;
  ptl1[p1_entry].base = PTR_TO_PTABLE_BASE(new_page);
}

void Page::InitIdentityMap() {
  uint64_t addr = 0;

  for(int i = 0; i < PAGE_TABLE_SIZE; i++) {
    identity_p3[i].base = PTR_TO_PTABLE_BASE(addr);
    identity_p3[i].p = 1;
    identity_p3[i].rw = 1;
    identity_p3[i].ps = 1;
    addr += 0x40000000;
  }
}

uint64_t Page::KernStackOffSet = 0;

void* Page::AllocKernStackMem() {
  uint64_t entries = Page::KernStackOffSet / Page::KTHREAD_STACK_SIZE;
  uint16_t p3_pos = entries / PAGE_TABLE_SIZE;
  uint64_t p2_pos = entries % PAGE_TABLE_SIZE;

  // If P3 entry hasn't been populated, create it;
  if(!kern_stack_p3[p3_pos].p) {
    void * pos = Frame::Alloc();
    memset(pos, 0, Frame::FRAME_SIZE);
    kern_stack_p3[p3_pos].base = PTR_TO_PTABLE_BASE(pos);
    kern_stack_p3[p3_pos].p = 1;
    kern_stack_p3[p3_pos].rw = 1;
  }

  struct PTEntry* kern_stack_p2 = LOOKUP_PAGE(kern_stack_p3[p3_pos].base);
  kern_stack_p2[p2_pos].p = 1;
  kern_stack_p2[p2_pos].rw = 1;
  struct PTEntry* kern_stack_p1 = (struct PTEntry*) Frame::Alloc();
  memset(kern_stack_p1, 0, Frame::FRAME_SIZE);
  kern_stack_p2[p2_pos].base = PTR_TO_PTABLE_BASE(kern_stack_p1);
  for(int i = 0; i < PAGE_TABLE_SIZE; i++) {
    kern_stack_p1[i].rw = 1;
    kern_stack_p1[i].avl_1 = 1;
  }

  void * ret = (void *) (Page::KSTACK_START_ADDR + Page::KernStackOffSet);
  Page::KernStackOffSet += KTHREAD_STACK_SIZE;
  return ret;
}

Page::Page() {
  Page::PTableLoc = Frame::Alloc();
  memset(Page::PTableLoc, 0, Frame::FRAME_SIZE);
  struct PTEntry* p4 = (struct PTEntry*) Page::PTableLoc;

  // Identity Map
  p4[0].base = PTR_TO_PTABLE_BASE(&identity_p3[0]);
  p4[0].p = 1;
  p4[0].rw = 1;

  // Kernel Stack
  p4[1].base = PTR_TO_PTABLE_BASE(&kern_stack_p3[0]);
  p4[1].p = 1;
  p4[1].rw = 1;
}

void Page::Load() {
  set_reg("cr3", (uint64_t) Page::PTableLoc);
  IRQ::Register(PF_FAULT, pf_irq_handler);
  Page::CurrentPage = this;
}

Page* Page::GetCurrentPage() {
  return Page::CurrentPage;
}

void* Page::GetPageTableLocation() {
  return Page::PTableLoc;
}
