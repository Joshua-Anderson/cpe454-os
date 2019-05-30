#ifndef __MULTIBOOT_H__
#define __MULTIBOOT_H__

#include <stdint.h>
#include "elf.h"

#define MULTIBOOT_MMAP_REGION_RAM 1

extern "C" {

struct mb_mm_entry {
  uint64_t base;
  uint64_t len;
  uint32_t type;
  uint32_t res;
} __attribute__((packed));

struct mb_info {
  uint32_t mem_lower;
  uint32_t mem_upper;
  uint32_t mmap_entry_size;
  void *elf_tag;
  uint16_t elf_ent_size;
  uint16_t elf_num_entries;
  void* mmap_max;
  struct mb_mm_entry *mmap;
} __attribute__((packed));

struct mb_info parse_multiboot(uint32_t magic, uint8_t* header);
struct mb_mm_entry* next_mb_mmap(struct mb_mm_entry*, struct mb_info* info);
struct elf_section_hdr* get_mb_elf_section(uint16_t id, struct mb_info* info);
}


#endif
