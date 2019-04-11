ARCH ?= x86_64
CROSS_CPP ?= /usr/local/454-cross/bin/x86_64-elf-g++
CROSS_LD ?= /usr/local/454-cross/bin/x86_64-elf-ld

override CPPFLAGS+=-Wall -Wextra -Werror -ffreestanding -fno-exceptions -fno-rtti -lgcc -Ilib -Ilib/$(ARCH)

ifeq ($(DEBUG),y)
   override CPPFLAGS+=-g
endif

KERNEL := out/sol.bin
IMG := out/sol.img
ROOTFS := out/rootfs

ASM_SRC := $(wildcard arch/$(ARCH)/*.asm)
ASM_OBJ := $(patsubst arch/$(ARCH)/%.asm, \
	out/arch/$(ARCH)/%.o, $(ASM_SRC))

C_HDR := $(wildcard lib/*.h lib/$(ARCH)/*.h drivers/display/*.h drivers/display/$(ARCH)/*.h)
C_SRC := $(wildcard init/*.cpp lib/*.cpp lib/$(ARCH)/*.cpp drivers/display/$(ARCH)/*.cpp)
C_OBJ := $(patsubst %.cpp, out/%.o, $(C_SRC))

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
	$(CROSS_LD) -n -T $(LD_SCRIPT) -o $(KERNEL) $(ASM_OBJ) $(C_OBJ)

out/arch/$(ARCH)/%.o: arch/$(ARCH)/%.asm
	@mkdir -p $(shell dirname $@)
	nasm -felf64 $< -o $@

out/init/%.o: init/%.cpp
	@mkdir -p $(shell dirname $@)
	$(CROSS_CPP) $(CPPFLAGS) -c $< -o $@

out/lib/%.o: lib/%.cpp
	@mkdir -p $(shell dirname $@)
	$(CROSS_CPP) $(CPPFLAGS) -Wno-builtin-declaration-mismatch -c $< -o $@

out/lib/$(ARCH)/%.o: lib/$(ARCH)/%.cpp
	@mkdir -p $(shell dirname $@)
	$(CROSS_CPP) $(CPPFLAGS) -Wno-builtin-declaration-mismatch -c $< -o $@

out/drivers/display/$(ARCH)/%.o: drivers/display/$(ARCH)/%.cpp
	@mkdir -p $(shell dirname $@)
	$(CROSS_CPP) $(CPPFLAGS) -c $< -o $@
