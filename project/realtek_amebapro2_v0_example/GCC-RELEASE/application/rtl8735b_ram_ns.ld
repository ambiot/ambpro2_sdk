/* Linker script to configure memory regions. */

/* !! the include symbole may failed if the symbol file name is too long!! */
INCLUDE "romsym_ns.so"

/* unit - Mega byte */
DDR_SIZE = 128;
NN_SIZE = 8;

DDR_END = 0x70000000 + ((DDR_SIZE - 2) - NN_SIZE)*1024*1024;
NN_END  = 0x70000000 + (DDR_SIZE - 2)*1024*1024;

MEMORY
{
  ITCM_RAM (wrx) 		: ORIGIN = 0x00010000, LENGTH = 0x00017000 - 0x00010000		/*  28KB */
  DTCM_RAM (wrx) 		: ORIGIN = 0x20000000, LENGTH = 0x20002000 - 0x20000000		/*   8KB */

  /* The vector table, it must start with 256 bytes aligned address */
  /* The Reset_Handler also is the NS region code entry function */
  VECTORS_RAM (rwx)     : ORIGIN = 0x20100000, LENGTH = 0x20100200 - 0x20100000		/*  512B */
  
  /* 0x20100200 ~ 0x20101200 4KB is reserved for ROM used RAM bss */
  /* 0x20101200 ~ 0x20105200 16KB is reserved for Bootloader RAM code used */
  
  /* RAM functions entry table */
  RAM_FUN_TABLE (rwx)   : ORIGIN = 0x20105200, LENGTH = 0x201052F0 - 0x20105200		/*  240B */
  RAM_IMG_SIGN (rwx)    : ORIGIN = 0x201052F0, LENGTH = 0x20105300 - 0x201052F0  	/*   16B */
  
  /* Internal SRAM for text, rodata & data */
  RAM_1 (rwx)           : ORIGIN = 0x20105300, LENGTH = 0x20120000 - 0x20105300  /* 107KB */
  RAM_RETENTION (rwx)   : ORIGIN = 0x20120000, LENGTH = 0x20140000 - 0x20120000  /* 128KB retention data */
  RAM_2 (rwx)           : ORIGIN = 0x20140000, LENGTH = 0x20156000 - 0x20140000  /* 88KB */

  /* DDR memory */
  VOE    (rwx)          : ORIGIN = 0x70000000, LENGTH = 0x70100000 - 0x70000000	/*  1MB */
  /* DDR 64M */
  /*DDR    (rwx) 		: ORIGIN = 0x70100000, LENGTH = 0x73600000 - 0x70100000*/ /* 53MB */
  /*NN     (rwx)      	: ORIGIN = 0x73600000, LENGTH = 0x73E00000 - 0x73600000*/	/* 8MB */
  /* DDR */
  DDR    (rwx) 			: ORIGIN = 0x70100000, LENGTH = DDR_END - 0x70100000 /* 55 + 64MB */
  NN     (rwx)      	: ORIGIN =    DDR_END, LENGTH = NN_END - DDR_END 	 /* 8MB */  
}

/* Library configurations */

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __copy_table_start__
 *   __copy_table_end__
 *   __zero_table_start__
 *   __zero_table_end__
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 *   __Vectors_End
 *   __Vectors_Size
 */

ENTRY(ram_start)

__sram_end__		= ORIGIN(RAM_2) + LENGTH(RAM_2);
__eram_end__		= ORIGIN(DDR) + LENGTH(DDR);		

__voe_code_size__   = LENGTH(VOE);
__nn_eram_start__   = ORIGIN(NN);
__nn_eram_end__     = ORIGIN(NN) + LENGTH(NN);

SECTIONS
{
	.itcm_ram.code_rodata :
	{
		. = ALIGN(4);
		__itcm_ram_code_rodata_start__ = .;
		*(.itcm_ram.text*)

		*freertos_service.*o*(.text*)
		*cmsis_os.*o*(.text*)
		*freertos_cb.*o*(.text*)
		*freertos*event_groups.*o*(.text*)
		*freertos*croutine.*o*(.text*)
		*heap_4_2.*o*(.text*)
		*freertos*timers.*o*(.text*)
		*freertos*tasks.*o*(.text*)
		*freertos*queue.*o*(.text*)
		*freertos*port.*o*(.text*)
		*freertos*list.*o*(.text*)
		*freertos*stream_buffer.*o*(.text*)
		
		*(.itcm_ram.rodata*)
		*freertos*list.*o*(.rodata*)
		*freertos*queue.*o*(.rodata*)
		*freertos*stream_buffer.*o*(.rodata*)
		*freertos*tasks.*o*(.rodata*)
		*freertos*timers.*o*(.rodata*)
		*freertos*port.*o*(.rodata*)
		*heap_4_2.*o*(.rodata*)		
		*freertos_service.*o*(.rodata*)
		*freertos_service.*o*(.rodata.str1*)

		__itcm_ram_code_rodata_end__ = .;

	} > ITCM_RAM
	
	.ram.vector :
	{
		. = ALIGN(4);
		__ram_vector_start__ = .;
		KEEP(*(.ram_vector_table))
		__ram_vector_end__ = .;
	} > VECTORS_RAM

	.ram.img.signature :
	{
		__ram_img_signature__ = .;
		KEEP(*(.start.ram.sign*))		
	} > RAM_IMG_SIGN
	
	.ram.func.table :
	{
		__ram_start_table_start__ = .;
		KEEP(*(SORT(.start.ram.data*)))
		__ram_start_table_end__ = .;

	} > RAM_FUN_TABLE

    /* for C++ supporting */
	.ddr.code.text : 
    {
        . = ALIGN(32);
        __ddr_code_text_start__ = .;
        //*libgcc.a:pr-support.o*(.text*)
        //*libstdc++.a:list.o*(.text*)
		*libstdc++.a:*(.text*)
		*libgcc.a:*(.text*)
		*lib_a*(.text*)
        __ddr_code_text_end__ = .;
    } > DDR   	

	.dtcm_ram.bss :
	{
		/* Note: Boot loader data buffer & stack is allocated in DTCM RAM !! */
		/*       so do not allocate data/rodata/text in this section, it may cause boot loader failed */
		__dtcm_ram_bss_start__ = .;

		*(.dtcm_ram.bss*)
		
		__dtcm_ram_bss_end__ = .;
	} > DTCM_RAM

	.ram.data : AT (__etext)
	{
		__fw_img_start__ = .;
		__etext = .; 
		__data_start__ = .;
		__bl4voe_data_start__ = .;
        KEEP(*(.bl4voe.ram.data))
        __bl4voe_data_end__ = .;
		*(vtable)
		*lib_a*(.data*)
		*hal_sys_ctrl.*o*(.data*)
		*hal_dram*.*o*(.data*)
		*(.ram.data)

		. = ALIGN(4);
		/* preinit data */
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP(*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);

		. = ALIGN(4);
		/* init data */
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		PROVIDE_HIDDEN (__init_array_end = .);


		. = ALIGN(4);
		/* finit data */
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array))
		PROVIDE_HIDDEN (__fini_array_end = .);

		KEEP (*(.ctors*))
		KEEP (*(.dtors*))

		KEEP(*(.jcr*))
		. = ALIGN(4);
		/* All data end */
		__data_end__ = .;

	} > RAM_1

	.ram.code_text : AT (__etext2)
	{
		. = ALIGN(4);
		__etext2 = .;
		. = ALIGN(32);
		__ram_entry_text_start__ = .;
		*(.ram_entry_func.text*)
		__ram_entry_text_end__ = .;
		
		__ram_code_text_start__ = .;
		*ram_start.*o*(.text*)
		*app_start.*o*(.text*)
		*mpu_config.*o*(.text*)
		*cmd_shell.*o*(.text*)

		*hal_sys_ctrl.*o*(.text*)
		*hal_dram*.*o*(.text*)
		*hal_timer.*o*(.text*)
		*hal_uart.*o*(.text*)
		*hal_gdma.*o*(.text*)
		*hal_flash.*o*(.text*)
		*hal_wdt.*o*(.text*)
		*libc_wrap.*o*(.text*)
		
		. = ALIGN(4);
		__ram_code_text_end__ = .;		
	} > RAM_1

	.ram.code_rodata : AT (__ram_code_rodata_start__)
	{
		. = ALIGN(4);
		__ram_code_rodata_start__ = .;
		*lib_a*(.rodata*)
		*hal_sys_ctrl.*o*(.rodata*)
		*hal_dram*.*o*(.rodata*)
		*hal_flash.*o*(.rodata*)
		. = ALIGN(4);
		__ram_code_rodata_end__ = .;
	} > RAM_1

	.ram.retention.data :
	{
		*(.retention.data*)
	} > RAM_RETENTION

	.ram.bss :
	{
		. = ALIGN(4);
		__bss_start__ = .;
		*lib_a*(.bss*)
		*(COMMON)
		*hal_sys_ctrl.*o*(.bss*)
		*hal_dram*.*o*(.bss*)
		*hal_flash.*o*(.bss*)
	} > RAM_2

	.non_secure.bss :
	{
		. = ALIGN(16);
		__ns_bss_start__ = .;
		*(.nonsecure.bss*)
		*lib_a*(.nonsecure.bss*)

		. = ALIGN(4);
		__ns_bss_end__ = .;
		__bss_end__ = .;
		__sram_heap_start__ = .;
		
	} > RAM_2
	
	.heap (COPY):
	{
		__HeapBase = .;
		__end__ = .;
		end = __end__;
		KEEP(*(.heap*))
		__HeapLimit = .;
	} > DTCM_RAM

	/* .stack_dummy section doesn't contains any symbols. It is only
	 * used for linker to calculate size of stack sections, and assign
	 * values to stack symbols later */
	.stack_dummy (COPY):
	{
		KEEP(*(.stack*))
	} > DTCM_RAM

	.ddr.text :
	{
		. = ALIGN(4);
		__eram_text_start__ = .;
		*(.text*)
		*(.sdram.text*)
		__eram_text_end__ = .;
	} > DDR
	
	.ddr.rodata : 
	{
		. = ALIGN(4);
		__eram_rodata_start__ = .;
		*(.rodata*)
		*(.rodata.str1*)
		*(.sdram.rodata*)
		__eram_rodata_end__ = .;
	} > DDR
	
	/* Add This for C++ support */
	.ARM.extab  : 
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*) 

	} > DDR

	.ARM.exidx :
	{
		__exidx_start = .;
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
		__exidx_end = .;
	} > DDR
	
	.ddr.data :
	{
		. = ALIGN(4);
		__eram_data_start__ = .;
		*(.data*)
		*(.ddr.data)
		*(.sdram.data*)
		__eram_data_end__ = .;
	} > DDR

	.ddr.bss :
	{
		. = ALIGN(4);
		__eram_bss_start__ = .;
		*(.bss*)
		*(.sdram.bss*)
		__eram_bss_end__ = .;
		__eram_heap_start__ = .;
	} > DDR

	.voe :
	{
		. = ALIGN(4);
		__voe_code_start__ = .;
		
	} > VOE
	
	/* Set stack top to end of RAM, and stack limit move down by
	 * size of stack_dummy section */
	__StackTop = ORIGIN(DTCM_RAM) + LENGTH(DTCM_RAM);
	__StackLimit = __StackTop - SIZEOF(.stack_dummy);
	PROVIDE(__stack = __StackTop);

	/* Check if data + heap + stack exceeds RAM limit */
/* TODO:	ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack") */
}
