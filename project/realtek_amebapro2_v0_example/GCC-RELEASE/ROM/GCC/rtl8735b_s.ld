/* Linker script to configure memory regions. */

/* Select External RAM type - 1: LPDDR SDRAM 2: PSRAM */
useERAM = 1;

MEMORY
{
  /* !!! These memory map configuration MUST match with the SAU setting !!! */
  ITCM_ROM (rx) 		: ORIGIN = 0x00000000, LENGTH = 0x00003000					/*  12 KB    */
  ROM_STUB (rx) 		: ORIGIN = 0x10000000, LENGTH = 0x10004000 - 0x10000000		/*  16 KB    */
  ROM (rx) 				: ORIGIN = 0x10004000, LENGTH = 0x1004A800 - 0x10004000		/*  282KB    */
  
  /* The vector table, it must start with 256 bytes aligned address */
  VECTORS_RAM (rwx)     : ORIGIN = 0x20002000, LENGTH = 0x20002200 - 0x20002000		/*  512B     */
  DTCM_BOOT_TEMP (rwx) 	: ORIGIN = 0x20002200, LENGTH = 0x20003000 - 0x20002200 	/*  3KB+512B */
  DTCM_RAM (rwx) 		: ORIGIN = 0x20003000, LENGTH = 0x20004000 - 0x20003000 	/*  4KB      */

  DTCM_ROM (rx) 		: ORIGIN = 0x20010000, LENGTH = 0x20017800 - 0x20010000		/*  30KB     */

  /* Non-Secure Callable: for Non-Secure callable functions and secure gateway */  
  NSC (rwx)             : ORIGIN = 0x20156000, LENGTH = 0x20164300 - 0x20156000		/*  56.75KB  */

  /* RAM functions entry table */
  RAM_FUN_TABLE (rwx)   : ORIGIN = 0x20164300, LENGTH = 0x201643F0 - 0x20164300     /* 240B 	 */
  
  /* RAM image Signature */
  RAM_IMG_SIGN (rwx)    : ORIGIN = 0x201643F0, LENGTH = 0x20164400 - 0x201643F0		/*  16B      */
    
  /* Internal SRAM for text, rodata & data */
  RAM (rwx)             : ORIGIN = 0x20164400, LENGTH = 0x20177800 - 0x20164400		/*  77KB     */
  
  /* Non-Cacheable memory, defined by MPU configuration */
  /* RAM_NC (rwx)          : ORIGIN = 0x2017BB00, LENGTH = 0x2017DF00 - 0x2017BB00	*/	/*  9KB 	 */
  
  /* Internal RAM for boot temp use to load image1, will be recycle when leave ROM code */
  RAM_BOOT_TEMP (rwx)   : ORIGIN = 0x20177800, LENGTH = 0x2017F000 - 0x20177800  	/*  30kB	 */
    
  /* reserved for ROM code */
  ROM_USED_RAM (rwx)    : ORIGIN = 0x2017F000, LENGTH = 0x20180000 - 0x2017F000		/*  4KB      */

  /* DDR memory */
  VOE (rwx)             : ORIGIN = 0x70000000, LENGTH = 0x70100000 - 0x70000000		/*  1MB */
  DDR (rwx)	            : ORIGIN = 0x73E00000, LENGTH = 0x74000000 - 0x73E00000		/*  2MB */
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
ENTRY(Reset_Handler)

__sram_end__		= ORIGIN(RAM) + LENGTH(RAM);
__eram_end__		= ORIGIN(DDR) + LENGTH(DDR);
__eram_start__		= ORIGIN(DDR);

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

	.rom.dtcm.data :
	{
		/* Only??? ROM boot code ro-data can be put here, 
		 * these data cannot be referenced after jump to RAM code */
		KEEP(*startup_rtl8735b.o(.rodata.str1*))
		KEEP(*startup.o(.rodata.str1*))
		KEEP(*startup.o(.rodata*))
		KEEP(*fw_pg.o(.rodata*))
		KEEP(*fw_pg.o(.rodata.str1*))
		KEEP(*fw_uart_boot.o(.rodata*))
		KEEP(*fw_uart_boot.o(.rodata.str1*))
		KEEP(*fw_flash_boot.o(.rodata*))
		KEEP(*fw_flash_boot.o(.rodata.str1*))
		KEEP(*fw_voe_rom_boot.o(.rodata*))
		KEEP(*fw_voe_rom_boot.o(.rodata.str1*))
		KEEP(*fw_snand_boot.o(.rodata*))
		KEEP(*(.rom.ssl.dtcm))
		KEEP(*(.rom.lfc_ctrl_sts.dtcm))
	} > DTCM_ROM

	.rom.itcm.text :
	{
		__itcm_rom_start__ = .;
		/* boot ROM code */
		KEEP(*startup_rtl8735b.o(.text*))
		KEEP(*system_rtl8735b.o(.text*))
		KEEP(*(.sysstart.text))
		KEEP(*(.startup.text))
		__itcm_rom_end__ = .;
	} > ITCM_ROM

	.rom.text :
	{
		. = ALIGN(0x100);
		KEEP(*(.vector.text))
		KEEP(*(.sys_misc.text))
		KEEP(*(.rom.irq.rodata*))

		/**** HAL ROM Code ****/
		KEEP(*(.rom.*.text))
		KEEP(*(.rom.*.rodata))
		
		/**** Utility ROM code ****/
		KEEP(*(.infra.rom.text))
		KEEP(*(.stdioport.text))
		KEEP(*(.rtlprintf.text))
		KEEP(*(.rtlprintf.rodata))
		KEEP(*(.rtlprintf.text))
		KEEP(*(.shell.text))
		KEEP(*(.shell.rodata))
		KEEP(*(.shellcmd.text))
		KEEP(*(.shellcmd.rodata))
		KEEP(*(.ccmd.text))
		KEEP(*(.ccmd.rodata))
		KEEP(*(.rom.utility.text*))

		KEEP(*rtl_printf.o(.rodata*))
		KEEP(*strproc.o(.text*))
		KEEP(*strproc.o(.rodata*))
		KEEP(*atoi.o(.text*))
		KEEP(*atoi.o(.rodata*))

		KEEP(*xmodem.o(.text*))
		KEEP(*xmodem.o(.rodata*))
		KEEP(*xmport_uart.o(.text*))
		KEEP(*xmport_uart.o(.rodata*))
		KEEP(*fw_pg.o(.text*))
		KEEP(*fw_uart_boot.o(.text*))
		KEEP(*fw_flash_boot.o(.text*))
		KEEP(*fw_voe_rom_boot.o(.text*))
		KEEP(*fw_snand_boot.o(.text*))
		
/*		KEEP(*cmse_nonsecure_call.o(.text)) */

		/**** SSL ROM code ****/
		KEEP(*(.rom.ssl.text))
		KEEP(*(.rom.ssl.rodata))

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

		KEEP(*shell.o(.rodata.str1*))
		KEEP(*rtl_printf.o(.rodata.str1*))
		KEEP(*utility.o(.rodata.str1*))
		KEEP(*rtl8735b_irq.o(.rodata.str1*))
		KEEP(*rtl8735b_sys_ctrl.o(.rodata.str1*))
		KEEP(*rtl8735b_timer.o(.rodata.str1*))
		KEEP(*rtl8735b_i2c.o(.rodata.str1*))
		KEEP(*rtl8735b_pwm.o(.rodata.str1*))
		KEEP(*rtl8735b_gpio.o(.rodata.str1*))
		KEEP(*rtl8735b_crypto.o(.rodata.str1*))
		KEEP(*rtl8735b_rsa.o(.rodata.str1*))
		KEEP(*rtl8735b_eddsa.o(.rodata.str1*))
		KEEP(*rtl8735b_ecdsa.o(.rodata.str1*))
		KEEP(*rtl8735b_flash.o(.rodata.str1*))
		KEEP(*rtl8735b_spic.o(.rodata.str1*))
		KEEP(*rtl8735b_misc.o(.rodata.str1*))
		KEEP(*rtl8735b_gdma.o(.rodata.str1*))
		KEEP(*rtl8735b_pinmux.o(.rodata.str1*))
		KEEP(*rtl8735b_otp.o(.rodata.str1*))
		KEEP(*rtl8735b_snand.o(.rodata.str1*))
		KEEP(*rtl8735b_hkdf.o(.rodata.str1*))
		KEEP(*rtl8735b_flash_sec.o(.rodata.str1*))
		
		KEEP(*rtl8735b_timer.o(.rodata*))
		KEEP(*rtl8735b_pwm.o(.rodata*))
		KEEP(*rtl8735b_uart.o(.rodata*))
		KEEP(*rtl8735b_i2c.o(.rodata*))
		KEEP(*rtl8735b_gpio.o(.rodata*))
		KEEP(*rtl8735b_ecdsa.o(.rodata*))
		KEEP(*rtl8735b_crypto.o(.rodata*))
		KEEP(*rtl8735b_rsa.o(.rodata*))
		KEEP(*rtl8735b_pinmux.o(.rodata*))
		KEEP(*rtl8735b_otp.o(.rodata*))
		KEEP(*rtl8735b_snand.o(.rodata*))
		KEEP(*rtl8735b_hkdf.o(.rodata*))
		KEEP(*rtl8735b_flash_sec.o(.rodata*))
		KEEP(*rtl8735b_trng.o(.rodata*))
		KEEP(*rtl8735b_sys_ctrl.o(.rodata*))

		KEEP(*rom_mbedtls_bignum.o(.rodata.str1*))
		KEEP(*rom_mbedtls_ecp.o(.rodata.str1*))
		KEEP(*rom_mbedtls_ecp_curves.o(.rodata.str1*))

		
		KEEP(*(.eh_frame*))
	} > ROM

	.ARM.extab :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > ROM

	__exidx_start = .;
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > ROM
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

	.rom.stubs :
	{

		. = ALIGN(0x10);
		__rom_stubs_hal_timer = .;
		__rom_stubs_hal_timer_s = .;
		KEEP(*(.rom.hal_timer.stubs))

		. = ALIGN(0x10);
		__rom_stubs_int_vector = .;
		KEEP(*(.rom.hal_vector.stubs))

		. = ALIGN(0x10);
		__rom_stubs_hal_crypto = .;
		KEEP(*(.rom.hal_crypto_s.stubs))
		
		. = ALIGN(0x10);
		__rom_stubs_hal_rsa = .;
		KEEP(*(.rom.hal_rsa.stubs))

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
		__rom_stubs_hal_i2c = .;
		KEEP(*(.rom.hal_i2c.stubs))
		
		. = ALIGN(0x10);
		__rom_stubs_hal_gpio = .;
		KEEP(*(.rom.hal_gpio.stubs))
		
		. = ALIGN(0x10);
		__rom_stubs_hal_otp = .;
		KEEP(*(.rom.hal_otp.stubs))

        . = ALIGN(0x10);
		__rom_stubs_hal_gdma = .;
		__rom_stubs_hal_gdma_s = .;
		KEEP(*(.rom.hal_gdma.stubs))

        . = ALIGN(0x10);
		__rom_stubs_hal_spic = .;
		KEEP(*(.rom.hal_spic.stubs))

        . = ALIGN(0x10);
		__rom_stubs_hal_flash = .;
		KEEP(*(.rom.hal_flash.stubs))

        . = ALIGN(0x10);
		__rom_stubs_hal_pinmux = .;
		KEEP(*(.rom.hal_pinmux.stubs))

        . = ALIGN(0x10);
		__rom_stubs_hal_cache = .;
		KEEP(*(.rom.hal_cache.stubs))

		. = ALIGN(0x10);
		__rom_stubs_hal_misc = .;
		KEEP(*(.rom.hal_misc.stubs))

		. = ALIGN(0x10);
		__rom_stubs_hal_wdt = .;
		KEEP(*(.rom.hal_wdt.stubs))
		
		. = ALIGN(0x10);
		__rom_stubs_flash_boot = .;
		KEEP(*(.rom.sboot.stubs))
		
		. = ALIGN(0x10);
		__rom_stubs_hal_sys_ctrl = .;
		KEEP(*(.rom.hal_sys_ctrl.stubs))

		. = ALIGN(0x10);
		__rom_stubs_hal_snand = .;
		KEEP(*(.rom.hal_snand.stubs))
		
		. = ALIGN(0x10);
		__rom_stubs_hal_hkdf = .;
		KEEP(*(.rom.hal_hkdf.stubs))
		
		. = ALIGN(0x10);
		__rom_stubs_hal_flash_sec = .;
		KEEP(*(.rom.hal_flash_sec.stubs))
        
		. = ALIGN(0x10);
		__rom_stubs_stdprintf_s = .;
		KEEP(*(.rom.printf.stubs))		

		. = ALIGN(0x10);
		__rom_stubs_strproc = .;
		KEEP(*(.rom.strproc.stubs))

		. = ALIGN(0x10);
		__rom_stubs_utility = .;
		KEEP(*(.rom.utility.stubs))	

		. = ALIGN(0x10);
		__rom_stubs_ssl = .;
		__rom_stubs_ssl_s = .;
		KEEP(*(.rom.ssl.stubs))

		/**** New stubs for A-Cut ROM start from here ****/
		. = ALIGN(0x10);
		__rom_stubs_flash_boot_tlv = .;
		KEEP(*(.rom.sboot_tlv.stubs))
		
		. = ALIGN(0x10);
		__rom_stubs_hal_hkdf_extend = .;
		KEEP(*(.rom.hal_hkdf_extend.stubs))

		. = ALIGN(0x10);
		__rom_stubs_hal_trng = .;
		KEEP(*(.rom.hal_trng.stubs))
		
		. = ALIGN(0x10);
		__rom_stubs_hal_flash_sec_extend = .;
		KEEP(*(.rom.hal_flash_sec_extend.stubs))

		. = ALIGN(0x10);
		__rom_stubs_snand_boot = .;
		KEEP(*(.rom.snand_boot.stubs))

		/**** New stubs for B-Cut ROM start from here ****/
		
		/**** New stubs for C-Cut ROM start from here ****/
		. = ALIGN(0x10);
		__rom_stubs_hal_trng_sec = .;
		KEEP(*(.rom.hal_trng_sec.stubs))

		. = ALIGN(0x10);
		__rom_stubs_hal_sys_ctrl_hgih_val_prot = .;
		KEEP(*(.rom.hal_sys_ctrl_high_val_prot.stubs))
		
	} > ROM_STUB


/************* End of ROM **************/

/* MPU protection Area Start */
	.ram.vector :
	{
		. = ALIGN(4);
		__ram_vector_start__ = .;
		KEEP(*(.ram_vector_table))
		__ram_vector_end__ = .;
		. = ALIGN(4);
		/* thses should be protected by MPU */
		KEEP(*(.rom.irq.bss))
	} > VECTORS_RAM
/* MPU protection Area End */

	.data_of_rom : AT (__rom_etext)		/* for ROM image building */
	{
		. = ALIGN(4);
		__rom_data_start__ = .;

		/**** ROM data ****/
		KEEP(*system_rtl8735b.o(.data*))
	 	KEEP(*startup.o(.data*))		
		KEEP(*rtl8735b_irq.o(.data*))
		KEEP(*fw_pg.o(.data*))
		KEEP(*fw_uart_boot.o(.data*))
		KEEP(*fw_flash_boot.o(.data*))
		KEEP(*fw_vow_rom_boot.o(.data*))
		KEEP(*(.rom.snand_boot.data))
		KEEP(*(.rom.hal_uart.data))
		KEEP(*(.rom.hal_i2c.data))
		KEEP(*(.rom.hal_timer.data))
		KEEP(*(.rom.hal_pwm.data))
		KEEP(*(.rom.hal_gpio.data))
		KEEP(*(.rom.hal_otp.data))
		KEEP(*(.rom.hal_cache.data))
		KEEP(*(.rom.hal_flash.data))
		KEEP(*(.rom.hal_gdma.data))
		KEEP(*(.rom.hal_spic.data))
		KEEP(*(.rom.hal_crypto_s.data))
		KEEP(*(.rom.hal_rsa.data))
		KEEP(*(.rom.hal_hkdf.data))
		KEEP(*(.rom.hal_snand.data))
		KEEP(*(.rom.hal_flash_sec.data))
		KEEP(*(.rom.hal_trng_sec.data))
		
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
		KEEP(*(.rom.hal_timer.bss))
		KEEP(*(.rom.hal_pwm.bss))
		KEEP(*(.rom.hal_gpio.bss))
		KEEP(*(.rom.hal_uart.bss))
		KEEP(*(.rom.hal_i2c.bss))
		KEEP(*(.rom.hal_otp.bss))
        KEEP(*(.rom.hal_gdma.bss))
        KEEP(*(.rom.hal_spic.bss))
        KEEP(*(.rom.hal_flash.bss))        
		KEEP(*(.rom.hal_pinmux.bss))
		KEEP(*(.rom.hal_cache.bss))
		KEEP(*(.rom.hal_sec.bss))
		KEEP(*(.rom.hal_wdt.bss))
		KEEP(*(.rom.hal_crypto_s.bss))
		KEEP(*(.rom.hal_rsa.bss))
		KEEP(*(.rom.hal_ecdsa.bss))
		KEEP(*(.rom.hal_eddsa.bss))
		KEEP(*(.rom.hal_hkdf.bss))
		KEEP(*(.rom.hal_snand.bss))
		KEEP(*(.rom.hal_flash_sec.bss))
		KEEP(*(.rom.hal_trng.bss))
		KEEP(*(.infra.rom.bss))
		KEEP(*(.stdioport.bss))
		KEEP(*strproc.o(.bss*))
		KEEP(*(.rom.wlan.bss))
		KEEP(*(.rom.ssl.bss))
		KEEP(*(.rom.snand_boot.bss))
		KEEP(*(.rom.hal_sys_ctrl.bss))
		KEEP(*(.rom.hal_trng_sec.bss))
		__rom_bss_end__ = .;

	} > ROM_USED_RAM

	/* Non-secure callable function */
	/* .sgstubs : for Old ASDK */
	.sgstubs :
	{
		KEEP(*(.ns_entry_func.text*))
	} > NSC

	.gnu.sgstubs :
	{
		__nsc_start__ = .;
		KEEP(*(.ns_entry_func.text*))
		__nsc_end__ = .;
	} > NSC

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

	.ram.data : AT (__etext)
	{
		__fw_img_start__ = .;
  	    __etext = .; 
		__data_start__ = .;
		*(vtable)
		*ram_start.o(.data*)
		*hal_sys_ctrl.o(.data*)
		*hal_pinmux.o(.data*)
		*hal_gdma.o(.data*)
		*hal_gpio.o(.data*)
		*hal_flash.o(.data*)
		*hal_timer.o(.data*)
		*hal_uart.o(.data*)
		*hal_i2s.o(.data*)
		*lib_a*(.data*)
		*hal_dram*.o(.data*)
		
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
		*libc_wrap.o(.text*)
		*lib_a*(.text*)
        /* contain NSC function */
        *rtos_api_s.o(.text*)
        *stack_s.o(.text*)
        *task_s.o(.text*)
        *cmse_nonsecure_call.o*(.text*)
		. = ALIGN(4);
		__ram_code_text_end__ = .;
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
		*lib_a*(.rodata*)
		*mpu_config.o(.rodata*)
		*hal_dram*.o(.rodata*)
		. = ALIGN(4);
		__ram_code_rodata_end__ = .;
	} > RAM

	.ram.bss :
	{
		. = ALIGN(16);
		*(.ram.bss.noinit*)

		__bss_start__ = .;
		*lib_a*(.bss*)
		*lib_a*(COMMON)
		*(.ram.*.bss*)		
		*ram_start.o(.bss*)
		*ram_start.o(COMMON)
		*hal_dram*.o(.bss*)
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
	/*
	.non_cacheable :
	{
		. = ALIGN(32);
		__nc_data_start__ = .;
		*(.non_cache.data*)
		__nc_bss_start__ = .;
		*(.non_cache.bss*)
		__nc_bss_end__ = .;
	} > RAM_NC
	*/

	/* templary BSS for ROM code, it will be recycled
	   and be used by RAM code */
	.rom_temp_use_ram.bss :
	{
		. = ALIGN(4);
		__rom_temp_bss_start__ = .;
		KEEP(*startup.o(.rom_temp.bss))	
		KEEP(*fw_pg.o(.rom_temp.bss))
		KEEP(*fw_pg.o(.rom.pg.secure.bss))
		KEEP(*fw_flash_boot.o(.rom_temp.bss))
		KEEP(*fw_voe_rom_boot.o(.rom_temp.bss))
		KEEP(*fw_uart_boot.o(.rom_temp.bss))	
		KEEP(*(.rom_temp.bss))
		__rom_temp_bss_end__ = .;

		. = ALIGN(32);
		
		__rom_boot_loader_tmp_buf_start__ = .;
		KEEP(*(.rom_boot_export.bss*))		
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
		__rom_sboot_bss_end__ = .;	

	} > DTCM_BOOT_TEMP

	.ddr.bss :
	{
		. = ALIGN(4);
		__eram_bss_start__ = .;	
		*(.bss*)
		*(COMMON)
		__eram_bss_end__ = .;
		__eram_heap_start__ = .;
		
	} > DDR
	
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
	
    .voe :
	{
		. = ALIGN(4);
		__voe_code_start__ = .;
		
	} > VOE	

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

	/* Set stack top to end of RAM, and stack limit move down by
	 * size of stack_dummy section */
	__StackTop = ORIGIN(DTCM_RAM) + LENGTH(DTCM_RAM);
	__StackLimit = __StackTop - SIZEOF(.stack_dummy);
	PROVIDE(__stack = __StackTop);

	/* Check if data + heap + stack exceeds RAM limit */
/* TODO:	ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack") */
}
