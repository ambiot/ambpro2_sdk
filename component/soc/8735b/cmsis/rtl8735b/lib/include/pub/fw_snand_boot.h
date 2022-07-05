/**************************************************************************//**
 * @file     fw_snand_boot.h
 * @brief    Declare the booting from nand flash.
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

#ifndef _FW_SNAND_BOOT_H_
#define _FW_SNAND_BOOT_H_

#include "cmsis.h"
#include "rtl8735b_ramstart.h"
#include "fw_img_tlv.h"

#define NAND_PAGE_MAX_LEN           (2 * 0x840)

/// Fixed ctrl info start blk
#define NAND_CTRL_INFO_START_BLK    0

/// Max count of virtual block map
#define SNAND_VMAP_MAX              48

/// NAND flash page data size in byte
#define NAND_PAGE_LEN               ((const u32)NAND_CTRL_INFO.page_size)

/// Total block count in flash
#define NAND_BLK_CNT                ((const u32)NAND_CTRL_INFO.blk_cnt)

/// Get offset of address inside page
#define NAND_PAGE_OFST(addr)        ((addr) & NAND_CTRL_INFO.page_mask)

/// Convert address to page
#define NAND_ADDR2PAGE(addr)        ((addr) >> NAND_CTRL_INFO.addr2page_shift)

typedef u16 snand_vblk_idx_t;

typedef struct snand_raw_ctrl_info {
	u32 blk_cnt;
	u32 page_per_blk;
	u32 page_size;
	u32 spare_size;
	u32 vendor_info[2];
	u32 bbt_start;
	u32 bbt_dup_cnt;
	u32 par_tbl_start;
	u32 par_tbl_dup_cnt;
	u16 vrf_alg;            // verify image algorithm selection
	u16 rsvd1;
	u32 rsvd2;
	u32 magic_num;
	u32 rsvd3[11];
} snand_raw_ctrl_info_t;

typedef struct snand_ctrl_info {
	u32 blk_cnt;
	u32 page_per_blk;
	u32 page_size;
	u32 spare_size;
	u32 page_mask;
	u32 blk_mask;
	u8 page2blk_shift;
	u8 addr2page_shift;
	u8 cache_rsvd[6];
	// cache line size
	snand_raw_ctrl_info_t raw;
	// Following data is not present in < C cut
	u32 ctrl_info_blk_idx;
	u32 part_tbl_blk_idx;
	u32 rsvd2[14];
} snand_ctrl_info_t;

// NAND Flash Physical Address
typedef struct snand_addr {
	u32 page;
	u16 col;
} snand_addr_t;

// NAND Flash virtual map info
typedef struct snand_vmap {
	// For map_size > SNAND_VMAP_MAX, need rotation
	u16 map_size;
	u16 cur_rec_idx;
	snand_addr_t vmap0_addr;

	// virtual physical block mapping
	snand_vblk_idx_t map[SNAND_VMAP_MAX];
} snand_vmap_t;

// NAND Flash Address with virtual map (virtual address)
typedef struct snand_vaddr {
	u32 page;
	u16 col;
	snand_vmap_t *vmap;
} snand_vaddr_t;

// Partition record raw format on Flash
typedef struct snand_part_record {
	u32 magic_num;
	u16 type_id;
	u16 blk_cnt;
	u16 serno;

	u8 rsvd2[22];
	snand_vblk_idx_t vblk[SNAND_VMAP_MAX];
} snand_part_record_t;

// Partition Table Info on RAM
typedef struct snand_part_entry {
	u16 type_id;
	u16 blk_cnt;
	snand_addr_t vmap_addr;
	u32 rsvd[6];
} snand_part_entry_t;

// Partition Table on RAM
typedef struct snand_partition_tbl {
	part_fst_info_t mfst;
	snand_part_entry_t entrys[PARTITION_RECORD_MAX];
} snand_partition_tbl_t;

// Stubs for NAND Flash Boot
typedef struct hal_snand_boot_stubs {
	u8 *rom_snand_boot;
	snand_ctrl_info_t *ctrl_info;
	snand_partition_tbl_t *part_tbl;
	hal_status_t (*snand_memcpy_update)(hal_snafc_adaptor_t *snafc_adpt, void *dest, snand_addr_t *snand_addr, u32 size);
	hal_status_t (*snand_memcpy)(hal_snafc_adaptor_t *snafc_adpt, void *dest, const snand_addr_t *snand_addr, u32 size);
	void (*snand_offset)(snand_addr_t *snand_addr, u32 offset);
	hal_status_t (*snand_vmemcpy_update)(hal_snafc_adaptor_t *snafc_adpt, void *dest, snand_vaddr_t *snand_addr, u32 size);
	hal_status_t (*snand_vmemcpy)(hal_snafc_adaptor_t *snafc_adpt, void *dest, snand_vaddr_t *snand_addr, u32 size);
	void (*snand_voffset)(snand_vaddr_t *snand_addr, u32 offset);
	s32(*snand_img_sel)(snand_partition_tbl_t *part_tbl, const u8 img_obj, const u8 img_sel_op);
	u32 rsvd[38];
} hal_snand_boot_stubs_t;

extern const hal_snand_boot_stubs_t hal_snand_boot_stubs;

#define NAND_CTRL_INFO              snand_ctrl_info
extern snand_ctrl_info_t            snand_ctrl_info;

#endif  // end of "#define _FW_SNAND_BOOT_H_"