/**************************************************************************//**
* @file        rtl8735b_sys_ctrl.h
* @brief       The HAL API implementation for SYSTEM CONTROL
*
* @version     V1.00
* @date        2022-07-22
*
* @note
*
******************************************************************************
*
* Copyright(c) 2007 - 2017 Realtek Corporation. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the License); you may
* not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an AS IS BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************************/

#ifdef CONFIG_BUILD_LIB
#error ERROR, only for application
#endif

#ifndef _RTL8735B_SYS_CTRL_H_
#define _RTL8735B_SYS_CTRL_H_
//#include "cmsis.h"
#include "rtl8735b_syson_s_type.h"
#include "rtl8735b_trng_sec.h"

#ifdef  __cplusplus
extern "C"
{
#endif

typedef enum {
	LXBUS_CTRL_SNAND        = 0x0,
	LXBUS_CTRL_CRYPTO       = 0x1,
	LXBUS_CTRL_SDHOST       = 0x2,
	LXBUS_CTRL_MII          = 0x3,
	LXBUS_CTRL_I2S          = 0x4,
} LXBUS_SHARE_USED_IDX_T;

/**
* @addtogroup N/A
* @{
*/

typedef enum {
	EDDSA_SYS = 0,
	GPIO_SYS = 1,
	GPIO_PON = 2,
	GPIO_AON = 3,

	DDR_SYS = 4,

	GDMA0_SYS = 5,
	GDMA1_SYS = 6,
	FLASH_SYS = 7,
	SPI0_SYS = 8,
	SPI1_SYS = 9,
	HS_SPI0_SYS = 10,
	HS_SPI1_SYS = 11,
	UART0_SYS = 12,
	UART1_SYS = 13,
	UART2_SYS = 14,
	UART3_SYS = 15,
	UART4_SYS = 16,
	TIMER0_SYS = 17,
	TIMER1_SYS = 18,
	TIMER2_SYS = 19,
	TIMER3_SYS = 20,
	PWM_SYS = 21,
	RSA_SYS = 22,
	CRYPTO_SYS = 23,
	I2C0_SYS = 24,
	I2C1_SYS = 25,
	I2C2_SYS = 26,
	I2C3_SYS = 27,
	ECDSA_SYS = 28,
	ADC_SYS = 29,
	RTC_SYS = 30,
	SPORT_SYS = 31,
	I2S0_SYS = 32,
	I2S1_SYS = 33,
	LXBUS_SYS = 34,

	ISP_SYS = 35,
	MIPI_SYS = 36,
	ENC_SYS = 37,
	NN_SYS = 38,
	VOE_SYS = 39,

	CPU_SYS = 40,
	SGPIO_SYS = 41,
	TRNG_SYS = 42,

	SI_SYS = 43,
	AUDIO_CODEC_EN = 44,
	AUDIO_CODEC_SCLK_EN = 45,
	AUDIO_CODEC_PORB_EN = 46,
	AUDIO_CODEC_LDO_EN = 47,
	AUDIO_CODEC_EXTE_EN = 48,
	AUDIO_CODEC_LDO_TUNE = 49,
	TRNG_32K = 50,
	TRNG_128K = 51,
	LDO_SDIO_3V3_EN = 52,
	TRNG_SEC = 53,
	SDHOST_SYS = 54,
	SDHOST_CLK1_PHASE = 55,
	SDHOST_CLK2_PHASE = 56,
	LDO_SDIO_1V8_CTRL = 57,
	OTG_SYS_CTRL = 58,
	SNAND_SYS = 59,
	SNAND_MUX = 60,

} IPs_CLK_FUNC_t;

typedef enum {
	UART_IRC_4M = 0,
	UART_PERI_40M,
	UART_XTAL,
	RSVD
} UART0_CLK_SELECT_t;

typedef enum {
	NN_500M = 0,   // CLK 500Mhz
	NN_400M,       // CLK 400Mhz
	NN_250M,       // CLK 2500Mhz
} NN_CLK_SELECT_t;

typedef hal_status_t (*hv_prot_trng_init_func_t)(void);
typedef hal_status_t (*hv_prot_trng_set_clk_func_t)(uint8_t sel_val);
typedef hal_status_t (*hv_prot_trng_swrst_en_func_t)(void);
typedef hal_status_t (*hv_prot_trng_ld_def_setting_func_t)(uint8_t selft_en);
typedef hal_status_t (*hv_prot_trng_deinit_func_t)(void);
typedef uint32_t (*hv_prot_trng_get_rand_func_t)(void);

/**
  \brief  The data structure of the stubs function for the SYSON HAL functions in ROM
*/
typedef struct hal_sys_ctrl_func_stubs_s {
	void (*hal_sys_peripheral_en)(uint8_t id, uint8_t en);
	void (*hal_sys_set_clk)(uint8_t id, uint8_t sel_val);
	uint32_t (*hal_sys_get_clk)(uint8_t id);
	uint32_t (*hal_sys_boot_info_get_val)(uint8_t info_idx);
	void (*hal_sys_boot_info_assign_val)(uint8_t info_idx, uint32_t info_v);
	void (*hal_sys_boot_footpath_init)(uint8_t info_idx);
	void (*hal_sys_boot_footpath_store)(uint8_t info_idx, uint8_t fp_v);
	void (*hal_sys_sjtag_fixed_key_enable)(void);
	void (*hal_sys_sjtag_non_fixed_key_set)(uint8_t set_sjtag_obj, uint8_t *pkey);
	uint32_t (*hal_sys_get_video_info)(uint8_t idx);
	void (*hal_sys_set_video_info)(uint8_t idx, uint8_t en_ctrl);
	void (*hal_sys_get_chip_id)(uint32_t *pchip_id);
	void (*hal_sys_boot_footpath_clear)(uint8_t info_idx, uint8_t fp_v);
	uint8_t (*hal_sys_get_rma_state)(void);
	void (*hal_sys_high_value_assets_otp_lock)(const uint8_t lock_obj);
	uint32_t (*hal_sys_get_uuid)(void);
	void (*hal_sys_lxbus_shared_en)(uint8_t used_id, uint8_t en);
	hal_status_t (*hal_sys_adc_vref_setting)(uint8_t set_value);
	void (*hal_sys_set_sw_boot_rom_trap_op)(uint8_t op_idx, uint8_t ctrl_status);
	uint8_t (*hal_sys_get_atld_cfg)(const uint8_t op);
	uint8_t (*hal_sys_get_lfc_state)(void);
	uint8_t (*hal_sys_chk_lfc_func_ctrl)(const uint32_t lfc_func_op);
	uint32_t reserved[2];  // reserved space for next ROM code version function table extending.
} hal_sys_ctrl_func_stubs_t;

/**
  \brief  The data structure of the stubs function for the high value assets protect load functions of system ctrl in ROM
*/
typedef struct hal_sys_ctrl_high_val_prot_func_stubs_s {
	void (*hal_sys_high_val_protect_init_hook)(hv_prot_trng_init_func_t trng_init_f, hv_prot_trng_deinit_func_t trng_deinit_f,
			hv_prot_trng_ld_def_setting_func_t trng_ld_def_set_f, hv_prot_trng_get_rand_func_t trng_get_rand_f,
			hv_prot_trng_set_clk_func_t trng_set_clk_f, hv_prot_trng_swrst_en_func_t trng_swrst_enf_f);
	void (*hal_sys_high_val_protect_init)(void);
	void (*hal_sys_high_val_protect_deinit)(void);
	void (*hal_sys_high_val_protect_ld)(const uint32_t otp_addr, uint8_t *p_otp_v, const uint32_t ld_size);
	void (*hal_sys_high_val_protect_ld_delay)(uint8_t delay_unit_sel);
	uint8_t (*hal_sys_check_high_val_protect_init)(void);
	int (*hal_sys_high_val_protect_cmp)(const void *a, const void *b, size_t size);
	void (*hal_sys_high_val_mem_protect_ld)(void *s1, const void *s2, size_t ld_size);
} hal_sys_ctrl_high_val_prot_func_stubs_t;

typedef struct hal_sys_ctrl_extend_func_stubs_s {
	uint8_t *pld_img_bl_vrf_digest;
	uint8_t *pcust_uid_derived;
	uint32_t reserved[30];  // reserved space for next ROM code version function table extending.
} hal_sys_ctrl_extend_func_stubs_t;

/** @} */ /* End of group hs_hal_efuse */

#ifdef  __cplusplus
}
#endif


#endif  // end of "#define _RTL8735B_SYS_CTRL_H_"


