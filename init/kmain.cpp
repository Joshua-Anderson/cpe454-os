#include <stddef.h>

#include "arch/Platform.h"
#include "irq/IRQ.h"
#include "kmain.h"
#include "kmalloc.h"
#include "mm/Frame.h"
#include "mm/Page.h"
#include "printk.h"
#include "proc/Scheduler.h"
#include "snakes.h"
#include "syscall/SysCall.h"
#include "mbr.h"

void kdb_print(void *) {
  while (1) {
    printk("%c\n", Platform::GetDflInput()->GetChar());
  }
}

void reading_block(void *arg) {
  uint64_t *block = (uint64_t *)arg;
  uint8_t dst[512] = {0};
  Platform::GetDflHardDrive()->ReadBlock(*block, dst);
  printc("Read From Block %lx:\n", *block);
  for (int i = 0; i < 512 / 8; i++) {
    uint16_t b = i*8;
    printc("0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", dst[b],
           dst[b + 1], dst[b + 2], dst[b + 3], dst[b + 4], dst[b + 5],
           dst[b + 6], dst[b + 7]);
  }
  SysCall::ProcExit();
}

void print_mbr(void *) {
  uint8_t dst[512] = {0};
  Platform::GetDflHardDrive()->ReadBlock(0, dst);
  struct mbr_header *hdr = (struct mbr_header *) dst;
  if(hdr->boot_sig != MASTER_BOOT_RECORD_SIG) {
    printk("Invalid Master Boot Record Signature: Got 0x%x, Expected 0x%x\n", hdr->boot_sig, MASTER_BOOT_RECORD_SIG);
    return;
  }
  printk("Partition 1: Start Block 0x%x, Length 0x%x\n", hdr->part_1.lba_addr, hdr->part_1.num_sectors);
  printk("Partition 2: Start Block 0x%x, Length 0x%x\n", hdr->part_2.lba_addr, hdr->part_2.num_sectors);
  printk("Partition 3: Start Block 0x%x, Length 0x%x\n", hdr->part_3.lba_addr, hdr->part_3.num_sectors);
  printk("Partition 4: Start Block 0x%x, Length 0x%x\n", hdr->part_4.lba_addr, hdr->part_4.num_sectors);
  SysCall::ProcExit();
}

void kmain() {
  Scheduler::Add(kdb_print, NULL);
  uint64_t block1 = 0;
  Scheduler::Add(reading_block, &block1);
  uint64_t block2 = 3;
  Scheduler::Add(reading_block, &block2);
  Scheduler::Add(print_mbr, NULL);

  while (1) {
    printk("Running Threads...\n");
    SysCall::ProcRun();
    printk("Threads done...\n");
    IRQ::BlockForIRQ();
  }
  return;
}
