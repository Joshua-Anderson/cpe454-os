#include "drivers/char/x86_64/PS2.h"
#include "arch/ArchInit.h"
#include "arch/Platform.h"
#include "irq/IRQ.h"
#include "printk.h"
#include "mm/Frame.h"
#include "mm/Page.h"

extern "C" void kmain(uint32_t, void *);

void test_page_alloc() {
  void *addr = Frame::Alloc();
  while(addr) {
    printc("Alloced Page: 0x%p\n", addr);
    Frame::Free(addr);
    addr = Frame::Alloc();
  }

  printa("Ran out of frames\n");
}

void kmain(uint32_t mb_magic, void *mb_header) {
  printk("Loading Project SOL v0.alpha...\n");

  ArchInit::Init(mb_magic, mb_header);

  printk("Display only...\n");
  printc("Console only...\n");
  printa("Both...\n");
  printa("Really long test testing testing one two three four...\n");

  Platform::GetDflInput()->GetChar();

  Page::InitIdentityMap();
  printa("Ident Map Done\n");
  Page p = Page();
  p.Load();
  printa("Post Page Table Switch\n");

  // int test = *((int *)0xFFFFFFFFFFFFFFFF);
  // test++;

  test_page_alloc();

  printk("> ");

  while (1) {
     printk("%c", Platform::GetDflInput()->GetChar());
  }
  return;
}
