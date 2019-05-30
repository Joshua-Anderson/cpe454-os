#include "arch/ArchInit.h"
#include "arch/Platform.h"
#include "GDT.h"
#include "irq/IRQ.h"
#include "mm/Frame.h"
#include "multiboot.h"
#include "elf.h"
#include "printk.h"

#define INIT_OK() printk(" [OK]\n")
#define INIT_WARN() printk(" [WARN]\n")

#define INIT(msg, func)              \
  printk("Initializing %s...", msg); \
  func;                              \
  INIT_OK();

void init_mem_regions(struct mb_info* mbinfo) {
  printk("Initializing MM  ...");

  struct mb_mm_entry *mm_entry = mbinfo->mmap;
  int res = 0;
  uint64_t prev_start = mm_entry->base;
  uint64_t prev_end = mm_entry->base + mm_entry->len;
  mm_entry = next_mb_mmap(mm_entry, mbinfo);
  while(mm_entry) {
    uint64_t start = mm_entry->base;
    uint64_t end = mm_entry->base + mm_entry->len;

    if(prev_end == start) {
      prev_end = end;
    } else {
      if(Frame::AddMemRegion(prev_start, prev_end)) {
        res = 1;
      }
      prev_start = start;
      prev_end = end;
    }

    mm_entry = next_mb_mmap(mm_entry, mbinfo);
  }

  // Register last chunk that hasn't been registered yet
  if(Frame::AddMemRegion(prev_start, prev_end)) {
    res = 1;
  }

  int missing_elf_entries = mbinfo->elf_num_entries < 2;

  res = 0;
  if(!missing_elf_entries) {
    struct elf_section_hdr* s = get_mb_elf_section(1, mbinfo);
    prev_start = s->vaddr;
    prev_end = prev_start + s->fsize;
  }
  for(int i = 2; i < mbinfo->elf_num_entries; i++) {
    struct elf_section_hdr* s = get_mb_elf_section(i, mbinfo);

    uint64_t start = s->vaddr;
    uint64_t end = start + s->fsize;

    // If the gap is less than a frame, just exclude it anyway
    if(start - prev_end < Frame::FRAME_SIZE) {
      prev_end = end;
      continue;
    }

    if(Frame::AddMemExcludeRegion(prev_start, prev_end)) {
      res = 1;
    }
    prev_start = start;
    prev_end = end;
  }

  // Register last chunk that hasn't been registered yet
  if(!missing_elf_entries && Frame::AddMemExcludeRegion(prev_start, prev_end)) {
    res = 1;
  }

   if(res || missing_elf_entries) {
    INIT_WARN();
  } else {
    INIT_OK();
  }

  if(res) {
    printk("\tUnaligned Memory Segments\n");
  }

  if(missing_elf_entries) {
    printk("\tNo Kernel Segments in Multiboot\n");
  }

  Frame::PrintMemRegions();

}

void ArchInit::Init(uint32_t mb_magic, void *mb_header) {
  INIT("GDT ", GDT::InitGDT());
  INIT("TSS ", GDT::InitTSS());
  INIT("IRQs", IRQ::Init());
  INIT("UART", Platform::GetDflConsole());
  INIT("KBD ", Platform::GetDflInput());
  INIT("MB  ", struct mb_info mbinfo = parse_multiboot(mb_magic, (uint8_t *) mb_header));
  init_mem_regions(&mbinfo);
}
