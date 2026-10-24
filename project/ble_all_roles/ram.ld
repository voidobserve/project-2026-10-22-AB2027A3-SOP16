#include "config.h"

ENTRY(_start)

//Define the flash max size
__max_flash_size = FLASH_CODE_SIZE;

__base = 0x10000000;

// Define the area size
__cache_stack_vma   = 0x10600;  //for loader
__cache_stack_size  = 512;      //for loader
__stack_vma         = 0x10800;
__stack_ram_size    = 1k;
__sram_vma          = 0x10c00;
__sram_ram_size     = 29k;
__comm_vma          = __sram_vma;
__bram_vma          = 0x18000;
__bram_ram_size     = 32k;



MEMORY
{
    init            : org = __base,             len = 512
    flash(rx)       : org = __base + 512,       len = __max_flash_size

    stack           : org = __stack_vma,        len = __stack_ram_size
    sram            : org = __sram_vma,         len = __sram_ram_size
    bram            : org = __bram_vma,         len = __bram_ram_size
}

SECTIONS
{
    .init : {
        *(.reset)
    } > init

    .comm : {
        *(.vector)
        *(.com_text*)
        *(.com_rodata*)
        *debug.o(.rodata*)
        #if SYS_SLEEP_EN
        *(.com_sleep*)
        #endif
        *(.com_periph.*)
        *(.data*)
        *(.sdata*)
        *(.mp3_text*)
        . = ALIGN(512);
    } > sram AT > flash

    .flash : {
         __code_start_app_proc = .;
        *(.text.app.proc.*)
        . = ALIGN(512);
        __code_end_app_proc = .;

        __code_start_stack_send = .;
        *(.text.stack.send.*)
        . = ALIGN(512);
        __code_end_stack_send = .;

        #if !SYS_SLEEP_EN
        __code_start_sleep_send = .;
        *(.com_sleep*)
        . = ALIGN(512);
        __code_end_sleep_send = .;
        #endif

        *(.text*)
        *(.rodata*)
        *(.srodata*)
        *(.rela*)
        LONG(0)
        . = ALIGN(512);

    } > flash

    .stack (NOLOAD) : {
        __irq_stack_start = .;
        . = 0x400;
        __irq_stack = .;
    } > stack
    __irq_stack_size = __irq_stack - __irq_stack_start;

    .bss (NOLOAD) : {
         __heap_start = .;
        *(.mem_heap)
        __heap_end = .;

        __bss_start = .;
        *(COMMON)
        *(.bss*)
        *(.sbss*)
        *(.buf*)
        *(.btmem.stack.l2cap)
        __bss_end = .;
    } > sram
    __bss_size = __bss_end - __bss_start;
    __heap_size = __heap_end - __heap_start;

    .bram __bram_vma (NOLOAD) : {
        __bram_start = .;
        *(.btmem.bthw)
        *(.btmem*)

        *(.ble_cache*)
        *(.ble_buf*)
        *(.codec*)
        *(.fot_data.*)
        *(.tbox.*)
        *(.dut.huart_buf)
        *(.hci.h3*)
        *(.prod_test*)
        __bram_end = .;
    } > bram
    __bram_size = __bram_end - __bram_start;
}

//Calc the lma
__bank_size = SIZEOF(.flash);
__comm_lma = LOADADDR(.comm);
__comm_size = SIZEOF(.comm);
