#include <stddef.h>
#include <stdint.h>

#include "GDT.h"
#include "stdlib.h"

struct TSS_struct {
  uint32_t res1;
  uint64_t rsp[3];
  uint64_t res2;
  uint64_t ist[7];
  uint64_t res3;
  uint16_t iopb;
  uint16_t res4;
} __attribute__((packed));

struct GDT_entry8 {
  uint16_t limit_1;
  uint16_t base_1;
  uint8_t base_2;
  // access bits
  struct {
    uint8_t ac : 1;
    uint8_t rw : 1;
    uint8_t dc : 1;
    uint8_t ex : 1;
    uint8_t s : 1;
    uint8_t privl : 2;
    uint8_t pr : 1;
  } access;
  uint8_t limit_2 : 4;
  uint8_t zero : 1;
  uint8_t l : 1;
  uint8_t sz : 1;
  uint8_t gr : 1;
  uint8_t base_3;
} __attribute__((packed));

struct GDT_entry16 {
  uint16_t limit_1;
  uint16_t base_1;
  uint8_t base_2;
  // access bits
  struct {
    uint8_t ac : 1;
    uint8_t rw : 1;
    uint8_t dc : 1;
    uint8_t ex : 1;
    uint8_t s : 1;
    uint8_t privl : 2;
    uint8_t pr : 1;
  } access;
  uint8_t limit_2 : 4;
  uint8_t zero : 1;
  uint8_t l : 1;
  uint8_t sz : 1;
  uint8_t gr : 1;
  uint8_t base_3;
  uint32_t base_4;
  uint32_t res;
} __attribute__((packed));

struct GDT_entries_def {
  struct GDT_entry8 null;
  struct GDT_entry8 cs;
  struct GDT_entry16 tss;
} __attribute__((packed)) GDT_entries;

uint16_t GDT::CS_SEG = offsetof(struct GDT_entries_def, cs);
uint16_t GDT::TSS_SEG = offsetof(struct GDT_entries_def, tss);

struct TSS_struct TSS;

void GDT::InitTSS() {
  memset(&TSS, 0, sizeof(TSS_struct));

  TSS.iopb = sizeof(TSS_struct);

  asm volatile("ltr %0" : : "r"(GDT::TSS_SEG));
}
void GDT::LoadIST(int num, void *sp) { TSS.ist[num - 1] = (uint64_t)sp; }

static inline void load_gdt(void *gdt, uint16_t size) {
  struct {
    uint16_t limit;
    void *base;
  } __attribute__((packed)) GDTR = {size, gdt};

  asm volatile("lgdt %0" : : "m"(GDTR));
}

void GDT::InitGDT() {
  memset(&GDT_entries, 0, sizeof(GDT_entries));

  // Setup Code Segment
  GDT_entries.cs.access.pr = 1;
  GDT_entries.cs.access.s = 1;
  GDT_entries.cs.access.ex = 1;
  GDT_entries.cs.access.rw = 1;
  GDT_entries.cs.gr = 1;
  GDT_entries.cs.l = 1;

  // Setup TSS Segment
  GDT_entries.tss.base_1 = ((uint64_t)(&TSS)) & 0xFFFF;
  GDT_entries.tss.base_2 = (((uint64_t)(&TSS)) >> 16) & 0xFF;
  GDT_entries.tss.base_3 = (((uint64_t)(&TSS)) >> 24) & 0xFF;
  GDT_entries.tss.base_3 = (((uint64_t)(&TSS)) >> 32) & 0xFFFFFFFF;
  GDT_entries.tss.limit_1 = (sizeof(TSS) - 1) & 0xFFFF;
  GDT_entries.tss.limit_2 = ((sizeof(TSS) - 1) >> 16) & 0xF;
  GDT_entries.tss.access.pr = 1;
  GDT_entries.tss.access.ex = 1;
  GDT_entries.tss.access.ac = 1;
  GDT_entries.tss.sz = 1;

  load_gdt(&GDT_entries, sizeof(GDT_entries) - 1);
}
