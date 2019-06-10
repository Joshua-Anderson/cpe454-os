#ifndef __ELF_H__
#define __ELF_H__

#include <stdint.h>

#define ELF_SHT_NULL 0x0
#define ELF_SHT_PROGBITS 0x1
#define ELF_SHT_SYMTAB 0x2
#define ELF_SHT_STRTAB 0x3
#define ELF_SHT_NOBITS 0x8

extern "C" {

struct elf_section_hdr {
  uint32_t str_name;
  uint32_t type;
  uint64_t flags;
  uint64_t vaddr;
  uint64_t offset;
  uint64_t fsize;
  uint32_t link;  // Index of section
  uint32_t info;
  uint64_t addr_align;
  uint64_t entsize;
};
}

#endif
