ARCH ?= x86_64
CROSS_CC ?= /usr/local/454-cross/bin/x86_64-elf-gcc

override CFLAGS+=-Wall -Werror -ffreestanding -lgcc -Ilib

ifeq ($(DEBUG),y)
   override CFLAGS+=-g
endif

KERNEL := out/sol.bin
IMG := out/sol.img
ROOTFS := out/rootfs

ASM_SRC := $(wildcard arch/$(ARCH)/*.asm)
ASM_OBJ := $(patsubst arch/$(ARCH)/%.asm, \
	out/arch/$(ARCH)/%.o, $(ASM_SRC))

C_HDR := $(wildcard lib/*.h)
C_SRC := $(wildcard init/*.c lib/*.c)
C_OBJ := $(patsubst %.c, out/%.o, $(C_SRC))

LD_SCRIPT := arch/$(ARCH)/linker.ld
GRUB_CFG := arch/$(ARCH)/grub.cfg

all: $(KERNEL)

.PHONY: run
run: $(IMG)
	qemu-system-x86_64 -drive format=raw,file=out/sol.img

.PHONY: clean
clean:
	rm -rf out

.PHONY: img
img: $(IMG)

.PHONY : format
format:
	clang-format -i -style=google $(C_SRC) $(C_HDR)

$(IMG): $(KERNEL) $(GRUB_CFG)
	mkdir -p $(ROOTFS)/boot/grub2
	cp $(GRUB_CFG) $(ROOTFS)/boot/grub2/
	cp $(KERNEL) $(ROOTFS)/boot/
	./mkimage.sh $(IMG) $(ROOTFS)

$(KERNEL): $(ASM_OBJ) $(C_OBJ) $(LD_SCRIPT)
	ld -n -T $(LD_SCRIPT) -o $(KERNEL) $(ASM_OBJ) $(C_OBJ)

out/arch/$(ARCH)/%.o: arch/$(ARCH)/%.asm
	@mkdir -p $(shell dirname $@)
	nasm -felf64 $< -o $@

out/init/%.o: init/%.c
	@mkdir -p $(shell dirname $@)
	$(CROSS_CC) $(CFLAGS) -c $< -o $@

out/lib/%.o: lib/%.c
	@mkdir -p $(shell dirname $@)
	$(CROSS_CC) $(CFLAGS) -Wno-builtin-declaration-mismatch -c $< -o $@
