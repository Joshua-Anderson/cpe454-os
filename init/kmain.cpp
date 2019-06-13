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

void thread1(void *arg) {
  int a = 61;
  printk("Thread 1: Testing %d, 0x%p\n", a, arg);
  SysCall::ProcYield();
  printk("Thread 1: Testing %d, 0x%p\n", a, arg);
  SysCall::ProcExit();
}

void thread2(void *arg) {
  int a = 42;
  printk("Thread 2: Testing %d, 0x%p\n", a, arg);
  SysCall::ProcYield();
  printk("Thread 2: Testing %d, 0x%p\n", a, arg);
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
  setup_snakes(1);

  // int test = 0;
  // int anothertest = 1;
  // Scheduler::Add(thread1, &test);
  // Scheduler::Add(thread2, &anothertest);

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
