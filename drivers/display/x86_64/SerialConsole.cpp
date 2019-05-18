#include <stdint.h>
#include "stdlib.h"
#include "arch/x86_64/arch.h"
#include "SerialConsole.h"
#include "printk.h"
#include "irq/IRQ.h"

#define SERIAL_DATA_REG 0x3F8
#define SERIAL_IE_REG SERIAL_DATA_REG+1
#define SERIAL_FIFO_REG SERIAL_DATA_REG+2
#define SERIAL_II_REG SERIAL_FIFO_REG
#define SERIAL_LC_REG SERIAL_DATA_REG+3
#define SERIAL_MC_REG SERIAL_DATA_REG+3
#define SERIAL_LS_REG SERIAL_DATA_REG+5

#define PARITY_NONE 0
#define DATA_BITS_8 3
#define ONE_STOP_BIT 0
#define FIFO_DEPTH_1 0

#define STUCT2BYTE(s) *((uint8_t *) &s)

struct IER {
  uint8_t data_avail : 1;
  uint8_t tx_empty : 1;
  uint8_t err : 1;
  uint8_t stat_change : 1;
  uint8_t res : 4;
} __attribute__((packed));

struct LCR {
  uint8_t data_bits : 2;
  uint8_t stop_bits : 1;
  uint8_t parity : 3;
  uint8_t res : 1;
  uint8_t dlab : 1;
} __attribute__((packed));

struct FCR {
  uint8_t fifo_en : 1;
  uint8_t fifo_clr_rx : 1;
  uint8_t fifo_clr_tx : 1;
  uint8_t dma_mode : 1;
  uint8_t res : 1;
  uint8_t fifo_64 : 1;
  uint8_t fifo_queue_depth : 2;
} __attribute__((packed));

struct MCR {
  uint8_t data_ready : 1;
  uint8_t req_to_send : 1;
  uint8_t aux_1 : 1;
  uint8_t aux_2 : 1;
  uint8_t loop : 1;
  uint8_t autoflow : 1;
  uint8_t res : 2;
} __attribute__((packed));

struct LSR {
  uint8_t data_ready : 1;
  uint8_t overrun_err : 1;
  uint8_t parity_err : 1;
  uint8_t framing_err : 1;
  uint8_t break_indicator : 1;
  uint8_t tx_empty_hld : 1;
  uint8_t tx_empty : 1;
  uint8_t impending_err : 1;
} __attribute__((packed));

#define IIR_STATUS_TX_EMPTY 0b001

struct IIR {
  uint8_t irq_pend : 1;
  uint8_t status : 3;
  uint8_t res : 2;
  uint8_t fifo_info : 2;
} __attribute__((packed));

#define BUF_SIZE 256
#define BUF_INC(v) v = ((v+1) % BUF_SIZE)

char buf[256];
int buf_prod = 0;
int buf_consumer = 0;

inline void serial_tx(char c) {
  outb(SERIAL_DATA_REG, c);
}

void serial_irq_handler(unsigned int, unsigned int) {
  printk("Serial interrupt\n");

  uint8_t iir_dat = inb(SERIAL_II_REG);
  struct IIR* iir = (struct IIR*) &iir_dat;

  if(iir->status != IIR_STATUS_TX_EMPTY) {
    printk("ERR: Serial IRQ not a TX Empty Interrupt %hx\n", iir_dat);
    return;
  }

  if(buf_prod == buf_consumer) {
    return;
  }

  serial_tx(buf[buf_consumer]);
  BUF_INC(buf_prod);
}

inline int serial_idle() {
  uint8_t lsr_dat = inb(SERIAL_LS_REG);
  struct LSR* lsr = (struct LSR *) &lsr_dat;
  return lsr->tx_empty;
}

void SerialConsole::PrintChar(char c) {
  IRQ::Disable();
  buf[buf_prod] = c;
  BUF_INC(buf_prod);

  if(buf_consumer == buf_prod) {
    BUF_INC(buf_consumer);
  }

  if(!serial_idle()) {
    return;
  }

  serial_tx(c);
  BUF_INC(buf_consumer);
  IRQ::Enable();
}

SerialConsole::SerialConsole() {
  // Disable and register IRQs
  struct IER ier = {};
  outb(SERIAL_IE_REG, STUCT2BYTE(ier));
  IRQ::Register(SERIAL_IRQ, serial_irq_handler);

  // Set baud rate to 115200
  // Requires setting the divisior to 1 by setting dlab bit
  struct LCR lcr = {};
  lcr.dlab = 1;
  outb(SERIAL_LC_REG, STUCT2BYTE(lcr));
  outb(SERIAL_DATA_REG, 1); // LSB
  outb(SERIAL_IE_REG, 0); // MSB

  // Clear dlab and set 8N1 serial mode
  lcr.dlab = 0;
  lcr.data_bits = DATA_BITS_8;
  lcr.stop_bits = ONE_STOP_BIT;
  lcr.parity = PARITY_NONE;
  outb(SERIAL_LC_REG, STUCT2BYTE(lcr));

  struct FCR fcr = {};
  fcr.fifo_en = 1;
  fcr.fifo_clr_tx = 1;
  fcr.fifo_queue_depth = FIFO_DEPTH_1;
  outb(SERIAL_FIFO_REG, STUCT2BYTE(fcr));

  struct MCR mcr = {};
  mcr.data_ready = 1;
  mcr.aux_2 = 1;
  outb(SERIAL_MC_REG, STUCT2BYTE(mcr));

  ier.tx_empty = 1;
  outb(SERIAL_IE_REG, STUCT2BYTE(ier));

  IRQ::ClearMask(SERIAL_IRQ);
}
