ARCH ?= x86_64

KERNEL := out/sol.bin
IMG := out/sol.img
ROOTFS := out/rootfs

ASM_SRC := $(wildcard arch/$(ARCH)/*.asm)
ASM_OBJ := $(patsubst arch/$(ARCH)/%.asm, \
	out/arch/$(ARCH)/%.o, $(ASM_SRC))

LD_SCRIPT := arch/$(ARCH)/linker.ld
GRUB_CFG := arch/$(ARCH)/grub.cfg

all: $(KERNEL)

run: $(IMG)
	qemu-system-x86_64 -drive format=raw,file=out/sol.img

clean:
	rm -rf out

img: $(IMG)

$(IMG): $(KERNEL) $(GRUB_CFG)
	mkdir -p $(ROOTFS)/boot/grub2
	cp $(GRUB_CFG) $(ROOTFS)/boot/grub2/
	cp $(KERNEL) $(ROOTFS)/boot/
	./mkimage.sh $(IMG) $(ROOTFS)

$(KERNEL): $(ASM_OBJ) $(LD_SCRIPT)
	ld -n -T $(LD_SCRIPT) -o $(KERNEL) $(ASM_OBJ)

out/arch/$(ARCH)/%.o: arch/$(ARCH)/%.asm
	@mkdir -p $(shell dirname $@)
	nasm -felf64 $< -o $@
