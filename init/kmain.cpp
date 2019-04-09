extern "C" void kmain();

void kmain() {
  long *vga = (long *)0xb8000;
  *vga = 0x2f592f412f4b2f4f;
  return;
}
