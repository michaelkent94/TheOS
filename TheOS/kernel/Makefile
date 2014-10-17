HOST?=$(shell ../default-host.sh)
HOSTARCH:=$(shell ../target-triplet-to-arch.sh $(HOST))

CFLAGS?=-O2 -g
CPPFLAGS?=
LDFLAGS?=
LIBS?=

DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BOOTDIR?=$(EXEC_PREFIX)/boot
INCLUDEDIR?=$(PREFIX)/include

CFLAGS:=$(CFLAGS) -D__is_theos_kernel -Iinclude -ffreestanding -fbuiltin -Wall -Wextra
CPPFLAGS:=$(CPPFLAGS) -fno-rtti -fno-exceptions
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -nostdlib -lk -lgcc

ARCHDIR:=arch/$(HOSTARCH)

include $(ARCHDIR)/make.config

CFLAGS:=$(CFLAGS) $(KERNEL_ARCH_CFLAGS)
CPFLAGS:=$(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
LDFLAGS:=$(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)
LIBS:=$(LIBS) $(KERNEL_ARCH_LIBS)

OBJS:=\
$(KERNEL_ARCH_OBJS) \
kernel/kernel.o \
kernel/tables.o \
kernel/interrupts.o \
kernel/timer.o \
kernel/keyboard.o \
kernel/rtc.o \
kernel/pmm.o \

CRTI_OBJ:=$(ARCHDIR)/crti.o
CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtend.o)
CRTN_OBJ:=$(ARCHDIR)/crtn.o

ALL_OUR_OBJS:=\
$(CRTI_OBJ) \
$(OBJS) \
$(CRTN_OBJ) \

OBJ_LINK_LIST:=\
$(CRTI_OBJ) \
$(CRTBEGIN_OBJ) \
$(OBJS) \
$(CRTEND_OBJ) \
$(CRTN_OBJ) \

all: theos.kernel

.PHONY: all clean install install-headers install-kernel

theos.kernel: $(OBJ_LINK_LIST) $(ARCHDIR)/linker.ld
	$(CPP) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(OBJ_LINK_LIST) $(LDFLAGS) $(LIBS)

%.o: %.cpp
	$(CPP) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ -std=gnu11 $(CFLAGS)

%.o: %.s
	$(CC) -c $< -o $@ $(CFLAGS)
	
%.o: %.nasm
	$(NASM) -f elf32 -o $@ $<

clean:
	rm -f theos.kernel $(OBJS) $(ALL_OUR_OBJS) *.o */*.o */*/*.o

install: install-headers install-kernel

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -RTv include $(DESTDIR)$(INCLUDEDIR)

install-kernel: theos.kernel
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp theos.kernel $(DESTDIR)$(BOOTDIR)
