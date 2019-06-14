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
    printc("0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", dst[i],
           dst[i + 1], dst[i + 2], dst[i + 3], dst[i + 4], dst[i + 5],
           dst[i + 6], dst[i + 7]);
  }
  SysCall::ProcExit();
}

void kmain() {
  void *small = kmalloc(3);
  printc("Small Adder: %p\n", small);
  void *med = kmalloc(200);
  printc("Med Addr: %p\n", med);
  kfree(small);
  small = kmalloc(3);
  printc("New Small Adder: %p\n", small);
  kfree(med);
  kfree(small);
  void *huge = kmalloc(6000);
  printc("Huge Adder: %p\n", huge);
  kfree(huge);

  Scheduler::Add(kdb_print, NULL);
  uint64_t block1 = 0;
  Scheduler::Add(reading_block, &block1);
  uint64_t block2 = 0;
  Scheduler::Add(reading_block, &block2);

  while (1) {
    printk("Running Threads...\n");
    SysCall::ProcRun();
    printk("Threads done...\n");
    IRQ::BlockForIRQ();
  }
  return;
}
