/* FPGA Linker script to configure memory regions.  */
MEMORY
{
  ITCM_ROM (rx) 		: ORIGIN = 0x00000000, LENGTH = 0x00008000               /*  32KB */
  ITCM_RAM (rx) 		: ORIGIN = 0x00010000, LENGTH = 0x00030000 - 0x00010000  /* 128KB */
  
  /* Internal RAM for boot temp use to load image1, will be recycle when leave ROM code */
  DTCM_TEMP_RAM (wrx)   : ORIGIN = 0x20000000, LENGTH = 0x20003000 - 0x20000000  /*  12KB */
  DTCM_RAM (wrx) 		: ORIGIN = 0x20003000, LENGTH = 0x20004000 - 0x20003000  /*   4KB */
  DTCM_ROM (rx) 		: ORIGIN = 0x20010000, LENGTH = 0x20024000 - 0x20010000  /*  80KB */

  ROM (rx) 				: ORIGIN = 0x10000000, LENGTH = 0x100BE000 - 0x10000000  /* 768KB */

  VECTORS_RAM (rwx)     : ORIGIN = 0x20100000, LENGTH = 0x20100200 - 0x20100000  /* 512B */
  ROM_USED_RAM (rwx)    : ORIGIN = 0x20100200, LENGTH = 0x20101200 - 0x20100200  /* 4KB */
  RAM_FUN_TABLE (rwx)   : ORIGIN = 0x20101200, LENGTH = 0x201012F0 - 0x20101200  /* 240B */
  RAM_IMG_SIGN (rwx)    : ORIGIN = 0x201012F0, LENGTH = 0x20101300 - 0x201012F0  /*  16B */
  RAM (rwx)             : ORIGIN = 0x20101300, LENGTH = 0x20174800 - 0x20101300  /* 461.25KB */
  RAM_BOOT_TEMP (rwx)   : ORIGIN = 0x20174800, LENGTH = 0x2017C000 - 0x20174800  /*  30KB */
  RAM_NC     (rwx)      : ORIGIN = 0x2017C000, LENGTH = 0x20180000 - 0x2017C000  /*  16KB*/

  ROM_STUB   (rx) 		: ORIGIN = 0x100BE000, LENGTH = 0x100C0000 - 0x100BE000  /* */
  
/* DDR memory */  /* Raymond  workaround for JIRA-537 issue 0x70000000 --> 0x70000040 */						  
  DDR        (rwx)      : ORIGIN = 0x70000040, LENGTH = 0x72C00000 - 0x70000040 /* 44MB */
  DDR_NC     (rwx)      : ORIGIN = 0x72C00000, LENGTH = 0x72E00000 - 0x72C00000	/*  2MB */
  VOE        (rwx)      : ORIGIN = 0x72E00000, LENGTH = 0x73000000 - 0x72E00000	/*  2MB */
  NN         (rwx)      : ORIGIN = 0x73000000, LENGTH = 0x74000000 - 0x73000000	/* 16MB */
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
__eram_ncnb_end__	= ORIGIN(DDR_NC) + LENGTH(DDR_NC);	

__voe_code_size__   = LENGTH(VOE);

SECTIONS
{


	
	.rom.vector_tbl :
	{
		KEEP(*(.rom.vectors))
		__Vectors_End = .;
		__Vectors_Size = __Vectors_End - __Vectors;
		__end__ = .;

	} > ITCM_ROM

	.rom.itcm.text :
	{
		KEEP(*(.rom.ssl.itcm))
		/* KEEP(*(.rom.apple.itcm)) */
	} > ITCM_ROM

	.rom.dtcm.data :
	{
		KEEP(*(.rom.ssl.dtcm))
		KEEP(*(.rom.apple.dtcm)) 
		__rom_heap_start__ = .;
		__rom_heap_end__ = .;

	} > DTCM_ROM
/*
	.sysview :
	{
		KEEP(*(.sysview.start))	
		KEEP(*SEGGER_*.o(.data*))
		KEEP(*SEGGER_*.o(.text*))
		KEEP(*SEGGER_*.o(.rodata*))		
	} > ITCM_RAM
*/
	.rom.text :
	{

		. = ALIGN(0x100);

		KEEP(*startup_rtl8735b.o(.text*))
		KEEP(*system_rtl8735b.o(.text*))
		KEEP(*(.startup.text))
		KEEP(*(.vector.text))
		KEEP(*(.sysstart.text))
		KEEP(*(.sys_misc.text))
		KEEP(*startup.o(.rodata*))

		/**** HAL ROM Code ****/
		KEEP(*(.rom.*.text))
		KEEP(*(.rom.*.rodata))

		/**** Utility ROM code ****/
		KEEP(*(.infra.rom.text))
		KEEP(*(.stdioport.text))
		KEEP(*(.xprintf.text))
		KEEP(*(.xprintf.rodata))
		KEEP(*(.rtlprintf.text))	 
		KEEP(*(.rtlprintf.rodata))
		KEEP(*(.logbuf.text))
		KEEP(*(.shell.text))
		KEEP(*(.shell.rodata))
		KEEP(*(.shellcmd.text))
		KEEP(*(.shellcmd.rodata))
		KEEP(*(.ccmd.text))
		KEEP(*(.ccmd.rodata))
		KEEP(*(.rom.utility.text*))

		KEEP(*xprintf.o(.rodata*))
		KEEP(*rtl_printf.o(.rodata*))
		KEEP(*strproc.o(.text*))
		KEEP(*strproc.o(.rodata*))
		KEEP(*atoi.o(.text*))
		KEEP(*atoi.o(.rodata*))
		KEEP(*scanf.o(.text*))
		KEEP(*scanf.o(.rodata*))
		KEEP(*xmodem.o(.text*))
		KEEP(*xmodem.o(.rodata*))
		KEEP(*xmport_uart.o(.text*))
		KEEP(*xmport_uart.o(.rodata*))
		KEEP(*fw_pg.o(.text*))
		KEEP(*fw_pg.o(.rodata*))
		KEEP(*fw_pg.o(.rodata.str1*))
		KEEP(*fw_uart_boot.o(.text*))
		KEEP(*fw_uart_boot.o(.rodata*))
		KEEP(*fw_uart_boot.o(.rodata.str1*))
		KEEP(*fw_flash_boot.o(.text*))
		KEEP(*fw_flash_boot.o(.rodata*))
		KEEP(*fw_flash_boot.o(.rodata.str1*))
		KEEP(*fw_snand_boot.o(.text*))
		KEEP(*fw_snand_boot.o(.rodata*))

		/**** WLAN ROM code ****/
		KEEP(*(.rom.wlan.text))
		KEEP(*(.rom.wlan.rodata))

		/**** Apple crypto ROM code ****/
		KEEP(*(.rom.apple.text))
		KEEP(*(.rom.apple.rodata))

		/**** SSL ROM code ****/
		KEEP(*(.rom.ssl.text))
		KEEP(*(.rom.ssl.rodata))

		/**** FAAC ROM code ****/
		KEEP(*(.rom.faac.text))
		KEEP(*(.rom.faac.rodata))

		__lib_gcc_start = .;
		*_arm_cmpdf2.o(.text*)
		*_aeabi_uldivmod.o(.text*)
		*_aeabi_ldivmod.o(.text*)
		*_aeabi_*.o(.text*)
		*bpabi.o(.text*)
		*_muldi3.o(.text*)
		*_divdi3.o(.text*)
		*_fixunsdfsi.o(.text*)
		*_fixdfdi.o(.text*)
		*_fixunsdfdi.o(.text*)
		*_floatdidf.o(.text*)
		*_floatundidf.o(.text*)
		*_udivmoddi4.o(.text*)
		*adddf3.o(.text*)
		*divdf3.o(.text*)
		*eqdf2.o(.text*)
		*gedf2.o(.text*)
		*ledf2.o(.text*)
		*muldf3.o(.text*)
		*subdf3.o(.text*)
		*fixdfsi.o(.text*)
		*floatsidf.o(.text*)
		*floatunsidf.o(.text*)
		*truncdfsf2.o(.text*)
		*_thumb1_case_uhi.o(.text*)
		*_dvmd_tls.o(.text*)
		*_clzsi2.o(.text*)
		*divsf3.o(.text*)
		*mulsf3.o(.text*)
		*addsf3.o(.text*)
		*subsf3.o(.text*)
		*extendsfdf2.o(.text*)
		*_thumb1_case_uqi.o(.text*)
		*floatsisf.o(.text*)
		*_fixunssfsi.o(.text*)
		*fixsfsi.o(.text*)
		*floatunsisf.o(.text*)
		*_arm_cmpsf2.o(.text*)
		*eqsf2.o(.text*)
		*gesf2.o(.text*)
		*lesf2.o(.text*)		
		__lib_gcc_end = .;
		
		KEEP(*(.init))
		KEEP(*(.fini))

		/* .ctors */
		*crtbegin.o(.ctors)
		*crtbegin?.o(.ctors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
		*(SORT(.ctors.*))
		*(.ctors)

		/* .dtors */
 		*crtbegin.o(.dtors)
 		*crtbegin?.o(.dtors)
 		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
 		*(SORT(.dtors.*))
 		*(.dtors)

		KEEP(*startup_rtl8735b.o(.rodata.str1*))
		KEEP(*startup.o(.rodata.str1*))
		KEEP(*consol_cmds.o(.rodata.str1*))
		KEEP(*shell.o(.rodata.str1*))
		KEEP(*xprintf.o(.rodata.str1*))
		KEEP(*rtl_printf.o(.rodata.str1*))
		KEEP(*utility.o(.rodata.str1*))
		KEEP(*rtl8735b_irq.o(.rodata.str1*))
		KEEP(*rtl8735b_sys_ctrl.o(.rodata.str1*))
		KEEP(*rtl8735b_adc.o(.rodata.str1*))
		KEEP(*rtl8735b_timer.o(.rodata.str1*))
		KEEP(*rtl8735b_pwm.o(.rodata.str1*))
		KEEP(*rtl8735b_gpio.o(.rodata.str1*))
		KEEP(*rtl8735b_eddsa.o(.rodata.str1*))
		KEEP(*rtl8735b_ecdsa.o(.rodata.str1*))
		KEEP(*rtl8735b_flash.o(.rodata.str1*))
		KEEP(*rtl8735b_misc.o(.rodata.str1*))
		KEEP(*rtl8735b_spic.o(.rodata.str1*))
		KEEP(*rtl8735b_i2s.o(.rodata.str1*))
		KEEP(*rtl8735b_gdma.o(.rodata.str1*))
		KEEP(*rtl8735b_sdmmc_host.o(.rodata.str1*))
		KEEP(*rtl8735b_otp.o(.rodata.str1*))
		KEEP(*rtl8735b_pinmux.o(.rodata.str1*))
		KEEP(*rtl8735b_snand.o(.rodata.str1*))
		KEEP(*rtl8735b_timer.o(.rodata*))
		KEEP(*rtl8735b_pwm.o(.rodata*))
		KEEP(*rtl8735b_uart.o(.rodata*))
		KEEP(*rtl8735b_i2c.o(.rodata*))
		KEEP(*rtl8735b_gpio.o(.rodata*))
		KEEP(*rtl8735b_ecdsa.o(.rodata*))
		KEEP(*timers.o(.rodata*))
		KEEP(*rtl8735b_otp.o(.rodata*))
		KEEP(*rtl8735b_pinmux.o(.rodata*))
		KEEP(*rtl8735b_snand.o(.rodata*))
		
		KEEP(*rom_rtw_message_e.o(.rodata.str1*))
		KEEP(*rom_rtw_message_f.o(.rodata.str1*))
		KEEP(*rom_mbedtls_bignum.o(.rodata.str1*))
		KEEP(*rom_mbedtls_ecp.o(.rodata.str1*))
		KEEP(*rom_mbedtls_ecp_curves.o(.rodata.str1*))
		KEEP(*rom_mbedtls_oid.o(.rodata*))
		KEEP(*rom_mbedtls_md.o(.rodata*))
		KEEP(*rom_mbedtls_md_wrap.o(.rodata.str1*))
		KEEP(*rom_mbedtls_pem.o(.rodata.str1*))
		KEEP(*rom_mbedtls_dhm.o(.rodata.str1*))
		KEEP(*rom_mbedtls_ecjpake.o(.rodata.str1*))
		KEEP(*rom_mbedtls_pk.o(.rodata*))
		KEEP(*rom_mbedtls_pk_wrap.o(.rodata.str1*))
		KEEP(*rom_mbedtls_pkwrite.o(.rodata.str1*))



		KEEP(*(.eh_frame*))
	} > ROM
	

	.ARM.extab :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > ITCM_ROM

	__exidx_start = .;
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > ITCM_ROM
	__exidx_end = .;

	/* To copy multiple ROM to RAM sections,
	 * uncomment .copy.table section and,
	 * define __STARTUP_COPY_MULTIPLE in startup_ARMCMx.S */
	/*	
	.copy.table :
	{
		. = ALIGN(4);
		__copy_table_start__ = .;
		LONG (__etext)
		LONG (__data_start__)
		LONG (__data_end__ - __data_start__)
		LONG (__etext2)
		LONG (__ram_code_text_start__)
		LONG (__ram_code_text_end__ - __ram_code_text_start__)
		__copy_table_end__ = .;
	} > ROM
	*/	
	. = ALIGN(4);
	__rom_etext = .;

	/* To clear multiple BSS sections,
	 * uncomment .zero.table section and,
	 * define __STARTUP_CLEAR_BSS_MULTIPLE in startup_ARMCMx.S */
	/*
	.zero.table :
	{
		. = ALIGN(4);
		__zero_table_start__ = .;
		LONG (__bss_start__)
		LONG (__bss_end__ - __bss_start__)
		LONG (__bss2_start__)
		LONG (__bss2_end__ - __bss2_start__)
		__zero_table_end__ = .;
	} > ROM
	*/

	.note.gnu.build-id :
	{
		*(.note.gnu.build-id)
	} > ITCM_ROM

	.rom.stubs :
	{
		. = ALIGN(0x10);
		__rom_stubs_hal_syson = .;
		KEEP(*(.rom.hal_syson.stubs))

		. = ALIGN(0x10);
		__rom_stubs_hal_timer = .;
		KEEP(*(.rom.hal_timer.stubs))

		. = ALIGN(0x10);
		__rom_stubs_int_vector = .;
		KEEP(*(.rom.hal_vector.stubs))

		. = ALIGN(0x10);
		__rom_stubs_hal_crypto = .;
		KEEP(*(.rom.hal_crypto_s.stubs))
		
		. = ALIGN(0x10);
		__rom_stubs_hal_ecdsa = .;
		KEEP(*(.rom.hal_ecdsa.stubs))
		
		. = ALIGN(0x10);
		__rom_stubs_hal_eddsa = .;
		KEEP(*(.rom.hal_eddsa.stubs))

		. = ALIGN(0x10);
		__rom_stubs_hal_pwm = .;
		KEEP(*(.rom.hal_pwm.stubs))

		. = ALIGN(0x10);
		__rom_stubs_hal_uart = .;
		KEEP(*(.rom.hal_uart.stubs))

		. = ALIGN(0x10);
		__rom_stubs_hal_gpio = .;
		KEEP(*(.rom.hal_gpio.stubs))

		. = ALIGN(0x10);
		__rom_stubs_hal_icc = .;
		KEEP(*(.rom.hal_icc.stubs))

		. = ALIGN(0x10);
		__rom_stubs_hal_otp = .;
		KEEP(*(.rom.hal_otp.stubs))

		. = ALIGN(0x10);
		__rom_stubs_hal_sdioh = .;
		KEEP(*(.rom.hal_sdioh.stubs))

        . = ALIGN(0x10);
		__rom_stubs_hal_gdma = .;
		__rom_stubs_hal_gdma_s = .;
		__rom_stubs_hal_gdma_ns = .;
		KEEP(*(.rom.hal_gdma.stubs))

        . = ALIGN(0x10);
		__rom_stubs_hal_spic = .;
		KEEP(*(.rom.hal_spic.stubs))

        . = ALIGN(0x10);
		__rom_stubs_hal_flash = .;
		KEEP(*(.rom.hal_flash.stubs))

        . = ALIGN(0x10);
		__rom_stubs_hal_i2s = .;
		KEEP(*(.rom.hal_i2s.stubs))			

        . = ALIGN(0x10);
		__rom_stubs_hal_pinmux = .;
		KEEP(*(.rom.hal_pinmux.stubs))

        . = ALIGN(0x10);
		__rom_stubs_hal_cache = .;
		KEEP(*(.rom.hal_cache.stubs))
		
		. = ALIGN(0x10);
		__rom_stubs_hal_adc = .;
		KEEP(*(.rom.hal_adc.stubs))
    
		. = ALIGN(0x10);
		__rom_stubs_hal_eth = .;
		KEEP(*(.rom.hal_eth.stubs))
		
		. = ALIGN(0x10);
		__rom_stubs_hal_i2c = .;
		KEEP(*(.rom.hal_i2c.stubs))

		. = ALIGN(0x10);
		__rom_stubs_flash_boot = .;
		KEEP(*(.rom.sboot.stubs))		

		. = ALIGN(0x10);
		__rom_stubs_hal_misc = .;
		KEEP(*(.rom.hal_misc.stubs))

		. = ALIGN(0x10);
		__rom_stubs_hal_sys_ctrl = .;
		KEEP(*(.rom.hal_sys_ctrl.stubs))

		. = ALIGN(0x10);
		__rom_stubs_hal_wdt = .;
		KEEP(*(.rom.hal_wdt.stubs))
		
        . = ALIGN(0x10);
		__rom_stubs_hal_sec = .;
		KEEP(*(.rom.hal_sec.stubs))
						
		. = ALIGN(0x10);
		__rom_stubs_shell = .;
		KEEP(*(.rom.shellcmd.stubs))

		. = ALIGN(0x10);
		__rom_stubs_stdprintf_s = .;
		__rom_stubs_stdprintf_ns = .;
		KEEP(*(.rom.printf.stubs))		

		. = ALIGN(0x10);
		__rom_stubs_wlan = .;
		KEEP(*(.rom.wlan.stubs))

		. = ALIGN(0x10);
		__rom_stubs_apple = .;
		KEEP(*(.rom.apple.stubs))

		. = ALIGN(0x10);
		__rom_stubs_strproc = .;
		KEEP(*(.rom.strproc.stubs))

		. = ALIGN(0x10);
		__rom_stubs_utility = .;
		__rom_stubs_utility_ns = .;
		KEEP(*(.rom.utility.stubs))

		. = ALIGN(0x10);
		__rom_stubs_ssl = .;
		KEEP(*(.rom.ssl.stubs))

		. = ALIGN(0x10);
		__rom_stubs_faac = .;
		KEEP(*(.rom.faac.stubs))

		. = ALIGN(0x10);
		__rom_stubs_snand = .;
		KEEP(*(.rom.hal_snand.stubs))

		/**** New stubs for A-Cut ROM start from here ****/
		. = ALIGN(0x10);
		__rom_stubs_hal_hkdf_extend = .;
		KEEP(*(.rom.hal_hkdf_extend.stubs))

		. = ALIGN(0x10);
		__rom_stubs_hal_trng = .;
		KEEP(*(.rom.hal_trng.stubs))

		. = ALIGN(0x10);
		__rom_stubs_snand_boot = .;
		KEEP(*(.rom.snand_boot.stubs))

		/**** New stubs for B-Cut ROM start from here ****/
	} > ROM_STUB

/************* End of ROM **************/

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

	.data_of_rom : AT (__rom_etext)		/* for ROM image building */
	{
		. = ALIGN(4);
		__rom_data_start__ = .;

		/**** ROM data ****/
		KEEP(*system_rtl8735b.o(.data*))
		KEEP(*fw_pg.o(.data*))
		KEEP(*fw_uart_boot.o(.data*))
		KEEP(*fw_snand_boot.o(.data*))
		KEEP(*(.rom.hal_uart.data))
		KEEP(*(.rom.hal_timer.data))
		KEEP(*(.rom.hal_pwm.data))
		KEEP(*(.rom.hal_gpio.data))
		KEEP(*(.rom.hal_otp.data))
		KEEP(*(.rom.hal_sdioh.data))
		KEEP(*(.rom.hal_i2s.data))
		KEEP(*(.rom.hal_pcm.data))
		KEEP(*(.rom.hal_eth.data))
		KEEP(*(.xprintf.data))
		KEEP(*(.rom.hal_adc.data))
		KEEP(*rtl8735b_adc.o(.data*))
		KEEP(*(.rom.hal_i2c.data))
		KEEP(*rtl8735b_i2c.o(.data*))
		KEEP(*(.rom.hal_snand.data))
		/* All ROM data end */
		__rom_data_end__ = .;

	} > ROM_USED_RAM

	.bss_of_rom (NOLOAD) :
	{
		. = ALIGN(4);
		
		/**** ROM BSS ****/
		__rom_bss_start__ = .;
		
		KEEP(*(.sysstart.bss))
		KEEP(*(.sys_misc.bss))
		KEEP(*(.diag.bss))
		KEEP(*(.rom.hal_syson.bss))
		KEEP(*(.rom.hal_uart.bss))
		KEEP(*(.rom.hal_timer.bss))
		KEEP(*(.rom.hal_pwm.bss))
		KEEP(*(.rom.hal_gpio.bss))
		KEEP(*(.rom.hal_adc.bss))
		KEEP(*(.rom.hal_i2c.bss))
        KEEP(*(.rom.hal_gdma.bss))
        KEEP(*(.rom.hal_spic.bss))
        KEEP(*(.rom.hal_flash.bss))
        KEEP(*(.rom.hal_ssi.bss))
		KEEP(*(.rom.hal_otp.bss))
		KEEP(*(.rom.hal_sdioh.bss))
		KEEP(*(.rom.hal_pinmux.bss))
		KEEP(*(.rom.hal_cache.bss))
		KEEP(*(.rom.hal_i2s.bss))
		KEEP(*(.rom.hal_pcm.bss))
		KEEP(*(.rom.hal_eth.bss))
		KEEP(*(.rom.hal_crypto_s.bss))
		KEEP(*(.rom.hal_lpi.bss))
		KEEP(*(.rom.hal_sce.bss))
		KEEP(*(.rom.hal_wdt.bss))
		KEEP(*(.rom.hal_ecdsa.bss))
		KEEP(*(.rom.hal_eddsa.bss))
		KEEP(*(.xprintf.bss))
		KEEP(*(.logbuf.bss))
		KEEP(*(.stdioport.bss))
		KEEP(*strproc.o(.bss*))
		KEEP(*rtl8735b_adc.o(.bss*))
		KEEP(*rtl8735b_i2c.o(.bss*))
		KEEP(*rtl8735b_flash.o(.bss*))
		KEEP(*(.rom.hal_snand.bss))
		KEEP(*fw_snand_boot.o(.bss*))
		KEEP(*fw_snand_boot.o(COMMON))

		/* ??? Is it ok ??? Should remove becaure not all rtl8735b_xxx are ROM codes*/
		KEEP(*rtl8735b_*.o(.bss*))

		KEEP(*(.rom.wlan.bss))
		KEEP(*(.rom.ssl.bss))
		__rom_bss_end__ = .;

	} > ROM_USED_RAM

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
		*hal_pinmux.o(.text*)
		*app_start.o(.text*)
		*hal_sys_ctrl.o(.text*)
		*hal_dram_init.o(.text*)
		
/*			

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
*/	

		*hal_gdma.o(.text*)
		*hal_gpio.o(.text*)
		*hal_spic.o(.text*)
		*hal_flash.o(.text*)
		*hal_timer.o(.text*)
		*hal_wdt.o(.text*)
		*hal_uart.o(.text*)
		*hal_i2s.o(.text*)
		*lib_a*(.text*)

		*(.ram.*.text*)		
/*
		*main.o(.text*)

		*(.text*)
*/		
		. = ALIGN(4);
		__ram_code_text_end__ = .;
	} > RAM


	.ram.data : AT (__etext)
	{
		__fw_img_start__ = .;
	    __etext = .; 
		__data_start__ = .;
		*(vtable)
		*hal_sys_ctrl.o(.data*)
		*hal_pinmux.o(.data*)
		*hal_gdma.o(.data*)
		*hal_gpio.o(.data*)
		*hal_flash.o(.data*)
		*hal_timer.o(.data*)
		*hal_uart.o(.data*)
		*hal_i2s.o(.data*)
		*lib_a*(.data*)
		. = ALIGN(4);
		*hal_dram_init.o(.data*)		
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

	.ram.code_rodata : AT (__ram_code_rodata_start__)
	{
		. = ALIGN(4);
		__ram_code_rodata_start__ = .;
		*ram_start.o(.rodata*)
		*hal_pinmux.o(.rodata*)
		*hal_sys_ctrl.o(.rodata*)
		*hal_gdma.o(.rodata*)
		*hal_gpio.o(.rodata*)
		*hal_spic.o(.rodata*)
		*hal_flash.o(.rodata*)
		*hal_timer.o(.rodata*)
		*hal_uart.o(.rodata*)
		*hal_i2s.o(.rodata*)
		*lib_a*(.rodata*)
		*mpu_config.o(.rodata*)
		. = ALIGN(4);
		*hal_dram_init.o(.rodata*)
		*hal_dram_init.o(.rodata.str1*)
		. = ALIGN(4);
		__ram_code_rodata_end__ = .;
	} > RAM

	.voe.ddr.bss :
	{
		. = ALIGN(4);
		__eram_voe_bss_start__ = .;
        *clk-rts3915.o(.bss*)
        *encasiccontroller.o(.bss*)
		*ewl_ameba.o(.bss*)
        *fixp.o(.bss*)
        *hal_voe.o(.bss*)
        *isp_errno.o(.bss*)
        *isp_hw_eeh.o(.bss*)
        *isp_hw_intp.o(.bss*)
        *isp_hw_vreg.o(.bss*)
        *isp_log.o(.bss*)
        *isp_mod_ae.o(.bss*)
        *isp_mod_af.o(.bss*)
        *isp_mod_awb.o(.bss*)
        *isp_mod_blc.o(.bss*)
        *isp_mod_ccm.o(.bss*)
        *isp_mod_control.o(.bss*)
        *isp_mod_crop.o(.bss*)
        *isp_mod_dehaze.o(.bss*)
        *isp_mod_eeh.o(.bss*)
        *isp_mod_flick.o(.bss*)
        *isp_mod_gamma.o(.bss*)
        *isp_mod_global.o(.bss*)
        *isp_mod_interface.o(.bss*)
        *isp_mod_intp.o(.bss*)
        *isp_mod_iq.o(.bss*)
        *isp_mod_ldc.o(.bss*)
        *isp_mod_mask.o(.bss*)
        *isp_mod_mcrop.o(.bss*)
        *isp_mod_mlsc.o(.bss*)
        *isp_mod_nlsc.o(.bss*)
        *isp_mod_rawstat.o(.bss*)
        *isp_mod_rgb2yuv.o(.bss*)
        *isp_mod_sensor.o(.bss*)
        *isp_mod_spe.o(.bss*)
        *isp_mod_statis.o(.bss*)
        *isp_mod_tnr.o(.bss*)
        *isp_mod_uvtune.o(.bss*)
        *isp_mod_vctrl.o(.bss*)
        *isp_mod_wdr.o(.bss*)
        *isp_mod_ygamma.o(.bss*)
        *isp_mod_ygc.o(.bss*)
        *isp_mod_ystat.o(.bss*)
        *isp_shm.o(.bss*)
        *isp_top.o(.bss*)
        *pinctrl-rts3915.o(.bss*)
        *reset-rts3915.o(.bss*)
        *rlx_sh_irq.o(.bss*)
        *rts_camera_isp.o(.bss*)
        *rts_camera_isp_mfd.o(.bss*)
        *rts_camera_md.o(.bss*)
        *rts_camera_osd2.o(.bss*)
        *rts_camera_soc.o(.bss*)
        *rts_camera_soc_3915.o(.bss*)
        *rts_camera_verify.o(.bss*)
        *rts_camera_video.o(.bss*)
        *rts_camera_zoom.o(.bss*)
        *run_video.o(.bss*)
        *vb2_utils.o(.bss*)
        __eram_voe_bss_end__ = .;
		
	} > DDR	

	.ram.bss :
	{
		. = ALIGN(4);
		__bss_start__ = .;
		*(.ram.*.bss*)		
		*hal_dram_init.o(.bss*)
		*ram_start.o(.bss*)
		*hal_pinmux.o(.bss*)
		*hal_sys_ctrl.o(.bss*)
		*hal_gdma.o(.bss*)
		*hal_gpio.o(.bss*)
		*hal_spic.o(.bss*)
		*hal_flash.o(.bss*)
		*hal_timer.o(.bss*)
		*hal_uart.o(.bss*)
		*hal_i2s.o(.bss*)
		*lib_a*(.bss*)
		
		*main.o(COMMON)
		*ram_start.o(COMMON)
		*lib_a*(COMMON)
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
	.rom_temp_use_ram.bss :
	{
		. = ALIGN(4);
        __rom_temp_bss_start__ = .;
        KEEP(*startup.o(.rom_temp.bss))
        KEEP(*fw_pg.o(.rom_temp.bss))
        KEEP(*fw_flash_boot.o(.rom_temp.bss))
        KEEP(*fw_uart_boot.o(.rom_temp.bss))
        KEEP(*fw_snand_boot.o(.rom_temp.bss))
        __rom_temp_bss_end__ = .;

        . = ALIGN(32);

        __rom_boot_loader_tmp_buf_start__ = .;
        KEEP(*(.rom.boot_export.bss*))
        __rom_boot_loader_tmp_buf_end__ = .;

	} > RAM_BOOT_TEMP

	/* templary BSS for ROM code, it will be recycled
	   and be used by RAM code */
	.rom_temp_use_dtcm.bss :
	{
		. = ALIGN(4);
        __rom_temp_dtcm_bss_start__ = .;
        __rom_temp_dtcm_bss_end__ = .;

        . = ALIGN(32);

        __rom_sboot_bss_start__ = .;
        KEEP(*fw_flash_boot.o(.rom.sboot.bss))
		KEEP(*fw_pg.o(.rom.pg.secure.bss))
        __rom_sboot_bss_end__ = .;

	} > DTCM_TEMP_RAM

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
		*(COMMON)		
		*(.bss*)
		__eram_bss_end__ = .;
		__eram_heap_start__ = .;
		
	} > DDR	
	
	.ddr_nc.bss :
	{
		. = ALIGN(4);
		__eram_ncnb_start__ = .;
		
	} > DDR_NC	
	
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
