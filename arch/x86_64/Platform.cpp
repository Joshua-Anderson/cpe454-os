#include "arch/Platform.h"
#include "drivers/char/x86_64/PS2.h"
#include "drivers/display/x86_64/SerialConsole.h"
#include "drivers/display/x86_64/VgaConsole.h"

Display* Platform::GetDflConsole() {
  static SerialConsole console = SerialConsole();
  return &console;
}

Display* Platform::GetDflDisplay() {
  static VgaConsole console = VgaConsole();
  return &console;
}

CharIn* Platform::GetDflInput() {
  static PS2 ps2 = PS2();

  return &ps2;
}

Page* Platform::GetDflPageTable() {
  static Page page = Page();

  return &page;
}
