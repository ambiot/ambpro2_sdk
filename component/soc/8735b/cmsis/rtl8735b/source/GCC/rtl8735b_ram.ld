INCLUDE "romsym_is.so"

/* FPGA Linker script to configure memory regions.  */
MEMORY
{

  ITCM_RAM (wrx) 		: ORIGIN = 0x00010000, LENGTH = 0x00018000 - 0x00010000  /* 32KB */
  
  DTCM_RAM (wrx) 		: ORIGIN = 0x20000000, LENGTH = 0x20004000 - 0x20000000  /*  16KB */

  VECTORS_RAM (rwx)     : ORIGIN = 0x20100000, LENGTH = 0x20100200 - 0x20100000  /* 512B */
  
  /* Jump cross 0x20100200 ~ 0x20101200 for NS ROM code used */
  /* Jump cross 0x20101200 ~ 0x20105200 for Bootloader RAM code used */
  /* Jump cross 0x20105200 ~ 0x20106200 for Bootloader RAM BSS  used */
  RAM_FUN_TABLE (rwx)   : ORIGIN = 0x20106200, LENGTH = 0x201062F0 - 0x20106200  /* 240B */
  RAM_IMG_SIGN (rwx)    : ORIGIN = 0x201062F0, LENGTH = 0x20106300 - 0x201062F0  /*  16B */
  RAM (rwx)             : ORIGIN = 0x20106300, LENGTH = 0x20177B00 - 0x20106300  /* 454KB */
  RAM_NC (rwx)          : ORIGIN = 0x20177B00, LENGTH = 0x20179800 - 0x20177B00  /*  7KB*/
  /* Jump cross 0x2017BB00 ~ 0x20180000 for S ROM code used*/
  
/* DDR memory */
						  
  VOE    (rwx)          : ORIGIN = 0x70000000, LENGTH = 0x70100000 - 0x70000000	/*  1MB */
  DDR    (rwx) 		    : ORIGIN = 0x70100000, LENGTH = 0x72C00000 - 0x70100000 /* 43MB */
  NN     (rwx)          : ORIGIN = 0x72C00000, LENGTH = 0x74000000 - 0x72C00000	/* 20MB */
/*  DDR_NC (rwx)	        : ORIGIN = 0x72F00000, LENGTH = 0x72C00000 - 0x72F00000	  1MB */ 
  
  /* XIP Chiper section: TEXT/RODATA in this section can be encrypted (decrypt by SEC) */
  XIP_FLASH  (rx) 		: ORIGIN = 0x08300000, LENGTH = 0x08380000 - 0x08300000 /* 512KB */
  XIP_RO_FLASH  (rx) 	: ORIGIN = 0x08380000, LENGTH = 0x08390000 - 0x08380000 /* 64KB */
  
}

/* total code section on ROM space for FPGA by Raymond */


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
ENTRY(Reset_Handler)

_start_addr = 0x10000000;
ENTRY(_start_addr)

__sram_end__		= ORIGIN(RAM) + LENGTH(RAM);
__eram_end__		= ORIGIN(DDR) + LENGTH(DDR);		
/* __eram_ncnb_end__	= ORIGIN(DDR_NC) + LENGTH(DDR_NC); */	

__voe_code_size__   = LENGTH(VOE);


SECTIONS
{

	.itcm_ram :
	{
		. = ALIGN(4);
		__itcm_ram_start__ = .;
/*		
		*dhry21a.o(.text*)
		*dhry21b.o(.text*)
		*ds_timers.o(.text*)

		*dhry21a.o(.bss*)
		*dhry21b.o(.bss*)
		*ds_timers.o(.bss*)

		*dhry21a.o(COMMON)
		*dhry21b.o(COMMON)
		*ds_timers.o(COMMON)	

		*dhry21a.o(.rodata*)
		*dhry21b.o(.rodata*)
		*ds_timers.o(.rodata*)
*/				
		__itcm_ram_end__ = .;

	} > ITCM_RAM

	.ram.vector :
	{
		. = ALIGN(4);
		__ram_vector_start__ = .;
		KEEP(*(.ram_vector_table))
		__ram_vector_end__ = .;
		. = ALIGN(4);
		KEEP(*(.rom.irq.bss))
	} > VECTORS_RAM /* VECTORS_RAM --> ROM Modify By Raymond */

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
		*libc_wrap.o(.text*)	
		*hal_sys_ctrl.o(.text*)
		*hal_dram*.o(.text*)
		*hal_gdma.o(.text*)
		*hal_gpio.o(.text*)
		*hal_flash.o(.text*)
		*hal_timer.o(.text*)
		*hal_uart.o(.text*)
		*hal_wdt.o(.text*)
		*hal_pinmux.o(.text*)
		*hal_spic.o(.text*)
		*(.ram.hal_pmc.text*)
		*lib_a*(.text*)

		. = ALIGN(4);
		__ram_code_text_end__ = .;
	} > RAM


	.ram.data : AT (__etext)
	{
		__fw_img_start__ = .;
	    __etext = .; 
		__data_start__ = .;
		__bl4fw_data_start__ = .;
        KEEP(*(.bl4fw.ram.data))
        __bl4fw_data_end__ = .;
		__bl4voe_data_start__ = .;
        KEEP(*(.bl4voe.ram.data))
        __bl4voe_data_end__ = .;
        __bl4voe_ldinfo_data_start__ = .;
        KEEP(*(.bl4voe.ldinfo.ram.data))
        __bl4voe_ldinfo_data_end__ = .;
        __bl4voe_peri_data_start__ = .;
        KEEP(*(.bl4voe.peri.ram.data))
        __bl4voe_peri_data_end__ = .;
		*(vtable)
/*
*/
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
		*(.ram.hal_pmc.data*)
		. = ALIGN(4);
		/* All data end */
		__data_end__ = .;

	} > RAM

	.ram.code_rodata : AT (__ram_code_rodata_start__)
	{
		. = ALIGN(4);
		__ram_code_rodata_start__ = .;
		*ram_start.o(.rodata*)
		*app_start.o(.rodata*)
		*lib_a*(.rodata*)
		*hal_sys_ctrl.o(.rodata*)
		*hal_dram*.o(.rodata*)
		*hal_flash.o(.rodata*)
		*hal_spic.o(.rodata*)
		*hal_uart.o(.rodata*)
		*hal_pinmux.o(.rodata*)
		*(.ram.hal_pmc.rodata*)
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
        *ram_start.o(.bss*)
		*hal_spic.o(.bss*)
		*(.ram.*.bss*)
		*(.ram.hal_pmc.bss*)
        *ram_start.o(COMMON)
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

	/* Non-Cacheable memory, the memory range should be configured by a MPU region */
	.non_cacheable :
	{
		. = ALIGN(32);
		__nc_data_start__ = .;
		*(.non_cache.data*)
		__nc_bss_start__ = .;
		*(.non_cache.bss*)
		__nc_bss_end__ = .;
	} > RAM_NC


	/* templary BSS for ROM code, it will be recycled
	   and be used by RAM code */
	.rom_temp_use.bss :
	{
		. = ALIGN(4);
		__rom_temp_bss_start__ = .;
		KEEP(*(.rom_temp.bss))		
		__rom_temp_bss_end__ = .;
	} > DTCM_RAM

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
	
	/* Set stack top to end of RAM, and stack limit move down by
	 * size of stack_dummy section */
	__StackTop = ORIGIN(DTCM_RAM) + LENGTH(DTCM_RAM);
	__StackLimit = __StackTop - SIZEOF(.stack_dummy);
	PROVIDE(__stack = __StackTop);

	/* Check if data + heap + stack exceeds RAM limit */
/* TODO:	ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack") */
}
