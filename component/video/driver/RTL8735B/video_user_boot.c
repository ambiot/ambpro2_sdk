#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "platform_stdlib.h"
#include <unistd.h>
#include <sys/wait.h>
#include "base_type.h"
#include "cmsis.h"
#include "error.h"
#include "hal.h"
#include "hal_video.h"
#include "hal_voe.h"
#include "video_api.h"
#include "video_boot.h"
#include "hal_snand.h"
#include "diag.h"
#include "hal_spic.h"
#include "hal_flash.h"
#include "sensor.h"
//#define USE_2K_SENSOR
#define CHANGE_PARAMETER
//#define ROI_TEST
//#define PRIVATE_TEST
//#define META_DATA_TEST
//#define ISP_CONTROL_TEST
video_boot_stream_t video_boot_stream = {
	.video_params[STREAM_V1].stream_id = STREAM_V1,
	.video_params[STREAM_V1].type = CODEC_H264,
	.video_params[STREAM_V1].resolution = 0,
	.video_params[STREAM_V1].width  = sensor_params[USE_SENSOR].sensor_width,
	.video_params[STREAM_V1].height = sensor_params[USE_SENSOR].sensor_height,
	.video_params[STREAM_V1].bps = 2 * 1024 * 1024,
	.video_params[STREAM_V1].fps = 15,
	.video_params[STREAM_V1].gop = 15,
	.video_params[STREAM_V1].rc_mode = 2,
	.video_params[STREAM_V1].minQp = 25,
	.video_params[STREAM_V1].maxQp = 48,
	.video_params[STREAM_V1].jpeg_qlevel = 0,
	.video_params[STREAM_V1].rotation = 0,
	.video_params[STREAM_V1].out_buf_size = V1_ENC_BUF_SIZE,
	.video_params[STREAM_V1].out_rsvd_size = 0,
	.video_params[STREAM_V1].direct_output = 0,
	.video_params[STREAM_V1].use_static_addr = 0,
	.video_snapshot[STREAM_V1] = 0,
	.video_drop_frame[STREAM_V1] = 0,
	.video_params[STREAM_V1].fcs = 1,//Enable the fcs for channel 1
	.auto_rate_control[STREAM_V1].sampling_time = sensor_params[USE_SENSOR].sensor_fps,
	.auto_rate_control[STREAM_V1].maximun_bitrate = 2 * 1024 * 1024 * 1.2,
	.auto_rate_control[STREAM_V1].minimum_bitrate = 2 * 1024 * 1024 * 0.8,
	.auto_rate_control[STREAM_V1].target_bitrate = 2 * 1024 * 1024,
	.video_params[STREAM_V2].stream_id = STREAM_V2,
	.video_params[STREAM_V2].type = CODEC_H264,
	.video_params[STREAM_V2].resolution = 0,
	.video_params[STREAM_V2].width = 1280,
	.video_params[STREAM_V2].height = 720,
	.video_params[STREAM_V2].bps = 1 * 1024 * 1024,
	.video_params[STREAM_V2].fps = 15,
	.video_params[STREAM_V2].gop = 15,
	.video_params[STREAM_V2].rc_mode = 0,
	.video_params[STREAM_V2].minQp = 25,
	.video_params[STREAM_V2].maxQp = 48,
	.video_params[STREAM_V2].jpeg_qlevel = 0,
	.video_params[STREAM_V2].rotation = 0,
	.video_params[STREAM_V2].out_buf_size = V2_ENC_BUF_SIZE,
	.video_params[STREAM_V2].out_rsvd_size = 0,
	.video_params[STREAM_V2].direct_output = 0,
	.video_params[STREAM_V2].use_static_addr = 0,
	.video_params[STREAM_V2].fcs = 0,
	.video_snapshot[STREAM_V2] = 0,
	.video_drop_frame[STREAM_V2] = 0,
	.auto_rate_control[STREAM_V2].sampling_time = 0,
	.auto_rate_control[STREAM_V2].maximun_bitrate = 0,
	.auto_rate_control[STREAM_V2].minimum_bitrate = 0,
	.auto_rate_control[STREAM_V2].target_bitrate = 0,
	.video_params[STREAM_V3].stream_id = STREAM_V3,
	.video_params[STREAM_V3].type = CODEC_H264,
	.video_params[STREAM_V3].resolution = 0,
	.video_params[STREAM_V3].width = 0,
	.video_params[STREAM_V3].height = 0,
	.video_params[STREAM_V3].bps = 0,
	.video_params[STREAM_V3].fps = 0,
	.video_params[STREAM_V3].gop = 0,
	.video_params[STREAM_V3].rc_mode = 0,
	.video_params[STREAM_V3].minQp = 0,
	.video_params[STREAM_V3].maxQp = 0,
	.video_params[STREAM_V3].jpeg_qlevel = 0,
	.video_params[STREAM_V3].rotation = 0,
	.video_params[STREAM_V3].out_buf_size = V3_ENC_BUF_SIZE,
	.video_params[STREAM_V3].out_rsvd_size = 0,
	.video_params[STREAM_V3].direct_output = 0,
	.video_params[STREAM_V3].use_static_addr = 0,
	.video_params[STREAM_V3].fcs = 0,
	.video_snapshot[STREAM_V3] = 0,
	.video_drop_frame[STREAM_V3] = 0,
	.video_params[STREAM_V4].stream_id = STREAM_V4,
	.video_params[STREAM_V4].type = 0,
	.video_params[STREAM_V4].resolution = 0,
	.video_params[STREAM_V4].width = 640,
	.video_params[STREAM_V4].height = 480,
	.video_params[STREAM_V4].bps = 0,
	.video_params[STREAM_V4].fps = 0,
	.video_params[STREAM_V4].gop = 0,
	.video_params[STREAM_V4].rc_mode = 0,
	.video_params[STREAM_V4].minQp = 0,
	.video_params[STREAM_V4].maxQp = 0,
	.video_params[STREAM_V4].jpeg_qlevel = 0,
	.video_params[STREAM_V4].rotation = 0,
	.video_params[STREAM_V4].out_buf_size = 0,
	.video_params[STREAM_V4].out_rsvd_size = 0,
	.video_params[STREAM_V4].direct_output = 0,
	.video_params[STREAM_V4].use_static_addr = 0,
	.video_params[STREAM_V4].fcs = 0,
	.video_enable[STREAM_V1] = 1,
	.video_enable[STREAM_V2] = 1,
	.video_enable[STREAM_V3] = 0,
	.video_enable[STREAM_V4] = 1,
#ifdef USE_FCS_LOOKUPTABLE_SAMPLE
	.fcs_isp_ae_enable = 1,
	.fcs_isp_awb_enable = 1,
	.fcs_lookup_count = 5,
	.fcs_als_thr[0] = 0,
	.fcs_isp_ae_table_exposure[0] = 2500,
	.fcs_isp_ae_table_gain[0] = 256,
	.fcs_isp_awb_table_rgain[0] = 500,
	.fcs_isp_awb_table_bgain[0] = 300,
	.fcs_isp_mode_table[0] = 0,
	.fcs_als_thr[1] = 10000,
	.fcs_isp_ae_table_exposure[1] = 5000,
	.fcs_isp_ae_table_gain[1] = 256,
	.fcs_isp_awb_table_rgain[1] = 450,
	.fcs_isp_awb_table_bgain[1] = 330,
	.fcs_isp_mode_table[1] = 0,
	.fcs_als_thr[2] = 5000,
	.fcs_isp_ae_table_exposure[2] = 10000,
	.fcs_isp_ae_table_gain[2] = 256,
	.fcs_isp_awb_table_rgain[2] = 400,
	.fcs_isp_awb_table_bgain[2] = 350,
	.fcs_isp_mode_table[2] = 0,
	.fcs_als_thr[3] = 2500,
	.fcs_isp_ae_table_exposure[3] = 20000,
	.fcs_isp_ae_table_gain[3] = 256,
	.fcs_isp_awb_table_rgain[3] = 400,
	.fcs_isp_awb_table_bgain[3] = 350,
	.fcs_isp_mode_table[3] = 0,
	.fcs_als_thr[4] = 500,
	.fcs_isp_ae_table_exposure[4] = 30000,
	.fcs_isp_ae_table_gain[4] = 256,
	.fcs_isp_awb_table_rgain[4] = 380,
	.fcs_isp_awb_table_bgain[4] = 400,
	.fcs_isp_mode_table[4] = 0,
	.fcs_als_thr[5] = 200,
	.fcs_isp_ae_table_exposure[5] = 30000,
	.fcs_isp_ae_table_gain[5] = 1024,
	.fcs_isp_awb_table_rgain[5] = 256,
	.fcs_isp_awb_table_bgain[5] = 256,
	.fcs_isp_mode_table[5] = 1,
#else
	.fcs_isp_ae_enable = 0,
	.fcs_isp_ae_init_exposure = 0,
	.fcs_isp_ae_init_gain = 0,
	.fcs_isp_awb_enable = 0,
	.fcs_isp_awb_init_rgain = 0,
	.fcs_isp_awb_init_bgain = 0,
	.fcs_isp_init_daynight_mode = 0,
	.fcs_isp_gray_mode = 0,
#endif
	.voe_heap_size = 0,
	.voe_heap_addr = 0,
	.isp_info.sensor_width = sensor_params[USE_SENSOR].sensor_width,
	.isp_info.sensor_height = sensor_params[USE_SENSOR].sensor_height,
	.isp_info.sensor_fps = sensor_params[USE_SENSOR].sensor_fps,
	.isp_info.md_enable = 1,
	.isp_info.hdr_enable = 1,
	.isp_info.osd_enable = 1,
	.fcs_channel = 1,//FCS_TOTAL_NUMBER
	.fcs_status = 0,
	.fcs_setting_done = 0,
	.fcs_isp_iq_id = 0,
};
//#define FCS_PARTITION //Use the FCS data to change the parameter from bootloader.If mark the marco, it will use the FTL config.
extern hal_snafc_adaptor_t boot_snafc_adpt;
extern hal_spic_adaptor_t _hal_spic_adaptor;
uint8_t snand_data[2112] __attribute__((aligned(32)));
#define NAND_FLASH_FCS 0x7080000 //900*128*1024 It msut be first page for the block
#define NAND_PAGE_SIZE 2048
#define NAND_PAGE_COUNT  64
#define NAND_BLOCK_COUNT 1024
#define NOR_FLASH_FCS  (0xF00000 + 0xD000)
#define NOR_FLASH_BASE 0x08000000

int boot_get_remap_table_index(unsigned char *buf, int block_index, int *remap_index)
{
	int i = 0;
	int ret = -1;
	int bad_block = 0;
	for (i = 1; i < NAND_PAGE_SIZE / 8; i++) {
		if ((buf[i * 8] == 'B') && (buf[i * 8 + 1] == 'B')) {
			if (buf[i * 8 + 6] != 'b' && buf[i * 8 + 7] != 'b') {
				break;//The remap table is not correct
			} else {
				bad_block = buf[i * 8 + 2] | buf[i * 8 + 3] << 8;
				if (bad_block == block_index) {
					*remap_index = buf[i * 8 + 4] | buf[i * 8 + 5] << 8;
					//dbg_printf("remap_index %d\r\n",*remap_index);
					ret = 0;
					break;
				}
			}
		}
	}
	return ret;
}

int boot_get_fcs_remap_block(unsigned int address, unsigned int *remap_value)
{
	int i = 0;
	int ret = -1;
	int remap_index = 0;
	for (i = (NAND_BLOCK_COUNT - 4); i < NAND_BLOCK_COUNT; i++) {
		hal_snand_page_read(&boot_snafc_adpt, &snand_data[0], NAND_PAGE_SIZE + 32, i * NAND_PAGE_COUNT);
		if (snand_data[NAND_PAGE_SIZE] == 0xff) {
			ret = boot_get_remap_table_index(&snand_data[0], address / (NAND_PAGE_SIZE * NAND_PAGE_COUNT), &remap_index);
			if (ret >= 0) {
				*remap_value = remap_index;
				//dbg_printf("remap_value %d\r\n",*remap_value);
				ret = 0;
				break;
			}
		} else {
			//dbg_printf("bad block %d\r\n",i);
		}
	}
	return ret;
}
int boot_read_flash_data(unsigned int address, unsigned char *buf, int length)
{
	int ret = 0;
	if (length > NAND_PAGE_SIZE) {
		return -1;
	}
	if (hal_sys_get_boot_select() == 1) { //For nand flash
		hal_snand_page_read(&boot_snafc_adpt, snand_data, 2048 + 32, address / NAND_PAGE_SIZE);
		if (snand_data[2048] != 0xff) { //Check the bad block
			int ret = 0;
			unsigned int remap_value = 0;
			ret = boot_get_fcs_remap_block(address, &remap_value);
			if (ret >= 0) {
				//dbg_printf("Find remap value %d\r\n",remap_value);
				hal_snand_page_read(&boot_snafc_adpt, &snand_data[0], NAND_PAGE_SIZE + 32, remap_value * NAND_PAGE_COUNT);
				memcpy(buf, snand_data, length);
			} else {
				dbg_printf("It can't get the remap table\r\n");
				ret = -1;
			}
		} else {
			memcpy(buf, snand_data, length);
		}
	} else {
		dcache_invalidate_by_addr((uint32_t *)(NOR_FLASH_BASE + address), length);
		unsigned char flash_id = _hal_spic_adaptor.flash_id[2];
		if (flash_id < FLASH_ID_4ADDR) {
			hal_flash_stream_read(&_hal_spic_adaptor, length, address, buf);
		} else {
			memcpy(buf, (void *)(NOR_FLASH_BASE + address), length);
		}
		//hal_flash_stream_read(&_hal_spic_adaptor, length, address, buf);

	}
	return ret;
}
static void set_fcs_boottime_information(void)
{
	video_boot_stream.fcs_start_time = hal_read_cur_time() / 1000; // get the time from booloader to fcs OK
	(* ((volatile uint32_t *) 0xe000edfc)) |= (1 << 24);    // DEMCR, bit 24 TRCENA
	(* ((volatile uint32_t *) 0xe0001004)) = 0;             // DWT_CYCCNT
	(* ((volatile uint32_t *) 0xe0001000)) |= 1;            // DWT_CTRL, bit 0 CYCCNTENA
}

int user_disable_fcs(void)
{
	return 0;//1:disable fcs, 0:Don't care
}

void user_boot_config_init(void *parm)
{
	//Insert your code into here
	//dbg_printf("user_boot_config_init\r\n");
	video_boot_stream_t *fcs_data = NULL;

	// when use fcs mode, we need this api to keep the fcs boot time information
	set_fcs_boottime_information();

#ifdef FCS_PARTITION
	unsigned char *boot_data = NULL;
	boot_data = (unsigned char *)parm;
	if (parm == NULL) {
		dbg_printf("Can't get parm\r\n");
		return;
	}
#else
	int ret = 0;
	uint8_t boot_data[2048] __attribute__((aligned(32)));
	unsigned int address = 0;
	memset(boot_data, 0x00, sizeof(boot_data));
	if (hal_sys_get_boot_select() == 1) {
		address = NAND_FLASH_FCS;
	} else {
		address = NOR_FLASH_FCS;
	}
	ret = boot_read_flash_data(address, boot_data, sizeof(boot_data));
	if (ret < 0) {
		return;
	}
#endif

	if (boot_data[0] == 'F' && boot_data[1] == 'C' && boot_data[2] == 'S' && boot_data[3] == 'D') {
		unsigned int checksum_tag = boot_data[4] | boot_data[5] << 8 | boot_data[6] << 16 | boot_data[7] << 24;
		unsigned int checksum_value = 0;
		for (int i = 0; i < sizeof(video_boot_stream_t); i++) {
			checksum_value += boot_data[i + 8];
		}
		if (checksum_tag == checksum_value) {
			fcs_data = (video_boot_stream_t *)(boot_data + 8);
			if ((video_boot_stream.isp_info.sensor_width == fcs_data->isp_info.sensor_width) &&
				(video_boot_stream.isp_info.sensor_height == fcs_data->isp_info.sensor_height)) {
				uint32_t fcs_start_time = video_boot_stream.fcs_start_time;
				memcpy(&video_boot_stream, fcs_data, sizeof(video_boot_stream_t));
				video_boot_stream.fcs_start_time = fcs_start_time;
				//dbg_printf("Update parameter\r\n");
			}
		} else {
			dbg_printf("Check sum fail\r\n");
		}
	} else {
		dbg_printf("Can't find %x %x %x %x\r\n", boot_data[0], boot_data[1], boot_data[2], boot_data[3]);
	}
#ifdef USE_FCS_LOOKUPTABLE_SAMPLE
	int i;
	int ALS_value = 20000; //Get From HW ADC, user can modify it for difference init value testing
	for (i = 1; i <= video_boot_stream.fcs_lookup_count; i++) {
		if (ALS_value > video_boot_stream.fcs_als_thr[i]) {
			video_boot_stream.fcs_isp_ae_init_exposure = video_boot_stream.fcs_isp_ae_table_exposure[i];
			video_boot_stream.fcs_isp_ae_init_gain = video_boot_stream.fcs_isp_ae_table_gain[i];
			video_boot_stream.fcs_isp_awb_init_rgain =	video_boot_stream.fcs_isp_awb_table_rgain[i];
			video_boot_stream.fcs_isp_awb_init_bgain =	video_boot_stream.fcs_isp_awb_table_bgain[i];
			//0 = RGB mode (with color), 1=IR mode (w/o color), 2 = Spot light mode (with color)
			if (video_boot_stream.fcs_isp_mode_table[i] == 2) {
				video_boot_stream.fcs_isp_init_daynight_mode = video_boot_stream.fcs_isp_mode_table[i];
				video_boot_stream.fcs_isp_gray_mode = 0;
			} else {
				video_boot_stream.fcs_isp_init_daynight_mode = video_boot_stream.fcs_isp_mode_table[i];
				video_boot_stream.fcs_isp_gray_mode = video_boot_stream.fcs_isp_mode_table[i];
			}
			break;
		}
	}
	if (i == (video_boot_stream.fcs_lookup_count + 1))	{
		video_boot_stream.fcs_isp_ae_init_exposure = video_boot_stream.fcs_isp_ae_table_exposure[0];
		video_boot_stream.fcs_isp_ae_init_gain = video_boot_stream.fcs_isp_ae_table_gain[0];
		video_boot_stream.fcs_isp_awb_init_rgain = video_boot_stream.fcs_isp_awb_table_rgain[0];
		video_boot_stream.fcs_isp_awb_init_bgain = video_boot_stream.fcs_isp_awb_table_bgain[0];
		//0 = RGB mode (with color), 1=IR mode (w/o color), 2 = Spot light mode (with color)
		if (video_boot_stream.fcs_isp_mode_table[i] == 2) {
			video_boot_stream.fcs_isp_init_daynight_mode = video_boot_stream.fcs_isp_mode_table[0];
			video_boot_stream.fcs_isp_gray_mode = 0;
		} else {
			video_boot_stream.fcs_isp_init_daynight_mode = video_boot_stream.fcs_isp_mode_table[0];
			video_boot_stream.fcs_isp_gray_mode = video_boot_stream.fcs_isp_mode_table[0];
		}
	}
#endif

#ifdef ROI_TEST
	video_boot_stream.video_params[STREAM_V1].use_roi = 1;//Enable the ROI function
	// (xmax - xmin) should be larger than v1 & v2 width
	// (ymax - ymin) should be larger than v1 & v2 height
	video_boot_stream.video_params[STREAM_V1].roi.xmin = 0;
	video_boot_stream.video_params[STREAM_V1].roi.ymin = 0;
	video_boot_stream.video_params[STREAM_V1].roi.xmax = video_boot_stream.isp_info.sensor_width;
	video_boot_stream.video_params[STREAM_V1].roi.ymax = video_boot_stream.isp_info.sensor_height;
#endif

#ifdef PRIVATE_TEST
	video_boot_stream.private_mask.enable = 1;
	video_boot_stream.private_mask.color = 0xff0080;
	//Rect 0
	video_boot_stream.private_mask.en[PRIVATE_MASK_RECT_ID_0] = 1;
	video_boot_stream.private_mask.start_x[PRIVATE_MASK_RECT_ID_0] = 0;
	video_boot_stream.private_mask.start_y[PRIVATE_MASK_RECT_ID_0] = 0;
	video_boot_stream.private_mask.w[PRIVATE_MASK_RECT_ID_0] = 320;
	video_boot_stream.private_mask.h[PRIVATE_MASK_RECT_ID_0] = 300;
	//Rect 1
	video_boot_stream.private_mask.en[PRIVATE_MASK_RECT_ID_1] = 1;
	video_boot_stream.private_mask.start_x[PRIVATE_MASK_RECT_ID_1] = 100;
	video_boot_stream.private_mask.start_y[PRIVATE_MASK_RECT_ID_1] = 100;
	video_boot_stream.private_mask.w[PRIVATE_MASK_RECT_ID_1] = 320;
	video_boot_stream.private_mask.h[PRIVATE_MASK_RECT_ID_1] = 300;
	//Grid
	video_boot_stream.private_mask.en[PRIVATE_MASK_GRID] = 1;
	video_boot_stream.private_mask.start_x[PRIVATE_MASK_GRID] = 320;
	video_boot_stream.private_mask.start_y[PRIVATE_MASK_GRID] = 300;
	video_boot_stream.private_mask.w[PRIVATE_MASK_GRID] = 320;
	video_boot_stream.private_mask.h[PRIVATE_MASK_GRID] = 300;
	video_boot_stream.private_mask.cols = 8;
	video_boot_stream.private_mask.rows = 4;
	memset(video_boot_stream.private_mask.bitmap, 0xaa, sizeof(video_boot_stream.private_mask.bitmap));
#endif
#ifdef META_DATA_TEST
	video_boot_stream.meta_enable = 1;
	video_boot_stream.meta_size = VIDEO_BOOT_META_USER_SIZE;
#endif
#ifdef ISP_CONTROL_TEST
	//If you don't want to setup the parameters, you can setup the 0xffff to skip the procedure.For example video_boot_stream.init_isp_items.init_brightness = 0xffff;
	video_boot_stream.init_isp_items.enable = 1;
	video_boot_stream.init_isp_items.init_brightness = 10;    //Default:0
	video_boot_stream.init_isp_items.init_contrast = 100;     //Default:50
	video_boot_stream.init_isp_items.init_flicker = 2;        //Default:1
	video_boot_stream.init_isp_items.init_hdr_mode = 0;       //Default:0
	video_boot_stream.init_isp_items.init_mirrorflip = 0xf3;  //Default:0xf0
	video_boot_stream.init_isp_items.init_saturation = 75;    //Default:50
	video_boot_stream.init_isp_items.init_wdr_level = 80;     //Default:50
	video_boot_stream.init_isp_items.init_wdr_mode = 1;       //Default:0
#endif
}
