## Project SOL Operating System

This is a toy operating system I wrote from scratch as part of Cal Poly's CPE 454 - Implementation of Operating Systems course.

It currently targets x86_64, but there's nothing preventing it from being ported to other platforms.

It supports:
  - Virtual Memory and automatic physical memory detection from multiboot headers
  - PS2 keyboards
  - Graphical console (currently targeting IBM's standard VGA Text Mode)
  - Serial console
  - Kernel-space threads and scheduling (with blocking queue support)
  - ATA block devices

It's a little rough right now, but I hope to clean it up at some point it the future and add proper filesystem and userspace support.

All the code was written from scratch by me, with these exceptions:
  - The course provided some snippets to help us to the proper CPU bootstrapping to long mode (see `arch/x86_64/boot.asm`)
  - The snakes demo program to stress test our multithreading was written by Professor Niko for his operating systems course (see `init/snakes.cpp`).

