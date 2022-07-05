/**************************************************************************//**
 * @file     fw_img_export.h
 * @brief    Define data format exported after boot.
 * @version  V1.00
 * @date     2022-07-08
 *
 * @note
 *
 ******************************************************************************
 *
 * Copyright(c) 2007 - 2022 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/
#if defined(CONFIG_BUILD_LIB) && (CONFIG_BUILD_LIB == 1)
#error ERROR, only for application
#endif

#ifndef _FW_IMG_EXPORT_H_
#define _FW_IMG_EXPORT_H_

#include "fw_snand_boot.h"
#include "fw_img_tlv.h"
#include "voe_boot_loader.h"

#ifdef  __cplusplus
extern "C"
{
#endif

enum user_load_fw_idx_e {
	USER_LOAD_FW_FOLLOW_DEFAULT = 0x0,
	USER_LOAD_FW1               = 0x1,
	USER_LOAD_FW2               = 0x2,
};

typedef struct fw_img_pre_boot_fail_sts_s {
	uint8_t img_preld_fail_chk_en;
	uint8_t img_preidx_ld_fail;
	uint8_t kc_preld_fail_bind_img_en;
	uint8_t resv;
} fw_img_pre_boot_fail_sts_t, *pfw_img_pre_boot_fail_sts_t;

typedef struct fw_img_manifest_ld_sel_sts_s {
	uint8_t valid;
	uint8_t resv[3];
	uint32_t timestamp;
	uint8_t version[IMG_MANIFEST_IE_VERSION_SIZE];
} fw_img_manifest_ld_sel_sts_t, *pfw_img_manifest_ld_sel_sts_t;

typedef struct fw_img_user_export_info_type_s {
	fw_img_manifest_ld_sel_sts_t fw1_ld_sel_info;
	fw_img_manifest_ld_sel_sts_t fw2_ld_sel_info;
	fw_img_pre_boot_fail_sts_t pre_boot_fail_sts;
} fw_img_user_export_info_type_t, *pfw_img_user_export_info_type_t;

typedef struct voe_ld_export_info_type_s {
	isp_multi_fcs_ld_info_t isp_multi_sensor_ld_info;
	voe_img_ld_info_type_t  voe_img_ld_info;
} voe_ld_export_info_type_t, *pvoe_ld_export_info_type_t;

typedef struct _BL4VOE_INFO_T_ {
	struct {
		uint8_t str_sign[BL4VOE_STR_SIGN_MAX_SIZE];
		uint32_t resv[4];
	} hdr;
	uint8_t data[BL4VOE_INFO_DATA_MAX_SIZE];
} BL4VOE_INFO_T;

typedef struct _BL4VOE_LOAD_INFO_T_ {
	struct {
		uint8_t str_sign[BL4VOE_STR_SIGN_MAX_SIZE];
		uint32_t resv[4];
	} hdr;
	voe_ld_export_info_type_t data;
} BL4VOE_LOAD_INFO_T;

#ifdef  __cplusplus
}
#endif

#endif  // end of "#define _FW_IMG_EXPORT_H_"