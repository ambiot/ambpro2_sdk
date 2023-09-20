#include "hal_osd_util.h"
#include "video_api.h"
#include "osd_custom.h"
#include "osd_api.h"
#include "osd_render.h"
#include "osdep_service.h"

#undef printf
#include <stdio.h>

static SemaphoreHandle_t osd_render_task_stop_sema = NULL;
static QueueHandle_t canvas_msg_queue = NULL;
static osd_render_info_t osd_render_info;
static int availible_block_num[OSD_OBJ_MAX_CH] = {0};
static int available_block_idx[OSD_OBJ_MAX_CH][OSD_OBJ_MAX_NUM] = {0};
static int osd_render_task_stop_flag = 1;

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif
#define LIMIT(x, lower, upper) if(x<lower) x=lower; else if(x>upper) x=upper;

int canvas_send_msg(canvas_msg_t *canvas_msg)
{
	if (!canvas_msg_queue || osd_render_task_stop_flag) {
		//printf("canvas_send_msg not ready\r\n");
		return 0;
	}

	if (availible_block_num[canvas_msg->ch] <= canvas_msg->idx) {
		printf("canvas_send_msg: idx larger than available block\r\n");
		return 0;
	}

	if (!uxQueueSpacesAvailable(canvas_msg_queue)) {
		printf("canvas_send_msg not available\r\n");
		return 0;
	}
	if (xQueueSendToBack(canvas_msg_queue, (void *)canvas_msg, 100) != pdPASS) {
		printf("canvas_send_msg failed\r\n");
		return 0;
	}
	return 1;
}

int canvas_create_bitmap(int ch, int idx, enum rts_osd2_blk_fmt bmp_format)
{
	canvas_msg_t canvas_msg;
	canvas_msg.ch = ch;
	canvas_msg.idx = idx;
	canvas_msg.msg_type = CANVAS_CREATE_BMP;
	canvas_msg.draw_data.bmp.bmp_format = bmp_format;
	int pic_idx = ch * OSD_OBJ_MAX_NUM + available_block_idx[ch][idx];
	return canvas_send_msg(&canvas_msg);
}

int canvas_update(int ch, int idx, int ready2update)
{
	canvas_msg_t canvas_msg;
	canvas_msg.ch = ch;
	canvas_msg.idx = idx;
	canvas_msg.msg_type = CANVAS_MSG_DRAW;
	canvas_msg.draw_data.ready2update = ready2update;
	return canvas_send_msg(&canvas_msg);
}

int canvas_set_point(int ch, int idx, int x, int y, int point_width, uint32_t color)
{
	canvas_msg_t canvas_msg;
	canvas_msg.ch = ch;
	canvas_msg.idx = idx;
	canvas_msg.msg_type = CANVAS_MSG_POINT;
	canvas_msg.draw_data.point.x = x;
	canvas_msg.draw_data.point.y = y;
	canvas_msg.draw_data.point.pt_width = point_width;
	canvas_msg.color.argb_u32 = color;
	return canvas_send_msg(&canvas_msg);
}

int canvas_set_line(int ch, int idx, int xstart, int ystart, int xend, int yend, int line_width, uint32_t color)
{
	canvas_msg_t canvas_msg;
	canvas_msg.ch = ch;
	canvas_msg.idx = idx;
	canvas_msg.msg_type = CANVAS_MSG_LINE;
	canvas_msg.draw_data.line.line_width = line_width;
	canvas_msg.draw_data.line.start_point.x = xstart;
	canvas_msg.draw_data.line.start_point.y = ystart;
	canvas_msg.draw_data.line.end_point.x = xend;
	canvas_msg.draw_data.line.end_point.y = yend;
	canvas_msg.color.argb_u32 = color;
	return canvas_send_msg(&canvas_msg);
}

int canvas_set_rect(int ch, int idx, int xmin, int ymin, int xmax, int ymax, int line_width, uint32_t color)
{
	canvas_msg_t canvas_msg;
	canvas_msg.ch = ch;
	canvas_msg.idx = idx;
	canvas_msg.msg_type = CANVAS_MSG_RECT;
	canvas_msg.draw_data.rect.line_width = line_width;
	canvas_msg.draw_data.rect.start_point.x = xmin;
	canvas_msg.draw_data.rect.start_point.y = ymin;
	canvas_msg.draw_data.rect.end_point.x = xmax;
	canvas_msg.draw_data.rect.end_point.y = ymax;
	canvas_msg.color.argb_u32 = color;
	return canvas_send_msg(&canvas_msg);
}

int canvas_set_text(int ch, int idx, int xmin, int ymin, char *text_string, uint32_t color)
{
	canvas_msg_t canvas_msg;
	canvas_msg.ch = ch;
	canvas_msg.idx = idx;
	canvas_msg.msg_type = CANVAS_MSG_TEXT;
	canvas_msg.draw_data.text.start_point.x = xmin;
	canvas_msg.draw_data.text.start_point.y = ymin;
	snprintf(canvas_msg.draw_data.text.text_str, sizeof(canvas_msg.draw_data.text.text_str), "%s", text_string);
	canvas_msg.color.argb_u32 = color;
	return canvas_send_msg(&canvas_msg);
}

void osd_render_task_stop(void)
{
	if (!osd_render_task_stop_flag) {
		osd_render_task_stop_flag = 1;
	} else {
		printf("osd_render_task already closing.\r\n");
		return;
	}

	if (xSemaphoreTake(osd_render_task_stop_sema, portMAX_DELAY) == pdTRUE) {
		printf("finish close nn osd\r\n");
		vSemaphoreDelete(osd_render_task_stop_sema);
		osd_render_task_stop_sema = NULL;
		return;
	}
	return;
}

void osd_render_task(void *arg)
{
	int i, j;
	canvas_msg_t cavas_msg_recieve;

#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	rtw_create_secure_context(2048);
#endif

	osd_render_info.ready2draw = 1;
	while (!osd_render_task_stop_flag) {
		if (xQueueReceive(canvas_msg_queue, &cavas_msg_recieve, 100) == pdPASS) {
			int ch = cavas_msg_recieve.ch;
			int block_idx = cavas_msg_recieve.idx;
			int pic_idx = ch * OSD_OBJ_MAX_NUM + available_block_idx[ch][block_idx];
			osd_render_obj_t *obj = &osd_render_info.render_obj[pic_idx];
			osd_pict_st *osd2_pic = &osd_render_info.render_obj[pic_idx].osd2_pic;
			int bimap_index = obj->buff_used_index;
			uint8_t **buff_bmp = &(obj->bitmap[bimap_index].buff);
			if (availible_block_num[ch] && (rts_osd_get_status(ch)) && (video_get_stream_info(ch))) {
				if (cavas_msg_recieve.msg_type == CANVAS_CREATE_BMP) {
					obj->bitmap[bimap_index].bmp_format = cavas_msg_recieve.draw_data.bmp.bmp_format;
					if (obj->bitmap[bimap_index].bmp_format != RTS_OSD2_BLK_FMT_RGBA2222 && obj->bitmap[bimap_index].bmp_format != RTS_OSD2_BLK_FMT_1BPP) {
						printf("osd_render_task CANVAS_CREATE_BMP failed: not suppoted bitmap format\r\n");
						return;
					}
					obj->bitmap[bimap_index].start_point.x = osd_render_info.channel_xmax[ch];
					obj->bitmap[bimap_index].start_point.y = osd_render_info.channel_ymax[ch];
					obj->bitmap[bimap_index].end_point.x = 0;
					obj->bitmap[bimap_index].end_point.y = 0;
					obj->canvas_draw_msg_count = 0;
				} else if (cavas_msg_recieve.msg_type == CANVAS_MSG_DRAW) {
					//coordinate must be even
					obj->bitmap[bimap_index].start_point.x = obj->bitmap[bimap_index].start_point.x & (~1);
					obj->bitmap[bimap_index].start_point.y = obj->bitmap[bimap_index].start_point.y & (~1);
					obj->bitmap[bimap_index].end_point.x = obj->bitmap[bimap_index].end_point.x & (~1);
					obj->bitmap[bimap_index].end_point.y = obj->bitmap[bimap_index].end_point.y & (~1);

					//check bitmap range
					LIMIT(obj->bitmap[bimap_index].start_point.x, 0, osd_render_info.channel_xmax[ch])
					LIMIT(obj->bitmap[bimap_index].start_point.y, 0, osd_render_info.channel_ymax[ch])
					LIMIT(obj->bitmap[bimap_index].end_point.x, 0, osd_render_info.channel_xmax[ch])
					LIMIT(obj->bitmap[bimap_index].end_point.y, 0, osd_render_info.channel_ymax[ch])

					int width = canvas_get_bitmap_width(&obj->bitmap[bimap_index]);
					int height = canvas_get_bitmap_height(&obj->bitmap[bimap_index]);
					obj->bitmap[bimap_index].buff_len = width * height;

					//draw nothing
					if (width <= 0 || height <= 0) {
						obj->bitmap[bimap_index].start_point.x = 0;
						obj->bitmap[bimap_index].start_point.y = 0;
						obj->bitmap[bimap_index].end_point.x = 8;
						obj->bitmap[bimap_index].end_point.y = 8;
						obj->bitmap[bimap_index].buff_len = 64;
						obj->canvas_draw_msg_count = 0;
					}

					if (obj->canvas_draw_msg_count_last == 0 && obj->canvas_draw_msg_count == 0 && cavas_msg_recieve.draw_data.ready2update == 0) { //skip draw nothing
						continue;
					}

					obj->canvas_draw_msg_count_last = obj->canvas_draw_msg_count;

					//malloc bitmap
					if (obj->bitmap[bimap_index].buff) {
						free(obj->bitmap[bimap_index].buff);
						obj->bitmap[bimap_index].buff = NULL;
					}
					if (!obj->bitmap[bimap_index].buff) {
						obj->bitmap[bimap_index].buff = (uint8_t *)malloc(obj->bitmap[bimap_index].buff_len);
						//printf("osd_render_task: ch%d id%d w %d h %d\r\n", ch, pic_idx, (obj->bitmap[bimap_index].end_point.x - obj->bitmap[bimap_index].start_point.x), (obj->bitmap[bimap_index].end_point.y - obj->bitmap[bimap_index].start_point.y));
						//printf("osd_render_task: ch%d id%d malloc(%d)\r\n", ch, pic_idx, obj->bitmap[bimap_index].buff_len);
					} else {
						printf("osd_render_task: ch%d id%d malloc(%d) failed\r\n", ch, pic_idx, obj->bitmap[bimap_index].buff_len);
					}

					//draw bitmap and update
					uint8_t **buff_bmp = &(obj->bitmap[bimap_index].buff);
					if (*buff_bmp) {
						memset(*buff_bmp, 0x00, obj->bitmap[bimap_index].buff_len);
						for (int i = 0; i < obj->canvas_draw_msg_count; i++) {
							if (obj->canvas_draw_msg[i].msg_type == CANVAS_MSG_TEXT) {
								draw_text_on_bitmap(&obj->bitmap[bimap_index], ch, &obj->canvas_draw_msg[i].draw_data.text, &obj->canvas_draw_msg[i].color);
							} else if (obj->canvas_draw_msg[i].msg_type == CANVAS_MSG_RECT) {
								draw_rect_on_bitmap(&obj->bitmap[bimap_index], &obj->canvas_draw_msg[i].draw_data.rect, &obj->canvas_draw_msg[i].color);
							} else if (obj->canvas_draw_msg[i].msg_type == CANVAS_MSG_LINE) {
								draw_line_on_bitmap(&obj->bitmap[bimap_index], &obj->canvas_draw_msg[i].draw_data.line, &obj->canvas_draw_msg[i].color);
							} else if (obj->canvas_draw_msg[i].msg_type == CANVAS_MSG_POINT) {
								draw_point_on_bitmap(&obj->bitmap[bimap_index], &obj->canvas_draw_msg[i].draw_data.point, &obj->canvas_draw_msg[i].color);
							} else {
								printf("unknown type\r\n");
							}
						}

						//update osd
						osd2_pic->osd2.start_x = obj->bitmap[bimap_index].start_point.x;
						osd2_pic->osd2.start_y = obj->bitmap[bimap_index].start_point.y;
						osd2_pic->osd2.end_x = obj->bitmap[bimap_index].end_point.x;
						osd2_pic->osd2.end_y = obj->bitmap[bimap_index].end_point.y;
						osd2_pic->osd2.blk_fmt = obj->bitmap[bimap_index].bmp_format;
						osd2_pic->osd2.color_1bpp = obj->bitmap[bimap_index].color_1bpp;
						osd2_pic->osd2.buf = *buff_bmp;
						osd2_pic->osd2.len = obj->bitmap[bimap_index].buff_len;

						rts_osd_bitmap_update(osd2_pic->chn_id, &(osd2_pic->osd2), cavas_msg_recieve.draw_data.ready2update);
						obj->buff_used_index = obj->buff_used_index ^ 0x01;
					}
				} else {
					if (obj->canvas_draw_msg_count >= MAX_DRAW_MSG) {
						break;
					}
					switch (cavas_msg_recieve.msg_type) {
					case CANVAS_MSG_TEXT:
						obj->bitmap[bimap_index].start_point.x = MIN(obj->bitmap[bimap_index].start_point.x, cavas_msg_recieve.draw_data.text.start_point.x);
						obj->bitmap[bimap_index].start_point.y = MIN(obj->bitmap[bimap_index].start_point.y, cavas_msg_recieve.draw_data.text.start_point.y);
						obj->bitmap[bimap_index].end_point.x = MAX(obj->bitmap[bimap_index].end_point.x, cavas_msg_recieve.draw_data.text.start_point.x + TXT_STR_MAX_LEN * CHAR_MAX_W);
						obj->bitmap[bimap_index].end_point.y = MAX(obj->bitmap[bimap_index].end_point.y, cavas_msg_recieve.draw_data.text.start_point.y + CHAR_MAX_H);

						obj->canvas_draw_msg[obj->canvas_draw_msg_count] = cavas_msg_recieve;
						obj->canvas_draw_msg_count++;
						break;
					case CANVAS_MSG_RECT:
						obj->bitmap[bimap_index].start_point.x = MIN(obj->bitmap[bimap_index].start_point.x, cavas_msg_recieve.draw_data.rect.start_point.x);
						obj->bitmap[bimap_index].start_point.y = MIN(obj->bitmap[bimap_index].start_point.y, cavas_msg_recieve.draw_data.rect.start_point.y);
						obj->bitmap[bimap_index].end_point.x = MAX(obj->bitmap[bimap_index].end_point.x, cavas_msg_recieve.draw_data.rect.end_point.x);
						obj->bitmap[bimap_index].end_point.y = MAX(obj->bitmap[bimap_index].end_point.y, cavas_msg_recieve.draw_data.rect.end_point.y);

						obj->canvas_draw_msg[obj->canvas_draw_msg_count] = cavas_msg_recieve;
						obj->canvas_draw_msg_count++;
						break;
					case CANVAS_MSG_LINE:
						obj->bitmap[bimap_index].start_point.x = MIN(obj->bitmap[bimap_index].start_point.x,
								cavas_msg_recieve.draw_data.line.start_point.x - cavas_msg_recieve.draw_data.line.line_width);
						obj->bitmap[bimap_index].start_point.y = MIN(obj->bitmap[bimap_index].start_point.y,
								cavas_msg_recieve.draw_data.line.start_point.y - cavas_msg_recieve.draw_data.line.line_width);
						obj->bitmap[bimap_index].end_point.x = MAX(obj->bitmap[bimap_index].end_point.x,
															   cavas_msg_recieve.draw_data.line.end_point.x + cavas_msg_recieve.draw_data.line.line_width);
						obj->bitmap[bimap_index].end_point.y = MAX(obj->bitmap[bimap_index].end_point.y,
															   cavas_msg_recieve.draw_data.line.end_point.y + cavas_msg_recieve.draw_data.line.line_width);

						obj->canvas_draw_msg[obj->canvas_draw_msg_count] = cavas_msg_recieve;
						obj->canvas_draw_msg_count++;
						break;
					case CANVAS_MSG_POINT:
						obj->bitmap[bimap_index].start_point.x = MIN(obj->bitmap[bimap_index].start_point.x,
								cavas_msg_recieve.draw_data.point.x - cavas_msg_recieve.draw_data.point.pt_width);
						obj->bitmap[bimap_index].start_point.y = MIN(obj->bitmap[bimap_index].start_point.y,
								cavas_msg_recieve.draw_data.point.y - cavas_msg_recieve.draw_data.point.pt_width);
						obj->bitmap[bimap_index].end_point.x = MAX(obj->bitmap[bimap_index].end_point.x,
															   cavas_msg_recieve.draw_data.point.x + cavas_msg_recieve.draw_data.point.pt_width);
						obj->bitmap[bimap_index].end_point.y = MAX(obj->bitmap[bimap_index].end_point.y,
															   cavas_msg_recieve.draw_data.point.y + cavas_msg_recieve.draw_data.point.pt_width);

						obj->canvas_draw_msg[obj->canvas_draw_msg_count] = cavas_msg_recieve;
						obj->canvas_draw_msg_count++;
						break;
					default:
						break;
					}
				}
			}
		}
	}

	printf("clear all the block when close\r\n");
	//clear all the block when close
	for (i = 0; i < OSD_OBJ_MAX_CH; i++) {
		for (j = 0; j < availible_block_num[i]; j++) {
			int pic_idx = i * OSD_OBJ_MAX_NUM + available_block_idx[i][j];
			osd_render_info.render_obj[pic_idx].buff_used_index = osd_render_info.render_obj[pic_idx].buff_used_index ^ 0x01;
			int bimap_index = osd_render_info.render_obj[pic_idx].buff_used_index;
			osd_pict_st *osd2_pic = &osd_render_info.render_obj[pic_idx].osd2_pic;
			uint8_t **buff_bmp = &(osd_render_info.render_obj[pic_idx].bitmap[bimap_index].buff);
			if (*buff_bmp && (rts_osd_get_status(i)) && (video_get_stream_info(i))) {
				osd2_pic->osd2.len = 8 * 8;
				osd2_pic->osd2.start_x = 0;
				osd2_pic->osd2.start_y = 0;
				osd2_pic->osd2.end_x = 8;
				osd2_pic->osd2.end_y = 8;
				osd2_pic->osd2.blk_fmt = RTS_OSD2_BLK_FMT_1BPP;
				memset(*buff_bmp, 0, osd2_pic->osd2.len);
				osd2_pic->osd2.buf = *buff_bmp;
				rts_osd_bitmap_update(i, &osd2_pic->osd2, 1);
				rts_osd_hide_bitmap(i, &osd2_pic->osd2);
			}
		}
	}

	for (i = 0; i < OSD_OBJ_MAX_CH; i++) {
		for (j = 0; j < availible_block_num[i]; j++) {
			int pic_idx = i * OSD_OBJ_MAX_NUM + available_block_idx[i][j];
			if (osd_render_info.render_obj[pic_idx].bitmap[0].buff) {
				free(osd_render_info.render_obj[pic_idx].bitmap[0].buff);
			}
			osd_render_info.render_obj[pic_idx].bitmap[0].buff = NULL;
			if (osd_render_info.render_obj[pic_idx].bitmap[1].buff) {
				free(osd_render_info.render_obj[pic_idx].bitmap[1].buff);
			}
			osd_render_info.render_obj[pic_idx].bitmap[1].buff = NULL;

		}
	}

	vQueueDelete(canvas_msg_queue);
	canvas_msg_queue = NULL;

	xSemaphoreGive(osd_render_task_stop_sema);

	vTaskDelete(NULL);
}

void osd_render_task_start(int *ch_visible, int *ch_width, int *ch_height)
{
	if (!osd_render_task_stop_flag || osd_render_task_stop_sema) {
		printf("osd_render_task start failed: task is not close or closing.\r\n");
		return;
	}

	for (int i = 0; i < OSD_OBJ_MAX_CH; i++) {
		if (ch_visible[i]) {
			if (!rts_osd_get_status(i)) {
				printf("osd_render_task start failed: Osd ch %d not init.\r\n", i);
				return;
			}
			osd_render_info.channel_en[i] = ch_visible[i];
			osd_render_info.channel_xmax[i] = ch_width[i] & (~7);
			osd_render_info.channel_ymax[i] = ch_height[i] & (~7);
			rts_osd_get_available_block(i, &availible_block_num[i], available_block_idx[i]);
			if (availible_block_num[i] == 0) {
				printf("osd_render_task start failed: Osd ch %d no block availible.\r\n", i);
				return;
			}
			if (availible_block_num[i] > OSD_OBJ_MAX_NUM) {
				availible_block_num[i] = OSD_OBJ_MAX_NUM;
			}
			printf("osd ch %d e%d num %d (%d, %d, %d)\r\n", i, ch_visible[i], availible_block_num[i], available_block_idx[i][0], available_block_idx[i][1],
				   available_block_idx[i][2]);

			//set osd boundary check
			rts_osd_set_frame_size(i, osd_render_info.channel_xmax[i], osd_render_info.channel_ymax[i]);
		}
	}

	osd_render_task_stop_flag = 0;
	osd_render_info.ready2draw = 0;

	printf("osd_render_task start\r\n");

	canvas_msg_queue = xQueueCreate(MAX_QUEUE_MSG, sizeof(canvas_msg_t));
	if (canvas_msg_queue == NULL) {
		printf("%s: canvas_msg_queue create fail \r\n", __FUNCTION__);
		return;
	}

	osd_render_task_stop_sema = xSemaphoreCreateBinary();
	if (osd_render_task_stop_sema == NULL) {
		printf("%s: osd_render_task_stop_sema create fail \r\n", __FUNCTION__);
		return;
	}

	enum rts_osd2_blk_fmt disp_format = RTS_OSD2_BLK_FMT_RGBA2222;
	memset(&osd_render_info.render_obj[0], 0, sizeof(osd_render_obj_t) * OSD_OBJ_MAX_CH * OSD_OBJ_MAX_NUM);
	for (int i = 0; i < OSD_OBJ_MAX_CH; i++) {
		for (int j = 0; j < OSD_OBJ_MAX_NUM; j++) {
			int pic_idx = i * OSD_OBJ_MAX_NUM + j;
			osd_pict_st *osd2_pic = &osd_render_info.render_obj[pic_idx].osd2_pic;
			memset(osd2_pic, 0, sizeof(osd_pict_st));
			osd_render_info.render_obj[pic_idx].bitmap[0].buff = NULL;
			osd_render_info.render_obj[pic_idx].bitmap[0].buff_len = 0;
			osd_render_info.render_obj[pic_idx].bitmap[1].buff = NULL;
			osd_render_info.render_obj[pic_idx].bitmap[1].buff_len = 0;
			osd2_pic->chn_id = i;
			osd2_pic->osd2.blk_idx = j;
			osd2_pic->osd2.blk_fmt = disp_format;
			osd2_pic->show = 0;
		}
	}

	if (xTaskCreate(osd_render_task, "osd_render_task", 10 * 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		printf("\n\r%s xTaskCreate failed", __FUNCTION__);
	}
	while (!osd_render_info.ready2draw) { //wait for task ready
		vTaskDelay(10);
	}
	return;
}

void osd_render_dev_init(int *ch_enable, int *char_resize_w, int *char_resize_h)
{
	int char_w, char_h;
	for (int i = 0; i < OSD_OBJ_MAX_CH; i++) {
		if (ch_enable[i]) {
			char_w = (char_resize_w[i] + 7) & (~7);
			char_h = (char_resize_h[i] + 7) & (~7);
			//hal_video_osd_enc_enable(i, 1);
			rts_osd_init(i, char_w, char_h, (int)(8.0f * 3600));
			rts_osd_release_init_protect();
		}
	}
}

void osd_render_dev_deinit(int ch)
{
	if (rts_osd_get_status(ch)) {
		rts_osd_deinit(ch);
	}
}

void osd_render_dev_deinit_all()
{
	for (int i = 0; i < OSD_OBJ_MAX_CH; i++) {
		if (rts_osd_get_status(i)) {
			rts_osd_deinit(i);
		}
	}

}