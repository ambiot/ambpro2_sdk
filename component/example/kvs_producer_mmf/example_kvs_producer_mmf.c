/******************************************************************************
*
* Copyright(c) 2007 - 2021 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "platform_opts.h"
#include "log_service.h"

#include "mmf2_link.h"
#include "mmf2_siso.h"

#include "module_video.h"
#include "mmf2_pro2_video_config.h"
#include "video_example_media_framework.h"

#include "example_kvs_producer_mmf.h"
#include "module_kvs_producer.h"
#include "sample_config.h"

#include "avcodec.h"

/*****************************************************************************
* ISP channel : 0
* Video type  : H264/HEVC
*****************************************************************************/

#define V1_CHANNEL 0
#define V1_RESOLUTION VIDEO_FHD
#define V1_FPS 15
#define V1_GOP 15
#define V1_BPS 1024*1024
#define V1_RCMODE 2 // 1: CBR, 2: VBR

#define USE_H265 0

#if USE_H265
#define VIDEO_TYPE VIDEO_HEVC
#define VIDEO_CODEC AV_CODEC_ID_H265
#else
#define VIDEO_TYPE VIDEO_H264
#define VIDEO_CODEC AV_CODEC_ID_H264
#endif

#if V1_RESOLUTION == VIDEO_VGA
#define V1_WIDTH	640
#define V1_HEIGHT	480
#elif V1_RESOLUTION == VIDEO_HD
#define V1_WIDTH	1280
#define V1_HEIGHT	720
#elif V1_RESOLUTION == VIDEO_FHD
#define V1_WIDTH	1920
#define V1_HEIGHT	1080
#endif

static mm_context_t *video_v1_ctx           = NULL;
static mm_context_t *kvs_producer_v1_ctx    = NULL;
static mm_siso_t *siso_video_kvs_v1         = NULL;

static video_params_t video_v1_params = {
	.stream_id = V1_CHANNEL,
	.type = VIDEO_TYPE,
	.resolution = V1_RESOLUTION,
	.width = V1_WIDTH,
	.height = V1_HEIGHT,
	.bps = V1_BPS,
	.fps = V1_FPS,
	.gop = V1_GOP,
	.rc_mode = V1_RCMODE,
	.use_static_addr = 1
};

#include "wifi_conf.h"
#include "lwip_netconf.h"
#define wifi_wait_time 500 //Here we wait 5 second to wiat the fast connect 
static void wifi_common_init(void)
{
	uint32_t wifi_wait_count = 0;

	while (!((wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID))) {
		vTaskDelay(10);
		wifi_wait_count++;
		if (wifi_wait_count == wifi_wait_time) {
			printf("\r\nuse ATW0, ATW1, ATWC to make wifi connection\r\n");
			printf("wait for wifi connection...\r\n");
		}
	}
}

static void atcmd_kvs_producer_init(void);

void example_kvs_producer_mmf_thread(void *param)
{
#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	rtw_create_secure_context(2048);
#endif

	atcmd_kvs_producer_init();

	wifi_common_init();

	int voe_heap_size = video_voe_presetting(1, V1_WIDTH, V1_HEIGHT, V1_BPS, 0,
						0, 0, 0, 0, 0,
						0, 0, 0, 0, 0,
						0, 0, 0);

	printf("\r\n voe heap size = %d\r\n", voe_heap_size);

	video_v1_ctx = mm_module_open(&video_module);
	if (video_v1_ctx) {
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_SET_PARAMS, (int)&video_v1_params);
		mm_module_ctrl(video_v1_ctx, MM_CMD_SET_QUEUE_LEN, 10);
		mm_module_ctrl(video_v1_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_APPLY, V1_CHANNEL);	// start channel 0
	} else {
		rt_printf("video open fail\n\r");
		goto example_kvs_producer_mmf;
	}

	kvs_producer_v1_ctx = mm_module_open(&kvs_producer_module);
	if (kvs_producer_v1_ctx) {
		mm_module_ctrl(kvs_producer_v1_ctx, CMD_KVS_PRODUCER_SET_APPLY, 0);
	} else {
		rt_printf("KVS open fail\n\r");
		goto example_kvs_producer_mmf;
	}

	siso_video_kvs_v1 = siso_create();
	if (siso_video_kvs_v1) {
#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
		siso_ctrl(siso_video_kvs_v1, MMIC_CMD_SET_SECURE_CONTEXT, 1, 0);
#endif
		siso_ctrl(siso_video_kvs_v1, MMIC_CMD_ADD_INPUT, (uint32_t)video_v1_ctx, 0);
		siso_ctrl(siso_video_kvs_v1, MMIC_CMD_ADD_OUTPUT, (uint32_t)kvs_producer_v1_ctx, 0);
		siso_start(siso_video_kvs_v1);
	} else {
		rt_printf("siso2 open fail\n\r");
		goto example_kvs_producer_mmf;
	}
	rt_printf("siso_video_kvs_v1 started\n\r");

example_kvs_producer_mmf:

	vTaskDelete(NULL);
}

static void fKVSC(void *arg)
{
	uint32_t t0 = xTaskGetTickCount();
	//Pause Linker
	siso_pause(siso_video_kvs_v1);

	//Stop module
	mm_module_ctrl(kvs_producer_v1_ctx, CMD_KVS_PRODUCER_PAUSE, 0);
	mm_module_ctrl(video_v1_ctx, CMD_VIDEO_STREAM_STOP, V1_CHANNEL);

	//Delete linker
	siso_delete(siso_video_kvs_v1);

	//Close module
	mm_module_close(video_v1_ctx);
	mm_module_close(kvs_producer_v1_ctx);

	//Video Deinit
	video_deinit();

	printf(">>>>>> Deinit take %d ms \r\n", xTaskGetTickCount() - t0);
}

static void fKVS(void *arg)
{
	if (!strcmp(arg, "P")) {
		printf("Pause KVS producer \r\n");
		siso_pause(siso_video_kvs_v1);
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_STREAM_STOP, V1_CHANNEL);
		mm_module_ctrl(kvs_producer_v1_ctx, CMD_KVS_PRODUCER_PAUSE, 0);
	} else if (!strcmp(arg, "R")) {
		printf("Resume KVS producer \r\n");
		siso_resume(siso_video_kvs_v1);
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_APPLY, V1_CHANNEL);	// start channel 0
		mm_module_ctrl(kvs_producer_v1_ctx, CMD_KVS_PRODUCER_RECONNECT, 0);
	} else {
		printf("invalid cmd");
	}
}

static log_item_t kvs_items[] = {
	{"KVSC", fKVSC,},
	{"KVS", fKVS,},
};

static void atcmd_kvs_producer_init(void)
{
	log_service_add_table(kvs_items, sizeof(kvs_items) / sizeof(kvs_items[0]));
}

void example_kvs_producer_mmf(void)
{
	/*user can start their own task here*/
	if (xTaskCreate(example_kvs_producer_mmf_thread, ((const char *)"example_kvs_producer_mmf_thread"), 4096, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		printf("\r\n example_kvs_producer_mmf_thread: Create Task Error\n");
	}
}