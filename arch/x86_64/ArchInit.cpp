#include "arch/ArchInit.h"
#include "arch/Platform.h"
#include "GDT.h"
#include "irq/IRQ.h"
#include "printk.h"

#define INIT(msg, func)              \
  printk("Initializing %s...", msg); \
  func;                              \
  printk(" [OK]\n");

void ArchInit::Init() {
  INIT("GDT ", GDT::InitGDT());
  INIT("TSS ", GDT::InitTSS());
  INIT("IRQs", IRQ::Init());
  INIT("UART", Platform::GetDflConsole());
}
