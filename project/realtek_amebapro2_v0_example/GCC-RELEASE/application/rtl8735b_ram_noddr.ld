INCLUDE "romsym_is.so"

/* FPGA Linker script to configure memory regions.  */
MEMORY
{

  ITCM_RAM (rx) 		: ORIGIN = 0x00010000, LENGTH = 0x00030000 - 0x00010000  /* 128KB */
  
  DTCM_RAM (wrx) 		: ORIGIN = 0x20000000, LENGTH = 0x20004000 - 0x20000000  /*  16KB */

  VECTORS_RAM (rwx)     : ORIGIN = 0x20100000, LENGTH = 0x20100200 - 0x20100000  /* 512B */
  
  /* Jump cross 0x20100200 ~ 0x20100A00 for NS ROM code used */
  RAM_FUN_TABLE (rwx)   : ORIGIN = 0x20100A00, LENGTH = 0x20100AF0 - 0x20100A00  /* 240B */
  RAM_IMG_SIGN (rwx)    : ORIGIN = 0x20100AF0, LENGTH = 0x20100B00 - 0x20100AF0  /*  16B */
  RAM (rwx)             : ORIGIN = 0x20100B00, LENGTH = 0x20177B00 - 0x20100B00  /* 476KB */
  RAM_NC (rwx)          : ORIGIN = 0x20177B00, LENGTH = 0x2017BB00 - 0x20177B00  /*  16KB*/
  /* Jump cross 0x2017BB00 ~ 0x20180000 for S ROM code used*/
  
/* DDR memory */
						/* Raymond  workaround for JIRA-537 issue 0x70000000 --> 0x70000040 */
						  
  DDR (rwx) 		    : ORIGIN = 0x70000040, LENGTH = 0x72C00000 - 0x70000040 /* 44MB */
  DDR_NC (rwx)	        : ORIGIN = 0x72C00000, LENGTH = 0x72E00000 - 0x72C00000	/*  2MB */ 
  VOE        (rwx)      : ORIGIN = 0x72E00000, LENGTH = 0x73000000 - 0x72E00000	/*  2MB */
  NN         (rwx)      : ORIGIN = 0x73000000, LENGTH = 0x74000000 - 0x73000000	/* 16MB */
  
  /* XIP Chiper section: TEXT/RODATA in this section can be encrypted (decrypt by SCE) */
  XIP_FLASH  (rx) 		: ORIGIN = 0x0B000000, LENGTH = 0x0BF00000 - 0x0B000000
}

/* total code section on ROM space for FPGA by Raymond */


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
ENTRY(Reset_Handler)

_start_addr = 0x10000000;
ENTRY(_start_addr)

__sram_end__		= ORIGIN(RAM) + LENGTH(RAM);
__eram_end__		= ORIGIN(DDR) + LENGTH(DDR);		
__eram_ncnb_end__	= ORIGIN(DDR_NC) + LENGTH(DDR_NC);	

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

	.xip_reserved :
	{
		/* XIP image section must start with 64K aligned address, reserve 0x140 for image header */
		. += 0x140;
		/* If the XIP is the 1st sub-image, then we should reserve 0xE0 bytes meore for OTA Sign. and 6 Public Key */
		/*. += 0xE0; */
	} > XIP_FLASH

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

		*cmsis_os.o(.text*)
		*freertos_cb.o(.text*)
		*event_groups.o(.text*)
		*heap_4_2.o(.text*)
		*timers.o(.text*)
		*tasks.o(.text*)
		*queue.o(.text*)
		*port.o(.text*)
		*list.o(.text*)
		*mpu_config.o(.text*)
		
		*libc_wrap.o(.text*)	
		*cmd_shell.o(.text*)

		*hal_sys_ctrl.o(.text*)
		*hal_dram_init.o(.text*)
		*hal_sgpio.o(.text*)
		*hal_gdma.o(.text*)
		*hal_gpio.o(.text*)
		*hal_flash.o(.text*)
		*hal_timer.o(.text*)
		*hal_uart.o(.text*)
		*hal_wdt.o(.text*)
		
		*timer_api.o(.text*)
		*sgpio_api.o(.text*)
		*dram_init.o(.text*)
		*main.o(.text*)
		*verify_example.o(.text*)
		*verify_gpio.o(.text*)
		*verify_timer.o(.text*)
		*verify_sgpio.o(.text*)
		*verify_uart.o(.text*)
		*verify_uart_irq_ram.o(.text*)
		*lib_a*(.text*)
	
		. = ALIGN(4);
		__ram_code_text_end__ = .;
	} > RAM


	.ram.data : AT (__etext)
	{
		__fw_img_start__ = .;
	    __etext = .; 
		__data_start__ = .;
		*(vtable)
/*
*/
		*lib_a*(.data*)
		*hal_sys_ctrl.o(.data*)
		
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
		
		*(.data*)
		/* All data end */
		__data_end__ = .;

	} > RAM

	.ram.code_rodata : AT (__ram_code_rodata_start__)
	{
		. = ALIGN(4);
		__ram_code_rodata_start__ = .;
		*lib_a*(.rodata*)
		*hal_sys_ctrl.o(.rodata*)
		*hal_dram_init.o(.rodata*)
		*hal_flash.o(.rodata*)
		. = ALIGN(4);
		__ram_code_rodata_end__ = .;
	} > RAM

	.xip.code :
	{
		/* For xip encrypted section ram image signature */
		KEEP(*(.xip.ram.sign.s))
		/* code and RO data in this section will be encrypted */
		. = ALIGN(16);
		__xip_code_text_start__ = .;

		*(.xip.text*)
		*(.text*)
		
		__xip_code_text_end__ = .;
		
		/* code and RO data in this section will NOT be encrypted */
		/* put DMA RO data here */
		__xip_code_rodata_start__ = .;

		*(.xip.rodata*)
		*(.rodata*)
		*(.rodata.str1*)
		
		. = ALIGN(4);
		__xip_code_rodata_end__ = .;
	} > XIP_FLASH

	.ram.bss :
	{
		. = ALIGN(4);
		__bss_start__ = .;
		*lib_a*(.bss*)
		*(.bss*)
		*(COMMON)
		*hal_sys_ctrl.o(.bss*)
		*hal_dram_init.o(.bss*)
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
									
		__eram_text_end__ = .;
	} > DDR
	
	.ddr.data :
	{
		. = ALIGN(4);
		__eram_data_start__ = .;	
								
		__eram_data_end__ = .;	
	} > DDR

	.ddr.rodata : 
	{
		. = ALIGN(4);
		__eram_rodata_start__ = .;
		
		
		__eram_rodata_end__ = .;
	} > DDR
	
	.ddr.bss :
	{
		. = ALIGN(4);
		__eram_bss_start__ = .;	
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
