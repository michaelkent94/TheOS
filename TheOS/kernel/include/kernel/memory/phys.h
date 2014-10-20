#ifndef _PHYS_H
#define _PHYS_H 1

#include <stdbool.h>
#include <stdint.h>
#include <kernel/multiboot.h>

typedef uint32_t phys_addr_t;

void phys_init(multiboot_info_t *mbd, phys_addr_t kstart, phys_addr_t kend);
void phys_load_stack();
bool phys_is_stack_empty();
bool phys_is_stack_full();
phys_addr_t phys_frame_alloc();
void phys_frame_free(phys_addr_t frame);

#endif
