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

void thread1(void *) {
  printk("Hello from thread 1!\n");
  SysCall::ProcExit();
}

void thread2(void *) {
  printk("Hello from thread 2!\n");
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

  Platform::GetDflInput()->GetChar();
  setup_snakes(0);
  while (1) {
    printk("Running Threads...\n");
    SysCall::ProcRun();
    printk("Threads done...\n");
    IRQ::BlockForIRQ();
  }

  printk("> ");

  while (1) {
    printk("%c", Platform::GetDflInput()->GetChar());
  }
  return;
}
