/**************************************************************************//**
 * @file     fw_img_tlv.h
 * @brief    This file defines the image tlv(type-length-value) format for boot flow and some secure info type
 *           for secure boot.
 *
 * @version  V1.00
 * @date     2022-07-29
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
#if defined(CONFIG_BUILD_LIB) && (CONFIG_BUILD_LIB == 1)
#error ERROR, only for application
#endif

#ifndef _FW_IMG_TLV_H_
#define _FW_IMG_TLV_H_

#include "otp_boot_cfg.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/* NAND Boot */
#define _ERRNO_BOOT_NAND_BOOT_NONSUPPORT_NTLV_FORMAT                    (0x60)
#define _ERRNO_BOOT_SNAFC_INIT_FAIL                                     (0x61)
#define _ERRNO_BOOT_SNAFC_DEINIT_FAIL                                   (0x62)
#define _ERRNO_BOOT_SNAFC_MEMCPY_FAIL                                   (0x63)
#define _ERRNO_BOOT_SNAFC_OFFSET_FAIL                                   (0x64)
#define _ERRNO_BOOT_NAND_NO_VALID_CTRL_BLK                              (0x65)
#define _ERRNO_BOOT_NAND_INVALID_CTRL                                   (0x66)
#define _ERRNO_BOOT_NAND_NO_VALID_PART_BLK                              (0x67)
#define _ERRNO_BOOT_NAND_NO_VALID_ISP_IDX                               (0x68)
#define _ERRNO_BOOT_NAND_NO_VALID_BL_IDX                                (0x69)
#define _ERRNO_BOOT_NAND_BLK_OVERFLOW                                   (0x64)
#define _ERRNO_BOOT_NAND_GLB_VMAP_SLOT                                  (0x65)

#define _ERRNO_BOOT_NAND_NO_SUPPORT_TB                                  (0x70)

#define _ERRNO_BOOT_MSG_SIZE_OUT_OF_RANGE                               (0xA0)
#define _ERRNO_BOOT_INVALID_STR_SIGN_VAL                                (0xA1)
#define _ERRNO_BOOT_IDX_OUT_OF_RANGE                                    (0xA2)
#define _ERRNO_BOOT_DEV_CTRL_INIT_FAIL                                  (0xA3)
#define _ERRNO_BOOT_FCS_DATA_HEADER_FAIL                                (0xA4)

#define IMG_MANIFEST_IE_VERSION_SIZE                (32)

#define PARTITION_RECORD_MAX                        (12)

typedef struct gpio_pwr_on_trap_pin_s {
	uint16_t pin: 5;            /*!< bit: 4...0 the GPIO pin number */
	uint16_t port: 3;           /*!< bit: 7...6 the  GPIO port number */
	uint16_t io_lev: 1;         /*!< bit:  8 the IO level to trigger the trap */
	uint16_t reserved: 6;       /*!< bit: 14...9 reserved */
	uint16_t valid: 1;          /*!< bit:  15 is this trap valid */
} gpio_pwr_on_trap_pin_t;

typedef struct part_fst_info_s {
	uint8_t rec_num;
	uint8_t bl_p_idx;
	uint8_t bl_s_idx;
	uint8_t fw1_idx;
	uint8_t fw2_idx;
	uint8_t iq_idx;
	uint8_t nn_m_idx;
	uint8_t mp_idx;
	uint8_t keycert1_idx;   // For NAND Flash boot
	uint8_t keycert2_idx;   // For NAND Flash boot
	uint8_t fcs_para_idx;
	uint8_t resv1[5];

	gpio_pwr_on_trap_pin_t ota_trap;
	gpio_pwr_on_trap_pin_t mp_trap;
	uint32_t udl;
	uint8_t resv2[8];
} part_fst_info_t, *ppart_fst_info_t;


#ifdef  __cplusplus
}
#endif

#endif  // end of "#define _FW_IMG_TLV_H_"