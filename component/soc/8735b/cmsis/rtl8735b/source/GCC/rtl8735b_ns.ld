/* Linker script to configure memory regions. */

/* Select External RAM type - 1: LPDDR SDRAM 2: PSRAM */
useERAM = 1;

MEMORY
{
  ITCM_ROM (rx) 		: ORIGIN = 0x00003000, LENGTH = 0x00008000 - 0x00003000		/*  20 KB    */
  ITCM_RAM (rx) 		: ORIGIN = 0x00010000, LENGTH = 0x00017000 - 0x00010000		/*  28KB    */
  ROM (rx) 				: ORIGIN = 0x1004A800, LENGTH = 0x100BBF00 - 0x1004A800		/* 453KB+768B*/
  ROM_STUB (rx) 		: ORIGIN = 0x100BBF00, LENGTH = 0x100BFF00 - 0x100BBF00		/*  16 KB    */
  ROM_NS4S_STUB (rx)    : ORIGIN = 0x100BFF00, LENGTH = 0x100C0000 - 0x100BFF00		/*  256B     */
  
  DTCM_BOOT_TEMP (wrx) 	: ORIGIN = 0x20000000, LENGTH = 0x20000400 - 0x20000000 	/*   1 KB    */
  DTCM_RAM (wrx) 		: ORIGIN = 0x20000400, LENGTH = 0x20002000 - 0x20000400		/*   7 KB    */

  DTCM_ROM (rx) 		: ORIGIN = 0x20017800, LENGTH = 0x20024000 - 0x20017800		/*  50 KB    */

  /* The vector table, it must start with 256 bytes aligned address */
  /* The Reset_Handler also is the NS region code entry function */
  VECTORS_RAM (rwx)     : ORIGIN = 0x20100000, LENGTH = 0x20100200 - 0x20100000		/*  512B     */
  
  /* reserved for ROM code */
  ROM_USED_RAM (rwx)    : ORIGIN = 0x20100200, LENGTH = 0x20101200 - 0x20100200		/*  4KB      */

  /* 0x20101200 ~ 0x20105200 16KB is reserved for Bootloader RAM code used */
  /* 0x20105200 ~ 0x20106200 4KB is reserved for Bootloader RAM BSS used */
  
  /* RAM functions entry table */
  RAM_FUN_TABLE (rwx)   : ORIGIN = 0x20106200, LENGTH = 0x201062F0 - 0x20106200		/*  240B     */
  
  /* RAM image Signature */
  RAM_IMG_SIGN (rwx)    : ORIGIN = 0x201062F0, LENGTH = 0x20106300 - 0x201062F0		/*  16B      */
  
  /* Internal SRAM for text, rodata & data */
  RAM (rwx)             : ORIGIN = 0x20106300, LENGTH = 0x20156000 - 0x20106300		/* 319.25KB  */

  /* Non-Cacheable memory, defined by MPU configuration */
  /* RAM_NC (rwx)       : ORIGIN = 0x20128000, LENGTH = 0x20180000 - 0x20128000 */

  /* DDR memory */
  VOE (rwx)          	: ORIGIN = 0x70000000, LENGTH = 0x70100000 - 0x70000000		/*  1MB */
  DDR_NC (rwx)	        : ORIGIN = 0x70100000, LENGTH = 0x70300000 - 0x70100000		/*  2MB */
  DDR (rwx)	            : ORIGIN = 0x70300000, LENGTH = 0x72E00000 - 0x70300000		/* 43MB */
  NN  (rwx)      		: ORIGIN = 0x72E00000, LENGTH = 0x73E00000 - 0x72E00000		/* 16MB */ 
  
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
_start_addr = 0x1006f000;
ENTRY(_start_addr)

__sram_end__		= ORIGIN(RAM) + LENGTH(RAM);
__eram_end__		= ORIGIN(DDR) + LENGTH(DDR);
__eram_start__		= ORIGIN(DDR);

__voe_code_size__   = LENGTH(VOE);

SECTIONS
{
	.sysview :
	{
		KEEP(*(.sysview.start))	
		KEEP(*SEGGER_*.o(.data*))
		KEEP(*SEGGER_*.o(.text*))
		KEEP(*SEGGER_*.o(.rodata*))		
	} > ITCM_RAM

	.rom.itcm.text :
	{
		KEEP(*(.rom.ssl.itcm))
		KEEP(*(.rom.apple.itcm))
	} > ITCM_ROM

	.rom.dtcm.data :
	{
		KEEP(*(.rom.ssl.dtcm))
		KEEP(*(.rom.apple.dtcm))
	} > DTCM_ROM

	.rom.ns_vector_tbl :
	{
		KEEP(*(.rom_ns.vectors))
		__Vectors_End = .;
		__Vectors_Size = __Vectors_End - __Vectors_NS;
		__end__ = .;

	} > ROM

	.rom.text :
	{
		. = ALIGN(0x100);

		KEEP(*startup_rtl8735b.o(.text*))
		KEEP(*system_rtl8735b.o(.text*))
		KEEP(*(.startup.text))
		KEEP(*(.vector.text))
		KEEP(*(.sysstart.text))
		KEEP(*(.sys_misc.text))
		KEEP(*(.rom.irq.rodata*))

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
		KEEP(*rt_scanf.o(.text*))
		KEEP(*rt_scanf.o(.rodata*))

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
		KEEP(*rtl8735b_timer.o(.rodata.str1*))
		KEEP(*rtl8735b_pwm.o(.rodata.str1*))
		KEEP(*rtl8735b_gpio.o(.rodata.str1*))
		KEEP(*rtl8735b_eddsa.o(.rodata.str1*))
		KEEP(*rtl8735b_ecdsa.o(.rodata.str1*))
		KEEP(*rtl8735b_misc.o(.rodata.str1*))
		KEEP(*rtl8735b_gdma.o(.rodata.str1*))
		KEEP(*rtl8735b_sdmmc_host.o(.rodata.str1*))
		KEEP(*rtl8735b_crypto.o(.rodata.str1*))
		KEEP(*rtl8735b_rsa.o(.rodata.str1*))
		KEEP(*rtl8735b_i2c.o(.rodata.str1*))
		
		KEEP(*rtl8735b_timer.o(.rodata*))
		KEEP(*rtl8735b_pwm.o(.rodata*))
		KEEP(*rtl8735b_uart.o(.rodata*))
		KEEP(*rtl8735b_i2c.o(.rodata*))
		KEEP(*rtl8735b_gpio.o(.rodata*))
		KEEP(*rtl8735b_ecdsa.o(.rodata*))
		KEEP(*rtl8735b_crypto.o(.rodata*))
		KEEP(*rtl8735b_rsa.o(.rodata*))
		KEEP(*rtl8735b_trng.o(.rodata*))
	
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

	.note.gnu.build-id :
	{
		*(.note.gnu.build-id)
	} > ROM

	.rom.stubs :
	{

		. = ALIGN(0x10);
		__rom_stubs_hal_timer = .;
		KEEP(*(.rom.hal_timer.stubs))

		. = ALIGN(0x10);
		__rom_stubs_int_vector = .;
		KEEP(*(.rom.hal_vector.stubs))

		. = ALIGN(0x10);
		__rom_stubs_hal_crypto = .;
		__rom_stubs_hal_crypto_ns = .;
		KEEP(*(.rom.hal_crypto_ns.stubs))
		
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
		__rom_stubs_hal_gpio = .;
		KEEP(*(.rom.hal_gpio.stubs))
				
		. = ALIGN(0x10);
		__rom_stubs_hal_gdma = .;
		__rom_stubs_hal_gdma_ns = .;
		KEEP(*(.rom.hal_gdma.stubs))
 		
		. = ALIGN(0x10);
		__rom_stubs_hal_cache = .;
		KEEP(*(.rom.hal_cache.stubs))
       
		. = ALIGN(0x10);
		__rom_stubs_hal_i2c = .;
		KEEP(*(.rom.hal_i2c.stubs))

		. = ALIGN(0x10);
		__rom_stubs_hal_misc = .;
		KEEP(*(.rom.hal_misc.stubs))
				
		. = ALIGN(0x10);  
		__rom_stubs_shell = .;
		KEEP(*(.rom.shellcmd.stubs))

		. = ALIGN(0x10);
		__rom_stubs_stdprintf_ns = .;
		KEEP(*(.rom.printf.stubs))		

		. = ALIGN(0x10);
		__rom_stubs_strproc = .;
		KEEP(*(.rom.strproc.stubs))

		. = ALIGN(0x10);
		__rom_stubs_utility = .;
		__rom_stubs_utility_ns = .;
		KEEP(*(.rom.utility.stubs))
		
		. = ALIGN(0x10);
		__rom_stubs_wlan = .;
		KEEP(*(.rom.wlan.stubs))

		. = ALIGN(0x10);
		__rom_stubs_apple = .;
		KEEP(*(.rom.apple.stubs))

		. = ALIGN(0x10);
		__rom_stubs_ssl = .;
		__rom_stubs_ssl_ns = .;
		KEEP(*(.rom.ssl.stubs))

		. = ALIGN(0x10);
		__rom_stubs_faac = .;
		KEEP(*(.rom.faac.stubs))

		/**** New stubs for B-Cut ROM start from here ****/
		. = ALIGN(0x10);
		__rom_stubs_hal_trng = .;
		KEEP(*(.rom.hal_trng.stubs))

		/**** New stubs for C-Cut ROM start from here ****/
	} > ROM_STUB

	.rom.ns4s_stubs :
    {
    /* __rom_stubs_symb_ns4s always must be the 1st symbol of ROM stubs */
        __rom_stubs_symb_ns4s = .;
        KEEP(*(.rom.ns4s.stubs))
    } > ROM_NS4S_STUB

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
		KEEP(*rtl8735b_handler_ns.o(.data*))
		KEEP(*rtl8735b_irq.o(.data*))
		KEEP(*rtl8735b_symbns4s.o(.data*))
		KEEP(*startup_ns.o(.data*))
		KEEP(*(.rom.hal_uart.data))
		KEEP(*(.rom.hal_timer.data))
		KEEP(*(.rom.hal_pwm.data))
		KEEP(*(.rom.hal_gpio.data))
		KEEP(*(.rom.hal_sdioh.data))
		KEEP(*(.rom.hal_cache.data))
		KEEP(*(.rom.hal_gdma.data))
		KEEP(*(.rom.hal_crypto_ns.data))
		KEEP(*(.rom.hal_rsa.data))
		KEEP(*(.rom.hal_i2c.data))		
		KEEP(*(.xprintf.data))
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
		KEEP(*(.rom.hal_i2c.bss))
        KEEP(*(.rom.hal_gdma.bss))
		KEEP(*(.rom.hal_sdioh.bss))
		KEEP(*(.rom.hal_cache.bss))
		KEEP(*(.rom.hal_crypto_ns.bss))
		KEEP(*(.rom.hal_ecdsa.bss))
		KEEP(*(.rom.hal_rsa.bss))
		KEEP(*(.rom.hal_lpi.bss))
		KEEP(*(.rom.hal_trng.bss))
		KEEP(*(.xprintf.bss))
		KEEP(*(.logbuf.bss))
		KEEP(*(.stdioport.bss))
		KEEP(*strproc.o(.bss*))

		KEEP(*(.rom.wlan.bss))
		KEEP(*(.rom.ssl.bss))
		__rom_bss_end__ = .;

	} > ROM_USED_RAM

	.ram.func.table :
	{
		__fw_img_start__ = .;
		__ram_start_table_start__ = .;
		KEEP(*(SORT(.start.ram.data*)))
		__ram_start_table_end__ = .;
		
	} > RAM_FUN_TABLE

	.voe.ddr.data :
	{
		. = ALIGN(4);
		__eram_voe_data_start__ = .;
		*get_option.o(.data*)
        *clk-rts3915.o(.data*)
        *encasiccontroller.o(.data*)
		*ewl_ameba.o(.data*)
        *fixp.o(.data*)
        *hal_voe.o(.data*)
        *isp_errno.o(.data*)
        *isp_hw_eeh.o(.data*)
        *isp_hw_intp.o(.data*)
        *isp_hw_vreg.o(.data*)
        *isp_log.o(.data*)
        *isp_mod_ae.o(.data*)
        *isp_mod_af.o(.data*)
        *isp_mod_awb.o(.data*)
        *isp_mod_blc.o(.data*)
        *isp_mod_ccm.o(.data*)
        *isp_mod_control.o(.data*)
        *isp_mod_crop.o(.data*)
        *isp_mod_dehaze.o(.data*)
        *isp_mod_eeh.o(.data*)
        *isp_mod_flick.o(.data*)
        *isp_mod_gamma.o(.data*)
        *isp_mod_global.o(.data*)
        *isp_mod_interface.o(.data*)
        *isp_mod_intp.o(.data*)
        *isp_mod_iq.o(.data*)
        *isp_mod_ldc.o(.data*)
        *isp_mod_mask.o(.data*)
        *isp_mod_mcrop.o(.data*)
        *isp_mod_mlsc.o(.data*)
        *isp_mod_nlsc.o(.data*)
        *isp_mod_rawstat.o(.data*)
        *isp_mod_rgb2yuv.o(.data*)
        *isp_mod_sensor.o(.data*)
        *isp_mod_spe.o(.data*)
        *isp_mod_statis.o(.data*)
        *isp_mod_tnr.o(.data*)
        *isp_mod_uvtune.o(.data*)
        *isp_mod_vctrl.o(.data*)
        *isp_mod_wdr.o(.data*)
        *isp_mod_ygamma.o(.data*)
        *isp_mod_ygc.o(.data*)
        *isp_mod_ystat.o(.data*)
        *isp_shm.o(.data*)
        *isp_top.o(.data*)
        *pinctrl-rts3915.o(.data*)
        *reset-rts3915.o(.data*)
        *rlx_sh_irq.o(.data*)
        *rts_camera_isp.o(.data*)
        *rts_camera_isp_mfd.o(.data*)
        *rts_camera_md.o(.data*)
        *rts_camera_osd2.o(.data*)
        *rts_camera_soc.o(.data*)
        *rts_camera_soc_3915.o(.data*)
        *rts_camera_verify.o(.data*)
        *rts_camera_video.o(.data*)
        *rts_camera_zoom.o(.data*)
        *run_video.o(.data*)
        *vb2_utils.o(.data*)
		*rtl_crc.o(.data*)
		*rts_camera.o(.data*)
		*isp_driver.o(.data*)
		*voe.o(.data*)
        __eram_voe_data_end__ = .;
		
	} > DDR	
	
	.ram.data : AT (__etext)
	{
		__etext = .; 
		__data_start__ = .;
		*(vtable)
		*ram_start.o(.data*)
		*hal_gdma.o(.data*)
		*hal_gpio.o(.data*)
		*hal_timer.o(.data*)
		*hal_uart.o(.data*)

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

	.ram.code_text : AT (__ram_code_text_start__)
	{
		. = ALIGN(4);
		__etext2 = .;
		__ram_code_text_start__ = .;

/*		*(.text*) */
		*ram_start.o(.text*)
		*hal_timer.o(.text*)
		*hal_uart.o(.text*)
		*app_start.o(.text*)
		*hal_sys_ctrl.o(.text*)
		*hal_pinmux.o(.text*)
		*mpu_config.o(.text*)
		. = ALIGN(4);
		__ram_code_text_end__ = .;
	} > RAM

	.voe.ddr.rodata :
	{
		. = ALIGN(4);
		__eram_voe_rodata_start__ = .;
		*get_option.o(.rodata*)
        *clk-rts3915.o(.rodata*)
        *encasiccontroller.o(.rodata*)
		*ewl_ameba.o(.rodata*)
        *fixp.o(.rodata*)
        *hal_voe.o(.rodata*)
        *isp_errno.o(.rodata*)
        *isp_hw_eeh.o(.rodata*)
        *isp_hw_intp.o(.rodata*)
        *isp_hw_vreg.o(.rodata*)
        *isp_log.o(.rodata*)
        *isp_mod_ae.o(.rodata*)
        *isp_mod_af.o(.rodata*)
        *isp_mod_awb.o(.rodata*)
        *isp_mod_blc.o(.rodata*)
        *isp_mod_ccm.o(.rodata*)
        *isp_mod_control.o(.rodata*)
        *isp_mod_crop.o(.rodata*)
        *isp_mod_dehaze.o(.rodata*)
        *isp_mod_eeh.o(.rodata*)
        *isp_mod_flick.o(.rodata*)
        *isp_mod_gamma.o(.rodata*)
        *isp_mod_global.o(.rodata*)
        *isp_mod_interface.o(.rodata*)
        *isp_mod_intp.o(.rodata*)
        *isp_mod_iq.o(.rodata*)
        *isp_mod_ldc.o(.rodata*)
        *isp_mod_mask.o(.rodata*)
        *isp_mod_mcrop.o(.rodata*)
        *isp_mod_mlsc.o(.rodata*)
        *isp_mod_nlsc.o(.rodata*)
        *isp_mod_rawstat.o(.rodata*)
        *isp_mod_rgb2yuv.o(.rodata*)
        *isp_mod_sensor.o(.rodata*)
        *isp_mod_spe.o(.rodata*)
        *isp_mod_statis.o(.rodata*)
        *isp_mod_tnr.o(.rodata*)
        *isp_mod_uvtune.o(.rodata*)
        *isp_mod_vctrl.o(.rodata*)
        *isp_mod_wdr.o(.rodata*)
        *isp_mod_ygamma.o(.rodata*)
        *isp_mod_ygc.o(.rodata*)
        *isp_mod_ystat.o(.rodata*)
        *isp_shm.o(.rodata*)
        *isp_top.o(.rodata*)
        *pinctrl-rts3915.o(.rodata*)
        *reset-rts3915.o(.rodata*)
        *rlx_sh_irq.o(.rodata*)
        *rts_camera_isp.o(.rodata*)
        *rts_camera_isp_mfd.o(.rodata*)
        *rts_camera_md.o(.rodata*)
        *rts_camera_osd2.o(.rodata*)
        *rts_camera_soc.o(.rodata*)
        *rts_camera_soc_3915.o(.rodata*)
        *rts_camera_verify.o(.rodata*)
        *rts_camera_video.o(.rodata*)
        *rts_camera_zoom.o(.rodata*)
        *run_video.o(.rodata*)
        *vb2_utils.o(.rodata*)
		*rtl_crc.o(.rodata*)
		*rts_camera.o(.rodata*)
		*isp_driver.o(.rodata*)
		*voe.o(.rodata*)
        __eram_voe_rodata_end__ = .;
		
	} > DDR	
	
	.ram.code_rodata : AT (__ram_code_rodata_start__)
	{
		. = ALIGN(4);
		__ram_code_rodata_start__ = .;
		*ram_start.o(.rodata*)
		*hal_gdma.o(.rodata*)
		*hal_gpio.o(.rodata*)
		*hal_timer.o(.rodata*)
		*hal_uart.o(.rodata*)
		*lib_a*(.rodata*)
		*mpu_config.o(.rodata*)
		*app_start.o(.rodata*)
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
		. = ALIGN(16);
		*(.ram.bss.noinit*)

		__bss_start__ = .;
		*lib_a*(.bss*)
		*lib_a*(COMMON)
		*(.ram.*.bss*)
		*ram_start.o(.bss*)
		*ram_start.o(COMMON)
		. = ALIGN(4);
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
	.rom_temp_use_dtcm.bss :
	{
		. = ALIGN(4);
		__rom_temp_bss_start__ = .;
		KEEP(*(.rom_temp.bss))		
		__rom_temp_bss_end__ = .;
	} > DTCM_BOOT_TEMP

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
}
