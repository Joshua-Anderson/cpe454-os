#include "../drivers/char/x86_64/PS2.h"
#include "arch/ArchInit.h"
#include "irq/IRQ.h"
#include "printk.h"

extern "C" void kmain();

void kmain() {
  printk("Loading Project SOL v0.alpha...\n");

  ArchInit::Init();
  PS2 ps2 = PS2();
  ps2.GetChar();
  ps2.Init();

  int test = *((int *)0xFFFFFFFFFFFFFFFF);
  test++;

  printk("Raw Keyboard Input: ");

  while (1) {
    printk("%c", ps2.GetChar());
  }
  return;
}
