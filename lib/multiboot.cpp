#include "stdlib.h"
#include "printk.h"
#include "multiboot.h"

#define MULTIBOOT_MAGIC 0x36d76289
#define MULTIBOOT_MEMINFO_TYPE 4
#define MULTIBOOT_MMAP_TYPE 6
#define MULTIBOOT_ELF_TYPE 9
#define MULTIBOOT_MMAP_REGION_RAM 1

#define PAD_PTR(x) x = (uint8_t*) ((((intptr_t) x) + 7) & ~7)

struct mb_header {
  uint32_t total_size;
  uint32_t res;
} __attribute__((packed));

struct mb_subheader {
  uint32_t type;
  uint32_t size;
} __attribute__((packed));

struct mb_mm_header {
  uint32_t type;
  uint32_t size;
  uint32_t entry_size;
  uint32_t entry_version;
} __attribute__((packed));

struct mb_elf_header {
  uint32_t type;
  uint32_t size;
  uint32_t num;
  uint32_t entry_size;
  uint32_t str_table_index;
} __attribute__((packed));

struct mb_meminfo_header {
  uint32_t type;
  uint32_t size;
  uint32_t mem_lower;
  uint32_t mem_upper;
} __attribute__((packed));

struct elf_section_hdr* get_mb_elf_section(uint16_t id, struct mb_info* info) {
  return (struct elf_section_hdr *) ((intptr_t) info->elf_tag + sizeof(struct mb_elf_header) + (id*info->elf_ent_size));
}


struct mb_mm_entry* next_mb_mmap(struct mb_mm_entry* entry, struct mb_info* info) {
  struct mb_mm_entry* nxt = (struct mb_mm_entry*) ((intptr_t) entry + info->mmap_entry_size);

  if((intptr_t) nxt < (intptr_t) info->mmap_max) {
    return nxt;
  }

  return NULL;
}

struct mb_info parse_multiboot(uint32_t magic, uint8_t* header) {
  struct mb_info info = {};

  if(magic != MULTIBOOT_MAGIC) {
    printk("Invalid Multiboot Magic (Got %x, Expected %x)...\n", magic, MULTIBOOT_MAGIC);
    return info;
  }

  struct mb_header* mbh = (struct mb_header*) header;

  uint8_t *max = header + mbh->total_size;
  uint8_t *pos = header + sizeof(mb_header);
  PAD_PTR(pos);

  while(pos < max) {
    struct mb_subheader* sh = (struct mb_subheader*) pos;
    printc("Pos: %p, Type: %u, Size %d\n", pos, sh->type, sh->size);

    if(sh->type == 0) {
      printc("Terminating tag\n");
      break;
    }

    if(sh->type == MULTIBOOT_MEMINFO_TYPE) {
      struct mb_meminfo_header* mh = (struct mb_meminfo_header *) sh;
      info.mem_lower = mh->mem_lower;
      info.mem_upper = mh->mem_upper;
    }

    if(sh->type == MULTIBOOT_MMAP_TYPE) {
      struct mb_mm_header* mh = (struct mb_mm_header *) sh;
      info.mmap_entry_size = mh->entry_size;
      info.mmap = (struct mb_mm_entry*) ((intptr_t) mh + sizeof(struct mb_mm_header));
      info.mmap_max = (void *) ((intptr_t) mh + mh->size);
    }

    if(sh->type == MULTIBOOT_ELF_TYPE) {
      struct mb_elf_header* eh = (struct mb_elf_header *) sh;
      info.elf_tag = eh;
      info.elf_num_entries = eh->num;
      info.elf_ent_size = eh->entry_size;
    }

    pos += sh->size;
    PAD_PTR(pos);
  }

  if(pos >= max) {
    printa("Err: Invalid Multiboot Header\n");
  }

  return info;
}
