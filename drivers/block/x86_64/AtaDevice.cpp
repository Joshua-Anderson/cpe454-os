#include "AtaDevice.h"
#include "arch/x86_64/arch.h"
#include "printk.h"
#include "proc/Queue.h"
#include "proc/Scheduler.h"
#include "syscall/SysCall.h"

#define ATA_REG_BASE 0x1F0
#define ATA_DATA_REG ATA_REG_BASE
#define ATA_ERR_REG ATA_REG_BASE + 1
#define ATA_FEAT_REG ATA_REG_BASE + 1
#define ATA_SECT_CNT_REG ATA_REG_BASE + 2
#define ATA_SECT_NUM_REG ATA_REG_BASE + 3
#define ATA_CYLN_LOW_REG ATA_REG_BASE + 4
#define ATA_CYLN_HIGH_REG ATA_REG_BASE + 5
#define ATA_HEAD_REG ATA_REG_BASE + 6
#define ATA_STATUS_REG ATA_REG_BASE + 7
#define ATA_CMD_REG ATA_REG_BASE + 7

#define ATA_REG_DEV_CTRL_BASE 0x3F6
#define ATA_ALT_STATUS_REG ATA_REG_DEV_CTRL_BASE
#define ATA_DEV_CNTRL_REG ATA_REG_DEV_CTRL_BASE
#define ATA_DRV_ADDR_REG ATA_REG_DEV_CTRL_BASE + 1

struct AtaErrReg {
  uint8_t amnf : 1;
  uint8_t tkznf : 1;
  uint8_t abort : 1;
  uint8_t mcr : 1;
  uint8_t id_not_found : 1;
  uint8_t mc : 1;
  uint8_t unc : 1;
  uint8_t bad_block : 1;
} __attribute__((__packed__));

struct AtaStatusReg {
  uint8_t err : 1;
  uint8_t zero : 2;
  uint8_t drq : 1;
  uint8_t srv : 1;
  uint8_t disk_fault : 1;
  uint8_t ready : 1;
  uint8_t busy : 1;
} __attribute__((__packed__));

struct AtaDevCntrlReg {
  uint8_t zero : 1;
  uint8_t irq_disable : 1;
  uint8_t sw_rst : 1;
  uint8_t res : 4;
  uint8_t hob : 1;
} __attribute__((__packed__));

struct AtaDrvAddrReg {
  uint8_t drive_0 : 1;
  uint8_t drive_1 : 1;
  uint8_t res : 6;
} __attribute__((__packed__));

static Queue inProgress = Queue();
static Queue blockedQueue = Queue();

void read_ata_block(uint64_t block) {
  outb(ATA_HEAD_REG, 0x40);                       // Set for master read
  outb(ATA_SECT_CNT_REG, 0x0);                    // Sector count high byte
  outb(ATA_SECT_NUM_REG, (block >> 24) & 0xFF);   // LBA4
  outb(ATA_CYLN_LOW_REG, (block >> 32) & 0xFF);   // LBA5
  outb(ATA_CYLN_HIGH_REG, (block >> 32) & 0xFF);  // LBA6
  outb(ATA_SECT_CNT_REG, 0x1);                    // Sector count low byte
  outb(ATA_SECT_NUM_REG, (block >> 0) & 0xFF);    // LBA1
  outb(ATA_CYLN_LOW_REG, (block >> 8) & 0xFF);    // LBA2
  outb(ATA_CYLN_HIGH_REG, (block >> 16) & 0xFF);  // LBA3
  outb(ATA_CMD_REG, 0x24);                        // Sector Read
}

struct AtaStatusReg get_ata_status() {
  struct AtaStatusReg status = {};
  uint8_t *val = (uint8_t *)&status;
  *val = inb(ATA_STATUS_REG);
  return status;
}

void ata_irq_handler(uint32_t, uint32_t) {
  printk("ATA IRQ!\n");
  get_ata_status();

  if (inProgress.Length() == 0) {
    FATAL("Unexpected ATA IRQ");
  }

  uint16_t *dest = (uint16_t *)inProgress.Peak()->block_data_2;
  for (int i = 0; i < ATA_BLOCK_SIZE / 2; i++) {
    *(dest + i) = inw(ATA_DATA_REG);
  }

  Scheduler::Unblock(&inProgress);
  if (blockedQueue.Length() > 0) {
    inProgress.Push(blockedQueue.Pop());
    read_ata_block(inProgress.Peak()->block_data_1);
  }
}

// Assuming master ATA device in slot one
// TODO: support drive autodetection and use multiple slots
AtaDevice::AtaDevice() {
  IRQ::Register(ATA_IRQ, ata_irq_handler);
  struct AtaDevCntrlReg dev_cntrl = {};
  dev_cntrl.sw_rst = 1;
  uint8_t *val = (uint8_t *)&dev_cntrl;
  outb(ATA_DEV_CNTRL_REG, *val);
  dev_cntrl.sw_rst = 0;
  outb(ATA_DEV_CNTRL_REG, *val);
  IRQ::ClearMask(ATA_IRQ);
  struct AtaStatusReg status = get_ata_status();
}

int AtaDevice::ReadBlock(uint64_t block, void *dest) {
  IRQ::Disable();
  Scheduler::BlockCurProc(&blockedQueue, block, (uint64_t)dest);

  if (inProgress.Length() == 0) {
    inProgress.Push(blockedQueue.Pop());
    read_ata_block(inProgress.Peak()->block_data_1);
  }

  IRQ::Enable();
  SysCall::ProcYield();
  return BlockSize();
}
