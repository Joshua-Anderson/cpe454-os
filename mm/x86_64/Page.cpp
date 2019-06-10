#include "../Page.h"
#include "arch/x86_64/arch.h"
#include "irq/IRQ.h"
#include "printk.h"
#include "stdlib.h"

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

struct PageOffsets {
  uint16_t l1;
  uint16_t l2;
  uint16_t l3;
  uint16_t l4;
};

struct PageTableLookup {
  struct PTEntry* l1;
  struct PTEntry* l2;
  struct PTEntry* l3;
  struct PTEntry* l4;
};

#define PTR_TO_PTABLE_BASE(x) (((uint64_t)x) >> 12)
#define PTABLE_BASE_TO_PTR(x) ((struct PTEntry*)((uint64_t)x << 12))

#define PAGE_TABLE_SIZE 512

struct PTEntry identity_p3[PAGE_TABLE_SIZE]
    __attribute__((aligned(0x1000))) = {};
struct PTEntry kern_stack_p3[PAGE_TABLE_SIZE]
    __attribute__((aligned(0x1000))) = {};
struct PTEntry kern_heap_p3[PAGE_TABLE_SIZE]
    __attribute__((aligned(0x1000))) = {};

static void addrToPageOffset(void* addr, struct PageOffsets* ptoff) {
  intptr_t a = (intptr_t)addr;
  ptoff->l1 = (a >> 12) & 0x1FF;
  ptoff->l2 = (a >> (12 + 9)) & 0x1FF;
  ptoff->l3 = (a >> (12 + 9 + 9)) & 0x1FF;
  ptoff->l4 = (a >> (12 + 9 + 9 + 9)) & 0x1FF;
}

static void walkPageTable(struct PTEntry* pt, struct PageOffsets* ptoff,
                          struct PageTableLookup* ptlook) {
  ptlook->l1 = NULL;
  ptlook->l2 = NULL;
  ptlook->l3 = NULL;

  ptlook->l4 = &pt[ptoff->l4];
  if (!ptlook->l4->p) {
    return;
  }

  ptlook->l3 = &PTABLE_BASE_TO_PTR(ptlook->l4->base)[ptoff->l3];
  if (!ptlook->l3->p) {
    return;
  }

  ptlook->l2 = &PTABLE_BASE_TO_PTR(ptlook->l3->base)[ptoff->l2];
  if (!ptlook->l2->p) {
    return;
  }

  ptlook->l1 = &PTABLE_BASE_TO_PTR(ptlook->l2->base)[ptoff->l1];
}

void pf_irq_handler(unsigned int, unsigned int err) {
  uint64_t fault_addr;
  get_reg("cr2", fault_addr);

  if (err) {
    printk("Fatal Page Fault: CR2 is 0x%lx, err is %u\n", fault_addr, err);
    hlt();
  }

  struct PTEntry* pt;
  get_reg("cr3", pt);

  struct PageOffsets ptoff;
  addrToPageOffset((void*)fault_addr, &ptoff);
  printk("Page Offset: ptable[%u][%u][%u][%u]\n", ptoff.l4, ptoff.l3, ptoff.l2,
         ptoff.l1);
  struct PageTableLookup ptlook;
  walkPageTable(pt, &ptoff, &ptlook);

  if (!ptlook.l4->p || !ptlook.l3->p || !ptlook.l2->avl_1 ||
      !ptlook.l1->avl_1) {
    printk("FATAL: Page fault on unallocated address %lx\n", fault_addr);
    hlt();
  }

  if (!ptlook.l2->p) {
    void* new_frame = Frame::AllocZeroed();
    ptlook.l2->p = 1;
    ptlook.l2->base = PTR_TO_PTABLE_BASE(new_frame);
    struct PTEntry* ptl1 = (struct PTEntry*)new_frame;
    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
      ptl1[i].rw = 1;
      ptl1[i].avl_1 = 1;
    }
    walkPageTable(pt, &ptoff, &ptlook);
  }

  void* new_frame = Frame::Alloc();
  ptlook.l1->p = 1;
  ptlook.l1->base = PTR_TO_PTABLE_BASE(new_frame);
}

void Page::InitIdentityMap() {
  uint64_t addr = 0;

  for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
    identity_p3[i].base = PTR_TO_PTABLE_BASE(addr);
    identity_p3[i].p = 1;
    identity_p3[i].rw = 1;
    identity_p3[i].ps = 1;
    addr += 0x40000000;
  }
}

#define L1_SIZE 0x200000

static int alloc_virt_4k_chunk(struct PTEntry* pt, void* start, void* end) {
  if ((intptr_t)start % Page::PAGE_SIZE || (intptr_t)end % Page::PAGE_SIZE) {
    printk("ERROR: Unaligned memory allocation %p->%p\n", start, end);
    return 1;
  }

  void* pos = start;
  while (pos < end) {
    struct PageOffsets ptoff;
    struct PageTableLookup ptlook;
    addrToPageOffset(pos, &ptoff);
    walkPageTable(pt, &ptoff, &ptlook);

    if (!ptlook.l4->p) {
      printk("ERROR: Unalloced Page Table Level 4 Segment %p\n", pos);
      return 1;
    }

    if (!ptlook.l3->p) {
      void* new_frame = Frame::AllocZeroed();
      ptlook.l3->base = PTR_TO_PTABLE_BASE(new_frame);
      ptlook.l3->p = 1;
      ptlook.l3->rw = 1;
      walkPageTable(pt, &ptoff, &ptlook);
    }

    if (!ptlook.l2->p) {
      // Skip Allocating L1 table if it's totally spanned
      if ((uint64_t)end - (uint64_t)pos >= L1_SIZE && ptoff.l1 == 0) {
        ptlook.l2->avl_1 = 1;
        pos = (void*)((intptr_t)pos + L1_SIZE);
        continue;
      }
      void* new_frame = Frame::AllocZeroed();
      ptlook.l2->base = PTR_TO_PTABLE_BASE(new_frame);
      ptlook.l2->p = 1;
      ptlook.l2->rw = 1;
      ptlook.l2->avl_1 = 1;
      walkPageTable(pt, &ptoff, &ptlook);
    }

    ptlook.l1->rw = 1;
    ptlook.l1->avl_1 = 1;

    pos = (void*)((intptr_t)pos + Page::PAGE_SIZE);
  }

  return 0;
}

static int free_virt_4k_chunk(struct PTEntry* pt, void* start, void* end) {
  if ((intptr_t)start % Page::PAGE_SIZE || (intptr_t)end % Page::PAGE_SIZE) {
    printk("ERROR: Unaligned memory allocation %p->%p\n", start, end);
    return 1;
  }

  void* pos = start;
  while (pos < end) {
    struct PageOffsets ptoff;
    struct PageTableLookup ptlook;
    addrToPageOffset(pos, &ptoff);
    walkPageTable(pt, &ptoff, &ptlook);

    if (!ptlook.l4->p) {
      printk("ERROR: Unalloced Page Table Level 4 Segment %p while freeing\n",
             pos);
      return 1;
    }

    if (ptlook.l1) {
      if (ptlook.l1->p) {
        Frame::Free(PTABLE_BASE_TO_PTR(ptlook.l1->base));
      }

      ptlook.l1->p = 0;
      ptlook.l1->avl_1 = 0;
    }

    if (ptlook.l2) {
      if (ptlook.l2->avl_1 && !ptlook.l2->p) {
        ptlook.l2->avl_1 = 0;
      }
    }

    // TODO: L2 and L3 Page Tables when no longer needed

    pos = (void*)((intptr_t)pos + Page::PAGE_SIZE);
  }

  return 0;
}

uint64_t Page::KernStackPos = Page::KSTACK_START_ADDR;

void* Page::AllocKernStackMem() {
  struct PTEntry* curPT;
  get_reg("cr3", curPT);
  void* start = (void*)Page::KernStackPos;
  void* end = (void*)(Page::KernStackPos + Page::KTHREAD_STACK_SIZE);
  int res = alloc_virt_4k_chunk(curPT, start, end);
  if (res) {
    return NULL;
  }
  Page::KernStackPos = (uint64_t)end;
  return start;
}

void Page::FreeKernStackMem(void* addr) {
  struct PTEntry* curPT;
  get_reg("cr3", curPT);
  free_virt_4k_chunk(curPT, addr,
                     (void*)((intptr_t)addr + Page::KTHREAD_STACK_SIZE));
}

uint64_t Page::KernHeapPos = Page::KHEAP_START_ADDR;

void* Page::AllocKernHeapPage() {
  struct PTEntry* curPT;
  get_reg("cr3", curPT);
  void* start = (void*)Page::KernHeapPos;
  void* end = (void*)(Page::KernHeapPos + Page::PAGE_SIZE);
  int res = alloc_virt_4k_chunk(curPT, start, end);
  if (res) {
    return NULL;
  }
  Page::KernHeapPos = (uint64_t)end;
  return start;
}

void Page::FreeKernHeapPage(void* addr) {
  struct PTEntry* curPT;
  get_reg("cr3", curPT);
  free_virt_4k_chunk(curPT, addr, (void*)((intptr_t)addr + Page::PAGE_SIZE));
}

Page::Page() {
  Page::PTableLoc = Frame::AllocZeroed();
  struct PTEntry* p4 = (struct PTEntry*)Page::PTableLoc;

  // Identity Map
  p4[0].base = PTR_TO_PTABLE_BASE(&identity_p3[0]);
  p4[0].p = 1;
  p4[0].rw = 1;

  // Kernel Stack
  struct PageOffsets ptoff;
  addrToPageOffset((void*)Page::KSTACK_START_ADDR, &ptoff);
  p4[ptoff.l4].base = PTR_TO_PTABLE_BASE(&kern_stack_p3[0]);
  p4[ptoff.l4].p = 1;
  p4[ptoff.l4].rw = 1;

  // Kernel Heap
  addrToPageOffset((void*)Page::KHEAP_START_ADDR, &ptoff);
  p4[ptoff.l4].base = PTR_TO_PTABLE_BASE(&kern_heap_p3[0]);
  p4[ptoff.l4].p = 1;
  p4[ptoff.l4].rw = 1;
}

void Page::Load() {
  set_reg("cr3", (uint64_t)Page::PTableLoc);
  IRQ::Register(PF_FAULT, pf_irq_handler);
}
