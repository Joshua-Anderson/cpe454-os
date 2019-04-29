#include "../IRQ.h"
#include "arch/x86_64/arch.h"
#include "printk.h"
#include "stdlib.h"

#define IDT_SIZE 256

#define PIC_MASTER_CMD_REG 0x20
#define PIC_MASTER_DATA_REG 0x21
#define PIC_SLAVE_CMD_REG 0xA0
#define PIC_SLAVE_DATA_REG 0xA1

#define PIC_INIT 0x11
#define ICW4_8086 0x01

#define PIC_MASTER_OFFSET 32
#define PIC_SLAVE_OFFSET 47

#define TYPE_IRQ 0xE
#define TYPE_TRAP 0xF

extern "C" {
extern void irq_1(void);
extern void irq_2(void);
extern void irq_3(void);
extern void irq_4(void);
extern void irq_5(void);
extern void irq_6(void);
extern void irq_7(void);
extern void irq_8(void);
extern void irq_9(void);
extern void irq_10(void);
extern void irq_11(void);
extern void irq_12(void);
extern void irq_13(void);
extern void irq_14(void);
extern void irq_15(void);
extern void irq_16(void);
extern void irq_17(void);
extern void irq_18(void);
extern void irq_19(void);
extern void irq_20(void);
extern void irq_21(void);
extern void irq_22(void);
extern void irq_23(void);
extern void irq_24(void);
extern void irq_25(void);
extern void irq_26(void);
extern void irq_27(void);
extern void irq_28(void);
extern void irq_29(void);
extern void irq_30(void);
extern void irq_31(void);
extern void irq_32(void);
extern void irq_33(void);
extern void irq_33(void);
extern void irq_34(void);
extern void irq_35(void);
extern void irq_36(void);
extern void irq_37(void);
extern void irq_38(void);
extern void irq_39(void);
extern void irq_40(void);
extern void irq_41(void);
extern void irq_42(void);
extern void irq_43(void);
extern void irq_44(void);
extern void irq_45(void);
extern void irq_46(void);
extern void irq_47(void);
}

struct IDT_entry {
  uint16_t tar_offset_1;
  uint16_t tar_sel;
  uint8_t res_1;
  uint8_t type : 4;
  uint8_t zero : 1;
  uint8_t dpl : 2;
  uint8_t p : 1;
  uint16_t tar_offset_2;
  uint32_t tar_offset_3;
  uint32_t res_2;
} __attribute__((packed));

struct IDT_entry IDT[IDT_SIZE];

static inline void load_idt(struct IDT_entry *idt, uint16_t size) {
  struct {
    uint16_t limit;
    void *base;
  } __attribute__((packed)) IDTR = {size, idt};

  asm volatile("lidt %0" : : "m"(IDTR));
}

static inline void init_idt(struct IDT_entry *idt, void (*handler)(void),
                            uint16_t sel, uint8_t type, uint8_t dpl) {
  idt->tar_offset_1 = (uint16_t)(((uintptr_t)handler >> 0) & 0xFFFF);
  idt->tar_offset_2 = (uint16_t)(((uintptr_t)handler >> 16) & 0xFFFF);
  idt->tar_offset_3 = (uint32_t)(((uintptr_t)handler >> 32) & 0xFFFFFFFF);
  idt->tar_sel = sel;
  idt->res_1 = 0;
  idt->res_2 = 0;
  idt->type = type;
  idt->dpl = dpl;
  idt->p = 1;
}

extern "C" void irq_handler() {
  int num, err;
  asm volatile("movl %%edi, %0" : "=rm"(num) :);
  asm volatile("movl %%esi, %0" : "=rm"(err) :);
  printk("IRQ %d Called (Err %d)!\n", num, err);
}

void IRQ::Enable() { asm volatile("sti" : :); }

void IRQ::Disable() { asm volatile("cli" : :); }

void IRQ::Init() {
  memset(IDT, 0, sizeof(struct IDT_entry) * IDT_SIZE);

  init_idt(&IDT[1], irq_1, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[2], irq_2, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[3], irq_3, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[4], irq_4, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[5], irq_5, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[6], irq_6, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[7], irq_7, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[8], irq_8, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[9], irq_9, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[10], irq_10, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[11], irq_11, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[12], irq_12, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[13], irq_13, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[14], irq_14, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[15], irq_15, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[16], irq_16, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[17], irq_17, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[18], irq_18, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[19], irq_18, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[20], irq_20, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[21], irq_21, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[22], irq_22, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[23], irq_23, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[24], irq_24, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[25], irq_25, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[26], irq_26, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[27], irq_27, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[28], irq_28, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[29], irq_29, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[30], irq_30, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[31], irq_31, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[32], irq_32, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[33], irq_33, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[34], irq_34, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[35], irq_35, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[36], irq_36, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[37], irq_37, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[38], irq_38, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[39], irq_39, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[40], irq_40, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[41], irq_41, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[42], irq_42, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[43], irq_43, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[44], irq_44, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[45], irq_45, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[46], irq_46, 0x8, TYPE_IRQ, 0);
  init_idt(&IDT[47], irq_47, 0x8, TYPE_IRQ, 0);

  // Start Initializing PIC
  outb(PIC_MASTER_CMD_REG, PIC_INIT);
  outb(PIC_SLAVE_CMD_REG, PIC_INIT);
  // ICW2: Master PIC vector offset
  outb(PIC_MASTER_DATA_REG, PIC_MASTER_OFFSET);
  // ICW2: Slave PIC vector offset
  outb(PIC_SLAVE_DATA_REG, PIC_SLAVE_OFFSET);
  // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
  outb(PIC_MASTER_DATA_REG, 4);
  // ICW3: tell Slave PIC its cascade identity (0000 0010)
  outb(PIC_SLAVE_DATA_REG, 2);

  outb(PIC_MASTER_DATA_REG, ICW4_8086);
  outb(PIC_SLAVE_DATA_REG, ICW4_8086);

  outb(PIC_MASTER_DATA_REG, 0);
  outb(PIC_SLAVE_DATA_REG, 0);

  load_idt(IDT, sizeof(struct IDT_entry) * (IDT_SIZE - 1));
  IRQ::Enable();
}
