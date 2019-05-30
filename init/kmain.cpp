#include "drivers/char/x86_64/PS2.h"
#include "arch/ArchInit.h"
#include "arch/Platform.h"
#include "irq/IRQ.h"
#include "printk.h"

extern "C" void kmain(uint32_t, void *);

void kmain(uint32_t mb_magic, void *mb_header) {
  printk("Loading Project SOL v0.alpha...\n");

  ArchInit::Init(mb_magic, mb_header);

  printk("Display only...\n");
  printc("Console only...\n");
  printa("Both...\n");
  printa("Really long test testing testing one two three four...\n");

  // int test = *((int *)0xFFFFFFFFFFFFFFFF);
  // test++;

  printk("> ");

  while (1) {
     printk("%c", Platform::GetDflInput()->GetChar());
  }
  return;
}
