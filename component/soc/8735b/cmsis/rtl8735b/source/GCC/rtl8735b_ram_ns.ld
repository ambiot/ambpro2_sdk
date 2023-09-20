/* Linker script to configure memory regions. */

/* !! the include symbole may failed if the symbol file name is too long!! */
INCLUDE "romsym_ns.so"

MEMORY
{
  ITCM_RAM (wrx) 		: ORIGIN = 0x00010000, LENGTH = 0x00017000 - 0x00010000		/*  28KB */
  DTCM_RAM (wrx) 		: ORIGIN = 0x20000000, LENGTH = 0x20002000 - 0x20000000		/*   8KB */

  /* The vector table, it must start with 256 bytes aligned address */
  /* The Reset_Handler also is the NS region code entry function */
  VECTORS_RAM (rwx)     : ORIGIN = 0x20100000, LENGTH = 0x20100200 - 0x20100000		/*  512B */
  
  /* 0x20100200 ~ 0x20101200 4KB is reserved for ROM used RAM bss */
  /* 0x20101200 ~ 0x20105200 16KB is reserved for Bootloader RAM code used */
  /* 0x20105200 ~ 0x20106200 4KB is reserved for Bootloader RAM BSS used */
  
  /* RAM functions entry table */
  RAM_FUN_TABLE (rwx)   : ORIGIN = 0x20106200, LENGTH = 0x201062F0 - 0x20106200		/*  240B */
  RAM_IMG_SIGN (rwx)    : ORIGIN = 0x201062F0, LENGTH = 0x20106300 - 0x201062F0  	/*   16B */
  
  /* Internal SRAM for text, rodata & data */
  RAM (rwx)             : ORIGIN = 0x20106300, LENGTH = 0x20156000 - 0x20106300		/*  319.25KB */

  /* DDR memory */
  VOE (rwx)          	: ORIGIN = 0x70000000, LENGTH = 0x70100000 - 0x70000000		/*  1MB */
  DDR_NC (rwx)	        : ORIGIN = 0x70100000, LENGTH = 0x70300000 - 0x70100000		/*  2MB */
  DDR (rwx)	            : ORIGIN = 0x70300000, LENGTH = 0x72E00000 - 0x70300000		/* 43MB */
  NN  (rwx)      		: ORIGIN = 0x72E00000, LENGTH = 0x73E00000 - 0x72E00000		/* 16MB */

  /* XIP Chiper section: TEXT/RODATA in this section can be encrypted (decrypt by SEC) */
  XIP_FLASH  (rx) 		: ORIGIN = 0x08300000, LENGTH = 0x08380000 - 0x08300000 /* 512KB */
  XIP_RO_FLASH  (rx) 	: ORIGIN = 0x08380000, LENGTH = 0x08390000 - 0x08380000 /* 64KB */
}

/* Library configurations */
GROUP(libgcc.a libc.a libm.a libnosys.a)

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

__sram_end__		= ORIGIN(RAM) + LENGTH(RAM);
__eram_end__		= ORIGIN(DDR) + LENGTH(DDR);		
__eram_ncnb_end__	= ORIGIN(DDR_NC) + LENGTH(DDR_NC);	

__voe_code_size__   = LENGTH(VOE);

SECTIONS
{
	.itcm_ram.code_rodata :
	{
		. = ALIGN(4);
		__itcm_ram_code_rodata_start__ = .;
		*(.itcm_ram.text*)

		*freertos_service.o(.text*)
		*cmsis_os.o(.text*)
		*freertos_cb.o(.text*)
		*event_groups.o(.text*)
		*croutine.o(.text*)
		*heap_4_2.o(.text*)
		*timers.o(.text*)
		*tasks.o(.text*)
		*queue.o(.text*)
		*port.o(.text*)
		*list.o(.text*)
		*stream_buffer.o(.text*)
		
		*(.itcm_ram.rodata*)
		*list.o(.rodata*)
		*queue.o(.rodata*)
		*stream_buffer.o(.rodata*)
		*tasks.o(.rodata*)
		*timers.o(.rodata*)
		*port.o(.rodata*)
		*heap_4_2.o(.rodata*)		
		*freertos_service.o(.rodata*)
		*freertos_service.o(.rodata.str1*)

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
		*hal_sys_ctrl.o(.data*)
		*hal_dram*.o(.data*)
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

		KEEP(*(.jcr*))
		. = ALIGN(4);
		/* All data end */
		__data_end__ = .;

	} > RAM

	.ram.code_text : AT (__etext2)
	{
		. = ALIGN(4);
		__etext2 = .;
		. = ALIGN(32);
		__ram_entry_text_start__ = .;
		*(.ram_entry_func.text*)
		__ram_entry_text_end__ = .;
		
		__ram_code_text_start__ = .;
		*ram_start.o(.text*)
		*app_start.o(.text*)
		*mpu_config.o(.text*)
		*cmd_shell.o(.text*)

		*hal_sys_ctrl.o(.text*)
		*hal_dram*.o(.text*)
		*hal_timer.o(.text*)
		*hal_uart.o(.text*)
		*hal_gdma.o(.text*)
		*hal_flash.o(.text*)
		*hal_wdt.o(.text*)
		*lib_a*(.text*)
		*libc_wrap.o(.text*)
		
		. = ALIGN(4);
		__ram_code_text_end__ = .;		
	} > RAM

	.ram.code_rodata : AT (__ram_code_rodata_start__)
	{
		. = ALIGN(4);
		__ram_code_rodata_start__ = .;
		*lib_a*(.rodata*)
		*hal_sys_ctrl.o(.rodata*)
		*hal_dram*.o(.rodata*)
		*hal_flash.o(.rodata*)
		. = ALIGN(4);
		__ram_code_rodata_end__ = .;
	} > RAM

	.xip_reserved :
	{
		/* XIP image section must start with 4K aligned address, reserve 0x100 for image & section header */
		. += 0x100;
	} > XIP_FLASH
	
	.xip.code :
	{
		/* For xip encrypted section ram image signature */
		KEEP(*(.xip.ram.sign.s))
		/* code and RO data in this section will be encrypted */
		. = ALIGN(16);
		__xip_code_text_start__ = .;

		*(.xip.text*)						

		. = ALIGN(4);
		__xip_code_text_end__ = .;
	} > XIP_FLASH
	
	.xip_rodata_reserved :
	{
		/* XIP image section must start with 4K aligned address, reserve 0x100 for image & section header */
		. += 0x100;
	} > XIP_RO_FLASH
	
	.xip.rodata :
	{
		/* For xip encrypted section ram image signature */
		KEEP(*(.xip.ram.sign.s))
		/* code and RO data in this section will be encrypted */
		. = ALIGN(16);		
		
		/* code and RO data in this section will NOT be encrypted */
		/* put DMA RO data here */
		__xip_code_rodata_start__ = .;

        *(.xip.rodata*)
		
		. = ALIGN(4);
		__xip_code_rodata_end__ = .;
	} > XIP_RO_FLASH

	.ram.bss :
	{
		. = ALIGN(4);
		__bss_start__ = .;
		*lib_a*(.bss*)
		*hal_sys_ctrl.o(.bss*)
		*hal_dram*.o(.bss*)
		*hal_flash.o(.bss*)
	} > RAM

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
		
	} > RAM
	
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
		__eram_text_end__ = .;
	} > DDR
	
	.ddr.data :
	{
		. = ALIGN(4);
		__eram_data_start__ = .;	
		*(.data*)							
		*(.ddr.data)
		__eram_data_end__ = .;	
	} > DDR

	.ddr.rodata : 
	{
		. = ALIGN(4);
		__eram_rodata_start__ = .;
		*(.rodata*)
		*(.rodata.str1*)
		__eram_rodata_end__ = .;
	} > DDR
	
	.ddr.bss :
	{
		. = ALIGN(4);
		__eram_bss_start__ = .;	
		*(.bss*)
		*(COMMON)
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



    /* DWARF debug sections.
       Symbols in the DWARF debugging sections are relative to the beginning
       of the section so we begin them at 0.  */
    /* DWARF 1 */
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2 */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }
    /* DWARF 3 */
    .debug_pubtypes 0 : { *(.debug_pubtypes) }
    .debug_ranges   0 : { *(.debug_ranges) }
    /* DWARF Extension.  */
    .debug_macro    0 : { *(.debug_macro) }
    .gnu.attributes 0 : { KEEP (*(.gnu.attributes)) }
    .gptab.sdata : { *(.gptab.data) *(.gptab.sdata) }
    .gptab.sbss : { *(.gptab.bss) *(.gptab.sbss) }
    .mdebug.abi32 0 : { KEEP(*(.mdebug.abi32)) }
    .mdebug.abiN32 0 : { KEEP(*(.mdebug.abiN32)) }
    .mdebug.abi64 0 : { KEEP(*(.mdebug.abi64)) }
    .mdebug.abiO64 0 : { KEEP(*(.mdebug.abiO64)) }
    .mdebug.eabi32 0 : { KEEP(*(.mdebug.eabi32)) }
    .mdebug.eabi64 0 : { KEEP(*(.mdebug.eabi64)) }
    .gcc_compiled_long32 0 : { KEEP(*(.gcc_compiled_long32)) }
    .gcc_compiled_long64 0 : { KEEP(*(.gcc_compiled_long64)) }
    .ARM.attributes 0 : { *(.ARM.attributes) }

    /DISCARD/ :
    {
        /* Stabs debugging sections.  */
        *(.comment)
        *(.stabstr)
        *(.ARM.exidx)
        /*
        *(.pdr)
        *(.note.GNU-stack) *(.gnu_debuglink) *(.gnu.lto_*) *(.gnu.attributes)
        *(.reginfo)
        *(.rel.dyn)
        *(.MIPS.abiflags)
        *(.note.gnu.build-id)
         */
    }

    /* Collect unexpected sections.  */
    .trashcan :
    {
        *(*)
    } > RAM

    ASSERT(SIZEOF(.trashcan) == 0, "Some section is collect by trashcan. Check your map file.")
}
