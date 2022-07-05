#include "hal_osd_util.h"
#include "osd_custom.h"
#include "osd_api.h"
#include "nn_osd_draw.h"
#include "osdep_service.h"

static SemaphoreHandle_t nn_osd_sema = NULL;
static int nn_rect_txt_w = 16;
static int nn_rect_txt_h = 32;
static nn_osd_ctx_t nn_result;
static unsigned char block_occupied[OSD_OBJ_TOTAL_MAX_NUM] = {0, 0, 0, 0, 0, 0,
															  0, 0, 0, 0, 0, 0,
															  0, 0, 0, 0, 0, 0,
															 };

extern void osd_hide_bitmap(int ch, rt_osd2_info_st *posd2_pic);

int nn_osd_get_status(void)
{
	return nn_result.nn_osd_ready2draw;
}

int nn_osd_update(void)
{
	if (nn_osd_sema) {
		xSemaphoreGive(nn_osd_sema);
		return 1;
	}
	return 0;
}

int nn_osd_set_rect(int idx, int ch, nn_osd_rect_t *rect)
{
	if (idx >= OSD_OBJ_MAX_NUM) {
		printf("Invalid id!\n\r");
		return 0;
	}

	if (!nn_result.nn_osd_ready2draw) {
		printf("OSD not ready!\n\r");
		return 0;
	}
	idx = ch * OSD_OBJ_MAX_NUM + idx;
	if (rect) {
		//printf("set ch %d!\n\r", nn_result.obj[idx].ch);
		nn_result.obj[idx].draw_status = DRAW_RECT_ONLY;
		memcpy(&nn_result.obj[idx].rect, rect, sizeof(nn_osd_rect_t));
		return 1;
	} else {
		nn_result.obj[idx].draw_status = DRAW_NOTHING;
		return 0;
	}
}

int nn_osd_set_rect_with_text(int idx, int ch, nn_osd_text_t *text, nn_osd_rect_t *rect)
{
	if (idx >= OSD_OBJ_MAX_NUM) {
		printf("Invalid id!\n\r");
		return 0;
	}

	if (!nn_result.nn_osd_ready2draw) {
		printf("OSD not ready!\n\r");
		return 0;
	}

	idx = ch * OSD_OBJ_MAX_NUM + idx;
	if (text && rect) {
		nn_result.obj[idx].draw_status = DRAW_RECT_AND_TEXT;
	} else {
		nn_result.obj[idx].draw_status = DRAW_NOTHING;
		return 0;
	}

	if (text) {
		memcpy(&nn_result.obj[idx].text, text, sizeof(nn_osd_text_t));
	}

	if (rect) {
		memcpy(&nn_result.obj[idx].rect, rect, sizeof(nn_osd_rect_t));
	}

	return 1;
}


int nn_osd_set_point(int idx, int ch, int point_num, nn_osd_pt_t *pt)
{
	if (!nn_result.nn_osd_ready2draw) {
		printf("OSD not ready!\n\r");
		return 0;
	}
	idx = ch * OSD_OBJ_MAX_NUM + idx;
	if (pt && nn_result.obj[idx].draw_status != DRAW_NOTHING) {
		nn_result.obj[idx].point_num = point_num;
		printf("ptnum = %d\r\n", point_num);
		for (int i = 0; i < point_num; i++) {
			nn_result.obj[idx].pt[i].x = (pt[i].x + 7) & (~7);
			nn_result.obj[idx].pt[i].y = (pt[i].y + 7) & (~7);
			nn_result.obj[idx].pt[i].pt_width = pt[i].pt_width;
			nn_result.obj[idx].pt[i].color.r = pt[i].color.r;
			nn_result.obj[idx].pt[i].color.g = pt[i].color.g;
			nn_result.obj[idx].pt[i].color.b = pt[i].color.b;
		}
		return 1;
	}

	return 0;
}

int nn_osd_clear_bitmap(int idx, int ch)
{
	if (!nn_result.nn_osd_ready2draw) {
		printf("OSD not ready!\n\r");
		return 0;
	}
	idx = ch * OSD_OBJ_MAX_NUM + idx;
	nn_result.obj[idx].draw_status = DRAW_NOTHING;
	return 1;
}

int nn_osd_block_occupied(int idx, int ch)
{
	idx = ch * OSD_OBJ_MAX_NUM + idx;
	block_occupied[idx] = 1;
	return 1;
}

static void draw_point_on_bitmap(unsigned char *bitmap, int width, int height, nn_osd_pt_t *pt)
{

	int clr = 0xC0 + (pt->color.b >> 6) * 16 + (pt->color.g >> 6) * 4 + (pt->color.r >> 6);
	int pt_w = pt->pt_width;
	int pt_xmin = pt->x - pt_w / 2 < 0 ? 0 : pt->x - pt_w / 2;
	int pt_xmax = pt->x + pt_w / 2 < width ? pt->x + pt_w / 2 : width;
	int pt_ymin = pt->y - pt_w / 2 < 0 ? 0 : pt->y - pt_w / 2;
	int pt_ymax = pt->y + pt_w / 2 < height ? pt->y + pt_w / 2 : height;
	for (int j = pt_ymin; j < pt_ymax; j++) {
		for (int k = pt_xmin; k < pt_xmax; k++) {
			bitmap[j * width + k] = clr;
		}
	}
}

static void switch_point(nn_osd_pt_t *pt1, nn_osd_pt_t *pt2)
{
	nn_osd_pt_t pt = *pt1;
	*pt1 = *pt2;
	*pt2 = pt;
}

static void draw_line_on_bitmap(unsigned char *bitmap, int width, int height, nn_osd_pt_t *pt1, nn_osd_pt_t *pt2)
{
	nn_osd_pt_t pt = *pt1;
	int clr = 0xC0 + (pt.color.b >> 6) * 16 + (pt.color.g >> 6) * 4 + (pt.color.r >> 6);
	if (pt2->y == pt1->y) { //horizontal line
		if (pt1->x > pt2->x) {
			switch_point(pt1, pt2);
		}
		for (int i = pt1->x; i < pt2->x; i++) {
			pt.y = pt1->y;
			pt.x = i;
			int pt_ymin = pt.y - pt.pt_width / 2 < 0 ? 0 : pt.y - pt.pt_width / 2;
			int pt_ymax = pt.y + pt.pt_width / 2 < height ? pt.y + pt.pt_width / 2 : height;
			for (int j = pt_ymin; j < pt_ymax; j++) {
				bitmap[j * width + i] = clr;
			}
		}
	} else if (pt1->x == pt2->x) { //vertical line
		if (pt1->y > pt2->y) {
			switch_point(pt1, pt2);
		}
		for (int i = pt1->y; i < pt2->y; i++) {
			pt.x = pt1->x;
			pt.y = i;
			int pt_xmin = pt.x - pt.pt_width / 2 < 0 ? 0 : pt.x - pt.pt_width / 2;
			int pt_xmax = pt.x + pt.pt_width / 2 < height ? pt.x + pt.pt_width / 2 : width;
			for (int j = pt_xmin; j < pt_xmax; j++) {
				bitmap[i * width + j] = clr;
			}
		}
	} else if (abs(pt2->y - pt1->y) > abs(pt2->x - pt1->x)) { //y step lager tha x step
		if (pt1->y > pt2->y) {
			switch_point(pt1, pt2);
		}
		for (int i = pt1->y; i < pt2->y; i++) {
			pt.x = (pt2->x - pt1->x) * (i - pt1->y) / (pt2->y - pt1->y) + pt1->x;
			pt.y = i;
			int pt_xmin = pt.x - pt.pt_width / 2 < 0 ? 0 : pt.x - pt.pt_width / 2;
			int pt_xmax = pt.x + pt.pt_width / 2 < height ? pt.x + pt.pt_width / 2 : width;
			for (int j = pt_xmin; j < pt_xmax; j++) {
				bitmap[i * width + j] = clr;
			}
		}
	} else { //x step lager tha y step
		if (pt1->x > pt2->x) {
			switch_point(pt1, pt2);
		}
		for (int i = pt1->x; i < pt2->x; i++) {
			pt.x = i;
			pt.y = (pt2->y - pt1->y) * (i - pt1->x) / (pt2->x - pt1->x) + pt1->y;
			int pt_ymin = pt.y - pt.pt_width / 2 < 0 ? 0 : pt.y - pt.pt_width / 2;
			int pt_ymax = pt.y + pt.pt_width / 2 < height ? pt.y + pt.pt_width / 2 : height;
			for (int j = pt_ymin; j < pt_ymax; j++) {
				bitmap[j * width + i] = clr;
			}
		}
	}

}

void nn_osd_task(void *arg)
{
	//osd_ctx_t nn_result;
	BITMAP_S tag_bmp;
	enum rts_osd2_blk_fmt disp_format = RTS_OSD2_BLK_FMT_RGBA2222;
	osd_pict_st osd2_pic[OSD_OBJ_TOTAL_MAX_NUM];
	int nn_rect_init_ch = 0;
	int nn_rect_txt_w_l = nn_rect_txt_w;
	int nn_rect_txt_h_l = nn_rect_txt_h;
	int rect_w[OSD_OBJ_TOTAL_MAX_NUM] = {0};
	int rect_h[OSD_OBJ_TOTAL_MAX_NUM] = {0};
	int pic_w[OSD_OBJ_TOTAL_MAX_NUM] = {0};
	int pic_h[OSD_OBJ_TOTAL_MAX_NUM] = {0};
	int *pd1[OSD_OBJ_TOTAL_MAX_NUM] = {0};
	int *pd2[OSD_OBJ_TOTAL_MAX_NUM] = {0};
	int **pd[2] = {pd1, pd2};
	struct osd_rect_info_st rect_info;
	//int tag_max_num = 20;
	int count = 999;
#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	rtw_create_secure_context(2048);
#endif

	for (int i = 0; i < 3; i++) {
		if (nn_result.nn_osd_channel_en[i]) {
			printf("NN OSD Draw start %d \r\n", i);
			//hal_video_osd_enc_enable(i, 1);
			rts_osd_init(i, nn_rect_txt_w, nn_rect_txt_h, (int)(8.0f * 3600));
			rts_osd_release_init_protect();
			nn_rect_init_ch = i;
		} else {
			for (int j = 0; j < OSD_OBJ_MAX_NUM; j++) {
				block_occupied[i * OSD_OBJ_MAX_NUM + j] = 1;
			}
		}
	}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < OSD_OBJ_MAX_NUM; j++) {
			int idx = i * OSD_OBJ_MAX_NUM + j;
			if (!block_occupied[idx]) {
				osd2_pic[idx].chn_id = i;
				osd2_pic[idx].osd2.blk_idx = j;
				osd2_pic[idx].osd2.blk_fmt = disp_format;
				rts_osd_set_info(rts_osd2_type_pict, &osd2_pic[idx]);
			}
		}

	}

	tag_bmp.pData = malloc(((OSD_TXT_MAX_NUM * nn_rect_txt_w_l + 7) / 8) * 8 * nn_rect_txt_h_l);
	if (tag_bmp.pData == NULL) {
		printf("Create tag bitmap failed\r\n");
		return;
	}

	rect_info.type = 0;
	rect_info.format = disp_format;
	rect_info.line_width = 3;
	rect_info.rect_clr_r = 0;
	rect_info.rect_clr_g = 255;
	rect_info.rect_clr_b = 255;
	rect_info.txt_clr_r = 255;
	rect_info.txt_clr_g = 255;
	rect_info.txt_clr_b = 0;

	nn_result.nn_osd_ready2draw = 1;
	while (1) {
		//if (nn_data_queue && xQueueReceive(nn_data_queue, &nn_result, portMAX_DELAY) == pdTRUE) {
		if (xSemaphoreTake(nn_osd_sema, portMAX_DELAY) == pdTRUE) {
			unsigned long tick1 = xTaskGetTickCount();
			nn_result.nn_osd_ready2draw = 0;

			int **pd_pp = pd[count % 2];
			count--;
			if (count == 1) {
				count = 999;
			}

			for (int i = 0; i < OSD_OBJ_TOTAL_MAX_NUM; i++) {
				//if(nn_result.nn_osd_channel_en[i]){
				if (!block_occupied[i]) {
					if (nn_result.obj[i].draw_status == DRAW_RECT_ONLY) {
						nn_result.obj[i].rect.rect.xmin = nn_result.obj[i].rect.rect.xmin & (~7);
						nn_result.obj[i].rect.rect.xmax = nn_result.obj[i].rect.rect.xmax & (~7);
						nn_result.obj[i].rect.rect.ymin = nn_result.obj[i].rect.rect.ymin & (~7);
						nn_result.obj[i].rect.rect.ymax = nn_result.obj[i].rect.rect.ymax & (~7);
						pic_w[i] = nn_result.obj[i].rect.rect.xmax - nn_result.obj[i].rect.rect.xmin;
						pic_h[i] = nn_result.obj[i].rect.rect.ymax - nn_result.obj[i].rect.rect.ymin;

						//int align_w = (pic_w[i] + 7) & (~7);
						rect_w[i] = pic_w[i];
						rect_h[i] = pic_h[i];
						if (rect_h[i] < 0) {
							rect_h[i] = 0;
						}

						if (pd_pp[i]) {
							free(pd_pp[i]);
						}
						pd_pp[i] = malloc(pic_w[i] * (pic_h[i] + 8));
						if (pd_pp[i]) {
							rect_info.rect_w = rect_w[i];
							rect_info.rect_h = rect_h[i];

							rect_info.line_width = nn_result.obj[i].rect.line_width;
							rect_info.rect_clr_r = nn_result.obj[i].rect.color.r;
							rect_info.rect_clr_g = nn_result.obj[i].rect.color.g;
							rect_info.rect_clr_b = nn_result.obj[i].rect.color.b;

							rts_osd_rect_gen(pd_pp[i], pic_w[i], pic_h[i], rect_info, 1);

							osd2_pic[i].chn_id = nn_result.obj[i].ch;
							osd2_pic[i].osd2.len = pic_w[i] * pic_h[i];
							osd2_pic[i].osd2.start_x = nn_result.obj[i].rect.rect.xmin;
							osd2_pic[i].osd2.start_y = nn_result.obj[i].rect.rect.ymin;
							osd2_pic[i].osd2.end_x = osd2_pic[i].osd2.start_x + pic_w[i];
							osd2_pic[i].osd2.end_y = osd2_pic[i].osd2.start_y + pic_h[i];
						}
					} else if (nn_result.obj[i].draw_status == DRAW_RECT_AND_TEXT) { //rect & text
						//check obj coordinate
						nn_result.obj[i].rect.rect.xmin = nn_result.obj[i].rect.rect.xmin & (~7);
						nn_result.obj[i].rect.rect.xmax = nn_result.obj[i].rect.rect.xmax & (~7);
						nn_result.obj[i].rect.rect.ymin = (nn_result.obj[i].rect.rect.ymin & (~7)) - nn_rect_txt_h_l;
						if (nn_result.obj[i].rect.rect.ymin < 0) {
							nn_result.obj[i].rect.rect.ymin = 0;
						}
						nn_result.obj[i].rect.rect.ymax = nn_result.obj[i].rect.rect.ymax & (~7);

						pic_w[i] = nn_result.obj[i].rect.rect.xmax - nn_result.obj[i].rect.rect.xmin;
						pic_h[i] = nn_result.obj[i].rect.rect.ymax - nn_result.obj[i].rect.rect.ymin;

						//printf("osd draw ch = %d\r\n",nn_result.obj[i].ch);
						//printf("osd draw w = %d, h = %d\r\n",nn_result.nn_osd_channel_xmax[nn_result.obj[i].ch], nn_result.nn_osd_channel_ymax[nn_result.obj[i].ch]);

						//printf("osd num  = %d, class = %s, (l,t,r,b) = (%d, %d, %d, %d)\r\n", i, nn_result.obj[i].text.text_str, nn_result.obj[i].rect.rect.xmin, nn_result.obj[i].rect.rect.ymin,
						//	nn_result.obj[i].rect.rect.xmax, nn_result.obj[i].rect.rect.ymax);
						//text length longer than pic width
						int text_len = strlen(nn_result.obj[i].text.text_str);
						if (text_len * nn_rect_txt_w_l > pic_w[i]) {
							pic_w[i] = text_len * nn_rect_txt_w_l;
							if ((nn_result.obj[i].rect.rect.xmin + pic_w[i]) >= (nn_result.nn_osd_channel_xmax[nn_result.obj[i].ch] - 1)) {
								//printf("\r\nNN OSD: text string len exceed x limit\r\n");
								pic_w[i] = (nn_result.nn_osd_channel_xmax[nn_result.obj[i].ch] - 1 - nn_result.obj[i].rect.rect.xmin) & (~7);
							}
						}
						int align_w = (pic_w[i] + 7) & (~7);
						rect_w[i] = nn_result.obj[i].rect.rect.xmax - nn_result.obj[i].rect.rect.xmin;
						rect_h[i] = pic_h[i] - nn_rect_txt_h_l;
						if (rect_w[i] > pic_w[i]) {
							rect_w[i] = pic_w[i];
						}
						if (rect_h[i] < 0) {
							rect_h[i] = 0;
							pic_h[i] = nn_rect_txt_h_l;
						}

						//gen text bitmap
						if (tag_bmp.pData) {
							rect_info.txt_clr_r = nn_result.obj[i].text.color.r;
							rect_info.txt_clr_g = nn_result.obj[i].text.color.g;
							rect_info.txt_clr_b = nn_result.obj[i].text.color.b;

							osd_text2bmp(nn_result.obj[i].text.text_str, &tag_bmp, nn_result.obj[i].ch, nn_result.obj[i].ch);
						}
						//gen rect with text
						if (pd_pp[i]) {
							free(pd_pp[i]);
						}
						pd_pp[i] = (int *)malloc(align_w * (pic_h[i] + 8));
						if (pd_pp[i]) {
							rect_info.rect_w = rect_w[i];
							rect_info.rect_h = rect_h[i];

							rect_info.line_width = nn_result.obj[i].rect.line_width;
							rect_info.rect_clr_r = nn_result.obj[i].rect.color.r;
							rect_info.rect_clr_g = nn_result.obj[i].rect.color.g;
							rect_info.rect_clr_b = nn_result.obj[i].rect.color.b;

							rts_osd_rect_gen_with_txt(&tag_bmp, (void *)pd_pp[i], pic_w[i], pic_h[i], rect_info, 1);

							osd2_pic[i].chn_id = nn_result.obj[i].ch;
							osd2_pic[i].osd2.len = align_w * pic_h[i];
							osd2_pic[i].osd2.start_x = nn_result.obj[i].rect.rect.xmin;
							osd2_pic[i].osd2.start_y = nn_result.obj[i].rect.rect.ymin;
							osd2_pic[i].osd2.end_x = osd2_pic[i].osd2.start_x + pic_w[i];
							osd2_pic[i].osd2.end_y = osd2_pic[i].osd2.start_y + pic_h[i];
							if (nn_result.obj[i].point_num) {
								for (int j = 0; j < nn_result.obj[i].point_num; j++) {
									nn_result.obj[i].pt[j].x -= osd2_pic[i].osd2.start_x;
									nn_result.obj[i].pt[j].y -= osd2_pic[i].osd2.start_y;
									draw_point_on_bitmap((unsigned char *)pd_pp[i], pic_w[i], pic_h[i], &nn_result.obj[i].pt[j]);
									//printf("pt[%d] (%d,%d)\r\n", j, nn_result.obj[i].pt[j].xmin, nn_result.obj[i].pt[j].ymin);

									//if(j > 0){
									//	draw_line_on_bitmap(pd_pp[i], pic_w[i], pic_h[i], &nn_result.obj[i].pt[j], &nn_result.obj[i].pt[j-1]);
									//}
								}
							}
						}
					}
					//if no text and rect to draw
					if (nn_result.obj[i].draw_status == DRAW_NOTHING) {
						if (pd_pp[i]) {
							free(pd_pp[i]);
						}
						pd_pp[i] = (int *)malloc(8 * 8);
						if (pd_pp[i]) {
							osd2_pic[i].chn_id = nn_result.obj[i].ch;
							osd2_pic[i].osd2.len = 8 * 8;
							osd2_pic[i].osd2.start_x = 0;
							osd2_pic[i].osd2.start_y = 0;
							osd2_pic[i].osd2.end_x = 8;
							osd2_pic[i].osd2.end_y = 8;
							memset(pd_pp[i], 0, osd2_pic[i].osd2.len);
						}
					}

				}
			}

			//update osd
			for (int i = 0; i < OSD_OBJ_TOTAL_MAX_NUM; i++) {
				if (!block_occupied[i]) {
					if (pd_pp[i]) {
						osd2_pic[i].osd2.buf = (uint8_t *)pd_pp[i];
					}
					if (i % OSD_OBJ_MAX_NUM == 5) {
						rts_osd_bitmap_update(nn_result.obj[i].ch, &osd2_pic[i].osd2, 0);
					} else {
						rts_osd_bitmap_update(nn_result.obj[i].ch, &osd2_pic[i].osd2, 1);
					}
				}
			}
			//printf("\r\nOSD draw after %dms.\n", (xTaskGetTickCount() - tick1));

		}
		nn_result.nn_osd_ready2draw = 1;
		vTaskDelay(10);
	}
	vTaskDelay(200);
	for (int i = 0; i < OSD_OBJ_TOTAL_MAX_NUM; i++) {
		osd_hide_bitmap(0, &osd2_pic[i].osd2);
	}

	for (int i = 0; i < 3; i++) {
		if (nn_result.nn_osd_channel_en[i]) {
			osd_deinit(i);
		}
	}

	if (tag_bmp.pData) {
		free(tag_bmp.pData);
	}

	for (int i = 0; i < OSD_OBJ_TOTAL_MAX_NUM; i++) {
		if (pd1[i]) {
			free(pd1[i]);
		}
		pd1[i] = NULL;
		if (pd2[i]) {
			free(pd2[i]);
		}
		pd2[i] = NULL;
	}
	vTaskDelete(NULL);
}

void nn_osd_start(int v1_enable, int v1_w, int v1_h, int v2_enable, int v2_w, int v2_h,
				  int v3_enable, int v3_w, int v3_h, int txt_w, int txt_h)
{
	nn_rect_txt_w = (txt_w + 7) & (~7);
	nn_rect_txt_h = (txt_h + 7) & (~7);

	nn_result.nn_osd_channel_en[0] = v1_enable;
	nn_result.nn_osd_channel_xmax[0] = v1_w;
	nn_result.nn_osd_channel_ymax[0] = v1_h;

	nn_result.nn_osd_channel_en[1] = v2_enable;
	nn_result.nn_osd_channel_xmax[1] = v2_w;
	nn_result.nn_osd_channel_ymax[1] = v2_h;

	nn_result.nn_osd_channel_en[2] = v3_enable;
	nn_result.nn_osd_channel_xmax[2] = v3_w;
	nn_result.nn_osd_channel_ymax[2] = v3_h;

	nn_result.nn_osd_ready2draw = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < OSD_OBJ_MAX_NUM; j++) {
			int idx = i * OSD_OBJ_MAX_NUM + j;
			nn_result.obj[idx].ch = i;
			nn_result.obj[idx].idx = j;
			nn_result.obj[idx].draw_status = DRAW_NOTHING;
			nn_result.obj[idx].point_num = 0;
			nn_result.obj[idx].rect.line_width = 3;
			nn_result.obj[idx].rect.color.r = 0;
			nn_result.obj[idx].rect.color.g = 255;
			nn_result.obj[idx].rect.color.b = 255;
			nn_result.obj[idx].text.color.r = 255;
			nn_result.obj[idx].text.color.g = 255;
			nn_result.obj[idx].text.color.b = 0;
		}
	}

	printf("NN OSD Draw start\r\n");
	printf("nn_rect_txt_w:%d, nn_rect_txt_h:%d.\r\n", nn_rect_txt_w, nn_rect_txt_h);

	nn_osd_sema = xSemaphoreCreateBinary();
	if (nn_osd_sema == NULL) {
		printf("%s: nn_osd_sema create fail \r\n", __FUNCTION__);
		return;
	}

	if (xTaskCreate(nn_osd_task, "NN_OSD_DRAW", 10 * 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		printf("\n\r%s xTaskCreate failed", __FUNCTION__);
	}
}
