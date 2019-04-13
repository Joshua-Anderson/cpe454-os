#include "printk.h"

extern "C" void kmain();

void kmain() {
  printk("Hello World\n");

  printk("Test: %d - %d - %d\n", 0, 2, -42);
  printk("Test2: %c - %s - %x\n", '!', "Hi", 0xff);
  return;
}
