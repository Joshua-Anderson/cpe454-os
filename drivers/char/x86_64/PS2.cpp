#include <stdint.h>

#include "PS2.h"
#include "arch/x86_64/arch.h"

#define PS2_DATA_REG 0x60
#define PS2_CMD_REG 0x64
#define PS2_STATUS_REG 0x64

#define PS2_CMD_ENABLE_P1 0xAE
#define PS2_CMD_DISABLE_P1 0xAD
#define PS2_CMD_ENABLE_P2 0xA8
#define PS2_CMD_DISABLE_P2 0xA7
#define PS2_CMD_CTRL_CFG_R 0x20
#define PS2_CMD_CTRL_CFG_W 0x60

#define PS2_DEV_RST 0xFF

struct PS2_StatusReg {
  uint8_t outb : 1;
  uint8_t inb : 1;
  uint8_t cmd_or_data : 1;
  uint8_t unkn : 2;
  uint8_t tout : 1;
  uint8_t perr : 1;
} __attribute__((__packed__));

struct PS2_CtrlCfg {
  uint8_t p1_intr : 1;
  uint8_t p2_intr : 1;
  uint8_t sys_flg : 1;
  uint8_t zero : 1;
  uint8_t p1_clk : 1;
  uint8_t p2_clk : 1;
  uint8_t p1_trns : 1;
  uint8_t zero2 : 1;
} __attribute__((__packed__));

static inline void poll_ps2_outb(uint8_t val) {
  while (1) {
    uint8_t status_reg = inb(PS2_STATUS_REG);
    struct PS2_StatusReg* status = (struct PS2_StatusReg*)&status_reg;
    if (status->outb == val) {
      break;
    }
  }
}

static inline void poll_ps2_inb(uint8_t val) {
  while (1) {
    uint8_t status_reg = inb(PS2_STATUS_REG);
    struct PS2_StatusReg* status = (struct PS2_StatusReg*)&status_reg;
    if (status->inb == val) {
      break;
    }
  }
}

void PS2::Init() {
  // Disable both PS2 channels
  outb(PS2_CMD_REG, PS2_CMD_DISABLE_P1);
  outb(PS2_CMD_REG, PS2_CMD_DISABLE_P2);

  // Flush output buffer
  inb(PS2_DATA_REG);

  // Read existing controller configuration
  outb(PS2_CMD_REG, PS2_CMD_CTRL_CFG_R);
  poll_ps2_outb(1);
  uint8_t data_reg = inb(PS2_DATA_REG);
  struct PS2_CtrlCfg* ctrl_cfg = (struct PS2_CtrlCfg*)&data_reg;

  // Enable clock for keyboard
  ctrl_cfg->p1_intr = 1;
  ctrl_cfg->p1_clk = 1;

  outb(PS2_CMD_REG, PS2_CMD_CTRL_CFG_W);
  poll_ps2_inb(0);
  outb(PS2_DATA_REG, data_reg);

  // Renable and reset the keyboard
  outb(PS2_CMD_REG, PS2_CMD_ENABLE_P1);
  poll_ps2_inb(0);
  outb(PS2_DATA_REG, PS2_DEV_RST);
}

char PS2::GetLetter(char c) {
  if (PS2::shift) {
    return c - 32;
  }

  return c;
}

char PS2::GetChar() {
  // Poll into there is something in the output buffer
  poll_ps2_outb(1);

  switch (inb(PS2_DATA_REG)) {
    case 0x02:
      return '1';
    case 0x03:
      return '2';
    case 0x04:
      return '3';
    case 0x05:
      return '4';
    case 0x06:
      return '5';
    case 0x07:
      return '6';
    case 0x08:
      return '7';
    case 0x09:
      return '8';
    case 0x0A:
      return '9';
    case 0x0B:
      return '0';
    case 0x0C:
      return '-';
    case 0x0D:
      return '=';
    case 0x0E:
      return '\b';
    case 0x0F:
      return '\t';
    case 0x10:
      return PS2::GetLetter('q');
    case 0x11:
      return PS2::GetLetter('w');
    case 0x12:
      return PS2::GetLetter('e');
    case 0x13:
      return PS2::GetLetter('r');
    case 0x14:
      return PS2::GetLetter('t');
    case 0x15:
      return PS2::GetLetter('y');
    case 0x16:
      return PS2::GetLetter('u');
    case 0x17:
      return PS2::GetLetter('i');
    case 0x18:
      return PS2::GetLetter('o');
    case 0x19:
      return PS2::GetLetter('p');
    case 0x1A:
      return '[';
    case 0x1B:
      return ']';
    case 0x1C:
      return '\n';
    //...
    case 0x1E:
      return PS2::GetLetter('a');
    case 0x1F:
      return PS2::GetLetter('s');
    case 0x20:
      return PS2::GetLetter('d');
    case 0x21:
      return PS2::GetLetter('f');
    case 0x22:
      return PS2::GetLetter('g');
    case 0x23:
      return PS2::GetLetter('h');
    case 0x24:
      return PS2::GetLetter('j');
    case 0x25:
      return PS2::GetLetter('k');
    case 0x26:
      return PS2::GetLetter('l');
    case 0x27:
      return PS2::GetLetter(';');
    case 0x28:
      return PS2::GetLetter('\'');
    case 0x29:
      return PS2::GetLetter('`');
    case 0x2A:
      PS2::shift = true;
      return PS2::GetChar();
    case 0x2B:
      return PS2::GetLetter('\\');
    case 0x2C:
      return PS2::GetLetter('z');
    case 0x2D:
      return PS2::GetLetter('x');
    case 0x2E:
      return PS2::GetLetter('c');
    case 0x2F:
      return PS2::GetLetter('v');
    case 0x30:
      return PS2::GetLetter('b');
    case 0x31:
      return PS2::GetLetter('n');
    case 0x32:
      return PS2::GetLetter('m');
    case 0x33:
      return ',';
    case 0x34:
      return '.';
    case 0x35:
      return '/';
    case 0x36:
      PS2::shift = true;
      return PS2::GetChar();
    case 0x37:
      return '*';
    //...
    case 0x39:
      return ' ';
    //...
    case 0xAB:
      PS2::shift = false;
      return PS2::GetChar();
    //...
    case 0xB6:
      PS2::shift = false;
      return PS2::GetChar();
    default:
      return PS2::GetChar();
  }
}
