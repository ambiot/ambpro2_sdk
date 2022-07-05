/******************************************************************************
*
* Copyright(c) 2007 - 2021 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "mmf2_link.h"
#include "mmf2_siso.h"
#include "module_video.h"
#include "module_vipnn.h"
#include "module_rtsp2.h"
#include "mmf2_pro2_video_config.h"
#include "video_example_media_framework.h"
#include "log_service.h"
#include "avcodec.h"
#include "model_yolo.h"

/*****************************************************************************
* ISP channel : 4
* Video type  : RGB
*****************************************************************************/
#define RTSP_CHANNEL 0
#define RTSP_RESOLUTION VIDEO_FHD
#define RTSP_FPS 15
#define RTSP_GOP 15
#define RTSP_BPS 1*1024*1024
#define VIDEO_RCMODE 2 // 1: CBR, 2: VBR

#define USE_H265 0

#if USE_H265
#include "sample_h265.h"
#define RTSP_TYPE VIDEO_HEVC
#define RTSP_CODEC AV_CODEC_ID_H265
#else
#include "sample_h264.h"
#define RTSP_TYPE VIDEO_H264
#define RTSP_CODEC AV_CODEC_ID_H264
#endif

#if RTSP_RESOLUTION == VIDEO_FHD
#define RTSP_WIDTH	1920
#define RTSP_HEIGHT	1080
#endif

static video_params_t video_v1_params = {
	.stream_id 		= RTSP_CHANNEL,
	.type 			= RTSP_TYPE,
	.resolution 	= RTSP_RESOLUTION,
	.width 			= RTSP_WIDTH,
	.height 		= RTSP_HEIGHT,
	.bps            = RTSP_BPS,
	.fps 			= RTSP_FPS,
	.gop 			= RTSP_GOP,
	.rc_mode        = VIDEO_RCMODE,
	.use_static_addr = 1,
	//.fcs = 1
};

static rtsp2_params_t rtsp2_v1_params = {
	.type = AVMEDIA_TYPE_VIDEO,
	.u = {
		.v = {
			.codec_id = RTSP_CODEC,
			.fps      = RTSP_FPS,
			.bps      = RTSP_BPS
		}
	}
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
static int desired_class_num = 4;
static int desired_class_list[] = {0, 2, 5, 7};
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

#define V1_ENA 1
#define V4_ENA 1

static void atcmd_userctrl_init(void);
static mm_context_t *video_v1_ctx			= NULL;
static mm_context_t *rtsp2_v1_ctx			= NULL;
static mm_siso_t *siso_video_rtsp_v1			= NULL;

static mm_context_t *video_rgb_ctx			= NULL;
static mm_context_t *vipnn_ctx            = NULL;
static mm_siso_t *siso_video_vipnn         = NULL;


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

	int im_h = RTSP_HEIGHT;
	int im_w = RTSP_WIDTH;
    
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

	printf("object num = %d\r\n", res->obj_num);
	if (res->obj_num > 0) {
		nn_object.obj_num = 0;
		for (i = 0; i < res->obj_num; i++) {
			int obj_class = (int)res->result[6 * i ];
			if (nn_object.obj_num == OSD_OBJ_MAX_NUM) {
				break;
			}
			//printf("obj_class = %d\r\n",obj_class);

			int class_id = obj_class; //check_in_list(obj_class); //show class in desired_class_list
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
				printf("%d,c%d:%d %d %d %d\n\r", i, nn_object.class[ind], nn_object.rect[ind].xmin, nn_object.rect[ind].ymin, nn_object.rect[ind].xmax,
					   nn_object.rect[ind].ymax);
			}
		}
	} else {
		nn_object.obj_num = 0;
	}

	int nn_osd_ready2draw = nn_osd_get_status();
	if (nn_osd_ready2draw == 1) {
		for (i = 0; i < OSD_OBJ_MAX_NUM; i++) {
			if (i < nn_object.obj_num) {
				snprintf(osd_text.text_str, sizeof(osd_text.text_str), "%s %d", tag[nn_object.class[i]], nn_object.score[i]);
				memcpy(&osd_rect.rect, &nn_object.rect[i], sizeof(nn_rect_t));
				nn_osd_set_rect_with_text(i, RTSP_CHANNEL, &osd_text, &osd_rect);
			} else {
				nn_osd_clear_bitmap(i, RTSP_CHANNEL);
			}
			//printf("num=%d  %d, %d, %d, %d.\r\n", g_results.num, g_results.obj[i].left, g_results.obj[i].right, g_results.obj[i].top, g_results.obj[i].bottom);
		}
		nn_osd_update();
	}

}

void mmf2_video_example_vipnn_rtsp_init(void)
{
	atcmd_userctrl_init();

	int voe_heap_size = video_voe_presetting(V1_ENA, RTSP_WIDTH, RTSP_HEIGHT, RTSP_BPS, 0,
						0, 0, 0, 0, 0,
						0, 0, 0, 0, 0,
						V4_ENA, NN_WIDTH, NN_HEIGHT);

	printf("\r\n voe heap size = %d\r\n", voe_heap_size);

#if V1_ENA
	video_v1_ctx = mm_module_open(&video_module);
	if (video_v1_ctx) {
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_SET_PARAMS, (int)&video_v1_params);
		mm_module_ctrl(video_v1_ctx, MM_CMD_SET_QUEUE_LEN, RTSP_FPS * 3);
		mm_module_ctrl(video_v1_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
	} else {
		printf("video open fail\n\r");
		goto mmf2_example_vnn_rtsp_fail;
	}

	rtsp2_v1_ctx = mm_module_open(&rtsp2_module);
	if (rtsp2_v1_ctx) {
		mm_module_ctrl(rtsp2_v1_ctx, CMD_RTSP2_SELECT_STREAM, 0);
		mm_module_ctrl(rtsp2_v1_ctx, CMD_RTSP2_SET_PARAMS, (int)&rtsp2_v1_params);
		mm_module_ctrl(rtsp2_v1_ctx, CMD_RTSP2_SET_APPLY, 0);
		mm_module_ctrl(rtsp2_v1_ctx, CMD_RTSP2_SET_STREAMMING, ON);
	} else {
		printf("RTSP2 open fail\n\r");
		goto mmf2_example_vnn_rtsp_fail;
	}
#endif

	video_rgb_ctx = mm_module_open(&video_module);
	if (video_rgb_ctx) {
		mm_module_ctrl(video_rgb_ctx, CMD_VIDEO_SET_PARAMS, (int)&video_v4_params);
		mm_module_ctrl(video_rgb_ctx, MM_CMD_SET_QUEUE_LEN, 2);
		mm_module_ctrl(video_rgb_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
	} else {
		printf("video open fail\n\r");
		goto mmf2_example_vnn_rtsp_fail;
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
		goto mmf2_example_vnn_rtsp_fail;
	}
	printf("VIPNN opened\n\r");

	//--------------Link---------------------------

#if V1_ENA
	siso_video_rtsp_v1 = siso_create();
	if (siso_video_rtsp_v1) {
#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
		siso_ctrl(siso_video_rtsp_v1, MMIC_CMD_SET_SECURE_CONTEXT, 1, 0);
#endif
		siso_ctrl(siso_video_rtsp_v1, MMIC_CMD_ADD_INPUT, (uint32_t)video_v1_ctx, 0);
		siso_ctrl(siso_video_rtsp_v1, MMIC_CMD_ADD_OUTPUT, (uint32_t)rtsp2_v1_ctx, 0);
		siso_start(siso_video_rtsp_v1);
	} else {
		printf("siso2 open fail\n\r");
		goto mmf2_example_vnn_rtsp_fail;
	}

	mm_module_ctrl(video_v1_ctx, CMD_VIDEO_APPLY, RTSP_CHANNEL);
#endif

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
		goto mmf2_example_vnn_rtsp_fail;
	}

	mm_module_ctrl(video_rgb_ctx, CMD_VIDEO_APPLY, NN_CHANNEL);
	mm_module_ctrl(video_rgb_ctx, CMD_VIDEO_YUV, 2);
	printf("siso_video_vipnn started\n\r");

#if V1_ENA && V4_ENA
	nn_osd_start(1, RTSP_WIDTH, RTSP_HEIGHT, 0, 0, 0, 0, 0, 0, 16, 32);
#endif

	return;
mmf2_example_vnn_rtsp_fail:

	return;
}

static const char *example = "mmf2_video_example_vipnn_rtsp";
static void example_deinit(void)
{
	//Pause Linker
	siso_pause(siso_video_rtsp_v1);
	siso_pause(siso_video_vipnn);

	//Stop module
	mm_module_ctrl(rtsp2_v1_ctx, CMD_RTSP2_SET_STREAMMING, OFF);
	mm_module_ctrl(video_v1_ctx, CMD_VIDEO_STREAM_STOP, RTSP_CHANNEL);
	mm_module_ctrl(video_rgb_ctx, CMD_VIDEO_STREAM_STOP, NN_CHANNEL);

	//Delete linker
	siso_delete(siso_video_rtsp_v1);
	siso_delete(siso_video_vipnn);

	//Close module
	mm_module_close(rtsp2_v1_ctx);
	mm_module_close(video_v1_ctx);
	mm_module_close(video_rgb_ctx);

	//Video Deinit
	video_deinit();
}

static void fUC(void *arg)
{
	static uint32_t user_cmd = 0;

	if (!strcmp(arg, "TD")) {
		if (user_cmd & USR_CMD_EXAMPLE_DEINIT) {
			printf("invalid state, can not do %s deinit!\r\n", example);
		} else {
			example_deinit();
			user_cmd = USR_CMD_EXAMPLE_DEINIT;
			printf("deinit %s\r\n", example);
		}
	} else if (!strcmp(arg, "TSR")) {
		if (user_cmd & USR_CMD_EXAMPLE_DEINIT) {
			printf("reinit %s\r\n", example);
			sys_reset();
		} else {
			printf("invalid state, can not do %s reinit!\r\n", example);
		}
	} else {
		printf("invalid cmd");
	}

	printf("user command 0x%lx\r\n", user_cmd);
}

static log_item_t userctrl_items[] = {
	{"UC", fUC, },
};

static void atcmd_userctrl_init(void)
{
	log_service_add_table(userctrl_items, sizeof(userctrl_items) / sizeof(userctrl_items[0]));
}
