/******************************************************************************
*
* Copyright(c) 2021 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "platform_stdlib.h"
#include "hal_video.h"
#include "hal_voe.h"
#include "video_boot.h"
#include "diag.h"
#include "rtl8735b_i2c.h"
extern uint8_t __eram_end__[];
extern uint8_t __eram_start__[];
extern int __voe_code_start__[];
int fcs_flag = 0;//for disable fcs flag
extern video_boot_stream_t video_boot_stream;

unsigned int video_boot_malloc(unsigned int size) // alignment 32 byte
{
	unsigned int heap_size = 0;
	unsigned int heap_addr = 0;
	if (size % 32 == 0) {
		heap_size = size;
	} else {
		heap_size = size + (32 - (size % 32));
	}
	//printf("__eram_end__ %x - __eram_start__ %x\r\n",__eram_end__,__eram_start__);
	heap_addr = (unsigned int)__eram_end__ - heap_size;
	//printf("heap_addr %x  size %d\r\n", heap_addr, heap_size);
	return heap_addr;
}

void video_boot_set_private_mask(int ch, video_boot_private_mask_t *pmask)
{
	hal_video_reset_mask_status();
	hal_video_set_mask_color(0xff0080);
	for (int id = 0 ; id < PRIVATE_MAX_NUM; id++) {
		if (pmask->en[id] &&  id == PRIVATE_MASK_GRID) { //GRID MODE
			isp_grid_t grid;
			dbg_printf("[GRID Mode]:\r\n");
			if (pmask->start_x[id] % 2) {
				dbg_printf("[%s] invalid value pmask->start_x=%d", __FUNCTION__, pmask->start_x);
				continue;
			}
			if (pmask->start_y[id] % 2) {
				dbg_printf("[%s] invalid value pmask->start_y=%d", __FUNCTION__, pmask->start_y);
				continue;
			}
			if (pmask->cols % 8) {
				dbg_printf("[%s] invalid value pmask->cols=%d", __FUNCTION__, pmask->cols);
				continue;
			}
			if (pmask->w[id] % 16) {
				int cell_w = pmask->w[id] / pmask->cols;
				if (cell_w % 2) {
					dbg_printf("[%s] invalid value cell_w=%d", __FUNCTION__, cell_w);
					continue;
				}
			}


			grid.start_x = pmask->start_x[id];
			grid.start_y = pmask->start_y[id];
			grid.cols = pmask->cols;
			grid.rows = pmask->rows;
			grid.cell_w = pmask->w[id] / grid.cols;
			grid.cell_h = pmask->h[id] / grid.rows;
			hal_video_config_grid_mask(pmask->en[id], grid, (uint8_t *)pmask->bitmap);
		} else if (pmask->en[id]) { //RECT MODE
			isp_rect_t rect;
			if (pmask->start_x[id] % 2) {
				dbg_printf("[%s] invalid value pmask->start_x=%d", __FUNCTION__, pmask->start_x);
				continue;
			}
			if (pmask->start_y[id] % 2) {
				dbg_printf("[%s] invalid value pmask->start_y=%d", __FUNCTION__, pmask->start_y);
				continue;
			}
			rect.left = pmask->start_x[id];
			rect.top  = pmask->start_y[id];
			rect.right  = pmask->w[id] + rect.left;
			rect.bottom = pmask->h[id] + rect.top;
			hal_video_config_rect_mask(pmask->en[id], id - 1, rect);
		}
	}
	hal_video_fast_enable_mask(ch);
}

int video_boot_buf_calc(video_boot_stream_t vidoe_boot)
{
	int v3dnr_w = 2560;
	int v3dnr_h = 1440;
	int enc_buf_size_len = 0;
	int enc_buf_size = 0;
	int i = 0;

	if (vidoe_boot.isp_info.sensor_width && vidoe_boot.isp_info.sensor_height) {
		v3dnr_w = vidoe_boot.isp_info.sensor_width;
		v3dnr_h = vidoe_boot.isp_info.sensor_height;
	}
	//printf("v3dnr_w %d v3dnr_h %d\r\n", v3dnr_w, v3dnr_h);
	vidoe_boot.voe_heap_size += ((v3dnr_w * v3dnr_h * 3) / 2);

	if (vidoe_boot.isp_info.md_enable) {
		if (vidoe_boot.isp_info.md_buf_size) {
			vidoe_boot.voe_heap_size += vidoe_boot.isp_info.md_buf_size;
		} else {
			vidoe_boot.voe_heap_size += ENABLE_MD_BUF;
		}
	}

	if (vidoe_boot.isp_info.hdr_enable) {
		vidoe_boot.voe_heap_size += ENABLE_HDR_BUF;
	}

	for (i = 0; i < 3; i++) {
		if (vidoe_boot.video_enable[i]) {
			vidoe_boot.voe_heap_size += ((vidoe_boot.video_params[i].width * vidoe_boot.video_params[i].height * 3) / 2) * 2;//isp_ch_buf_num[0];
			//ISP common
			vidoe_boot.voe_heap_size += ISP_CREATE_BUF;
			//enc ref
			vidoe_boot.voe_heap_size += ((vidoe_boot.video_params[i].width * vidoe_boot.video_params[i].height * 3) / 2) * 2 +
										(vidoe_boot.video_params[i].width * vidoe_boot.video_params[i].height / 16) * 2;
			//enc common
			vidoe_boot.voe_heap_size += ENC_CREATE_BUF;
			//enc buffer
			if (i == 0) {
				enc_buf_size_len = V1_ENC_BUF_SIZE;
			} else if (i == 1) {
				enc_buf_size_len = V2_ENC_BUF_SIZE;
			} else if (i == 2) {
				enc_buf_size_len = V3_ENC_BUF_SIZE;
			}
			enc_buf_size = ((vidoe_boot.video_params[i].width * vidoe_boot.video_params[i].height) / VIDEO_RSVD_DIVISION + (vidoe_boot.video_params[i].bps *
							enc_buf_size_len) / 8);
			vidoe_boot.voe_heap_size += enc_buf_size;
			video_boot_stream.video_params[i].out_buf_size = enc_buf_size;
			//dbg_printf("channel %d size %d\r\n",i,video_boot_stream.video_params[i].out_buf_size);
			//shapshot
			if (vidoe_boot.video_snapshot[i]) {
				vidoe_boot.voe_heap_size += ((vidoe_boot.video_params[i].width * vidoe_boot.video_params[i].height * 3) / 2) + SNAPSHOT_BUF;
			}
			//osd common
			if (vidoe_boot.isp_info.osd_enable) {
				vidoe_boot.voe_heap_size += OSD_CREATE_BUF;
			}
		}
	}
	if (vidoe_boot.video_enable[STREAM_V4]) { //For NN memory
		//ISP buffer
		vidoe_boot.voe_heap_size += vidoe_boot.video_params[i].width * vidoe_boot.video_params[STREAM_V4].height * 3 * 2;
		//ISP common
		vidoe_boot.voe_heap_size += ISP_CREATE_BUF;
	}
	if (vidoe_boot.voe_heap_size % 32 == 0) {
		vidoe_boot.voe_heap_size = vidoe_boot.voe_heap_size;
	} else {
		vidoe_boot.voe_heap_size = vidoe_boot.voe_heap_size + (32 - (vidoe_boot.voe_heap_size % 32));
	}

	return vidoe_boot.voe_heap_size;
}

int video_boot_open(video_boot_params_t *v_stream)
{
	int ch = v_stream->stream_id;
	int fcs_v = v_stream->fcs;
	int isp_fps = video_boot_stream.isp_info.sensor_fps;
	int fps = 30;
	int gop = 30;
	int rcMode = 1;
	int bps = 2 * 1024 * 1024;
	int minQp = 0;
	int maxQp = 51;
	int rotation = 0;
	int jpeg_qlevel = 5;
	int type;
	int res = 0;
	int codec = 0;

	int enc_in_w = (video_boot_stream.video_params[ch].width + 15) & ~15;  //force 16 aligned
	int enc_in_h = video_boot_stream.video_params[ch].height;
	int enc_out_w = video_boot_stream.video_params[ch].width;  //will crop enc_in_w to enc_out_w
	int enc_out_h = video_boot_stream.video_params[ch].height;
	int enc_out_w_offset = (enc_in_w - enc_out_w) / 2;

	int out_rsvd_size = (enc_in_w * enc_in_h) / VIDEO_RSVD_DIVISION;
	int out_buf_size = 0;
	int jpeg_out_buf_size = out_rsvd_size * 3;
	switch (ch) {
	case 0:
		out_buf_size = (v_stream->bps * V1_ENC_BUF_SIZE) / 8 + out_rsvd_size;
		break;
	case 1:
		out_buf_size = (v_stream->bps * V2_ENC_BUF_SIZE) / 8 + out_rsvd_size;
		break;
	case 2:
		out_buf_size = (v_stream->bps * V3_ENC_BUF_SIZE) / 8 + out_rsvd_size;
		break;
	}

	bps = video_boot_stream.video_params[ch].bps;

	if (video_boot_stream.video_params[ch].rc_mode) {
		rcMode = video_boot_stream.video_params[ch].rc_mode - 1;
		if (rcMode) {
			minQp = 25;
			maxQp = 48;
			bps = video_boot_stream.video_params[ch].bps / 2;
		}
	}

	if (video_boot_stream.video_params[ch].minQp > 0 && video_boot_stream.video_params[ch].minQp <= 51) {
		minQp = video_boot_stream.video_params[ch].minQp;
	}
	if (video_boot_stream.video_params[ch].maxQp > 0 && video_boot_stream.video_params[ch].maxQp <= 51) {
		maxQp = video_boot_stream.video_params[ch].maxQp;
	}

	hal_video_adapter_t *v_adp = hal_video_get_adp();
	v_adp->cmd[ch]->lumWidthSrc = enc_in_w;
	v_adp->cmd[ch]->lumHeightSrc = enc_in_h;
	v_adp->cmd[ch]->width = enc_out_w;
	v_adp->cmd[ch]->height = enc_out_h;
	v_adp->cmd[ch]->horOffsetSrc = enc_out_w_offset;

	v_adp->cmd[ch]->outputRateNumer = video_boot_stream.video_params[ch].fps;
	v_adp->cmd[ch]->inputRateNumer = isp_fps;//video_boot_stream.video_params[ch].fps;
	v_adp->cmd[ch]->intraPicRate = video_boot_stream.video_params[ch].gop;
	v_adp->cmd[ch]->rotation = video_boot_stream.video_params[ch].rotation;
	v_adp->cmd[ch]->bitPerSecond = bps;//video_boot_stream.video_params[ch].bps;
	v_adp->cmd[ch]->qpMin = minQp;
	v_adp->cmd[ch]->qpMax = maxQp;

	v_adp->cmd[ch]->vbr = rcMode;

	v_adp->cmd[ch]->CodecType = v_stream->type;
	v_adp->cmd[ch]->fcs = 1;
	v_adp->cmd[ch]->EncMode = MODE_QUEUE;

	if (v_stream->type == CODEC_HEVC) {
		v_adp->cmd[ch]->outputFormat     = VCENC_VIDEO_CODEC_HEVC,
						v_adp->cmd[ch]->max_cu_size      = 64;
		v_adp->cmd[ch]->min_cu_size      = 8;
		v_adp->cmd[ch]->max_tr_size      = 16;
		v_adp->cmd[ch]->min_tr_size      = 4;
		v_adp->cmd[ch]->tr_depth_intra   = 2; 							 //mfu =>0
		v_adp->cmd[ch]->tr_depth_inter   = 4;							// (.max_cu_size == 64) ? 4 : 3,
		v_adp->cmd[ch]->level            = VCENC_HEVC_LEVEL_6;
		v_adp->cmd[ch]->profile          = VCENC_HEVC_MAIN_PROFILE;	// default is HEVC MAIN profile
	} else {
		v_adp->cmd[ch]->outputFormat     = VCENC_VIDEO_CODEC_H264;
		v_adp->cmd[ch]->max_cu_size      = 16;
		v_adp->cmd[ch]->min_cu_size      = 8;
		v_adp->cmd[ch]->max_tr_size      = 16;
		v_adp->cmd[ch]->min_tr_size      = 4;
		v_adp->cmd[ch]->tr_depth_intra   = 1;
		v_adp->cmd[ch]->tr_depth_inter   = 2;
		v_adp->cmd[ch]->level            = VCENC_H264_LEVEL_5_1;
		v_adp->cmd[ch]->profile          = VCENC_H264_HIGH_PROFILE;	// default is HEVC HIGH profile
	}
	v_adp->cmd[ch]->byteStream = VCENC_BYTE_STREAM;
	v_adp->cmd[ch]->gopSize = 1;
	v_adp->cmd[ch]->ch = v_stream->stream_id;
	if (video_boot_stream.isp_info.osd_enable) {
		v_adp->cmd[ch]->osd = 1;
	}

	if (video_boot_stream.video_snapshot[ch]) {
		v_adp->cmd[ch]->CodecType = v_stream->type | CODEC_JPEG;
		v_adp->cmd[ch]->JpegMode = MODE_SNAPSHOT;
		v_adp->cmd[ch]->jpg_buf_size = jpeg_out_buf_size;////hal_video_jpg_buf(ch, jpeg_out_buf_size, out_rsvd_size);
		v_adp->cmd[ch]->jpg_rsvd_size = out_rsvd_size;
	}

	if (video_boot_stream.fcs_channel == 1 && ch != 0) { //Disable the fcs for channel 1
		v_adp->cmd[0]->fcs = 0;//Remove the fcs setup for channel one
		dcache_clean_invalidate_by_addr((uint32_t *)v_adp->cmd[0], sizeof(commandLine_s));
	}
	v_adp->cmd[ch]->out_buf_size  = out_buf_size;
	v_adp->cmd[ch]->out_rsvd_size = out_rsvd_size;
	v_adp->cmd[ch]->isp_buf_num = 2;

	if (video_boot_stream.fcs_isp_ae_enable) {
		v_adp->cmd[ch]->set_AE_init_flag = 1;
		v_adp->cmd[ch]->all_init_iq_set_flag = 1;
		v_adp->cmd[ch]->direct_i2c_mode = 1;
		v_adp->cmd[ch]->init_exposure = video_boot_stream.fcs_isp_ae_init_exposure;
		v_adp->cmd[ch]->init_gain = video_boot_stream.fcs_isp_ae_init_gain;
	}
	if (video_boot_stream.fcs_isp_awb_enable) {
		v_adp->cmd[ch]->set_AWB_init_flag = 1;
		v_adp->cmd[ch]->all_init_iq_set_flag = 1;
		v_adp->cmd[ch]->init_r_gain = video_boot_stream.fcs_isp_awb_init_rgain;
		v_adp->cmd[ch]->init_b_gain = video_boot_stream.fcs_isp_awb_init_bgain;
	}

	if (video_boot_stream.fcs_isp_init_daynight_mode) {
		v_adp->cmd[ch]->all_init_iq_set_flag = 1;
		v_adp->cmd[ch]->init_daynight_mode = video_boot_stream.fcs_isp_init_daynight_mode;
	}

	if (video_boot_stream.fcs_isp_gray_mode) {
		v_adp->cmd[ch]->all_init_iq_set_flag = 1;
		v_adp->cmd[ch]->gray_mode = video_boot_stream.fcs_isp_gray_mode;
	}

	if (video_boot_stream.video_drop_frame[ch]) {
		v_adp->cmd[ch]->all_init_iq_set_flag = 1;
		v_adp->cmd[ch]->drop_frame_num = video_boot_stream.video_drop_frame[ch];
	}

	if (v_stream->use_roi) {
		v_adp->cmd[ch]->roix = v_stream->roi.xmin;
		v_adp->cmd[ch]->roiy = v_stream->roi.ymin;
		v_adp->cmd[ch]->roiw = v_stream->roi.xmax - v_stream->roi.xmin;
		v_adp->cmd[ch]->roih = v_stream->roi.ymax - v_stream->roi.ymin;
	}

	if (v_stream->level) {
		v_adp->cmd[ch]->level = v_stream->level;
	}
	if (v_stream->profile) {
		v_adp->cmd[ch]->profile = v_stream->profile;
	}
	if (v_stream->cavlc) {
		v_adp->cmd[ch]->enableCabac = 0;
	}

	if (video_boot_stream.private_mask.enable) {
		video_boot_set_private_mask(ch, &video_boot_stream.private_mask);
	}
	if (video_boot_stream.meta_enable) {
		int meta_size = video_boot_stream.meta_size + sizeof(isp_meta_t) + sizeof(isp_statis_meta_t);

		meta_size = meta_size + meta_size / 4; //Add the extra buffer to dummy bytes
		if (meta_size % 32) { //align 32 byte
			meta_size = meta_size + (32 - (meta_size % 32));
		}
		video_boot_stream.fcs_meta_offset = meta_size / 0xff;
		video_boot_stream.fcs_meta_total_size = meta_size;
		v_adp->cmd[ch]->isp_meta_out = 1;
		v_adp->cmd[ch]->userData = meta_size;

		if (v_adp->cmd[ch]->userData > VIDEO_BOOT_META_REV_BUF) {
			dbg_printf("Meta size %d is exceed the sei buffer %d\r\n", v_adp->cmd[ch]->userData, VIDEO_BOOT_META_REV_BUF);
			v_adp->cmd[ch]->userData = VIDEO_BOOT_META_REV_BUF;
			dbg_printf("Setup the meta size as %d\r\n", VIDEO_BOOT_META_REV_BUF);
		}
	}
	if (video_boot_stream.init_isp_items.enable) {
		video_isp_initial_items_t init_items;
		v_adp->cmd[ch]->all_init_iq_set_flag = 1;
		init_items.init_brightness = video_boot_stream.init_isp_items.init_brightness;
		init_items.init_contrast = video_boot_stream.init_isp_items.init_contrast;
		init_items.init_flicker = video_boot_stream.init_isp_items.init_flicker;
		init_items.init_hdr_mode = video_boot_stream.init_isp_items.init_hdr_mode;;
		init_items.init_mirrorflip = video_boot_stream.init_isp_items.init_mirrorflip;
		init_items.init_saturation = video_boot_stream.init_isp_items.init_saturation;
		init_items.init_wdr_level = video_boot_stream.init_isp_items.init_wdr_level;
		init_items.init_wdr_mode = video_boot_stream.init_isp_items.init_wdr_mode;
		hal_video_set_isp_init_items(ch, &init_items);
	} else {
		video_isp_initial_items_t init_items;
		init_items.init_brightness = 0xffff;
		init_items.init_contrast = 0xffff;
		init_items.init_flicker = 0xffff;
		init_items.init_hdr_mode = 0xffff;
		init_items.init_mirrorflip = 0xffff;
		init_items.init_saturation = 0xffff;
		init_items.init_wdr_level = 0xffff;
		init_items.init_wdr_mode = 0xffff;
		hal_video_set_isp_init_items(ch, &init_items);
	}

	dcache_clean_invalidate_by_addr((uint32_t *)v_adp->cmd[ch], sizeof(commandLine_s));

	//v_stream->out_buf_size = out_buf_size;
	//v_stream->out_rsvd_size = out_rsvd_size;
	return OK;
}

/*** KM BOOT LOADER handling ***/
#define WAIT_FCS_DONE_TIMEOUT 	1000000

_WEAK void user_boot_config_init(void *parm)
{

}

_WEAK int user_disable_fcs(void)
{
	return 0;//default not to use
}

extern uint8_t bl4voe_shared_test[];
int video_btldr_process(voe_fcs_load_ctrl_t *pvoe_fcs_ld_ctrl, int *code_start)
{
	int ret = OK;
	unsigned int addr = 0;
	uint8_t *p_fcs_data = NULL, *p_iq_data = NULL, *p_sensor_data = NULL;
	uint8_t fcs_id;
	voe_cpy_t isp_cpy = NULL;
	isp_multi_fcs_ld_info_t *p_fcs_ld_info = NULL;
	int i = 0;
	int video_boot_struct_size = 0;

	if (NULL == pvoe_fcs_ld_ctrl) {
		dbg_printf("voe FCS ld ctrl is NULL \n");
		ret = FCS_CPY_FUNC_ERR;
		return ret;
	} else {
		isp_cpy       = pvoe_fcs_ld_ctrl->isp_cpy;
		p_fcs_ld_info = pvoe_fcs_ld_ctrl->p_fcs_ld_info;
	}

	fcs_id = p_fcs_ld_info->fcs_id;
	p_fcs_data    = (uint8_t *)((p_fcs_ld_info->fcs_hdr_start) + (p_fcs_ld_info->sensor_set[fcs_id].fcs_data_offset));
	p_iq_data     = (uint8_t *)(p_fcs_data + (p_fcs_ld_info->sensor_set[fcs_id].iq_start_addr));
	p_sensor_data = (uint8_t *)(p_fcs_data + (p_fcs_ld_info->sensor_set[fcs_id].sensor_start_addr));
	if (hal_voe_fcs_check_OK()) {
		user_boot_config_init(pvoe_fcs_ld_ctrl->p_fcs_para_raw);
		if ((video_boot_stream.fcs_isp_iq_id != 0) && (video_boot_stream.fcs_isp_iq_id < p_fcs_ld_info->multi_fcs_cnt)) {
			p_fcs_data    = (uint8_t *)((p_fcs_ld_info->fcs_hdr_start) + (p_fcs_ld_info->sensor_set[video_boot_stream.fcs_isp_iq_id].fcs_data_offset));
			p_iq_data     = (uint8_t *)(p_fcs_data + (p_fcs_ld_info->sensor_set[video_boot_stream.fcs_isp_iq_id].iq_start_addr));
			p_sensor_data = (uint8_t *)(p_fcs_data + (p_fcs_ld_info->sensor_set[video_boot_stream.fcs_isp_iq_id].sensor_start_addr));
		}
		hal_video_load_iq((voe_cpy_t)isp_cpy, (int *) p_iq_data, (int *) __voe_code_start__);
		hal_video_load_sensor((voe_cpy_t)isp_cpy, (int *) p_sensor_data, (int *) __voe_code_start__);
		int fcs_ch = -1;
		for (i = 0; i < MAX_FCS_CHANNEL; i++) {
			if (video_boot_stream.video_params[i].fcs) {
				video_boot_open(&video_boot_stream.video_params[i]);
			}
			if ((fcs_ch == -1) && (video_boot_stream.video_params[i].fcs == 1)) { //Get the first start channel
				fcs_ch = i;
			}
		}
		__DSB();

		pvoe_fcs_peri_info_t fcs_peri_info_for_ram = pvoe_fcs_ld_ctrl->p_fcs_peri_info;


		if (fcs_peri_info_for_ram->i2c_id <= 3) {
			hal_video_isp_set_i2c_id(fcs_ch, fcs_peri_info_for_ram->i2c_id);
		}

		if (fcs_peri_info_for_ram->fcs_data_verion == 0x1) {
			if (fcs_peri_info_for_ram->gpio_cnt > 3) {
				hal_video_isp_set_sensor_gpio(fcs_ch, fcs_peri_info_for_ram->gpio_list[3], fcs_peri_info_for_ram->gpio_list[2], fcs_peri_info_for_ram->gpio_list[0]);
				hal_pinmux_unregister(fcs_peri_info_for_ram->gpio_list[1], PID_GPIO);
				//dbg_printf("snr_pwr 0x%02x pwdn 0x%02x rst 0x%02x i2c_id %d \n", fcs_peri_info_for_ram->gpio_list[0], fcs_peri_info_for_ram->gpio_list[2], fcs_peri_info_for_ram->gpio_list[3], fcs_peri_info_for_ram->i2c_id);
			}
		} else {
			if (fcs_peri_info_for_ram->gpio_cnt > 2) {
				hal_video_isp_set_sensor_gpio(fcs_ch, fcs_peri_info_for_ram->gpio_list[1], fcs_peri_info_for_ram->gpio_list[2], fcs_peri_info_for_ram->gpio_list[0]);
				//dbg_printf("snr_pwr 0x%02x pwdn 0x%02x rst 0x%02x i2c_id %d \n", fcs_peri_info_for_ram->gpio_list[0], fcs_peri_info_for_ram->gpio_list[2], fcs_peri_info_for_ram->gpio_list[1], fcs_peri_info_for_ram->i2c_id);

			}
		}

		int voe_heap_size = video_boot_buf_calc(video_boot_stream);
		addr = video_boot_malloc(voe_heap_size);
		video_boot_stream.voe_heap_addr = addr;
		video_boot_stream.voe_heap_size = voe_heap_size;
		if (video_boot_stream.fcs_start_time) { //Measure the fcs time
			video_boot_stream.fcs_voe_time = (* ((volatile uint32_t *) 0xe0001004)) / (500000) + video_boot_stream.fcs_start_time;
		}
		ret = hal_video_init((long unsigned int *)addr, voe_heap_size);
		video_boot_stream.fcs_status = 1;
	}
	video_boot_stream.fcs_voe_fw_addr = (int)pvoe_fcs_ld_ctrl->fw_addr;
	memcpy(&video_boot_stream.p_fcs_ld_info, p_fcs_ld_info, sizeof(isp_multi_fcs_ld_info_t));
	video_boot_struct_size = sizeof(video_boot_stream_t);
	if (fcs_flag == 1 && video_boot_stream.fcs_status == 0) {
		video_boot_stream.p_fcs_ld_info.fcs_id = 0;
	}
	if (video_boot_struct_size <= VIDEO_BOOT_STRUCT_MAX_SIZE) {
		memcpy(bl4voe_shared_test, &video_boot_stream, sizeof(video_boot_stream_t));
	} else {
		memcpy(bl4voe_shared_test, &video_boot_stream, VIDEO_BOOT_STRUCT_MAX_SIZE);
	}
	return ret;
}
extern void hal_voe_set_kmfw_base_addr(u32 val);
int video_btldr_fcs_terminated(voe_fcs_load_ctrl_t *pvoe_fcs_ld_ctrl)
{

	pvoe_fcs_peri_info_t fcs_peri_info_for_ram = pvoe_fcs_ld_ctrl->p_fcs_peri_info;
	hal_gpio_adapter_t sensor_en_gpio;
	volatile hal_i2c_adapter_t	i2c_master_video;
	hal_status_t ret = 0;

	int terminated_flag = user_disable_fcs();  // Application set terminate or not
	fcs_flag = terminated_flag;

	if (terminated_flag) {
		// disable I2C
		i2c_master_video.pltf_dat.scl_pin = fcs_peri_info_for_ram->i2c_scl;
		i2c_master_video.pltf_dat.sda_pin = fcs_peri_info_for_ram->i2c_sda;
		i2c_master_video.init_dat.index = fcs_peri_info_for_ram->i2c_id;
		ret = hal_i2c_pin_unregister_simple(&i2c_master_video);
		if (ret) {
			dbg_printf("[FCS Terminate] hal_i2c_pin_unregister_simple failed %d \n", ret);
		}

		if (fcs_peri_info_for_ram->gpio_cnt > 0) {

			// unregister pwr_ctrl_pin
			ret = hal_pinmux_unregister(fcs_peri_info_for_ram->gpio_list[0], PID_GPIO);
			if (ret) {
				dbg_printf("[FCS Terminate] hal_pinmux_unregister pwr_ctrl pin failed %d \n", ret);
			} else {
				// turn off sensor power
				ret = hal_gpio_init(&sensor_en_gpio, fcs_peri_info_for_ram->gpio_list[0]);
				if (ret) {
					dbg_printf("[FCS Terminate] hal_gpio_init pwr_ctrl pin failed %d \n", ret);
				} else {
					hal_gpio_set_dir(&sensor_en_gpio, GPIO_OUT);
					hal_gpio_write(&sensor_en_gpio, 0);
					hal_gpio_deinit(&sensor_en_gpio);
				}

			}
			// unregister GPIO
			for (int i = 1;  i < fcs_peri_info_for_ram->gpio_cnt; i++) {
				ret = hal_pinmux_unregister(fcs_peri_info_for_ram->gpio_list[i], PID_GPIO);
				if (ret) {
					dbg_printf("[FCS Terminate] hal_pinmux_unregister GPIO[%d] failed %d \n", i, ret);
				}
			}
			ret = hal_pinmux_unregister(fcs_peri_info_for_ram->snr_clk_pin, PID_SENSOR);
			if (ret) {
				dbg_printf("[FCS Terminate] hal_pinmux_unregister snr_clk_pin failed %d \n", ret);
			}
		}

		fcs_peri_info_for_ram->fcs_OK = 0;
		hal_voe_set_kmfw_base_addr(FCS_RUN_DATA_NG_KM);

	}
	return 0;
}
