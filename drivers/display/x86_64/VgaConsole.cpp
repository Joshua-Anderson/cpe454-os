#include "VgaConsole.h"
#include "../Color.h"
#include "stdint.h"
#include "stdlib.h"

#define VGA_BUF_START 0xb8000
#define TAB_SIZE 4

struct VgaChar {
  char cp;
  uint8_t fg_color : 4;
  uint8_t bg_color : 3;
  uint8_t blink : 1;
} __attribute__((__packed__));

bool VgaConsole::Scroll() {
  if (VgaConsole::pos < VgaConsole::cols * VgaConsole::rows) {
    return false;
  }

  struct VgaChar* c = (struct VgaChar*)VGA_BUF_START;
  memcpy(c, c + VgaConsole::cols,
         VgaConsole::cols * (VgaConsole::rows - 1) * sizeof(struct VgaChar));

  VgaConsole::pos = VgaConsole::cols * (VgaConsole::rows - 1);

  for (int i = VgaConsole::pos; i < VgaConsole::pos + VgaConsole::cols; i++) {
    c[i].cp = ' ';
  }

  return true;
}

void VgaConsole::PrintChar(char in) {
  struct VgaChar* c = (struct VgaChar*)VGA_BUF_START;

  switch (in) {
    case '\n':
      VgaConsole::pos += VgaConsole::cols - VgaConsole::pos % VgaConsole::cols;
      VgaConsole::Scroll();
      break;
    case '\b':
      VgaConsole::pos--;
      break;
    case '\t':
      VgaConsole::pos += TAB_SIZE - (VgaConsole::pos % VgaConsole::cols) % TAB_SIZE;
      break;
    case '\r':
      VgaConsole::pos = VgaConsole::pos - VgaConsole::pos % VgaConsole::cols;
      break;
    default:
      VgaConsole::Scroll();
      c[VgaConsole::pos].blink = 0;
      c[VgaConsole::pos].bg_color = Color::Black;
      c[VgaConsole::pos].fg_color = Color::White;
      c[VgaConsole::pos].cp = in;
      VgaConsole::pos++;
  }
}
