/**************************************************************************//**
* @file        hal_sys_ctrl.h
* @brief       The HAL API implementation for the System control
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

#ifndef _HAL_SYS_CTRL_H_
#define _HAL_SYS_CTRL_H_

#ifdef  __cplusplus
extern "C"
{
#endif

typedef enum {
	BT_UART_MUX_EXTERNAL = 0,
	BT_UART_MUX_INTERNAL = 1
} BT_UART_MUX_SELECT_t;

typedef enum {
	VIDEO_ISP_SENSOR_SET0               = 0x1,
	VIDEO_ISP_SENSOR_SET1               = 0x2,
	VIDEO_ISP_SENSOR_SET2               = 0x3,
	VIDEO_ISP_SENSOR_SET3               = 0x4,
	VIDEO_ISP_SENSOR_SET4               = 0x5,
	VIDEO_ISP_SENSOR_SET5               = 0x6,
	VIDEO_ISP_SENSOR_SET6               = 0x7,
	VIDEO_ISP_SENSOR_SET7               = 0x8,
	VIDEO_ISP_SENSOR_SET8               = 0x9,
	VIDEO_ISP_SENSOR_SET9               = 0xA
} VIDEO_ISPIQ_IMG_GET_SENSOR_SET_CTRL_t;

typedef enum {
	VIDEO_ISP_SENSOR_IQ                 = 0x1,
	VIDEO_ISP_SENSOR_DATA               = 0x2,
} VIDEO_ISPIQ_IMG_GET_SENSOR_MEMBER_CTRL_t;

typedef enum {
	VIDEO_ISP_SET0_IQ_OFFSET            = ((VIDEO_ISP_SENSOR_SET0 << 4) | VIDEO_ISP_SENSOR_IQ),
	VIDEO_ISP_SET0_SENSOR_OFFSET        = ((VIDEO_ISP_SENSOR_SET0 << 4) | VIDEO_ISP_SENSOR_DATA),
	VIDEO_ISP_SET1_IQ_OFFSET            = ((VIDEO_ISP_SENSOR_SET1 << 4) | VIDEO_ISP_SENSOR_IQ),
	VIDEO_ISP_SET1_SENSOR_OFFSET        = ((VIDEO_ISP_SENSOR_SET1 << 4) | VIDEO_ISP_SENSOR_DATA),
	VIDEO_ISP_SET2_IQ_OFFSET            = ((VIDEO_ISP_SENSOR_SET2 << 4) | VIDEO_ISP_SENSOR_IQ),
	VIDEO_ISP_SET2_SENSOR_OFFSET        = ((VIDEO_ISP_SENSOR_SET2 << 4) | VIDEO_ISP_SENSOR_DATA),
	VIDEO_ISP_SET3_IQ_OFFSET            = ((VIDEO_ISP_SENSOR_SET3 << 4) | VIDEO_ISP_SENSOR_IQ),
	VIDEO_ISP_SET3_SENSOR_OFFSET        = ((VIDEO_ISP_SENSOR_SET3 << 4) | VIDEO_ISP_SENSOR_DATA),
	VIDEO_ISP_SET4_IQ_OFFSET            = ((VIDEO_ISP_SENSOR_SET4 << 4) | VIDEO_ISP_SENSOR_IQ),
	VIDEO_ISP_SET4_SENSOR_OFFSET        = ((VIDEO_ISP_SENSOR_SET4 << 4) | VIDEO_ISP_SENSOR_DATA),
	VIDEO_ISP_SET5_IQ_OFFSET            = ((VIDEO_ISP_SENSOR_SET5 << 4) | VIDEO_ISP_SENSOR_IQ),
	VIDEO_ISP_SET5_SENSOR_OFFSET        = ((VIDEO_ISP_SENSOR_SET5 << 4) | VIDEO_ISP_SENSOR_DATA),
	VIDEO_ISP_SET6_IQ_OFFSET            = ((VIDEO_ISP_SENSOR_SET6 << 4) | VIDEO_ISP_SENSOR_IQ),
	VIDEO_ISP_SET6_SENSOR_OFFSET        = ((VIDEO_ISP_SENSOR_SET6 << 4) | VIDEO_ISP_SENSOR_DATA),
	VIDEO_ISP_SET7_IQ_OFFSET            = ((VIDEO_ISP_SENSOR_SET7 << 4) | VIDEO_ISP_SENSOR_IQ),
	VIDEO_ISP_SET7_SENSOR_OFFSET        = ((VIDEO_ISP_SENSOR_SET7 << 4) | VIDEO_ISP_SENSOR_DATA),
	VIDEO_ISP_SET8_IQ_OFFSET            = ((VIDEO_ISP_SENSOR_SET8 << 4) | VIDEO_ISP_SENSOR_IQ),
	VIDEO_ISP_SET8_SENSOR_OFFSET        = ((VIDEO_ISP_SENSOR_SET8 << 4) | VIDEO_ISP_SENSOR_DATA),
	VIDEO_ISP_SET9_IQ_OFFSET            = ((VIDEO_ISP_SENSOR_SET9 << 4) | VIDEO_ISP_SENSOR_IQ),
	VIDEO_ISP_SET9_SENSOR_OFFSET        = ((VIDEO_ISP_SENSOR_SET9 << 4) | VIDEO_ISP_SENSOR_DATA),
	VIDEO_VOE_OFFSET                    = 0xF0,
} VIDEO_IMG_GET_OBJ_CTRL_t;

typedef struct video_reld_img_ctrl_info_s {
	uint8_t fwin;
	uint8_t enc_en;
	uint8_t resv[2];
	uint32_t data_start_offset;     // ref img manifest start
} video_reld_img_ctrl_info_t;

enum dbg_port_mode_e {
	DBG_PORT_OFF            = 0,    ///< debug port off
	SWD_MODE                = 1,    ///< debugger use SWD mode
	JTAG_MODE               = 2,    ///< debugger use JTAG mode
	DBG_PORT_USE_DEFAULT    = 3,    ///< debugger use same as rom code select
};
typedef uint8_t     dbg_port_mode_t;

enum dbg_port_pin_sel_e {
	TMS_IO_S0_CLK_S0   = 0,
	TMS_IO_S1_CLK_S0   = 1,
	TMS_IO_S0_CLK_S1   = 2,
	TMS_IO_S1_CLK_S1   = 3,
	TMS_IO_DF_CLK_DF   = 4,         ///< debugger use pins same as rom code select
};
typedef uint8_t     dbg_port_pin_sel_t;

enum ADC_VREF_SEL_e {
	VREF_SEL_0p75_V = 0x0,
	VREF_SEL_0p8_V  = 0x1,
	VREF_SEL_0p85_V = 0x2,
	VREF_SEL_0p9_V  = 0x3,
};

void hal_sys_peripheral_en(uint8_t id, uint8_t en);
void hal_sys_set_clk(uint8_t id, uint8_t sel_val);
uint32_t hal_sys_get_clk(uint8_t id);
uint8_t hal_sys_get_rom_ver(void);
uint8_t hal_sys_get_ld_fw_idx(void);
hal_status_t hal_sys_get_video_img_ld_offset(void *ctrl_obj_info, const uint8_t ctrl_obj);

#ifdef  __cplusplus
}
#endif

#endif  // end of "#define _HAL_SYS_CTRL_H_"
