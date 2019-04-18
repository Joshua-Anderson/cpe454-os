#include "../drivers/char/x86_64/PS2.h"
#include "printk.h"

extern "C" void kmain();

void kmain() {
  printk("Hello World\n");

  printk("Test: %d - %d - %d\n", 0, 2, -42);
  printk("Test2: %c - %s - %x\n", '!', "Hi", 0xff);
  printk("Test3: %hd - 0x%p - 0x%lx\n\n", 4, kmain, (long)0x123ABC);

  printk("Raw Keyboard Input: ");
  PS2 ps2 = PS2();
  ps2.Init();

  while (1) {
    printk("%c", ps2.GetChar());
  }
  return;
}
