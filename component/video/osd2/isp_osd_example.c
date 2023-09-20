#include <platform_stdlib.h>
#include <platform_opts.h>
#include <build_info.h>
#include "log_service.h"
#include "atcmd_isp.h"
#include "main.h"
#include "flash_api.h"
#include "hal_osd_util.h"
#include "osd_custom.h"
#include "osd_pict_custom.h"
#include "osd_font_custom.h"
#include "osd_api.h"
#include "isp_osd_example.h"
#include "video_api.h"
#include "isp_ctrl_api.h"


static osd_text_info_st s_txt_info_time;
static osd_text_info_st s_txt_info_date;
static osd_text_info_st s_txt_info_string;
static osd_text_info_st s_txt_info_iq_string[6];
static char string_buf[6][64] = {0};
static char teststring[] = "RTK-AmebaPro2";
static char teststring_empty[] = " ";

void iq_update_info(void *arg)
{
	while (1) {
		unsigned char *iq_addr = video_get_iq_buf();
		int dvalue, dvalue2, dvalue3;
		int exp, gain;

		sprintf(string_buf[0], "IQ Version: %04d/%02d/%02d %02d:%02d:%02d", *(unsigned short *)(iq_addr + 12), iq_addr[14], iq_addr[15], iq_addr[16],
				iq_addr[17], *(unsigned short *)(iq_addr + 18));

		dvalue = -1;
		isp_get_exposure_time(&dvalue);
		exp = dvalue;
		dvalue2 = -1;
		isp_get_ae_gain(&dvalue2);
		gain = dvalue2;
		//sprintf(string_buf[1], "[AE]Exposure: %.3f AE-Gain: %.3f ET-Gain: %.3f", ((float)dvalue)/1000.0f, ((float)dvalue)/256.0f, ((float)(exp*gain))/25600.0f);
		sprintf(string_buf[1], "ET:%6d AEG:%4d", dvalue, dvalue2);

		dvalue = -1;
		isp_get_red_balance(&dvalue);
		dvalue2 = -1;
		isp_get_blue_balance(&dvalue2);
		dvalue3 = -1;
		isp_get_wb_temperature(&dvalue3);
		sprintf(string_buf[2], "R-Gain:%d B-Gain:%d CT:%d", dvalue, dvalue2, dvalue3);

		dvalue = -1;
		isp_get_day_night(&dvalue);
		sprintf(string_buf[3], "Mode:%d ETGain:%.2f", dvalue, ((float)(exp * gain)) / 25600.0f);

		for (int i = 0; i < 4; i++) {
			s_txt_info_iq_string[i].str = string_buf[i];
			rts_osd_update_info(rts_osd2_type_text, &s_txt_info_iq_string[i]);
			vTaskDelay(10);
		}
		vTaskDelay(250);
	}
}
static void init_osd_bitmap_pos(osd_pict_st *bmp_info, int chn_id, uint32_t start_x, uint32_t start_y, uint32_t width, uint32_t height)
{
	bmp_info->chn_id = chn_id;
	bmp_info->osd2.start_x = start_x;
	bmp_info->osd2.start_y = start_y;
	bmp_info->osd2.end_x = bmp_info->osd2.start_x + width;
	bmp_info->osd2.end_y = bmp_info->osd2.start_y + height;
}
static void init_osd_bitmap_blk(osd_pict_st *bmp_info, int blk_idx, enum rts_osd2_blk_fmt blk_fmt, uint32_t clr_1bpp)
{
	bmp_info->osd2.blk_idx = blk_idx;
	bmp_info->osd2.blk_fmt = blk_fmt;
	bmp_info->osd2.color_1bpp = clr_1bpp;//0xAABBGGRR
}
static void init_osd_bitmap_buf(osd_pict_st *bmp_info, uint8_t *buf, uint32_t buf_len)
{
	bmp_info->osd2.buf = buf;
	bmp_info->osd2.len = buf_len;
}

static void init_osd_txt(osd_text_info_st *txt_info, int chn_id, int blk_idx, rt_font_st font, uint32_t rotate, uint32_t start_x, uint32_t start_y, char *str)
{
	txt_info->chn_id = chn_id;
	txt_info->font = font;
	txt_info->blk_idx = blk_idx;
	txt_info->rotate = rotate;
	txt_info->start_x = start_x;
	txt_info->start_y = start_y;
	txt_info->str = str;
}

static rt_font_st font = {
	.bg_enable		= OSD_TEXT_FONT_BG_ENABLE,
	.bg_color		= OSD_TEXT_FONT_BG_COLOR,
	.ch_color		= OSD_TEXT_FONT_CH_COLOR,
	.block_alpha	= OSD_TEXT_FONT_BLOCK_ALPHA,
	.h_gap			= OSD_TEXT_FONT_H_GAP,
	.v_gap			= OSD_TEXT_FONT_V_GAP,
	.date_fmt		= osd_date_fmt_9,
	.time_fmt		= osd_time_fmt_24,
};

extern void rts_osd_task(void *arg);
static osd_pict_st posd2_pic_0, posd2_pic_1, posd2_pic_2;
void set_info(int ch_id, int txt_w, int txt_h)
{
	int ch = ch_id;
	printf("Text/Logo OSD Test\r\n");

	font.osd_char_w		= txt_w;
	font.osd_char_h		= txt_h;
	printf("[osd] Heap available:%d\r\n", xPortGetFreeHeapSize());

	init_osd_txt(&s_txt_info_time, ch, 0, font, OSD_TEXT_ROTATE, 10 + 320 + 50, 10, 0);
	init_osd_txt(&s_txt_info_date, ch, 1, font, OSD_TEXT_ROTATE, 10, 10, 0);
	init_osd_txt(&s_txt_info_string, ch, 5, font, RT_ROTATE_90L, 10, 10 + 100, teststring);

	init_osd_bitmap_pos(&posd2_pic_0, ch, 150, 200, PICT0_WIDTH, PICT0_HEIGHT);
	init_osd_bitmap_blk(&posd2_pic_0, 2, PICT0_BLK_FMT, 0);
	init_osd_bitmap_buf(&posd2_pic_0, PICT0_NAME, PICT0_SIZE);

	init_osd_bitmap_pos(&posd2_pic_1, ch, 150 + PICT0_WIDTH + 50, 200, PICT1_WIDTH, PICT1_HEIGHT);
	init_osd_bitmap_blk(&posd2_pic_1, 3, PICT1_BLK_FMT, 0);
	init_osd_bitmap_buf(&posd2_pic_1, PICT1_NAME, PICT1_SIZE);

	init_osd_bitmap_pos(&posd2_pic_2, ch, 150 + PICT0_WIDTH + 50 + PICT1_WIDTH + 50, 200, PICT2_WIDTH, PICT2_HEIGHT);
	init_osd_bitmap_blk(&posd2_pic_2, 4, PICT2_BLK_FMT, 0x000000FF);
	init_osd_bitmap_buf(&posd2_pic_2, PICT2_NAME, PICT2_SIZE);

	rts_osd_set_info(rts_osd2_type_date, &s_txt_info_date);
	rts_osd_set_info(rts_osd2_type_time, &s_txt_info_time);
	rts_osd_set_info(rts_osd2_type_pict, &posd2_pic_0);
	rts_osd_set_info(rts_osd2_type_pict, &posd2_pic_1);
	rts_osd_set_info(rts_osd2_type_pict, &posd2_pic_2);
	rts_osd_set_info(rts_osd2_type_text, &s_txt_info_string);
}
static const int ach_id[5] = {0, 1, 2, 3, 4};
void example_isp_osd(int idx, int ch_id, int txt_w, int txt_h)
{
	int ch = ch_id;
	printf("Text/Logo OSD Test\r\n");

	font.osd_char_w		= txt_w;
	font.osd_char_h		= txt_h;
	if (idx == 0) {

		printf("[osd] Heap available:%d\r\n", xPortGetFreeHeapSize());
		rts_osd_init(ch_id, txt_w, txt_h, (int)(8.0f * 3600));

		if (ch == 0) {
			init_osd_txt(&s_txt_info_time, ch, 0, font, OSD_TEXT_ROTATE, 10 + 320 + 50, 10, 0);
			init_osd_txt(&s_txt_info_date, ch, 1, font, OSD_TEXT_ROTATE, 10, 10, 0);
			init_osd_txt(&s_txt_info_string, ch, 5, font, RT_ROTATE_90L, 10, 10 + 100, teststring);

			init_osd_bitmap_pos(&posd2_pic_0, ch, 150, 200, PICT0_WIDTH, PICT0_HEIGHT);
			init_osd_bitmap_pos(&posd2_pic_1, ch, 150 + PICT0_WIDTH + 50, 200, PICT1_WIDTH, PICT1_HEIGHT);
			init_osd_bitmap_pos(&posd2_pic_2, ch, 150 + PICT0_WIDTH + 50 + PICT1_WIDTH + 50, 200, PICT2_WIDTH, PICT2_HEIGHT);
		} else if (ch == 1) {
			//rts_set_font_char_size(ch_id, txt_w, txt_h, eng_bin_custom, chi_bin_custom);
			init_osd_txt(&s_txt_info_time, ch, 0, font, OSD_TEXT_ROTATE, 10, 10, 0);
			init_osd_txt(&s_txt_info_date, ch, 1, font, OSD_TEXT_ROTATE, 10 + 320 + 50, 10, 0);
			init_osd_txt(&s_txt_info_string, ch, 5, font, RT_ROTATE_90R, 10, 10 + 100, teststring);

			init_osd_bitmap_pos(&posd2_pic_0, ch, 150 + PICT0_WIDTH + 50 + PICT1_WIDTH + 50, 200, PICT0_WIDTH, PICT0_HEIGHT);
			init_osd_bitmap_pos(&posd2_pic_1, ch, 150, 300, PICT1_WIDTH, PICT1_HEIGHT);
			init_osd_bitmap_pos(&posd2_pic_2, ch, 150 + PICT0_WIDTH + 50, 400, PICT2_WIDTH, PICT2_HEIGHT);
		} else if (ch == 2) {
			init_osd_txt(&s_txt_info_time, ch, 0, font, OSD_TEXT_ROTATE, 10, 10, 0);
			init_osd_txt(&s_txt_info_date, ch, 1, font, OSD_TEXT_ROTATE, 10 + 320 + 50, 10, 0);
			init_osd_txt(&s_txt_info_string, ch, 5, font, RT_ROTATE_90R, 10, 10 + 100, teststring);

			init_osd_bitmap_pos(&posd2_pic_0, ch, 150 + PICT0_WIDTH + 50, 200, PICT0_WIDTH, PICT0_HEIGHT);
			init_osd_bitmap_pos(&posd2_pic_1, ch, 150 + PICT0_WIDTH + 50 + PICT1_WIDTH + 50, 350, PICT1_WIDTH, PICT1_HEIGHT);
			init_osd_bitmap_pos(&posd2_pic_2, ch, 150, 450, PICT2_WIDTH, PICT2_HEIGHT);
		}
		init_osd_bitmap_blk(&posd2_pic_0, 2, PICT0_BLK_FMT, 0);
		init_osd_bitmap_buf(&posd2_pic_0, PICT0_NAME, PICT0_SIZE);
		init_osd_bitmap_blk(&posd2_pic_1, 3, PICT1_BLK_FMT, 0);
		init_osd_bitmap_buf(&posd2_pic_1, PICT1_NAME, PICT1_SIZE);
		init_osd_bitmap_blk(&posd2_pic_2, 4, PICT2_BLK_FMT, 0x000000FF);
		init_osd_bitmap_buf(&posd2_pic_2, PICT2_NAME, PICT2_SIZE);

		rts_osd_set_info(rts_osd2_type_date, &s_txt_info_date);
		rts_osd_set_info(rts_osd2_type_time, &s_txt_info_time);
		rts_osd_set_info(rts_osd2_type_pict, &posd2_pic_0);
		rts_osd_set_info(rts_osd2_type_pict, &posd2_pic_1);
		rts_osd_set_info(rts_osd2_type_pict, &posd2_pic_2);
		rts_osd_set_info(rts_osd2_type_text, &s_txt_info_string);

		printf("[osd] Heap available:%d\r\n", xPortGetFreeHeapSize());

		if (xTaskCreate(rts_osd_task, "OSD", 10 * 1024, (void *)(ach_id + ch), tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
			printf("\n\r%s xTaskCreate failed", __FUNCTION__);
		}
	} else if (idx == 1) {
		hal_video_print(0);
		printf("[osd] Heap available:%d\r\n", xPortGetFreeHeapSize());
		rts_osd_init(ch_id, txt_w, txt_h, (int)(8.0f * 3600));

		for (int i = 0; i < 6; i++) {
			init_osd_txt(s_txt_info_iq_string + i, ch, i, font, RT_ROTATE_0, 10, 10 + 10 + (txt_h + 5) * i, teststring_empty);
		}

		rts_osd_set_info(rts_osd2_type_text, &s_txt_info_iq_string[0]);
		rts_osd_set_info(rts_osd2_type_text, &s_txt_info_iq_string[1]);
		rts_osd_set_info(rts_osd2_type_text, &s_txt_info_iq_string[2]);
		rts_osd_set_info(rts_osd2_type_text, &s_txt_info_iq_string[3]);
		//rts_osd_set_info(rts_osd2_type_text, &s_txt_info_iq_string[4]);
		//rts_osd_set_info(rts_osd2_type_text, &s_txt_info_iq_string[5]);

		printf("[osd] Heap available:%d\r\n", xPortGetFreeHeapSize());

		if (xTaskCreate(rts_osd_task, "OSD", 10 * 1024, (void *)(ach_id + ch), tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
			printf("\n\r%s xTaskCreate failed", __FUNCTION__);
		}
		if (xTaskCreate(iq_update_info, "osd_iq_update", 10 * 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
			printf("\n\r%s xTaskCreate failed", __FUNCTION__);
		}
	}
}