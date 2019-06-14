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

  while (1) {
    printk("Running Threads...\n");
    SysCall::ProcRun();
    printk("Threads done...\n");
    IRQ::BlockForIRQ();
  }
  return;
}
