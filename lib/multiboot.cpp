#include "multiboot.h"
#include "printk.h"

#define MULTIBOOT_MAGIC 0x36d76289

#define PAD_PTR(x) x = (uint8_t*) ((((intptr_t) x) + 7) & ~7)

struct mb_header {
  uint32_t total_size;
  uint32_t res;
};

struct mb_subheader {
  uint32_t type;
  uint32_t size;
};

void parse_multiboot(uint32_t magic, uint8_t* header) {
  if(magic != MULTIBOOT_MAGIC) {
    printa("Invalid Multiboot Magic (Got %x, Expected %x)...\n", magic, MULTIBOOT_MAGIC);
    return;
  }

  struct mb_header* mbh = (struct mb_header*) header;

  printa("Pos: %p, Total header size: %u, Res: %u\n", header, mbh->total_size, mbh->res);

  uint8_t *max = header + mbh->total_size;
  uint8_t *pos = header + sizeof(mb_header);
  PAD_PTR(pos);

  while(pos < max) {
    struct mb_subheader* sh = (struct mb_subheader*) pos;
    printa("Pos: %p, Type: %u, Size %d\n", pos, sh->type, sh->size);

    if(sh->type == 0) {
      printa("Terminating tag\n");
      break;
    }

    pos += sh->size;
    PAD_PTR(pos);
  }

  if(pos >= max) {
    printa("Err: Invalid Multiboot Header\n");
    return;
  }
}
