/**************************************************************************//**
* @file        hal_efuse_nsc.h
* @brief       The HAL Non-secure callable API implementation for the EFUSE
*
* @version     V1.00
* @date        2022-11-15
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

#ifndef _HAL_SYS_CTRL_NSC_H_
#define _HAL_SYS_CTRL_NSC_H_
#include "cmsis.h"
#include <arm_cmse.h>   /* Use CMSE intrinsics */
#include "hal_sys_ctrl.h"
#ifdef  __cplusplus
extern "C"
{
#endif

#if defined(CONFIG_BUILD_NONSECURE)
void hal_sys_peripheral_nsc(uint8_t id, uint8_t en);
void hal_sys_set_clk_nsc(uint8_t id, uint8_t sel_val);
uint32_t hal_sys_get_clk_nsc(uint8_t id);
uint8_t hal_sys_get_ld_fw_idx_nsc(void);
hal_status_t hal_sys_get_video_img_ld_offset_nsc(void *ctrl_obj_info, const uint8_t ctrl_obj);
uint8_t hal_sys_get_rom_ver_nsc(void);

#if !defined(ENABLE_SECCALL_PATCH)
#define hal_sys_peripheral_en                 hal_sys_peripheral_nsc
#define hal_sys_set_clk                       hal_sys_set_clk_nsc
#define hal_sys_get_clk                       hal_sys_get_clk_nsc
#define hal_sys_get_ld_fw_idx                 hal_sys_get_ld_fw_idx_nsc
#define hal_sys_get_video_img_ld_offset       hal_sys_get_video_img_ld_offset_nsc
#define hal_sys_get_rom_ver                   hal_sys_get_rom_ver_nsc
#endif
#endif  // end of "#if defined(CONFIG_BUILD_NONSECURE)"


/** @} */ /* End of group hal_sys_ctrl_nsc */

#ifdef  __cplusplus
}
#endif


#endif  // end of "#define _HAL_SYS_CTRL_NSC_H_"
