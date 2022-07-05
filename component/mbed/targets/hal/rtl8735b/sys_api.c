/**************************************************************************//**
 * @file     sys_api.c
 * @brief    This file implements system related API functions.
 *
 * @version  V1.00
 * @date     2022-02-18
 *
 * @note
 *
 ******************************************************************************
 *
 * Copyright(c) 2007 - 2022 Realtek Corporation. All rights reserved.
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
#include <stdio.h>
#include <string.h>
#include "cmsis.h"
#include "sys_api.h"
#include "hal_sys_ctrl.h"


/**
  * @brief  system software reset.
  * @retval none
  */
void sys_reset(void)
{
	hal_sys_set_system_reset();
}

/**
  * @brief  Turn off the JTAG/SWD function.
  * @retval none
  */
void sys_jtag_off(void)
{
	hal_sys_dbg_port_cfg(DBG_PORT_OFF, TMS_IO_S0_CLK_S0);
	hal_sys_dbg_port_cfg(DBG_PORT_OFF, TMS_IO_S1_CLK_S1);
}

/**
  * @brief  Get currently selected boot device.
  * @retval boot device
  * @note
  *  BootFromNORFlash            = 0,
  *  BootFromNANDFlash           = 1,
  *  BootFromUART                = 2
  */
uint8_t sys_get_boot_sel(void)
{
	uint8_t boot_sel;
	boot_sel = hal_sys_get_boot_select();
	return boot_sel;
}

