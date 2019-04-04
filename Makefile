run: sol.img
	qemu-system-x86_64 -drive format=raw,file=sol.img

sol.img:
	./mkimage.sh
