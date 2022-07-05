/* Linker script to configure memory regions. */

/* !! the include symbole may failed if the symbol file name is too long!! */
/* this symbol file is from S & NS symbol merged */
INCLUDE "romsym_is.so"

MEMORY
{
  /* Internal RAM for boot temp use to load image1, will be recycle when leave ROM code */
  DTCM_RAM (wrx)		: ORIGIN = 0x20000000, LENGTH = 0x20004000 - 0x20000000  /*  16KB */
  
  /* Use Secure RAM last 60KB for boot loader used */
  VECTORS_RAM   (rwx)	: ORIGIN = 0x00018000, LENGTH = 0x00018200 - 0x00018000  /* 512B */
  RAM_FUN_TABLE (rwx)	: ORIGIN = 0x00018200, LENGTH = 0x00018300 - 0x00018200  /* 240B */ 
  ITCM_RAM_BSS  (rwx)	: ORIGIN = 0x00018300, LENGTH = 0x00020300 - 0x00018300  /* 32 KB */
  ITCM_RAM      (rwx)	: ORIGIN = 0x00020300, LENGTH = 0x00040000 - 0x00020300  /* 63.25KB */
  RAM           (rwx)	: ORIGIN = 0x20101200, LENGTH = 0x20105200 - 0x20101200  /* 16 KB */
  VOE           (rwx)	: ORIGIN = 0x70000000, LENGTH = 0x70100000 - 0x70000000  /* 1MB */
  NN     (rwx)          : ORIGIN = 0x72C00000, LENGTH = 0x74000000 - 0x72C00000	 /* 20MB For FCS*/
  
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
_start_addr = 0x10000000;
ENTRY(_start_addr)


__nn_code_size__    = LENGTH(NN);

SECTIONS
{

	.nn :
	{
		. = ALIGN(4);
		__nn_code_start__ = .;
		
	} > NN	
	
	.voe :
	{
		. = ALIGN(4);
		__voe_code_start__ = .;
		
	} > VOE	

	/* templary BSS for boot loader, it will be recycled
	   and be used by FW code */
	.boot_temp_use.bss :
	{
		. = ALIGN(4);
		__boot_temp_bss_start__ = .;
		KEEP(*(.rom_temp.bss))		
		__boot_temp_bss_end__ = .;
		__boot_sboot_bss_start__ = .;
		KEEP(*(.sboot.bss))		
		__boot_sboot_bss_end__ = .;
		
	} > DTCM_RAM

	.ram.vector :
	{
		/* vector table needs to confirm address is 256 bytes aligned */
		. = ALIGN(4);
		__ram_vector_start__ = .;
		KEEP(*(.ram_vector_table))
		__ram_vector_end__ = .;
		. = ALIGN(4);
	} > VECTORS_RAM

	.ram.func.table :
	{
		/* Boot image start here. for image decryption, 
		  the image start addr. should be aligned to 16 */
		. = ALIGN(32);
		__ram_start_table_start__ = .;
		KEEP(*(SORT(.start.ram.data*)))
		__ram_start_table_end__ = .;

	} > RAM_FUN_TABLE

	.bss :
	{
		. = ALIGN(4);
		__fw_img_start__ = .;	/*	This locate define is important!!! erase boot loader will reference this as start */
		__bss_start__ = .;
		*(.bss*)
		*(COMMON)
		*hal_dram_init.o(.bss*)
       	*(.ram.hal_pmc.bss*)
		. = ALIGN(4);
		__bss_end__ = .;
	} > ITCM_RAM_BSS

	.ram.data :
	{
		. = ALIGN(4);
		__data_start__ = .;
		*(vtable)
       	*(.ram.hal_pmc.data*)
		*(.data*)

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

	} > ITCM_RAM

	.ram.code_text :
	{
		. = ALIGN(32);
		__ram_code_text_start__ = .;

		__ram_img_signature__ = .;
		KEEP(*(.start.ram.sign*))
		*(.text*)
		*hal_sys_ctrl.o(.text*)
		*hal_dram_init.o(.text*)
		*hal_flash.o(.text*)
		*hal_timer.o(.text*)
		*hal_uart.o(.text*)	
        *(.ram.hal_pmc.text*)
		__ram_code_text_end__ = .;

	} > ITCM_RAM

	.ram.code_rodata :
	{
		__ram_code_rodata_start__ = .;
		*(.rodata*)
		*(.rodata.str1*)
		*hal_sys_ctrl.o(.rodata*)
		*hal_dram_init.o(.rodata*)	
       	*(.ram.hal_pmc.rodata*)
		__ram_code_rodata_end__ = .;	/*	This locate define is important!!! erase boot loader will reference this as end */

	} > RAM
	
	.heap (COPY):
	{
		__HeapBase = .;
		__end__ = .;
		end = __end__;
		KEEP(*(.heap*))
		__HeapLimit = .;
	} > DTCM_RAM

	/* Continue to use the stack memory space initialed by ROM code */
	/* .stack_dummy section doesn't contains any symbols. It is only
	 * used for linker to calculate size of stack sections, and assign
	 * values to stack symbols later */
	.stack_dummy (COPY):
	{
		KEEP(*(.stack*))
	} > DTCM_RAM

	/* Set stack top to end of RAM, and stack limit move down by
	 * size of stack_dummy section */
	__StackTop = ORIGIN(DTCM_RAM) + LENGTH(DTCM_RAM);
	__StackLimit = __StackTop - SIZEOF(.stack_dummy);
	PROVIDE(__stack = __StackTop);

	/* Check if data + heap + stack exceeds RAM limit */
/* TODO:	ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack") */

	/*-------------------------------------------------------------*/
/* Debug Section : will drop in elf                            */
/*-------------------------------------------------------------*/

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
    } > ITCM_RAM_BSS
    ASSERT(SIZEOF(.trashcan) == 0, "Some section is collect by trashcan.Check your map file.")
}
