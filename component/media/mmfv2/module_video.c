/******************************************************************************
*
* Copyright(c) 2021 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

#include <osdep_service.h>
#include "mmf2.h"
#include "mmf2_dbg.h"

#include "module_video.h"
#include "module_rtsp2.h"


#include <math.h>
#include "platform_stdlib.h"

#include <unistd.h>
#include <sys/wait.h>

#include "base_type.h"

#include "cmsis.h"
#include "error.h"


#include "hal.h"
#include "hal_video.h"

#include "ftl_common_api.h"

#define OSD_ENABLE 1
#define HDR_ENABLE 0

int framecnt = 0;
int jpegcnt = 0;
int incb[5] = {0, 0, 0, 0, 0};
int enc_queue_cnt[5] = {0, 0, 0, 0, 0};
int ch1framecnt = 0;
int ch2framecnt = 0;
int rgb_lock = 0;

#define MMF_VIDEO_DEBUG 0

///////////////////
#include "fw_img_export.h"
#include "sensor.h"
#include "video_boot.h"
int sensor_id_value = 0;
int isp_get_id(void);
int isp_set_sensor(int sensor_id);
void video_save_sensor_id(int SensorName);
extern video_boot_stream_t *isp_boot;
static int(*sensor_setup)(int status, int sensor_id) = NULL;
//////////////////
static int flash_sensor_id = -1;
static isp_info_t info;

#define CH_NUM 5
static int show_fps = 0;
static int ch_fps_cnt[CH_NUM]   = {0};
static int cb_tick[CH_NUM]   = {0};
static int ch_fps[CH_NUM]   = {0};

void video_show_fps(int enable)
{
	show_fps = enable;
}
int video_get_cb_fps(int chn)
{
	if (chn < 0 || chn > 4) {
		printf("[%s] %d is invalid, chn range is 0~4", chn, __FUNCTION__);
	}
	return ch_fps[chn];
}

void video_rate_control_process(video_ctx_t *ctx)
{
	int fps = 0;
	static int switch_fps[2] = {1, 1};
	float mul = (float) ctx->params.gop / ctx->rate_ctrl_p.current_framerate;

	if ((ctx->rate_ctrl_p.sample_bitrate > (ctx->rate_ctrl_p.rate_ctrl.maximun_bitrate * mul) && switch_fps[ctx->params.stream_id] == 1)) {
		fps = ctx->rate_ctrl_p.current_framerate / 2;
		VIDEO_DBG_INFO("\r\nch = %d sample rate = %ld	maximun bitrate = %ld	fps = %d\r\n",
					   ctx->params.stream_id, ctx->rate_ctrl_p.sample_bitrate, ctx->rate_ctrl_p.rate_ctrl.maximun_bitrate, fps);
		video_ctrl(ctx->params.stream_id, VIDEO_FPS, fps);
		switch_fps[ctx->params.stream_id] ^= 1;
	} else if ((ctx->rate_ctrl_p.sample_bitrate < (ctx->rate_ctrl_p.rate_ctrl.minimum_bitrate * mul) && switch_fps[ctx->params.stream_id] == 0)) {
		fps = ctx->rate_ctrl_p.current_framerate;
		VIDEO_DBG_INFO("\r\nch = %d sample rate = %ld	minimum bitrate = %ld	fps = %d\r\n",
					   ctx->params.stream_id, ctx->rate_ctrl_p.sample_bitrate, ctx->rate_ctrl_p.rate_ctrl.minimum_bitrate, fps);
		video_ctrl(ctx->params.stream_id, VIDEO_FPS, fps);
		switch_fps[ctx->params.stream_id] ^= 1;
	}
}

void video_rate_control_moniter_sample_rate(video_ctx_t *ctx, uint32_t frame_size)
{
	static uint32_t cnt_sr[2] = {0, 0};
	static uint32_t cnt_br[2] = {0, 0};
	static uint32_t sum_sr[2] = {0, 0};
	static uint32_t sum_br[2] = {0, 0};

	if (frame_size > 0) {
		cnt_br[ctx->params.stream_id]++;
		sum_br[ctx->params.stream_id] += frame_size;
		if (ctx->rate_ctrl_p.rate_ctrl_en) {
			cnt_sr[ctx->params.stream_id]++;
			sum_sr[ctx->params.stream_id] += frame_size;
			if (cnt_sr[ctx->params.stream_id] >= ctx->rate_ctrl_p.rate_ctrl.sampling_time) {
				ctx->rate_ctrl_p.sample_bitrate = sum_sr[ctx->params.stream_id] * 8;
				sum_sr[ctx->params.stream_id] = 0;
				cnt_sr[ctx->params.stream_id] = 0;
				video_rate_control_process(ctx);
			}
			if (cnt_br[ctx->params.stream_id] >= ctx->rate_ctrl_p.current_framerate) {
				ctx->rate_ctrl_p.current_bitrate = sum_br[ctx->params.stream_id] * 8;
				sum_br[ctx->params.stream_id] = 0;
				cnt_br[ctx->params.stream_id] = 0;
			}
		} else {
			if (cnt_br[ctx->params.stream_id] >= ctx->rate_ctrl_p.current_framerate) {
				ctx->rate_ctrl_p.current_bitrate = sum_br[ctx->params.stream_id] * 8;
				sum_br[ctx->params.stream_id] = 0;
				cnt_br[ctx->params.stream_id] = 0;
			}
		}
	}
}

isp_statis_meta_t _meta;
void video_default_meta_cb(void *parm)
{
	video_meta_t *m_parm = (video_meta_t *)parm;
	unsigned char *ptr = (unsigned char *)m_parm->video_addr;
	if (m_parm->type == AV_CODEC_ID_MJPEG) {
		video_sei_write(ptr + m_parm->meta_offset + VIDEO_JPEG_META_OFFSET + video_get_meta_offset(), m_parm->isp_statis_meta, m_parm->isp_meta_data, NULL, 0);
	} else if (m_parm->type == AV_CODEC_ID_H264) {
		video_sei_write(ptr + m_parm->meta_offset + VIDEO_H264_META_OFFSET + video_get_meta_offset(), m_parm->isp_statis_meta, m_parm->isp_meta_data, NULL, 0);
	} else if (m_parm->type == AV_CODEC_ID_H265) {
		video_sei_write(ptr + m_parm->meta_offset + VIDEO_HEVC_META_OFFSET + video_get_meta_offset(), m_parm->isp_statis_meta, m_parm->isp_meta_data, NULL, 0);
	} else {
		//printf("It don't support %d\r\n",m_parm->type);
	}
}

void video_ch4_delay_release_task(void *param)
{
	while (video_get_stream_info(4)) {
		vTaskDelay(33);
		if (video_ispbuf_release(4, (int)param) == OK) {
			break;
		}
	}
	vTaskDelete(NULL);
}

void video_ch4_delay_release(int ch4_release_fail_add)
{
	if (xTaskCreate(video_ch4_delay_release_task, ((const char *)"ch4_release"), 256, (void *)ch4_release_fail_add, tskIDLE_PRIORITY + 4, NULL) != pdPASS) {
		printf("\r\n video_ch4_delay_release: Create Task Error\n");
	}
}
void video_fix_param(video_ctx_t *ctx, int id)
{
	struct sensor_params_t cur_snr = sensor_params[sen_id[id]];
	if ((ctx->params.width > cur_snr.sensor_width || ctx->params.height > cur_snr.sensor_height) && (ctx->params.scale_up_en == 0)) {
		ctx->params.width = cur_snr.sensor_width;
		ctx->params.height = cur_snr.sensor_height;
	}
	if (ctx->params.fps > cur_snr.sensor_fps) {
		ctx->params.gop = (cur_snr.sensor_fps * ctx->params.gop) / ctx->params.fps;
		ctx->params.fps = cur_snr.sensor_fps;
	}
	info.sensor_fps = cur_snr.sensor_fps;
	video_set_isp_info(&info);
}
static uint32_t ch4_last_frame_tick = 0;
void video_frame_complete_cb(void *param1, void  *param2, uint32_t arg)
{

	enc2out_t *enc2out = (enc2out_t *)param1;
	incb[enc2out->ch] = 1;
	hal_video_adapter_t  *v_adp = (hal_video_adapter_t *)param2;
	commandLine_s *cml = (commandLine_s *)&v_adp->cmd[enc2out->ch];
	video_ctx_t *ctx = (video_ctx_t *)arg;
	mm_context_t *mctx = (mm_context_t *)ctx->parent;
	mm_queue_item_t *output_item;

	//uint32_t timestamp = video_get_system_ts_from_isp_ts(xTaskGetTickCount(), enc2out->time_stamp) + ctx->timestamp_offset;
	uint32_t timestamp = video_get_system_ts_from_isp_ts(mm_read_mediatime_ms(), enc2out->time_stamp, enc2out->ch) + ctx->timestamp_offset;

	int is_output_ready = 0;

#if MMF_VIDEO_DEBUG
	if (enc2out->codec & CODEC_JPEG) {
		VIDEO_DBG_INFO("jpeg in = 0x%X\r\n", enc2out->jpg_addr);
	} else if (enc2out->codec & CODEC_H264 || enc2out->codec & CODEC_HEVC) {
		VIDEO_DBG_INFO("hevc/h264 in = 0x%X\r\n", enc2out->enc_addr);
	} else {
		VIDEO_DBG_INFO("nv12/nv16/rgb in = 0x%X\r\n", enc2out->isp_addr);
	}
#endif

	// VOE status check
	if (enc2out->cmd_status == VOE_OK) {
		// Normal frame output
		//printf("<<test>><%s><%d> %x\n", __func__, __LINE__, enc2out->cmd);
	} else {
		// Video error handle

		switch (enc2out->cmd_status) {
		case VOE_ENC_BUF_OVERFLOW:
		case VOE_ENC_QUEUE_OVERFLOW:
			VIDEO_DBG_WARNING("VOE CH%d ENC %s full (queue/used/out/rsvd) %d/%dKB%dKB%dKB\n"
							  , enc2out->ch
							  , enc2out->cmd_status == VOE_ENC_BUF_OVERFLOW ? "buff" : "queue"
							  , enc2out->enc_time
							  , enc2out->enc_used >> 10
							  , ctx->params.out_buf_size >> 10
							  , ctx->params.out_rsvd_size >> 10);
			video_encbuf_clean(enc2out->ch, CODEC_H264 | CODEC_HEVC);
			video_ctrl(enc2out->ch, VIDEO_FORCE_IFRAME, 1);
			//enc_queue_cnt[enc2out->ch] = 0;
			break;
		case VOE_JPG_BUF_OVERFLOW:
		case VOE_JPG_QUEUE_OVERFLOW:
			VIDEO_DBG_WARNING("VOE CH%d JPG %s full (queue/used/out/rsvd) %d/%dKB\n"
							  , enc2out->ch
							  , enc2out->cmd_status == VOE_JPG_BUF_OVERFLOW ? "buff" : "queue"
							  , enc2out->jpg_time
							  , enc2out->jpg_used >> 10);
			//video_encbuf_clean(enc2out->ch, CODEC_JPEG);
			//enc_queue_cnt[enc2out->ch] = 0;
			break;
		default:
			VIDEO_DBG_ERROR("Error CH%d VOE cmd %x status %x\n", enc2out->ch, enc2out->cmd, enc2out->cmd_status);
			break;
		}
		incb[enc2out->ch] = 0;
		return;
	}

	if (ctx->params.direct_output == 1) {
		goto show_log;
	}

	// Snapshot JPEG
	if (enc2out->codec & CODEC_JPEG && enc2out->jpg_len > 0) { // JPEG
		if (ctx->snapshot_cb != NULL) {
			if (ctx->meta_cb) {
				video_meta_t parm;
				parm.type = AV_CODEC_ID_MJPEG;
				parm.video_addr = (uint32_t)enc2out->jpg_addr;
				parm.video_len = enc2out->jpg_len;
				parm.meta_offset = enc2out->meta_offset;
				parm.isp_meta_data = &(enc2out->isp_meta_data);
				parm.isp_statis_meta = &(enc2out->statis_data);
				ctx->meta_cb(&parm);
			}
			ctx->snapshot_cb((uint32_t)enc2out->jpg_addr, enc2out->jpg_len);
			video_encbuf_release(enc2out->ch, CODEC_JPEG, enc2out->jpg_len);
		} else {
			char *tempaddr = NULL;
			if (ctx->params.use_static_addr == 0) {
				tempaddr = (char *)malloc(enc2out->jpg_len);
				if (tempaddr == NULL) {
					video_encbuf_release(enc2out->ch, enc2out->codec, enc2out->jpg_len);
					VIDEO_DBG_ERROR("malloc fail = %d\r\n", enc2out->jpg_len);
					goto show_log;
				}
			}

			is_output_ready = xQueueReceive(mctx->output_recycle, (void *)&output_item, 0);
			if (is_output_ready) {
				if (ctx->params.use_static_addr) {
					output_item->data_addr = (uint32_t)enc2out->jpg_addr;
				} else {
					output_item->data_addr = (uint32_t)tempaddr;//malloc(enc2out->jpg_len);
					memcpy((void *)output_item->data_addr, (char *)enc2out->jpg_addr, enc2out->jpg_len);
					video_encbuf_release(enc2out->ch, enc2out->codec, enc2out->jpg_len);
				}
				output_item->size = enc2out->jpg_len;
				output_item->timestamp = timestamp;
				output_item->hw_timestamp = enc2out->time_stamp;
				output_item->type = AV_CODEC_ID_MJPEG;
				output_item->priv_data = enc2out->jpg_slot;//JPEG buffer used slot

				if (ctx->meta_cb) {
					video_meta_t parm;
					parm.type = AV_CODEC_ID_MJPEG;
					parm.video_addr = (uint32_t)enc2out->jpg_addr;
					parm.video_len = enc2out->jpg_len;
					parm.meta_offset = enc2out->meta_offset;
					parm.isp_meta_data = &(enc2out->isp_meta_data);
					parm.isp_statis_meta = &(enc2out->statis_data);
					ctx->meta_cb(&parm);
				}

				if (xQueueSend(mctx->output_ready, (void *)&output_item, 0) != pdTRUE) {
					video_encbuf_release(enc2out->ch, enc2out->codec, enc2out->jpg_len);
				} else {
					enc_queue_cnt[enc2out->ch]++;
				}

			} else {
				VIDEO_DBG_INFO("\r\n CH %d MMF JPEG Queue full \r\n", enc2out->ch);
				if (ctx->params.use_static_addr == 0) {
					free(tempaddr);
				} else {
					video_encbuf_release(enc2out->ch, enc2out->codec, enc2out->jpg_len);
				}
			}
		}
	}

	if (/*enc2out->enc_len > 0 && */(enc2out->codec & CODEC_H264 || enc2out->codec & CODEC_HEVC ||
									 enc2out->codec & CODEC_RGB || enc2out->codec & CODEC_NV12 ||
									 enc2out->codec & CODEC_NV16)) {
		char *tempaddr = NULL;

		is_output_ready = xQueueReceive(mctx->output_recycle, (void *)&output_item, 0);
		if (is_output_ready) {
			if (enc2out->codec == CODEC_H264 || enc2out->codec == (CODEC_H264 | CODEC_JPEG)) {
				output_item->type = AV_CODEC_ID_H264;
				output_item->size = enc2out->enc_len;
				video_rate_control_moniter_sample_rate(ctx, output_item->size);
			} else if (enc2out->codec == CODEC_HEVC || enc2out->codec == (CODEC_HEVC | CODEC_JPEG)) {
				output_item->type = AV_CODEC_ID_H265;
				output_item->size = enc2out->enc_len;
			} else if (enc2out->codec == CODEC_RGB) {
				output_item->type = AV_CODEC_ID_RGB888;
				output_item->size = enc2out->width * enc2out->height * 3;
			} else if (enc2out->codec == CODEC_NV12) {
				output_item->type = AV_CODEC_ID_NV12;
				output_item->size = enc2out->width * enc2out->height * 3 / 2;
			} else if (enc2out->codec == CODEC_NV16) {
				output_item->type = AV_CODEC_ID_NV16;
				output_item->size = enc2out->width * enc2out->height * 2;
			}

			if (ctx->params.use_static_addr == 0) {
				tempaddr = (char *)malloc(output_item->size);
				if (tempaddr == NULL) {
					VIDEO_DBG_ERROR("malloc fail = %d\r\n", output_item->size);
					if ((enc2out->codec & (CODEC_NV12 | CODEC_RGB | CODEC_NV16)) != 0) {
						video_ispbuf_release(enc2out->ch, (int)enc2out->isp_addr);
					} else {
						video_encbuf_release(enc2out->ch, enc2out->codec, output_item->size);
					}
					goto show_log;
				}
			}

			if (enc2out->codec & (CODEC_H264 | CODEC_HEVC)) {
				if (enc2out->codec == CODEC_H264 || enc2out->codec == CODEC_HEVC || enc2out->codec == (CODEC_H264 | CODEC_JPEG) ||
					enc2out->codec == (CODEC_HEVC | CODEC_JPEG)) {
					uint8_t *ptr = (uint8_t *)enc2out->enc_addr;
					if (ptr[0] != 0 || ptr[1] != 0) {
						VIDEO_DBG_ERROR("\r\nH264 stream error\r\n");
						VIDEO_DBG_ERROR("\r\n(%d/%d) %x %x %x %x\r\n", enc2out->enc_len, enc2out->finish, *ptr, *(ptr + 1), *(ptr + 2), *(ptr + 3));
					}
					if ((enc2out->codec & (CODEC_H264)) && (ctx->params.sps_pps_info.enable) && (ctx->params.sps_pps_info.status == 0)) {
						int type = ptr[4] & 0x1F;
						if (ptr[0] == 0 && ptr[1] == 0 && ptr[2] == 0 && ptr[3] == 1 && type == 0x07) {
							int ret = 0;
							ret = video_get_sps_pps(ptr + 4, enc2out->enc_len, enc2out->ch, &ctx->params.sps_pps_info);
							if (ret >= 0) {
								ctx->params.sps_pps_info.status = 1;
								if (ctx->sps_pps_cb) {
									ctx->sps_pps_cb(NULL);
								}
							}
						}
					}

					if ((enc2out->codec & (CODEC_HEVC)) && (ctx->params.sps_pps_info.enable) && (ctx->params.sps_pps_info.status == 0)) {
						int type = ptr[4];
						if (ptr[0] == 0 && ptr[1] == 0 && ptr[2] == 0 && ptr[3] == 1 && type == 0x40) {
							int ret = 0;
							ret = video_get_sps_pps_vps(ptr + 4, enc2out->enc_len, enc2out->ch, &ctx->params.sps_pps_info);
							if (ret >= 0) {
								ctx->params.sps_pps_info.status = 1;
								if (ctx->sps_pps_cb) {
									ctx->sps_pps_cb(NULL);
								}
							}
						}
					}
				}
				if (ctx->meta_cb) {
					video_meta_t parm;
					parm.meta_offset = enc2out->meta_offset;
					parm.type = output_item->type;
					parm.video_addr = (uint32_t)enc2out->enc_addr;
					parm.video_len = enc2out->enc_len;
					parm.isp_meta_data = &(enc2out->isp_meta_data);//frame_count
					parm.isp_statis_meta = &(enc2out->statis_data);
					ctx->meta_cb(&parm);
				}
				/* } */
				if (ctx->params.use_static_addr) {
					output_item->data_addr = (uint32_t)enc2out->enc_addr;
				} else {
					output_item->data_addr = (uint32_t)tempaddr;//malloc(enc2out->enc_len);
					memcpy((void *)output_item->data_addr, (char *)enc2out->enc_addr, output_item->size);
					if (ctx->params.use_static_addr == 0) {
						video_encbuf_release(enc2out->ch, enc2out->codec, output_item->size);
					}
				}

			} else {
				if (ctx->params.use_static_addr) {
					output_item->data_addr = (uint32_t)enc2out->isp_addr;
				} else {
					output_item->data_addr = (uint32_t)tempaddr;//malloc(enc2out->enc_len);
					memcpy((void *)output_item->data_addr, (char *)enc2out->isp_addr, output_item->size);
					video_ispbuf_release(enc2out->ch, (int)enc2out->isp_addr);
				}
			}

			output_item->timestamp = timestamp; //rtp timestamp
			output_item->hw_timestamp = enc2out->time_stamp;
			output_item->priv_data = enc2out->enc_slot;//ENC buffer used slot

			if (show_fps) {
				if (mm_read_mediatime_ms() - cb_tick[enc2out->ch] > 1000) {
					cb_tick[enc2out->ch] = mm_read_mediatime_ms();
					printf("[CH:%d] fps:%d.\r\n", enc2out->ch, ch_fps_cnt[enc2out->ch] - 1);
					ch_fps[enc2out->ch] = ch_fps_cnt[enc2out->ch] - 1;
					ch_fps_cnt[enc2out->ch] = 0;
				}
				ch_fps_cnt[enc2out->ch]++;
			}
			if (voe_boot_fsc_status()) {
				static int queue_len = 0;
				static int queue_timestamp = 0;
				static int queue_initial = 0;
				video_boot_stream_t *isp_fcs_info;
				video_get_fcs_info(&isp_fcs_info);
				//printf("===================voe_boot_fsc_status %d========================\r\n", queue_len);
				if (queue_initial == 0x00) {
					if (isp_fcs_info->video_params[enc2out->ch].fcs) {
						queue_len = output_item->priv_data;
						queue_timestamp = timestamp;
						VIDEO_DBG_INFO("output_item->priv_data %d %d\r\n", output_item->priv_data, queue_timestamp);
						queue_initial = 1;
						video_set_fcs_queue_info(queue_timestamp, queue_timestamp);
					}
				}
				if (queue_len > 0) {
					//output_item->timestamp = queue_timestamp - (queue_len - 1) * (1000 / isp_fcs_info->video_params[enc2out->ch].fps);
					//VIDEO_DBG_INFO("queue_len %d %d\r\n", queue_len, output_item->timestamp);
					VIDEO_DBG_INFO("queue_len %d %d\r\n", queue_len, output_item->timestamp);
					queue_len = queue_len - 1;
				}
			}
			//printf("Video TS => KM: %d en %d, TM: %d, diff: %d\r\n", enc2out->time_stamp, enc2out->enc_time, output_item->timestamp, enc2out->time_stamp - output_item->timestamp);
			if (xQueueSend(mctx->output_ready, (void *)&output_item, 0) != pdTRUE) {
				if (enc2out->codec <= CODEC_JPEG) {
					video_encbuf_release(enc2out->ch, enc2out->codec, output_item->size);
				} else {
					video_ispbuf_release(enc2out->ch, (int)enc2out->isp_addr);
				}
			} else {
				enc_queue_cnt[enc2out->ch]++;
				//printf("CH %d MMF enc_queue_cnt = %d\r\n",enc2out->ch, enc_queue_cnt[enc2out->ch]);
				// if (enc_queue_cnt[enc2out->ch] >= 90) {
				// printf("CH %d VOE queue full: MMF clean queue!!\r\n");
				// video_encbuf_clean(enc2out->ch, CODEC_H264 | CODEC_HEVC);
				// enc_queue_cnt[enc2out->ch] = 0;
				// return;
				// }
			}

		} else {
			VIDEO_DBG_WARNING("\r\n CH %d MMF ENC Queue full \r\n", enc2out->ch);
			//video_encbuf_clean(enc2out->ch, CODEC_H264 | CODEC_HEVC);
			//enc_queue_cnt[enc2out->ch] = 0;

			if (enc2out->codec <= CODEC_JPEG) {
				video_encbuf_release(enc2out->ch, enc2out->codec, enc2out->enc_len);
			} else {
				int ret = video_ispbuf_release(enc2out->ch, (int)enc2out->isp_addr);
				if (ret == NOK && enc2out->ch == 4) {
					video_ch4_delay_release((int)enc2out->isp_addr);
				}
			}
		}
	}

show_log:

	if (ctx->params.direct_output == 1) {

		if (enc2out->codec & (CODEC_H264 | CODEC_HEVC)) {
			VIDEO_DBG_INFO("(%s-%s)(0x%X -- %d)(ch%d)(wh=%d x %d) \n"
						   , (enc2out->codec & CODEC_H264) != 0 ? "H264" : "HEVC"
						   , (enc2out->type == VCENC_INTRA_FRAME) ? "I" : "P"
						   , enc2out->enc_addr, enc2out->enc_len, enc2out->ch, enc2out->width, enc2out->height);

		}
	}


	if (ctx->params.direct_output == 1) {
		if ((enc2out->codec & (CODEC_H264 | CODEC_HEVC)) != 0) {
			video_encbuf_release(enc2out->ch, enc2out->codec, enc2out->enc_len);
		} else if ((enc2out->codec & (CODEC_NV12 | CODEC_RGB | CODEC_NV16)) != 0) {
			video_ispbuf_release(enc2out->ch, (int)enc2out->isp_addr);
		} else if ((enc2out->codec & CODEC_JPEG) != 0) {
			video_encbuf_release(enc2out->ch, enc2out->codec, enc2out->jpg_len);
		}
	}

	//close output task
	if (enc2out->finish == LAST_FRAME) {

	}
	incb[enc2out->ch] = 0;
}

int video_control(void *p, int cmd, int arg)
{
	video_ctx_t *ctx = (video_ctx_t *)p;
	mm_context_t *mctx = (mm_context_t *)ctx->parent;
	mm_queue_item_t *tmp_item;

	switch (cmd) {
	case CMD_VIDEO_SET_PARAMS:
		memcpy(&ctx->params, (void *)arg, sizeof(video_params_t));
		ctx->rate_ctrl_p.current_framerate = ctx->params.fps;
		break;
	case CMD_VIDEO_GET_PARAMS:
		memcpy((void *)arg, &ctx->params, sizeof(video_params_t));
		break;
	case CMD_VIDEO_SET_RCPARAM: {
		int ch = ctx->params.stream_id;
		video_ctrl(ch, VIDEO_SET_RCPARAM, arg);
	}
	break;
	case CMD_VIDEO_STREAMID:
		ctx->params.stream_id = arg;
		break;
	case CMD_VIDEO_STREAM_STOP: {
		int ch = ctx->params.stream_id;
		while (incb[ch]) {
			vTaskDelay(1);
		}

		if (enc_queue_cnt[ch] > 0) {
			VIDEO_DBG_INFO("CH %d MMF enc_queue_cnt = %d\r\n", ch, enc_queue_cnt[ch]);
			video_encbuf_clean(ch, CODEC_H264 | CODEC_HEVC | CODEC_JPEG);
		}
		enc_queue_cnt[ch] = 0;
		vTaskDelay(10);
		ctx->rate_ctrl_p.rate_ctrl_en = 0;
		return video_close(ch);
	}
	break;
	case CMD_VIDEO_FORCE_IFRAME: {
		int ch = ctx->params.stream_id;
		video_ctrl(ch, VIDEO_FORCE_IFRAME, arg);
	}
	break;
	case CMD_VIDEO_BPS: {
		int ch = ctx->params.stream_id;
		video_ctrl(ch, VIDEO_BPS, arg);
	}
	break;
	case CMD_VIDEO_GOP: {
		int ch = ctx->params.stream_id;
		video_ctrl(ch, VIDEO_GOP, arg);
	}
	break;
	case CMD_VIDEO_FPS: {
		int ch = ctx->params.stream_id;
		video_ctrl(ch, VIDEO_FPS, arg);
		ctx->rate_ctrl_p.current_framerate = arg;
	}
	break;
	case CMD_VIDEO_ISPFPS: {
		int ch = ctx->params.stream_id;
		video_ctrl(ch, VIDEO_ISPFPS, arg);
	}
	break;
	case CMD_VIDEO_SNAPSHOT: {
		int ch = ctx->params.stream_id;
		video_ctrl(ch, VIDEO_JPEG_OUTPUT, arg);
	}
	break;
	case CMD_VIDEO_YUV: {
		int ch = ctx->params.stream_id;
		int type = ctx->params.type;
		switch (type) {
		case 0:
			VIDEO_DBG_ERROR("wrong type %d\r\n", type);
			break;
		case 1:
			VIDEO_DBG_ERROR("wrong type %d\r\n", type);
			break;
		case 2:
			VIDEO_DBG_ERROR("wrong type %d\r\n", type);
			break;
		case 3:
			video_ctrl(ch, VIDEO_NV12_OUTPUT, arg);
			break;
		case 4:
			video_ctrl(ch, VIDEO_RGB_OUTPUT, arg);
			break;
		case 5:
			video_ctrl(ch, VIDEO_NV16_OUTPUT, arg);
			break;
		case 6:
			VIDEO_DBG_ERROR("wrong type %d\r\n", type);
			break;
		case 7:
			VIDEO_DBG_ERROR("wrong type %d\r\n", type);
			break;
		}

	}
	break;
	case CMD_ISP_SET_RAWFMT: {
		int ch = ctx->params.stream_id;
		video_ctrl(ch, VIDEO_ISP_SET_RAWFMT, arg);
	}
	break;
	case CMD_VIDEO_SNAPSHOT_CB:
		ctx->snapshot_cb = (int (*)(uint32_t, uint32_t))arg;
		break;
	case CMD_VIDEO_META_CB:
		if (arg == MMF_VIDEO_DEFAULT_META_CB) {
			ctx->meta_cb = video_default_meta_cb;
		} else {
			ctx->meta_cb = (void (*)(void *))arg;
		}
		break;
	case CMD_VIDEO_UPDATE:

		break;
	case CMD_VIDEO_SET_VOE_HEAP:

		break;
	case CMD_VIDEO_PRINT_INFO: {
		int ch = ctx->params.stream_id;
		video_ctrl(ch, VIDEO_PRINT_INFO, arg);
	}
	break;
	case CMD_VIDEO_SET_SPS_PPS_INFO: {
		ctx->params.sps_pps_info.status = 0;//clean the before status
		ctx->params.sps_pps_info.enable = 1;//Enable to get the sps pps info
	}
	break;
	case CMD_VIDEO_GET_SPS_PPS_INFO: {
		if (ctx->params.sps_pps_info.status) {
			memcpy((void *)arg, &ctx->params, sizeof(video_params_t));
		}
		return ctx->params.sps_pps_info.status;;
	}
	break;
	case CMD_VIDEO_APPLY: {
		int ch = arg;
		int ret = 0;
		ctx->params.stream_id = ch;
		ctx->rate_ctrl_p.rate_ctrl_en = 0;

		video_params_t video_params = ctx->params;
		if (sensor_id_value > 0 && sensor_id_value < SENSOR_MAX) {
			video_fix_param(ctx, sensor_id_value);
		} else if (flash_sensor_id >= 0 && flash_sensor_id < SENSOR_MAX) {
			video_fix_param(ctx, flash_sensor_id);
		}

		ret = video_open(&ctx->params, video_frame_complete_cb, ctx);
		ctx->params = video_params;
#if MULTI_SENSOR
		if (ret < 0 && video_get_video_sensor_status() == 0) { //Change the sensor procedure
#if NONE_FCS_MODE
			for (int id = 0; id < isp_boot->p_fcs_ld_info.multi_fcs_cnt; id++) {
#else
			for (int id = 1; id < isp_boot->p_fcs_ld_info.multi_fcs_cnt; id++) {
#endif
				if (sensor_setup) {
					ret = sensor_setup(SENSOR_MULTI_SETUP_PROCEDURE, id);
					if (ret < 0) { //Skip the sensor
						continue;
					}
				}
				video_params = ctx->params;
				if (1) {
					video_reset_fw(ch, id);

					video_fix_param(ctx, id);
					ret = video_open(&ctx->params, video_frame_complete_cb, ctx);
					if (ret >= 0) {
						sensor_id_value = id;
						video_save_sensor_id(id);
						flash_sensor_id = id;
						VIDEO_DBG_INFO("It find the correct sesnor %d\r\n", id);
						break;
					}
				}
				ctx->params = video_params;
			}
		} else {
			if (video_get_video_sensor_status() == 0) {
				video_save_sensor_id(sensor_id_value);
			}
		}
#else
		if ((voe_boot_fsc_status() == 0) && (voe_boot_fsc_id() > 0) && (video_get_video_sensor_status() == 0)) {
			//The fcs mode fail that it need to do the reset.
			VIDEO_DBG_INFO("Reset fcs mode to common mode\r\n");
			video_reset_fw(ch, sensor_id_value);
			if (sensor_id_value > 0 && sensor_id_value < SENSOR_MAX) {
				video_fix_param(ctx, sensor_id_value);
			}
			ret = video_open(&ctx->params, video_frame_complete_cb, ctx);
		}
		if (ret < 0 && video_get_video_sensor_status() == 0) { //Change the sensor procedure
			VIDEO_DBG_ERROR("Please check sensor id first,the id is %d\r\n", sensor_id_value);
			return -1;
		} else {
			if (video_get_video_sensor_status() == 0) {
				video_save_sensor_id(sensor_id_value);
			}
		}
#endif
	}
	break;
	case CMD_VIDEO_SET_TIMESTAMP_OFFSET: {
		ctx->timestamp_offset = arg;
	}
	break;
	case CMD_VIDEO_SET_RATE_CONTROL: {
		memcpy(&ctx->rate_ctrl_p.rate_ctrl, (void *)arg, sizeof(rate_ctrl_t));
		if ((ctx->rate_ctrl_p.rate_ctrl.minimum_bitrate == 0) || \
			(ctx->rate_ctrl_p.rate_ctrl.minimum_bitrate >= ctx->rate_ctrl_p.rate_ctrl.maximun_bitrate)) {
			ctx->rate_ctrl_p.rate_ctrl.minimum_bitrate = ctx->rate_ctrl_p.rate_ctrl.target_bitrate;
		}
		ctx->rate_ctrl_p.sample_bitrate = 0;
		ctx->rate_ctrl_p.rate_ctrl_en = 1;
	}
	break;
	case CMD_VIDEO_GET_CURRENT_BITRATE: {
		*((uint32_t *)arg) = ctx->rate_ctrl_p.current_bitrate;
	}
	break;
	case CMD_VIDEO_GET_REMAIN_QUEUE_LENGTH: {
		*((uint32_t *)arg) = uxQueueSpacesAvailable(mctx->output_ready);
	}
	break;
	case CMD_VIDEO_GET_MAX_QP: {
		*((uint32_t *)arg) = video_get_maxqp(ctx->params.stream_id);
	}
	break;
	case CMD_VIDEO_SET_MAX_QP: {
		encode_rc_parm_t rc_parm;
		memset(&rc_parm, 0x0, sizeof(encode_rc_parm_t));
		rc_parm.maxQp = *((uint32_t *)arg);
		video_ctrl(ctx->params.stream_id, VIDEO_SET_RCPARAM, (int)&rc_parm);
	}
	break;
	case CMD_VIDEO_SET_PRIVATE_MASK: {
		struct private_mask_s *pmask = (struct private_mask_s *)arg;
		video_set_private_mask(ctx->params.stream_id, pmask);
	}
	break;
	case CMD_VIDEO_SET_MULTI_RCCTRL: {
		int ch = ctx->params.stream_id;
		rate_ctrl_s *rc_ctrl = (rate_ctrl_s *)arg;
		video_ctrl(ch, VIDEO_RC_CTRL, arg);
		if (rc_ctrl->fps) {
			ctx->rate_ctrl_p.current_framerate = rc_ctrl->fps;
		}
	}
	break;
	case CMD_VIDEO_SET_EXT_INPUT: {
		int ch = ctx->params.stream_id;
		return video_ext_in(ch, (uint32_t)arg);
	}
	break;
	case CMD_VIDEO_SPS_CB: {
		ctx->sps_pps_cb = (void (*)(void *))arg;
		break;
	}
	case CMD_VIDEO_PRE_INIT_PARM: {
		video_pre_init_setup_parameters((void *)arg);
		break;
	}
}
return 0;
}

int video_handle(void *ctx, void *input, void *output)
{
	return 0;
}

void *video_destroy(void *p)
{
	video_ctx_t *ctx = (video_ctx_t *)p;

	free(ctx);
	return NULL;
}

static int video_detect_sensor_id(void)
{
	int id_value;
	if (MANUAL_SENSOR_IQ > 0 && MANUAL_SENSOR_IQ < SENSOR_MAX) {
		id_value = MANUAL_SENSOR_IQ;
	} else {
		for (int i = 0; i < SENSOR_MAX; i++) {
			if (sen_id[i] == USE_SENSOR) {
				id_value = i;
				break;
			}
		}
	}
	return id_value;
}

void *video_create(void *parent)
{
	video_ctx_t *ctx = malloc(sizeof(video_ctx_t));
	int iq_addr, sensor_addr;
	int ret = 0;
	if (!ctx) {
		return NULL;
	}
	memset(ctx, 0, sizeof(video_ctx_t));

	ctx->parent = parent;


	if (voe_boot_fsc_status()) {
		sensor_id_value = voe_boot_fsc_id();
		voe_get_sensor_info(sensor_id_value, &iq_addr, &sensor_addr);
	} else {
#if MULTI_SENSOR
		int sensor_id = isp_get_id();
		flash_sensor_id = sensor_id;
		int sensor_status = 0;
		VIDEO_DBG_INFO("sensor_id %d\r\n", sensor_id);
		if (sensor_id != 0xff && sensor_id < isp_boot->p_fcs_ld_info.multi_fcs_cnt && sensor_id > 0) {
			sensor_id_value = sensor_id;
			sensor_status = SENSOR_MULTI_SAVE_VALUE;
		} else {
			if (sensor_id_value == 0) {
				sensor_id_value = video_detect_sensor_id();
			}
			//sensor_id_value = USE_SENSOR;
			sensor_status = SENSOR_MULTI_DEFAULT_SETUP;
		}
		if (sensor_setup) {
			ret = sensor_setup(sensor_status, sensor_id_value);
			if (ret >= 0) {
				sensor_id_value = ret;
			}
		}
		voe_get_sensor_info(sensor_id_value, &iq_addr, &sensor_addr);
#else
		if (!sensor_id_value) { //Use the default sensor, if the value equal to 0
			if (sensor_id_value == 0) {
				sensor_id_value = video_detect_sensor_id();
			}
			//sensor_id_value = USE_SENSOR;
		}
		if (sensor_setup) {
			ret = sensor_setup(SENSOR_SINGLE_DEFAULT_SETUP, sensor_id_value);
			if (ret >= 0) {
				sensor_id_value = ret;
			}
		}
		voe_get_sensor_info(sensor_id_value, &iq_addr, &sensor_addr);
#endif
	}
	VIDEO_DBG_INFO("ID %x iq_addr %x sensor_addr %x\r\n", sensor_id_value, iq_addr, sensor_addr);
	ctx->v_adp = video_init(iq_addr, sensor_addr);
	VIDEO_DBG_INFO("ctx->v_adp = 0x%X\r\n", ctx->v_adp);

	return ctx;
}

void *video_new_item(void *p)
{
	return NULL;
}

void *video_del_item(void *p, void *d)
{

	video_ctx_t *ctx = (video_ctx_t *)p;
	int ch = ctx->params.stream_id;

	if (ctx->params.use_static_addr == 0) {
		if (d) {
			free(d);
		}
	}
	return NULL;
}

void *video_voe_release_item(void *p, void *d, int length)
{
	video_ctx_t *ctx = (video_ctx_t *)p;
	mm_queue_item_t *free_item = (mm_queue_item_t *)d;
	int ch = ctx->params.stream_id;
	int codec = AV_CODEC_ID_UNKNOWN;
	switch (free_item->type) {
	case AV_CODEC_ID_H265:
		codec = CODEC_HEVC;
		break;
	case AV_CODEC_ID_H264:
		codec = CODEC_H264;
		break;
	case AV_CODEC_ID_MJPEG:
		codec = CODEC_JPEG;
		break;
	case AV_CODEC_ID_RGB888:
		codec = CODEC_RGB;
		break;
	}

	if (ctx->params.use_static_addr == 1) {
		enc_queue_cnt[ch]--;
		if (free_item->type == AV_CODEC_ID_H264 || free_item->type == AV_CODEC_ID_H265 || free_item->type == AV_CODEC_ID_MJPEG) {
			video_encbuf_release(ch, codec, length);
		} else if (free_item->type == AV_CODEC_ID_RGB888) {
			int ret = video_ispbuf_release(ch, free_item->data_addr);
			if (ret == NOK) {
				video_ch4_delay_release((int)free_item->data_addr);
			}
			rgb_lock = 0;
		} else {
			video_ispbuf_release(ch, free_item->data_addr);
		}

		if (enc_queue_cnt[ch] > 0) {
			enc_queue_cnt[ch]--;
		}
	}


	return NULL;
}

int video_voe_presetting(int v1_enable, int v1_w, int v1_h, int v1_bps, int v1_shapshot,
						 int v2_enable, int v2_w, int v2_h, int v2_bps, int v2_shapshot,
						 int v3_enable, int v3_w, int v3_h, int v3_bps, int v3_shapshot,
						 int v4_enable, int v4_w, int v4_h)
{
	int voe_heap_size = 0;

#if MULTI_SENSOR
	info.sensor_fps    = 0;
	info.sensor_width  = 0;
	info.sensor_height = 0;
	for (int i = 0; i < SENSOR_MAX; i++) {
		struct sensor_params_t cur_snr = sensor_params[sen_id[i]];
		if (cur_snr.sensor_width * cur_snr.sensor_height > info.sensor_width * info.sensor_height) {
			info.sensor_width  = cur_snr.sensor_width;
			info.sensor_height = cur_snr.sensor_height;
		}
		if (cur_snr.sensor_fps < info.sensor_fps) {
			info.sensor_fps  = cur_snr.sensor_fps;
		}
	}
#else
	if (sensor_id_value > 0 && sensor_id_value < SENSOR_MAX) {
		info.sensor_fps    = sensor_params[sen_id[sensor_id_value]].sensor_fps;
		info.sensor_width  = sensor_params[sen_id[sensor_id_value]].sensor_width;
		info.sensor_height = sensor_params[sen_id[sensor_id_value]].sensor_height;
	} else {
		info.sensor_fps    = sensor_params[USE_SENSOR].sensor_fps;
		info.sensor_width  = sensor_params[USE_SENSOR].sensor_width;
		info.sensor_height = sensor_params[USE_SENSOR].sensor_height;
	}
#endif
	printf("[%s] fps:%d  w:%d  h:%d   \r\n", __FUNCTION__, info.sensor_fps, info.sensor_width, info.sensor_height);

#if OSD_ENABLE
	info.osd_enable = 1;
#endif

#if HDR_ENABLE
	info.hdr_enable = 1;
#endif

	video_set_isp_info(&info);

	//do 16 alignment
	v1_w = (v1_w + 15) & ~15;
	v2_w = (v2_w + 15) & ~15;
	v3_w = (v3_w + 15) & ~15;
	v4_w = (v4_w + 15) & ~15;

	voe_heap_size =  video_buf_calc(v1_enable, v1_w, v1_h, v1_bps, v1_shapshot,
									v2_enable, v2_w, v2_h, v2_bps, v2_shapshot,
									v3_enable, v3_w, v3_h, v3_bps, v3_shapshot,
									v4_enable, v4_w, v4_h);

	return voe_heap_size;
}

void video_voe_release(void)
{
	video_buf_release();
}

mm_module_t video_module = {
	.create = video_create,
	.destroy = video_destroy,
	.control = video_control,
	.handle = video_handle,

	.new_item = video_new_item,
	.del_item = video_del_item,
	.rsz_item = NULL,
	.vrelease_item = video_voe_release_item,

	.output_type = MM_TYPE_VDSP,    // output for video algorithm
	.module_type = MM_TYPE_VSRC,    // module type is video source
	.name = "VIDEO"
};
//////////////////for multi sensor/////////////////
int isp_get_id(void)   ////It only for non-fcs settung
{
	unsigned char type[4] = {0};

#if NONE_FCS_MODE
	video_get_fw_isp_info();
#endif

	if (voe_boot_fsc_id() == 0) {
		ftl_common_read(ISP_FW_LOCATION, type, sizeof(type));
		if (type[0] == 'I' && type[1] == 'S' && type[2] == 'P') {
			return type[3];
		} else {
			return 0xff;
		}
	} else {
		return 0xff;
	}
}

int isp_set_sensor(int sensor_id)   //It only for non-fcs settung
{
	int value = 0;
	unsigned char status[4] = {0};
	if (voe_boot_fsc_id() == 0) {
		value = isp_get_id();
		status[0] = 'I';
		status[1] = 'S';
		status[2] = 'P';
		status[3] = sensor_id;
		if (value != sensor_id) {
			ftl_common_write(ISP_FW_LOCATION, status, sizeof(status));
			VIDEO_DBG_INFO("Store the sensor id %d %d\r\n", value, sensor_id);
		} else {
			VIDEO_DBG_INFO("The sensor id is the same\r\n");
		}
		return 0;
	} else {
		return -1;
	}
}

void video_save_sensor_id(int SensorName)
{
	video_fcs_write_sensor_id(SensorName);
	isp_set_sensor(SensorName);

	mult_sensor_info_t multi_sensor_info;
	multi_sensor_info.sensor_index = SensorName;
	multi_sensor_info.sensor_finish = 1;
	video_set_video_snesor_info(&multi_sensor_info);
}

void video_set_sensor_id(int SensorName)
{
	if (MANUAL_SENSOR_IQ > 0 && MANUAL_SENSOR_IQ < SENSOR_MAX) {
		sensor_id_value = SensorName;
	} else {
		VIDEO_DBG_INFO("video_set_sensor_id() is only available for manual mode of Sensor/IQ\r\n");
	}
}

void video_setup_sensor(void *sensor_setup_cb)
{
	sensor_setup = (int(*)(int, int))sensor_setup_cb;
}
