/**************************************************************************//**
 * @file     otp_boot_cfg.h
 * @brief    Define the structure types for boot flow configuration those from OTP.
 * @version  V1.00
 * @date     2022-07-27
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

#ifndef _OTP_BOOT_CFG_H
#define _OTP_BOOT_CFG_H

#ifdef __cplusplus
extern "C" {
#endif
#include "platform_conf.h"

typedef union {
	__IOM uint8_t byte;

	struct {
		__IOM uint8_t nand_clk_latch_ctrl_en  : 1;       /*!< [0..0] NAND Flash clock & latch enable control (1'b0: disable/ 1'b1: enable */
		__IOM uint8_t nand_clk_sel            : 2;       /*!< [2..1] NAND Flash clock select(2'b0: 62MHz, 2'b01: 31MHz, 2'b10: 20MHz, 3'b11: 12MHz */
		__IOM uint8_t nand_latch_sel          : 2;       /*!< [4..3] NAND Flash latch select(2'b0: no delay, 2'b01: delay 1T, 2'b10: delay 2T, 2'b11: delay 3T) */
		__IM  uint8_t : 3;
	} bit;
} otp_boot_cfg9_t, *potp_boot_cfg9_t;

#define OTPBootCfg9Offset               (0x1E)

#define OTPAutoLoadRegBase        (0x400090C0UL)

#define otpBootCfg9               ((otp_boot_cfg9_t *)(OTPAutoLoadRegBase + OTPBootCfg9Offset))

#ifdef __cplusplus
}
#endif

#endif /* _OTP_BOOT_CFG_H */
