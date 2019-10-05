#ifndef __MBR_H__
#define __MBR_H__

#include <stdint.h>

#define MASTER_BOOT_RECORD_SIG 0xAA55

struct mbr_part_entry {
  uint8_t status;
  uint8_t unused_1[3];
  uint8_t part_type;
  uint8_t unused_2[3];
  uint32_t lba_addr;
  uint32_t num_sectors;
} __attribute__((packed));

struct mbr_header {
  uint8_t boot_code[446];
  struct mbr_part_entry part_1;
  struct mbr_part_entry part_2;
  struct mbr_part_entry part_3;
  struct mbr_part_entry part_4;
  uint16_t boot_sig;
} __attribute__((packed));

#endif
