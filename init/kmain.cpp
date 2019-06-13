#include <stddef.h>

#include "arch/ArchInit.h"
#include "arch/Platform.h"
#include "irq/IRQ.h"
#include "kmalloc.h"
#include "mm/Frame.h"
#include "mm/Page.h"
#include "printk.h"
#include "proc/Scheduler.h"
#include "syscall/syscall.h"

extern "C" void kmain(uint32_t, void *);

void thread1(void *) {
  printk("Hello from thread 1!\n");
  thread_exit();
}

void thread2(void *) {
  printk("Hello from thread 2!\n");
  thread_exit();
}

void kmain(uint32_t mb_magic, void *mb_header) {
  printk("Loading Project SOL v0.alpha...\n");

  ArchInit::Init(mb_magic, mb_header);

  printk("Display only...\n");
  printc("Console only...\n");
  printa("Both...\n");
  printa("Really long test testing testing one two three four...\n");

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
  Scheduler::Add(thread1, NULL);
  Scheduler::Add(thread2, NULL);
  while (1) {
    printk("Running Threads...\n");
    thread_run();
    printk("Threads done...\n");
    IRQ::BlockForIRQ();
  }

  printk("> ");

  while (1) {
    printk("%c", Platform::GetDflInput()->GetChar());
  }
  return;
}
