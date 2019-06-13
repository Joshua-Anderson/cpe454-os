#include "VgaConsole.h"
#include "stdint.h"
#include "stdlib.h"

struct VgaChar {
  char cp;
  uint8_t fg_color : 4;
  uint8_t bg_color : 3;
  uint8_t blink : 1;
} __attribute__((__packed__));

#define VGA_BUF_START 0xb8000
#define TAB_SIZE 4

static struct VgaChar* vga_console = (struct VgaChar*)VGA_BUF_START;

static inline void clear_vga_char(unsigned pos) {
  vga_console[pos].blink = 0;
  vga_console[pos].cp = ' ';
}

bool VgaConsole::Scroll() {
  if (VgaConsole::pos < VgaConsole::cols * VgaConsole::rows) {
    return false;
  }

  memcpy(vga_console, vga_console + VgaConsole::cols,
         VgaConsole::cols * (VgaConsole::rows - 1) * sizeof(struct VgaChar));

  VgaConsole::pos = VgaConsole::cols * (VgaConsole::rows - 1);

  for (unsigned i = VgaConsole::pos; i < VgaConsole::pos + VgaConsole::cols;
       i++) {
    clear_vga_char(i);
  }

  return true;
}

void VgaConsole::PrintChar(char in, uint8_t fg, uint8_t bg, unsigned row,
                           unsigned col) {
  unsigned p = (row * VgaConsole::cols) + col;
  vga_console[p].blink = 0;
  vga_console[p].bg_color = bg;
  vga_console[p].fg_color = fg;
  vga_console[p].cp = in;
}

void VgaConsole::PrintChar(char in, uint8_t fg, uint8_t bg) {
  switch (in) {
    case '\n':
      VgaConsole::pos += VgaConsole::cols - VgaConsole::pos % VgaConsole::cols;
      VgaConsole::Scroll();
      break;
    case '\b':
      clear_vga_char(VgaConsole::pos);
      VgaConsole::pos--;
      break;
    case '\t':
      VgaConsole::pos +=
          TAB_SIZE - (VgaConsole::pos % VgaConsole::cols) % TAB_SIZE;
      break;
    case '\r':
      VgaConsole::pos = VgaConsole::pos - VgaConsole::pos % VgaConsole::cols;
      break;
    default:
      VgaConsole::Scroll();
      vga_console[VgaConsole::pos].blink = 0;
      vga_console[VgaConsole::pos].bg_color = bg;
      vga_console[VgaConsole::pos].fg_color = fg;
      vga_console[VgaConsole::pos].cp = in;
      VgaConsole::pos++;
  }
}

void VgaConsole::PrintChar(char in) {
  VgaConsole::PrintChar(in, VgaConsole::fg, VgaConsole::bg);
}

void VgaConsole::Clear() {
  for (unsigned i = 0; i < VgaConsole::rows * VgaConsole::cols; i++) {
    clear_vga_char(i);
  }
  VgaConsole::pos = 0;
}

unsigned VgaConsole::Rows() { return VgaConsole::rows; }

unsigned VgaConsole::Cols() { return VgaConsole::cols; }
