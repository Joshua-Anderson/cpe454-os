#include "printk.h"

extern "C" void kmain();

void kmain() {
  printk("Hello World");
  return;
}
