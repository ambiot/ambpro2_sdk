//---------------------------------------------------------
// Face Recognition - MobileFaceNet
//---------------------------------------------------------
#include <math.h>
#include <arm_math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "img_process/img_process.h"
#include "nn_utils/quantize.h"
#include "nn_utils/tensor.h"
#include "mmf2_module.h"
#include "module_vipnn.h"
#include "fwfs.h"
#include "hal_cache.h"

#define DBG_LEVEL	LOG_ERR

#define LOG_OFF		4
#define LOG_ERR		3
#define LOG_MSG		2
#define LOG_INF		1
#define LOG_ALL		0

#define dprintf(level, ...) if(level >= DBG_LEVEL) printf(__VA_ARGS__)

void *mbfacenet_get_network_filename(void)
{
	return (void *)"NN_MDL/mobilefacenet.nb";	// fix name for NN model binary
}

// before face recognition, the face should be aligned according to arcface reference landmark
// https://github.com/deepinsight/insightface/blob/master/python-package/insightface/utils/face_align.py
static float arcface_lm[5][2] = {
	{38.2946, 51.6963},
	{73.5318, 51.5014},
	{56.0252, 71.7366},
	{41.5493, 92.3655},
	{70.7299, 92.2041},
};

//--------CASCADE INPUT SETUP-------------------------------------------
static void mbfacenet_input_setup(void *last_res, int i, nn_data_param_t *input_param)
{
	/* setup roi and landmark */
	facedetect_res_t *fd_res = (facedetect_res_t *)last_res;

	// setup roi by result array
	int xmin, ymin, xmax, ymax;

	dprintf(LOG_MSG, "[mfn setup] %p, %d\n\r", fd_res, i);

	dprintf(LOG_MSG, "[mfn setup] w %d\n\r", input_param->img.width);
	dprintf(LOG_MSG, "[mfn setup] h %d\n\r", input_param->img.height);
	for (int k = 0; k < 6; k++) {
		dprintf(LOG_MSG, "[mfn setup] result %d %f\n\r", k, fd_res[i].result[k]);
	}

	// TODO : reduce type cast
	float tmp;
	tmp = fd_res[i].result[2];
	if (tmp <= 1.0) {
		xmin = (int)(tmp * input_param->img.width);
	} else {
		xmin = (int)(tmp);
	}

	tmp = fd_res[i].result[3];
	if (tmp <= 1.0) {
		ymin = (int)(tmp * input_param->img.height);
	} else {
		ymin = (int)(tmp);
	}

	tmp = fd_res[i].result[4];
	if (tmp <= 1.0) {
		xmax = (int)(tmp * input_param->img.width);
	} else {
		xmax = (int)(tmp);
	}

	tmp = fd_res[i].result[5];
	if (tmp <= 1.0) {
		ymax = (int)(tmp * input_param->img.height);
	} else {
		ymax = (int)(tmp);
	}


	for (int x = 0; x < 5; x++) {
		input_param->img.landmark.pos[x].x = fd_res[i].landmark.pos[x].x * input_param->img.width;
		input_param->img.landmark.pos[x].y = fd_res[i].landmark.pos[x].y * input_param->img.height;
	}

	input_param->img.roi.xmin = xmin;
	input_param->img.roi.ymin = ymin;
	input_param->img.roi.xmax = xmax;
	input_param->img.roi.ymax = ymax;
	input_param->priv = (void *)&fd_res[i];

	dprintf(LOG_MSG, "[mfn setup] update pic w %d, h %d\n\r", input_param->img.width, input_param->img.height);
	dprintf(LOG_MSG, "[mfn setup] update ROI %d %d %d %d\n\r", input_param->img.roi.xmin, input_param->img.roi.ymin, input_param->img.roi.xmax,
			input_param->img.roi.ymax);
}

//--------PRE PROCESS-------------------------------------------
static int face_in_width, face_in_height;
int mbfacenet_preprocess(void *data_in, nn_data_param_t *data_param, void *tensor_in, nn_tensor_param_t *tensor_param)
{
	void **tensor = (void **)tensor_in;
	rect_t *roi = (rect_t *)&data_param->img.roi;

	landmarki_t *landmark = &data_param->img.landmark;

	img_t img_in, img_out;

	img_in.width  = data_param->img.width;
	img_in.height = data_param->img.height;
	img_out.width  = tensor_param->dim[0].size[0];
	img_out.height = tensor_param->dim[0].size[1];
	face_in_width  = tensor_param->dim[0].size[0];
	face_in_height = tensor_param->dim[0].size[1];

	img_in.data = (uint8_t *)data_in;
	img_out.data = (uint8_t *)tensor[0];
	dprintf(LOG_MSG, "[mfn pre]src %d %d, dst %d %d\n\r", img_in.width, img_in.height, img_out.width, img_out.height);
	dprintf(LOG_MSG, "[mfn pre]roi %d %d %d %d \n\r", roi->xmin, roi->ymin, roi->xmax, roi->ymax);


	int len_adj = (landmark->pos[0].x - landmark->pos[1].x);
	int len_opp = (landmark->pos[0].y - landmark->pos[1].y);

	rotate_t rcfg;
	rcfg.cx = (landmark->pos[0].x + landmark->pos[1].x + landmark->pos[3].x + landmark->pos[4].x) / 4; // set point between eyes and mouth as rotation center x
	rcfg.cy = (landmark->pos[0].y + landmark->pos[1].y + landmark->pos[3].y + landmark->pos[4].y) / 4; // set point between eyes and mouth as rotation center y

	if (rcfg.cx == 0 && rcfg.cy == 0) {
		if (img_in.width == img_out.width && img_in.height == img_out.height) {
			dprintf(LOG_INF, "[nn preprocess]same size, just copy!\n\r");
			//memcpy(img_out.data, img_in.data, img_out.width * img_out.height * 3);
			img_dma_copy(img_out.data, img_in.data, img_out.width * img_out.height * 3);
		} else {
			dprintf(LOG_INF, "[nn preprocess]resize to NN input size!\n\r");
			img_resize_planar(&img_in, roi, &img_out);
		}
	} else {
		rcfg.angle = atan((float)len_opp / (float)len_adj);
		dprintf(LOG_MSG, "center %d,%d rotate %f \n\r", rcfg.cx, rcfg.cy, rcfg.angle * 180 / 3.1415926);
		img_rotation_fix(&img_in, roi, &rcfg, &img_out, &arcface_lm[2]);
	}

	dcache_clean_by_addr((uint32_t *)img_out.data, img_out.width * img_out.height * 3);

	return 0;
}

static float inner_product_arm(float *a, float *b, int len)
{
	float tmp;
	arm_dot_prod_f32(a, b, len, &tmp);
	return tmp;
}

static float sqrt_arm(float x)
{
	float tmp;
	arm_sqrt_f32(x, &tmp);
	return tmp;
}

static void face_feature_dump(int log_level, float *pfeature, int num)
{
	if (log_level >= DBG_LEVEL) {
		printf("--------------------------------------------------\r\n");
		printf("face feature dump\r\n");
		printf("--------------------------------------------------\r\n");
		for (int i = 0; i < num; i++) {
			printf("%6.2f ", pfeature[i]);
			if ((i + 1) % 16 == 0) {
				printf("\r\n");
			}
		}
		printf("--------------------------------------------------\r\n");
	}
}

//--------POST PROCESS-------------------------------------------

int mbfacenet_postprocess(void *tensor_out, nn_tensor_param_t *param, void *res)
{
	void **tensor = (void **)tensor_out;

	//int max_cnt = ctx->params.out_res_max_cnt;
	face_feature_res_t *ff_res = (face_feature_res_t *)res;

	facedetect_res_t *fd_res = (facedetect_res_t *)param->priv;

	// decode
	uint8_t *tt0 = (uint8_t *)tensor[0];
	int face_feature_dim = param->dim[0].size[0];

	float face_feature[face_feature_dim * sizeof(float)];

	//if(res_idx >= max_cnt)
	//	return 0;

	dprintf(LOG_INF, "face feature dim: %d\r\n", face_feature_dim);

	for (int i = 0; i < face_feature_dim; i++) {
		face_feature[i] = get_tensor_value(tt0, i, &param->format[0]);
	}

	// normalize
	float l2_norm = sqrt_arm(inner_product_arm(face_feature, face_feature, face_feature_dim));
	for (int i = 0; i < face_feature_dim; i++) {
		face_feature[i] = face_feature[i] / l2_norm;
	}

	// dump result
	face_feature_dump(LOG_INF, face_feature, face_feature_dim);

	// copy bbox and face feature vector
	memcpy(ff_res->result, fd_res->result, sizeof(fd_res->result));
	memcpy(ff_res->feature, face_feature, face_feature_dim * sizeof(float));

	// return handled object count
	return 1;
}

nnmodel_t mbfacenet_fwfs = {
	.nb 			= mbfacenet_get_network_filename,
	.preprocess 	= mbfacenet_preprocess,
	.postprocess 	= mbfacenet_postprocess,
	.cas_in_setup	= mbfacenet_input_setup,
	.model_src 		= MODEL_SRC_FILE,

	.name = "MBFACENET"
};
