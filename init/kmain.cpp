#include "arch/ArchInit.h"
#include "arch/Platform.h"
#include "irq/IRQ.h"
#include "kmalloc.h"
#include "mm/Frame.h"
#include "mm/Page.h"
#include "printk.h"
#include "syscall/syscall.h"

extern "C" void kmain(uint32_t, void *);

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
  thread_yield();

  printk("> ");

  while (1) {
    printk("%c", Platform::GetDflInput()->GetChar());
  }
  return;
}
