#include "stdint.h"
#include "stdlib.h"
#include "VgaConsole.h"
#include "../Color.h"

#define VGA_BUF_START 0xb8000

struct VgaChar {
  char cp;
  uint8_t fg_color : 4;
  uint8_t bg_color : 3;
  uint8_t blink : 1;
} __attribute__((__packed__));

void VgaConsole::Scroll() {
  if(VgaConsole::pos < VgaConsole::cols * VgaConsole::rows) {
    return;
  }

  struct VgaChar* c = (struct VgaChar*) VGA_BUF_START;
  memcpy(c, c+VgaConsole::cols, VgaConsole::cols);

  VgaConsole::pos = (VgaConsole::cols - 1) * VgaConsole::rows;
}

void VgaConsole::PrintChar(char in) {
  struct VgaChar* c = (struct VgaChar*) VGA_BUF_START;

  switch(in) {
  case '\n':
    VgaConsole::Scroll();
    VgaConsole::pos += VgaConsole::cols - VgaConsole::pos % VgaConsole::cols;
    break;
  case '\b':
    VgaConsole::pos--;
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
