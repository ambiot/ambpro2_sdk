#include "fw_img_export.h"
#include "hal_spic.h"

uint8_t user_boot_fw_selection(fw_img_user_export_info_type_t *pfw_img_user_export_info)
{
	uint8_t fw_ld_idx = USER_LOAD_FW_FOLLOW_DEFAULT;
	return fw_ld_idx;
}


void spic_user_select(uint8_t *pspic_bit_mode)
{
	*pspic_bit_mode = SPIC_BIT_MODE_SETTING;
}

