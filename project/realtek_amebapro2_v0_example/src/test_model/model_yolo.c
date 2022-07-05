//------------------------------------------------------
// Yolov3-tiny & Yolov4-tiny & Yolov7-tiny (Darknet)
//------------------------------------------------------
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "img_process/img_process.h"
#include "mmf2_module.h"
#include "module_vipnn.h"
#include "hal_cache.h"

static float yolo_confidence_thresh = 0.5;    // default
static float yolo_nms_thresh = 0.3;      // default
static int yolo_new_coords = 0;  // 0 --> yolov3 & yolov4,  1 --> yolov7
static float *pAnchor = NULL;

// yolov4-tiny & yolov3-tiny
// https://github.com/AlexeyAB/darknet/blob/master/cfg/yolov4-tiny.cfg
static float anchor_yolov3v4_2layer[6][2] = {
	{81, 82}, {135, 169}, {344, 319},		// 13x13
	{23, 27}, {37, 58}, {81, 82},			// 26x26
};

// yolov4-tiny-3l (3 output layer)
// https://github.com/AlexeyAB/darknet/blob/master/cfg/yolov4-tiny-3l.cfg
static float anchor_yolo_3layer[9][2] = {
	{142, 110}, {192, 243}, {459, 401},		// 13x13
	{36, 75}, {76, 55}, {72, 146},			// 26x26
	{12, 16}, {19, 36}, {40, 28},			// 52x52
};

// yolov7-tiny-3l (3 output layer)
// https://github.com/AlexeyAB/darknet/blob/master/cfg/yolov7-tiny.cfg
static float anchor_yolov7_3layer[9][2] = {
	{10, 13}, {16, 30}, {33, 23},			// 52x52
	{30, 61}, {62, 45}, {59, 119},			// 26x26
	{116, 90}, {156, 198}, {373, 326},		// 13x13
};

static float u8_to_f(uint8_t val, uint8_t zero_p, float scale)
{
	return  scale * ((float)val - (float)zero_p);
}

static float s8_to_f(int8_t val, int q)
{
	return (float)val / (float)(1 << q);
}

static float s16_to_f(int16_t val, int q)
{
	return (float)val / (float)(1 << q);
}

static float bf16_to_f(__fp16 val)
{
	return (float)val;
}

static float q2f(void *val, nn_tensor_format_t *fmt)
{
	switch (fmt->buf_type) {
	case VIP_BUFFER_FORMAT_INT16:
		// s16
		return s16_to_f(*(int16_t *)val, fmt->fix_point_pos);
	case VIP_BUFFER_FORMAT_INT8:
		// s8
		return s8_to_f(*(int8_t *)val, fmt->fix_point_pos);
	case VIP_BUFFER_FORMAT_UINT8:
		// u8
		return u8_to_f(*(uint8_t *)val, fmt->zero_point, fmt->scale);
	case VIP_BUFFER_FORMAT_FP16:
		// bf16
		return bf16_to_f(*(__fp16 *)val);
	default:
		// fp32
		return *(float *)val;
	}
}

static uint8_t f_to_u8(float val, uint8_t zero_p, float scale)
{
	return (uint8_t)(val / scale) + zero_p;
}

static int8_t f_to_s8(float val, int q)
{
	return (int8_t)(val * (1 << q));
}

static int16_t f_to_s16(float val, int q)
{
	return (int16_t)(val * (1 << q));
}

static __fp16 f_to_bf16(float val)
{
	return (__fp16)val;
}

static float sigmoid_alt1(float x)
{
	return 1 / (1 + exp(-x));
}

static float sigmoid_alt2(float x)
{
	return (x / (1 + fabs(x))) * 0.5 + 0.5;
}

// -5~5, 256+1 level
float sigmoid_tab[] = {
	0.0067, 0.0070, 0.0072, 0.0075, 0.0078, 0.0081, 0.0084, 0.0088, 0.0091, 0.0095, 0.0099, 0.0102, 0.0107, 0.0111, 0.0115, 0.0120, 0.0124, 0.0129, 0.0134, 0.0140, 0.0145, 0.0151, 0.0157, 0.0163, 0.0169, 0.0176, 0.0183, 0.0190, 0.0197, 0.0205, 0.0213, 0.0221, 0.0230, 0.0239, 0.0248, 0.0258, 0.0268, 0.0278, 0.0289, 0.0300, 0.0311, 0.0323, 0.0336, 0.0349, 0.0362, 0.0376, 0.0390, 0.0405, 0.0421, 0.0437, 0.0454, 0.0471, 0.0489, 0.0507, 0.0526, 0.0546, 0.0567, 0.0588, 0.0610, 0.0633, 0.0656, 0.0680, 0.0706, 0.0732, 0.0759, 0.0786, 0.0815, 0.0845, 0.0876, 0.0907, 0.0940, 0.0974, 0.1009, 0.1045, 0.1082, 0.1120, 0.1160, 0.1200, 0.1242, 0.1285, 0.1330, 0.1375, 0.1422, 0.1471, 0.1520, 0.1571, 0.1624, 0.1678, 0.1733, 0.1790, 0.1848, 0.1907, 0.1968, 0.2031, 0.2095, 0.2160, 0.2227, 0.2295, 0.2365, 0.2436, 0.2509, 0.2583, 0.2659, 0.2736, 0.2814, 0.2894, 0.2975, 0.3057, 0.3141, 0.3225, 0.3311, 0.3398, 0.3486, 0.3576, 0.3666, 0.3757, 0.3849, 0.3942, 0.4036, 0.4130, 0.4225, 0.4321, 0.4417, 0.4513, 0.4610, 0.4707, 0.4805, 0.4902, 0.5000, 0.5098, 0.5195, 0.5293, 0.5390, 0.5487, 0.5583, 0.5679, 0.5775, 0.5870, 0.5964, 0.6058, 0.6151, 0.6243, 0.6334, 0.6424, 0.6514, 0.6602, 0.6689, 0.6775, 0.6859, 0.6943, 0.7025, 0.7106, 0.7186, 0.7264, 0.7341, 0.7417, 0.7491, 0.7564, 0.7635, 0.7705, 0.7773, 0.7840, 0.7905, 0.7969, 0.8032, 0.8093, 0.8152, 0.8210, 0.8267, 0.8322, 0.8376, 0.8429, 0.8480, 0.8529, 0.8578, 0.8625, 0.8670, 0.8715, 0.8758, 0.8800, 0.8840, 0.8880, 0.8918, 0.8955, 0.8991, 0.9026, 0.9060, 0.9093, 0.9124, 0.9155, 0.9185, 0.9214, 0.9241, 0.9268, 0.9294, 0.9320, 0.9344, 0.9367, 0.9390, 0.9412, 0.9433, 0.9454, 0.9474, 0.9493, 0.9511, 0.9529, 0.9546, 0.9563, 0.9579, 0.9595, 0.9610, 0.9624, 0.9638, 0.9651, 0.9664, 0.9677, 0.9689, 0.9700, 0.9711, 0.9722, 0.9732, 0.9742, 0.9752, 0.9761, 0.9770, 0.9779, 0.9787, 0.9795, 0.9803, 0.9810, 0.9817, 0.9824, 0.9831, 0.9837, 0.9843, 0.9849, 0.9855, 0.9860, 0.9866, 0.9871, 0.9876, 0.9880, 0.9885, 0.9889, 0.9893, 0.9898, 0.9901, 0.9905, 0.9909, 0.9912, 0.9916, 0.9919, 0.9922, 0.9925, 0.9928, 0.9930, 0.9933
};

static float sigmoid_alt3(float x)
{
	if (x > 5) {
		return 1;
	}
	if (x < -5) {
		return 0;
	}

	int index = (int)(x * 25.6) + 128;

	return sigmoid_tab[index];
}

#define sigmoid(x) sigmoid_alt3(x)

static float inv_sigmoid(float x)
{
	return -log((1 - x) / x);
}

void *yolov3_get_network_filename_init(void)
{
	yolo_new_coords = 0;
	pAnchor = &anchor_yolov3v4_2layer[0][0]; // setup anchor
	return (void *)"NN_MDL/yolov3_tiny.nb";	// fix name for NN model binary
}

void *yolov4_get_network_filename_init(void)
{
	yolo_new_coords = 0;
	pAnchor = &anchor_yolov3v4_2layer[0][0];
	return (void *)"NN_MDL/yolov4_tiny.nb";
}

void *yolov7_get_network_filename_init(void)
{
	yolo_new_coords = 1;
	pAnchor = &anchor_yolov7_3layer[0][0];
	return (void *)"NN_MDL/yolov7_tiny.nb";
}

static int yolo_in_width, yolo_in_height;

int yolo_preprocess(void *data_in, nn_data_param_t *data_param, void *tensor_in, nn_tensor_param_t *tensor_param)
{
	void **tensor = (void **)tensor_in;
	rect_t *roi = &data_param->img.roi;

	img_t img_in, img_out;

	img_in.width  = data_param->img.width;
	img_in.height = data_param->img.height;
	img_out.width  = tensor_param->dim[0].size[0];
	img_out.height = tensor_param->dim[0].size[1];
	yolo_in_width  = tensor_param->dim[0].size[0];
	yolo_in_height = tensor_param->dim[0].size[1];

	img_in.data   = (unsigned char *)data_in;
	img_out.data   = (unsigned char *)tensor[0];
	//printf("src %d %d, dst %d %d\n\r", img_in.width, img_in.height, img_out.width, img_out.height);
	//printf("roi %d %d %d %d \n\r", roi->xmin, roi->ymin, roi->xmax, roi->ymax);

	// resize src ROI area to dst
	if (img_in.width == img_out.width && img_in.height == img_out.height) {
		//memcpy(img_out.data, img_in.data, img_out.width * img_out.height * 3);
		img_dma_copy(img_out.data, img_in.data, img_out.width * img_out.height * 3);
	} else {
		img_resize_planar(&img_in, roi, &img_out);
	}

	dcache_clean_by_addr((uint32_t *)img_out.data, img_out.width * img_out.height * 3);
	//dcache_clean();		// if don't care other hardware dma, you can use this. ugly method, should clean dst address with its data size
	return 0;
}

typedef struct data_format_s {
	nn_tensor_format_t *format;
	nn_tensor_dim_t *dim;
} data_format_t;

static int yolo_get_offset(data_format_t *fmt, int i, int j, int n, int ei, int class_num)
{
	int w = fmt->dim->size[0];
	int h = fmt->dim->size[1];
	int es = 1;
	switch (fmt->format->buf_type) {
	case VIP_BUFFER_FORMAT_INT16:
	case VIP_BUFFER_FORMAT_FP16:
		es = 2;
		break;
	case VIP_BUFFER_FORMAT_FP32:
		es = 4;
		break;
	default:
		break;
	}

	return (((5 + class_num) * n + ei) * h * w + w * j + i) * es;
}

static void *yolo_get_data_addr(void *data, data_format_t *fmt, int i, int j, int n, int ei, int class_num)
{
	int offset = yolo_get_offset(fmt, i, j, n, ei, class_num);
	uint8_t *data8 = (uint8_t *)data;

	return (void *)&data8[offset];
}

static float yolo_get_data(void *data, data_format_t *fmt, int i, int j, int n, int ei, int class_num)
{
	void *datax = (void *)yolo_get_data_addr(data, fmt, i, j, n, ei, class_num);
	return q2f(datax, fmt->format);
}

static int yolo_filter_class(void *data, data_format_t *fmt, int i, int j, int n, int *class_idx, float threshold, int class_num)
{
	int count = 0;
	switch (fmt->format->buf_type) {
	case VIP_BUFFER_FORMAT_INT16: {
		int16_t th16 = f_to_s16(threshold, fmt->format->fix_point_pos);
		for (int x = 0; x < class_num; x++) {
			int16_t tmp = *(int16_t *)yolo_get_data_addr(data, fmt, i, j, n, 5 + x, class_num);
			if (tmp > th16) {
				class_idx[count] = x;
				count++;
			}
		}
		break;
	}
	case VIP_BUFFER_FORMAT_INT8: {
		int8_t th8 = f_to_s8(threshold, fmt->format->fix_point_pos);
		for (int x = 0; x < class_num; x++) {
			int8_t tmp = *(int8_t *)yolo_get_data_addr(data, fmt, i, j, n, 5 + x, class_num);
			if (tmp > th8) {
				class_idx[count] = x;
				count++;
			}
		}
		break;
	}
	case VIP_BUFFER_FORMAT_UINT8: {
		uint8_t th8 = f_to_u8(threshold, fmt->format->zero_point, fmt->format->scale);
		for (int x = 0; x < class_num; x++) {
			uint8_t tmp = *(uint8_t *)yolo_get_data_addr(data, fmt, i, j, n, 5 + x, class_num);
			if (tmp > th8) {
				class_idx[count] = x;
				count++;
			}
		}
		break;
	}
	case VIP_BUFFER_FORMAT_FP16: {
		__fp16 th16f = f_to_bf16(threshold);
		for (int x = 0; x < class_num; x++) {
			__fp16 tmp = *(__fp16 *)yolo_get_data_addr(data, fmt, i, j, n, 5 + x, class_num);
			if (tmp > th16f) {
				class_idx[count] = x;
				count++;
			}
		}
		break;
	}
	default: {
		float th32f = threshold;
		for (int x = 0; x < class_num; x++) {
			float tmp = *(float *)yolo_get_data_addr(data, fmt, i, j, n, 5 + x, class_num);
			if (tmp > th32f) {
				class_idx[count] = x;
				count++;
			}
		}
		break;
	}
	}

	//if(count > 0)WTF

	//	printf("count %d\n\r", count);

	return count;
}

typedef struct box_s {
	float prob;
	int invalid;
	float objectness;
	float class_prob;
	int class_idx;
	float x, y;
	float w, h;
} box_t;

//MAX_DETECT_OBJ_NUM defined in module_vipnn.h
static box_t res_box[MAX_DETECT_OBJ_NUM];
static box_t *p_res_box[MAX_DETECT_OBJ_NUM];
static int box_idx;

//-----------------------------IOU-----------------------------

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

typedef enum {
	IOU,
	DIOU,
} iou_t;

float overlap(float x1, float w1, float x2, float w2)
{
	float l1 = x1;
	float l2 = x2;
	float left = max(l1, l2);
	float r1 = x1 + w1;
	float r2 = x2 + w2;
	float right = min(r1, r2);
	return right - left;
}

float box_intersection(box_t *a, box_t *b)
{
	float w = overlap(a->x, a->w, b->x, b->w);
	float h = overlap(a->y, a->h, b->y, b->h);
	if (w < 0 || h < 0) {
		return 0;
	}
	float area = w * h;
	return area;
}

float box_union(box_t *a, box_t *b)
{
	float i = box_intersection(a, b);
	float u = a->w * a->h + b->w * b->h - i;
	return u;
}

float box_iou(box_t *a, box_t *b)
{
	return box_intersection(a, b) / box_union(a, b);
}

// return a smallest box contain a and b
box_t box_box(box_t *a, box_t *b)
{
	box_t c;
	float c_topx = min(a->x, b->x);
	float c_topy = min(a->y, b->y);
	float c_botx = max(a->x + a->w, b->x + b->w);
	float c_boty = max(a->y + a->h, b->y + b->h);
	c.x = c_topx;
	c.y = c_topy;
	c.w = c_botx - c_topx;
	c.h = c_boty - c_topy;
	return c;
}

float box_diou(box_t *a, box_t *b)
{
	float a_cx = a->x + (a->w / 2);
	float a_cy = a->y + (a->h / 2);
	float b_cx = b->x + (b->w / 2);
	float b_cy = b->y + (b->h / 2);
	box_t ab = box_box(a, b);
	float c = ab.w * ab.w + ab.h * ab.h;
	float d = (a_cx - b_cx) * (a_cx - b_cx) + (a_cy - b_cy) * (a_cy - b_cy);
	float u = pow(d / c, 0.6);
	return box_iou(a, b) - u;
}

float box_nms_iou(box_t *a, box_t *b, iou_t iou_type)
{
	switch (iou_type) {
	case IOU:
		return box_iou(a, b);
	case DIOU:
		return box_diou(a, b);
	}
	return 0.0;
}

//-----------------------------NMS-----------------------------

int nms_comparator(const void *pa, const void *pb)
{
	box_t *a = *(box_t **)pa;
	box_t *b = *(box_t **)pb;
	float diff = a->prob - b->prob;
	if (diff < 0) {
		return 1;
	} else if (diff > 0) {
		return -1;
	}
	return 0;
}

static int setup_class_box(int class, int total)
{
	int class_cnt = 0;
	for (int i = 0; i < total; i++) {
		if (res_box[i].class_idx == class) {
			p_res_box[class_cnt] = &res_box[i];
			class_cnt++;
		}
	}
	return class_cnt;
}

static void do_nms(int classes, int total, float threshold, iou_t nms_kind)
{
	int class_cnt;
	box_t *a, *b;
	for (int c = 0; c < classes; c++) {
		class_cnt = setup_class_box(c, total);
		qsort(p_res_box, class_cnt, sizeof(box_t *), nms_comparator);
		for (int i = 0; i < class_cnt; i++) {
			a = p_res_box[i];
			for (int j = i + 1; j < class_cnt; j++) {
				b = p_res_box[j];
				if (box_nms_iou(a, b, nms_kind) > threshold) {
					b->invalid = 1;
				}
			}
		}
	}
}

static int __decode_yolo(data_format_t *fmt, void *out, int i, int j, int n, void *p_anchor)
{
	int max_i = 0;
	float cp = 0;
	float p = 0;
	int class_idx[80];
	int class_cnt = 0;
	int pass_cnt = 0;

	float (*anchor)[2] = (float (*)[2])p_anchor;

	//yolo_13_t *out13 = (yolo_13_t*)out;
	int grid_w = fmt->dim->size[0];
	int grid_h = fmt->dim->size[1];

	int classes = fmt->dim->size[2] / 3 - 5;

	if (yolo_new_coords) {
		p = yolo_get_data(out, fmt, i, j, n, 4, classes);
	} else {
		p = sigmoid(yolo_get_data(out, fmt, i, j, n, 4, classes));
	}

	if (p < yolo_confidence_thresh) {
		return 0;
	}
	//int max_i = __find_max_index13(out, i, j, n, 80);

	if (yolo_new_coords) {
		class_cnt = yolo_filter_class(out, fmt, i, j, n, class_idx, 0.5, classes);
	} else {
		class_cnt = yolo_filter_class(out, fmt, i, j, n, class_idx, inv_sigmoid(0.5), classes);
	}

	for (int ci = 0; ci < class_cnt; ci++) {
		max_i = class_idx[ci];
		if (yolo_new_coords) {
			cp = yolo_get_data(out, fmt, i, j, n, 5 + max_i, classes);
		} else {
			cp = sigmoid(yolo_get_data(out, fmt, i, j, n, 5 + max_i, classes));
		}

		if (cp * p >= yolo_confidence_thresh && box_idx < MAX_DETECT_OBJ_NUM) {

			float tx, ty, tw, th;
			float cx, cy, w, h;
			if (yolo_new_coords) {
				tx = yolo_get_data(out, fmt, i, j, n, 0, classes);
				ty = yolo_get_data(out, fmt, i, j, n, 1, classes);
				tw = yolo_get_data(out, fmt, i, j, n, 2, classes);
				th = yolo_get_data(out, fmt, i, j, n, 3, classes);

				cx = (i + tx * 2 - 0.5) / grid_w;
				cy = (j + ty * 2 - 0.5) / grid_h;
				w = tw * tw * 4 * anchor[n][0] / yolo_in_width;
				h = th * th * 4 * anchor[n][1] / yolo_in_height;
			} else {
				tx = sigmoid(yolo_get_data(out, fmt, i, j, n, 0, classes));
				ty = sigmoid(yolo_get_data(out, fmt, i, j, n, 1, classes));
				tw = yolo_get_data(out, fmt, i, j, n, 2, classes);
				th = yolo_get_data(out, fmt, i, j, n, 3, classes);

				cx = (i + tx) / grid_w;
				cy = (j + ty) / grid_h;
				w = exp(tw) * anchor[n][0] / yolo_in_width;
				h = exp(th) * anchor[n][1] / yolo_in_height;
			}

			float x = cx - w / 2;
			float y = cy - h / 2;
			x = max(x, 0);
			x = min(x, 1);
			y = max(y, 0);
			y = min(y, 1);

			//printf("x y w h = %f %f %f %f\n\r", x, y, w, h);
			//printf("x y w h = %3.0f %3.0f %3.0f %3.0f\n\r", x*yolo_in_width, y*yolo_in_height, w*yolo_in_width, h*yolo_in_height);
			//printf("tx %2.6f ty %2.6f tw %2.6f th %2.6f , p %2.6f, cp %2.6f, c %d, cn %d\n\r", tx, ty, tw, th, p, cp, max_i, class_cnt);

			res_box[box_idx].objectness = p;
			res_box[box_idx].class_prob = cp;
			res_box[box_idx].class_idx = max_i;
			res_box[box_idx].prob = cp * p;
			res_box[box_idx].x = x;
			res_box[box_idx].y = y;
			res_box[box_idx].w = w;
			res_box[box_idx].h = h;
			box_idx++;
			pass_cnt++;
		}
	}

	return pass_cnt;
}

static vipnn_res_t yolo_res;
void *yolo_postprocess(void *tensor_out, nn_tensor_param_t *param)
{
	void **tensor = (void **)tensor_out;

	int classes = param->dim[0].size[2] / 3 - 5;  // tensor depth = (5 + classes) * 3
	int output_count = param->count;

	// reset box index
	box_idx = 0;
	memset(res_box, 0, sizeof(res_box));

	// check anckor is set
	if (pAnchor == NULL) {
		return NULL;
	}

	data_format_t fmt;
	for (int n = 0; n < output_count; n++) {
		uint8_t *bb0 = (uint8_t *)tensor[n];
		fmt.format = &param->format[n];
		fmt.dim = &param->dim[n];
		int w = fmt.dim->size[0];
		int h = fmt.dim->size[1];

		//printf("BB0 %d x %d\n\r", w, h);
		for (int i = 0; i < w; i++) {
			for (int j = 0; j < h; j++) {
				int ret = 0;
				ret += __decode_yolo(&fmt, bb0, i, j, 0, (void *)&pAnchor[n * 3 * 2]);
				ret += __decode_yolo(&fmt, bb0, i, j, 1, (void *)&pAnchor[n * 3 * 2]);
				ret += __decode_yolo(&fmt, bb0, i, j, 2, (void *)&pAnchor[n * 3 * 2]);
				//if (ret > 0) {
				//	printf("%d %d\n\r-------\n\r", i, j);
				//}
			}
		}
	}

	do_nms(classes, box_idx, yolo_nms_thresh, DIOU);

	// dump result
	/*
	for (int i = 0; i < box_idx; i++) {
		box_t *b = &res_box[i];
		printf("x y w h = %f %f %f %f\n\r", b->x, b->y, b->w, b->h);
		printf("x y w h = %3.0f %3.0f %3.0f %3.0f\n\r", b->x * yolo_in_width, b->y * yolo_in_height, b->w * yolo_in_width, b->h * yolo_in_height);
		printf("p %2.6f, obj %2.6f, cp %2.6f, class %d, invalid %d\n\r", b->prob, b->objectness, b->class_prob, b->class_idx, b->invalid);
	}
	*/
	yolo_res.od_res.obj_num  = 0;
	for (int i = 0; i < box_idx; i++) {
		box_t *b = &res_box[i];

		if (b->invalid == 0) {
			yolo_res.od_res.result[yolo_res.od_res.obj_num * 6 + 0] = b->class_idx;
			yolo_res.od_res.result[yolo_res.od_res.obj_num * 6 + 1] = b->prob;
			yolo_res.od_res.result[yolo_res.od_res.obj_num * 6 + 2] = b->x;	// top_x
			yolo_res.od_res.result[yolo_res.od_res.obj_num * 6 + 3] = b->y;	// top_y
			yolo_res.od_res.result[yolo_res.od_res.obj_num * 6 + 4] = b->x + b->w; // bottom_x
			yolo_res.od_res.result[yolo_res.od_res.obj_num * 6 + 5] = b->y + b->h; // bottom_y
			yolo_res.od_res.obj_num++;
		}
	}

	return &yolo_res;
}

void yolo_set_confidence_thresh(void *confidence_thresh)
{
	yolo_confidence_thresh = *(float *)confidence_thresh;
	printf("set yolo confidence thresh to %f\n\r", *(float *)confidence_thresh);
}

void yolo_set_nms_thresh(void *nms_thresh)
{
	yolo_nms_thresh = *(float *)nms_thresh;
	printf("set yolo NMS thresh to %f\n\r", *(float *)nms_thresh);
}

nnmodel_t yolov3_tiny = {
	.nb 			= yolov3_get_network_filename_init,
	.preprocess 	= yolo_preprocess,
	.postprocess 	= yolo_postprocess,
	.model_src 		= MODEL_SRC_FILE,
	.set_confidence_thresh   = yolo_set_confidence_thresh,
	.set_nms_thresh     = yolo_set_nms_thresh,

	.name = "YOLOv3t"
};

nnmodel_t yolov4_tiny = {
	.nb 			= yolov4_get_network_filename_init,
	.preprocess 	= yolo_preprocess,
	.postprocess 	= yolo_postprocess,
	.model_src 		= MODEL_SRC_FILE,
	.set_confidence_thresh   = yolo_set_confidence_thresh,
	.set_nms_thresh     = yolo_set_nms_thresh,

	.name = "YOLOv4t"
};

nnmodel_t yolov7_tiny = {
	.nb 			= yolov7_get_network_filename_init,
	.preprocess 	= yolo_preprocess,
	.postprocess 	= yolo_postprocess,
	.model_src 		= MODEL_SRC_FILE,
	.set_confidence_thresh   = yolo_set_confidence_thresh,
	.set_nms_thresh     = yolo_set_nms_thresh,

	.name = "YOLOv7t"
};
