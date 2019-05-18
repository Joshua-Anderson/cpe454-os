#include "arch/Platform.h"
#include "drivers/display/x86_64/VgaConsole.h"
#include "drivers/display/x86_64/SerialConsole.h"

Display* Platform::GetDflConsole() {
  static SerialConsole console = SerialConsole();
  return &console;
}

Display* Platform::GetDflDisplay() {
  static VgaConsole console = VgaConsole();
  return &console;
}
