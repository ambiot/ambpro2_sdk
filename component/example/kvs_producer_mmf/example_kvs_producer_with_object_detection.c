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
#include "model_yolo.h"

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
static mm_context_t *video_rgb_ctx			= NULL;
static mm_context_t *vipnn_ctx              = NULL;

static mm_siso_t *siso_video_kvs_v1         = NULL;
static mm_siso_t *siso_video_vipnn          = NULL;

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

// NN model selction //
#define USE_NN_MODEL            YOLO_MODEL

#define NN_CHANNEL 4
#define NN_RESOLUTION VIDEO_VGA //don't care for NN
#define NN_FPS 15
#define NN_GOP NN_FPS
#define NN_BPS 1024*1024 //don't care for NN
#define NN_TYPE VIDEO_RGB

#define NN_MODEL_OBJ   yolov4_tiny  // yolov7_tiny
#define NN_WIDTH	416
#define NN_HEIGHT	416
static float nn_confidence_thresh = 0.5;
static float nn_nms_thresh = 0.3;
static int desired_class_num = 0;
static int desired_class_list[80] = {0};
static const char *tag[80] = {"person", "bicycle", "car", "motorbike", "aeroplane", "bus", "train", "truck", "boat", "traffic light",
							  "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow",
							  "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
							  "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard", "tennis racket", "bottle",
							  "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple", "sandwich", "orange",
							  "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "sofa", "pottedplant", "bed",
							  "diningtable", "toilet", "tvmonitor", "laptop", "mouse", "remote", "keyboard", "cell phone", "microwave", "oven",
							  "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear", "hair drier", "toothbrush"
							 };

#define SENSOR_MAX_WIDTH 1920
#define SENSOR_MAX_HEIGHT 1080

static video_params_t video_v4_params = {
	.stream_id 		= NN_CHANNEL,
	.type 			= NN_TYPE,
	.resolution	 	= NN_RESOLUTION,
	.width 			= NN_WIDTH,
	.height 		= NN_HEIGHT,
	.bps 			= NN_BPS,
	.fps 			= NN_FPS,
	.gop 			= NN_GOP,
	.direct_output 	= 0,
	.use_static_addr = 1,
	.use_roi = 1,
	.roi = {
		.xmin = 0,
		.ymin = 0,
		.xmax = SENSOR_MAX_WIDTH,
		.ymax = SENSOR_MAX_HEIGHT,
	}
};

static nn_data_param_t roi_nn = {
	.img = {
		.width = NN_WIDTH,
		.height = NN_HEIGHT,
		.rgb = 0, // set to 1 if want RGB->BGR or BGR->RGB
		.roi = {
			.xmin = 0,
			.ymin = 0,
			.xmax = NN_WIDTH,
			.ymax = NN_HEIGHT,
		}
	}
};

static int gkvsPause = 1;

static void kvs_pause(void)
{
	printf("Pause KVS producer \r\n");
	if (siso_video_kvs_v1) {
		siso_pause(siso_video_kvs_v1);
	}
	if (video_v1_ctx) {
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_STREAM_STOP, V1_CHANNEL);
	}
	if (kvs_producer_v1_ctx) {
		mm_module_ctrl(kvs_producer_v1_ctx, CMD_KVS_PRODUCER_PAUSE, 0);
	}
	gkvsPause = 1;
}

static void kvs_resume(void)
{
	printf("Resume KVS producer \r\n");
	if (siso_video_kvs_v1) {
		siso_resume(siso_video_kvs_v1);
	}
	if (video_v1_ctx) {
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_APPLY, V1_CHANNEL);	// start channel 0
	}
	if (kvs_producer_v1_ctx) {
		mm_module_ctrl(kvs_producer_v1_ctx, CMD_KVS_PRODUCER_RECONNECT, 0);
	}
	gkvsPause = 0;
}

static void kvs_start_30s_recording(void)
{
	printf("kvs start 30s recording... \r\n");
	kvs_resume();

	uint32_t t0 = xTaskGetTickCount();
	while ((xTaskGetTickCount() - t0) <= 30000) {
		vTaskDelay(50);
	}

	kvs_pause();
}

static int g_kvs_triggered = 0;

static void kvs_control_task(void *param)
{
	while (1) {
		if (g_kvs_triggered == 1) {

			kvs_start_30s_recording();

			//reset
			desired_class_num = 0;
			memset(desired_class_list, 0, sizeof(desired_class_list));

			g_kvs_triggered = 0;
		}
		vTaskDelay(100);
	}
}

//--------------------------------------------
// Draw Rect
//--------------------------------------------
#include "nn_osd_draw.h"
#define LIMIT(x, lower, upper) if(x<lower) x=lower; else if(x>upper) x=upper;

static nn_osd_draw_obj_t nn_object;

static nn_osd_rect_t osd_rect = {
	.line_width = 3,
	.color = {
		.r = 255,
		.g = 255,
		.b = 255
	}
};
static nn_osd_text_t osd_text = {
	.color = {
		.r = 0,
		.g = 255,
		.b = 255
	}
};
static int check_in_list(int class_indx)
{
	for (int i = 0; i < desired_class_num; i++) {
		if (class_indx == desired_class_list[i]) {
			return class_indx;
		}
	}
	return -1;
}

static void nn_set_object(void *p, void *img_param)
{
	int i = 0;
	objdetect_res_t *res = (objdetect_res_t *)p;
	nn_data_param_t *im = (nn_data_param_t *)img_param;

	if (!p || !img_param)	{
		return;
	}

	int im_h = V1_HEIGHT;
	int im_w = V1_WIDTH;

	float ratio_h, ratio_w;
	int roi_h, roi_w, roi_x, roi_y;

	if (video_v4_params.use_roi) {  // resize
		ratio_h = (float)im_h / (float)im->img.height;
		ratio_w = (float)im_w / (float)im->img.width;
		roi_h = (int)((im->img.roi.ymax - im->img.roi.ymin) * ratio_h);
		roi_w = (int)((im->img.roi.xmax - im->img.roi.xmin) * ratio_w);
		roi_x = (int)(im->img.roi.xmin * ratio_w);
		roi_y = (int)(im->img.roi.ymin * ratio_h);
	} else {  // crop
		ratio_h = (float)im_h / (float)im->img.height;
		roi_h = (int)((im->img.roi.ymax - im->img.roi.ymin) * ratio_h);
		roi_w = (int)((im->img.roi.xmax - im->img.roi.xmin) * ratio_h);
		roi_x = (int)(im->img.roi.xmin * ratio_h + (im_w - roi_w) / 2);
		roi_y = (int)(im->img.roi.ymin * ratio_h);
	}

	//printf("object num = %d\r\n", res->obj_num);
	if (res->obj_num > 0) {
		nn_object.obj_num = 0;
		for (i = 0; i < res->obj_num; i++) {
			int obj_class = (int)res->result[6 * i ];
			if (nn_object.obj_num == OSD_OBJ_MAX_NUM) {
				break;
			}
			//printf("obj_class = %d\r\n",obj_class);

			int class_id = check_in_list(obj_class); //show class in desired_class_list
			//int class_id = obj_class; //coco label
			if (class_id != -1) {
				int ind = nn_object.obj_num;
				nn_object.rect[ind].ymin = (int)(res->result[6 * i + 3] * roi_h) + roi_y;
				LIMIT(nn_object.rect[ind].ymin, 0, im_h - 1)

				nn_object.rect[ind].xmin = (int)(res->result[6 * i + 2] * roi_w) + roi_x;
				LIMIT(nn_object.rect[ind].xmin, 0, im_w - 1)

				nn_object.rect[ind].ymax = (int)(res->result[6 * i + 5] * roi_h) + roi_y;
				LIMIT(nn_object.rect[ind].ymax, 0, im_h - 1)

				nn_object.rect[ind].xmax = (int)(res->result[6 * i + 4] * roi_w) + roi_x;
				LIMIT(nn_object.rect[ind].xmax, 0, im_w - 1)

				nn_object.class[ind] = class_id;
				nn_object.score[ind] = (int)(res->result[6 * i + 1 ] * 100);
				nn_object.obj_num++;
				// printf("%d,c%d:%d %d %d %d\n\r", i, nn_object.class[ind], nn_object.rect[ind].xmin, nn_object.rect[ind].ymin, nn_object.rect[ind].xmax,
				// nn_object.rect[ind].ymax);

				if (g_kvs_triggered == 0) {
					g_kvs_triggered = 1;
				}
			}
		}
	} else {
		nn_object.obj_num = 0;
	}

	if (gkvsPause) {
		return;
	}

	int nn_osd_ready2draw = nn_osd_get_status();
	if (nn_osd_ready2draw == 1) {
		for (i = 0; i < OSD_OBJ_MAX_NUM; i++) {
			if (i < nn_object.obj_num) {
				snprintf(osd_text.text_str, sizeof(osd_text.text_str), "%s %d", tag[nn_object.class[i]], nn_object.score[i]);
				memcpy(&osd_rect.rect, &nn_object.rect[i], sizeof(nn_rect_t));
				nn_osd_set_rect_with_text(i, V1_CHANNEL, &osd_text, &osd_rect);
			} else {
				nn_osd_clear_bitmap(i, V1_CHANNEL);
			}
			//printf("num=%d  %d, %d, %d, %d.\r\n", g_results.num, g_results.obj[i].left, g_results.obj[i].right, g_results.obj[i].top, g_results.obj[i].bottom);
		}
		nn_osd_update();
	}

}

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

void example_kvs_producer_with_object_detection_thread(void *param)
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
		//mm_module_ctrl(video_v1_ctx, CMD_VIDEO_APPLY, V1_CHANNEL);	// start channel 0
	} else {
		printf("video open fail\n\r");
		goto example_kvs_producer_with_object_detection;
	}

	kvs_producer_v1_ctx = mm_module_open(&kvs_producer_module);
	if (kvs_producer_v1_ctx) {
		mm_module_ctrl(kvs_producer_v1_ctx, CMD_KVS_PRODUCER_SET_APPLY, 0);
	} else {
		printf("KVS open fail\n\r");
		goto example_kvs_producer_with_object_detection;
	}

	video_rgb_ctx = mm_module_open(&video_module);
	if (video_rgb_ctx) {
		mm_module_ctrl(video_rgb_ctx, CMD_VIDEO_SET_PARAMS, (int)&video_v4_params);
		mm_module_ctrl(video_rgb_ctx, MM_CMD_SET_QUEUE_LEN, 2);
		mm_module_ctrl(video_rgb_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
	} else {
		printf("video open fail\n\r");
		goto example_kvs_producer_with_object_detection;
	}

	// VIPNN
	vipnn_ctx = mm_module_open(&vipnn_module);
	if (vipnn_ctx) {
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_MODEL, (int)&NN_MODEL_OBJ);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_IN_PARAMS, (int)&roi_nn);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_DISPPOST, (int)nn_set_object);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_CONFIDENCE_THRES, (int)&nn_confidence_thresh);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_NMS_THRES, (int)&nn_nms_thresh);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_APPLY, 0);
	} else {
		printf("VIPNN open fail\n\r");
		goto example_kvs_producer_with_object_detection;
	}
	printf("VIPNN opened\n\r");

	//--------------Link---------------------------

	siso_video_kvs_v1 = siso_create();
	if (siso_video_kvs_v1) {
#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
		siso_ctrl(siso_video_kvs_v1, MMIC_CMD_SET_SECURE_CONTEXT, 1, 0);
#endif
		siso_ctrl(siso_video_kvs_v1, MMIC_CMD_ADD_INPUT, (uint32_t)video_v1_ctx, 0);
		siso_ctrl(siso_video_kvs_v1, MMIC_CMD_ADD_OUTPUT, (uint32_t)kvs_producer_v1_ctx, 0);
		siso_start(siso_video_kvs_v1);
		siso_pause(siso_video_kvs_v1); //////// pause at beginning
	} else {
		printf("siso2 open fail\n\r");
		goto example_kvs_producer_with_object_detection;
	}
	printf("siso_video_kvs_v1 started\n\r");

	siso_video_vipnn = siso_create();
	if (siso_video_vipnn) {
#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
		siso_ctrl(siso_video_vipnn, MMIC_CMD_SET_SECURE_CONTEXT, 1, 0);
#endif
		siso_ctrl(siso_video_vipnn, MMIC_CMD_ADD_INPUT, (uint32_t)video_rgb_ctx, 0);
		siso_ctrl(siso_video_vipnn, MMIC_CMD_SET_STACKSIZE, (uint32_t)1024 * 64, 0);
		siso_ctrl(siso_video_vipnn, MMIC_CMD_SET_TASKPRIORITY, 3, 0);
		siso_ctrl(siso_video_vipnn, MMIC_CMD_ADD_OUTPUT, (uint32_t)vipnn_ctx, 0);
		siso_start(siso_video_vipnn);
	} else {
		printf("siso_video_vipnn open fail\n\r");
		goto example_kvs_producer_with_object_detection;
	}

	mm_module_ctrl(video_rgb_ctx, CMD_VIDEO_APPLY, NN_CHANNEL);
	mm_module_ctrl(video_rgb_ctx, CMD_VIDEO_YUV, 2);
	printf("siso_video_vipnn started\n\r");

	nn_osd_start(1, V1_WIDTH, V1_HEIGHT, 0, 0, 0, 0, 0, 0, 16, 32);

example_kvs_producer_with_object_detection:

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
		kvs_pause();
	} else if (!strcmp(arg, "R")) {
		kvs_resume();
	} else {
		printf("invalid cmd");
	}
}

static void fATKVS(void *arg)
{
	//reset
	desired_class_num = 0;
	memset(desired_class_list, 0, sizeof(desired_class_list));

	int argc = 0;
	char *argv[MAX_ARGC] = {0};
	argc = parse_param(arg, argv);
	if (argc <= 1) {
		printf("invalid cmd");
		return;
	}

	for (int i = 0; i < 80; i++) {
		for (int j = 1; j < argc; j++) {
			if (!strcmp(argv[j], tag[i])) {
				desired_class_list[desired_class_num] = i;
				desired_class_num++;
				printf("Desired class: %d \r\n", i);
			}
		}
	}
}

static log_item_t kvs_items[] = {
	{"KVSC", fKVSC,},
	{"KVS", fKVS,},
	{"ATKVS", fATKVS},
};

static void atcmd_kvs_producer_init(void)
{
	log_service_add_table(kvs_items, sizeof(kvs_items) / sizeof(kvs_items[0]));
}

void example_kvs_producer_with_object_detection(void)
{
	/*user can start their own task here*/
	if (xTaskCreate(example_kvs_producer_with_object_detection_thread, ((const char *)"example_kvs_producer_with_object_detection_thread"), 4096, NULL,
					tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		printf("\r\n example_kvs_producer_with_object_detection_thread: Create Task Error\n");
	}
	if (xTaskCreate(kvs_control_task, ((const char *)"kvs_control_task"), 4096, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		printf("\r\n kvs_control_task: Create Task Error\n");
	}
}