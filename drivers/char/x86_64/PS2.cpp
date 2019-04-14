#include <stdint.h>
#include "PS2.h"

#define PS2_DATA_REG   0x60
#define PS2_CMD_REG    0x64
#define PS2_STATUS_REG 0x64

#define PS2_CMD_ENABLE_P1  0xAE
#define PS2_CMD_DISABLE_P1 0xAD
#define PS2_CMD_ENABLE_P2  0xA8
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

static inline void outb(uint16_t port, uint8_t val) {
  asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
  uint8_t ret;
  asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
  return ret;
}

static inline void poll_ps2_outb(uint8_t val) {
  while(1) {
    uint8_t status_reg = inb(PS2_STATUS_REG);
    struct PS2_StatusReg* status = (struct PS2_StatusReg*) &status_reg;
    if(status->outb == val) {
      break;
    }
  }
}

static inline void poll_ps2_inb(uint8_t val) {
  while(1) {
    uint8_t status_reg = inb(PS2_STATUS_REG);
    struct PS2_StatusReg* status = (struct PS2_StatusReg*) &status_reg;
    if(status->inb == val) {
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
  struct PS2_CtrlCfg* ctrl_cfg = (struct PS2_CtrlCfg*) &data_reg;

  // Enable clock for keyboard
  ctrl_cfg->p1_clk = 1;

  outb(PS2_CMD_REG, PS2_CMD_CTRL_CFG_W);
  poll_ps2_inb(0);
  outb(PS2_DATA_REG, data_reg);

  // Renable and reset the keyboard
  outb(PS2_CMD_REG, PS2_CMD_ENABLE_P1);
  poll_ps2_inb(0);
  outb(PS2_DATA_REG, PS2_DEV_RST);
}

char PS2::GetChar() {
  // Poll into there is something in the output buffer
  poll_ps2_outb(1);

  switch(inb(PS2_DATA_REG)) {
  case 0x02: return '1';
  case 0x03: return '2';
  case 0x04: return '3';
  case 0x05: return '4';
  case 0x06: return '5';
  case 0x07: return '6';
  case 0x08: return '7';
  case 0x09: return '8';
  case 0x0A: return '9';
  case 0x0B: return '0';
  case 0x0C: return '-';
  case 0x0D: return '=';
  case 0x0E: return '\b';
  case 0x0F: return '\t';
  default: return PS2::GetChar();
  }
}
