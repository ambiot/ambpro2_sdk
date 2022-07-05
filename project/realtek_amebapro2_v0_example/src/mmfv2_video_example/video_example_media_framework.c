/******************************************************************************
*
* Copyright(c) 2007 - 2018 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "video_example_media_framework.h"
#include <FreeRTOS.h>
#include <task.h>
#include "module_video.h"
#include "mmf2_pro2_video_config.h"

#define wifi_wait_time 500 //Here we wait 5 second to wiat the fast connect 
//------------------------------------------------------------------------------
// common code for network connection
//------------------------------------------------------------------------------
#include "wifi_conf.h"
#include "lwip_netconf.h"

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

//------------------------------------------------------------------------------
// video support examples
//------------------------------------------------------------------------------
static void example_mmf2_video_surport(void)
{

	// CH1 Video -> H264/HEVC -> RTSP
	//mmf2_video_example_v1_init();

	// H264 -> RTSP (V1)
	// RGB  -> NN object detect (V4)
	mmf2_video_example_vipnn_rtsp_init();
}

void video_example_main(void *param)
{
#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	rtw_create_secure_context(2048);
#endif
	if (!voe_boot_fsc_status()) {
		wifi_common_init();
	}

	example_mmf2_video_surport();

	// TODO: exit condition or signal
	while (1) {
		vTaskDelay(10000);
		// extern mm_context_t *video_v1_ctx;
		// mm_module_ctrl(video_v1_ctx, CMD_VIDEO_PRINT_INFO, 0);
	}
}

void video_example_media_framework(void)
{
	/*user can start their own task here*/
	if (xTaskCreate(video_example_main, ((const char *)"mmf2_video"), 4096, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		printf("\r\n video_example_main: Create Task Error\n");
	}
}
