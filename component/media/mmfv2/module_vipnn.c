/******************************************************************************
*
* Copyright(c) 2007 - 2018 Realtek Corporation. All rights reserved.
*
******************************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include "rtl8735b.h"
#include <hal_cache.h>
#include <hal_sys_ctrl.h>
#include "mmf2_module.h"
#include "osdep_service.h"
#include "module_vipnn.h"
#include "avcodec.h"

//#include "nn_api.h"

// TODO move to vipnn context
#define DBG_LEVEL	LOG_MSG

#define LOG_OFF		4
#define LOG_ERR		3
#define LOG_MSG		2
#define LOG_INF		1
#define LOG_ALL		0
#define LOG_DBG		-1

#define dprintf(level, ...) if((level >= DBG_LEVEL) || (level==LOG_DBG && level==DBG_LEVEL)) do{printf(__VA_ARGS__);}while(0)

static int vipnn_inited = 0;
static _mutex nn_mutex;

static int vipnn_module_cnt = 0;

#define TICK_INIT()
#define TICK_GET() (uint32_t)mm_read_mediatime_ms()

#define NN_MEASURE_INIT(n)	do{ctx->measure.tick[n] = 0; TICK_INIT();}while(0)
#define NN_MEASURE_START(n) do{ctx->measure.tick[n] = TICK_GET();}while(0)
#define NN_MEASURE_STOP(n)  do{ctx->measure.tick[n] = TICK_GET() - ctx->measure.tick[n];}while(0)
#if DBG_LEVEL <= LOG_MSG
#define NN_MEASURE_PRINT(n) do{printf("%s tick[%d] = %d\n\r", ctx->params.model->name, n, ctx->measure.tick[n]);}while(0)
#else
#define NN_MEASURE_PRINT(n) do{}while(0)
#endif

#define CHK_NN(condition)   do{if(!(condition)) return 0;}while(0)

int vipnn_deoply_network(void *p);
void *vipnn_new_item(void *p);
void *vipnn_del_item(void *p, void *d);

/*
void vipnn_mark(char *model_name, int num)
{
	mm_printf("vnn %s %d\n\r",model_name, num);
}
*/

//#define vipnn_mark() mm_printf("vnn %s %d\n\r",ctx->params.model->name, __LINE__);
#define vipnn_mark()

static int vipnn_handle(void *p, void *input, void *output)
{
	vipnn_ctx_t *ctx = (vipnn_ctx_t *)p;
	mm_queue_item_t *input_item = (mm_queue_item_t *)input;
	mm_queue_item_t *output_item = (mm_queue_item_t *)output;

	void *data;
	uint32_t file_size;
	uint32_t buff_size;
	vip_status_e status = VIP_SUCCESS;
	int preprocess_status = 0;

	CHK_NN(ctx->status == VIPNN_APPLIED);
	//void *post_res = NULL;
	int post_res_cnt = 0;
	void *out_tensor[6];

	if (ctx->status != VIPNN_APPLIED) {
		return 0;
	}

	dprintf(LOG_INF, "in %p out %p\n\r", input, output);
	dprintf(LOG_INF, "run %s model\n\r", ctx->params.model->name);

	if (ctx->measure.time0 == 0) {
		ctx->measure.time0 = (int)xTaskGetTickCount();
	}

	int mark = 0;
	vipnn_mark();
	// set input
	data = vip_map_buffer(ctx->input_buffers[0]);
	buff_size = vip_get_buffer_size(ctx->input_buffers[0]);

	void *in_tensor[6] = {NULL};
	for (int i = 0; i < ctx->input_count; i++) {
		in_tensor[i] = (void *)vip_map_buffer(ctx->input_buffers[i]);
	}

	vipnn_mark();

	vipnn_out_buf_t *last_model_output = (vipnn_out_buf_t *)input_item->data_addr;
	void *input_data = (void *)input_item->data_addr;
	int input_size = input_item->size;
	nn_data_param_t *input_param = ctx->params.in_param;
	int loop_cnt = 1;

	if (input_param != NULL && input_param->codec_type != input_item->type) {
		dprintf(LOG_MSG, "%s error: model input type[%d] not match to source type[%d]\n\r", ctx->params.model->name, input_param->codec_type, input_item->type);
		return 0;
	}

	if (ctx->cas_mode != 0) {
		dprintf(LOG_INF, "cascade mode\n\r");
		dprintf(LOG_INF, "cascade: input = %x\n\r", last_model_output);
		dprintf(LOG_INF, "cascade: obj = %d\n\r", last_model_output->res_cnt);
	}

	// cascade mode and last model object number = 0, stop inference
	if (ctx->cas_mode != 0 && last_model_output->res_cnt == 0) {
		return 0;
	}

	if (ctx->cas_mode == 2) {
		loop_cnt = last_model_output->res_cnt;
	}

	dprintf(LOG_INF, "loop cnt = %d\n\r", loop_cnt);

	if (loop_cnt > MAX_OUT_BUFFER_CNT) {
		loop_cnt = MAX_OUT_BUFFER_CNT;
		dprintf(LOG_INF, "set loop cnt to %d\n\r", MAX_OUT_BUFFER_CNT);
	}

	vipnn_out_buf_t *out = NULL;//(vipnn_out_buf_t *)(output_item->data_addr);

	if (output_item == NULL || output_item->data_addr == 0) {
		dprintf(LOG_INF, "[vipnn] use tmp buffer\n\r");
		out = ctx->tmp_item;
	} else {
		out = (vipnn_out_buf_t *)(output_item->data_addr);
	}

	out->res_cnt = 0;


	for (int i = 0; i < loop_cnt; i++) {
		// update input_param and input_data ==> input image and roi here

		nn_data_param_t cas_input_param;
		if (ctx->cas_mode != 0) {
			cas_input_param.codec_type = last_model_output->input_param->codec_type;
			cas_input_param.img.width = last_model_output->input_param->img.width;
			cas_input_param.img.height = last_model_output->input_param->img.height;
			cas_input_param.img.rgb = 0;

			if (ctx->params.model->cas_in_setup) {
				ctx->params.model->cas_in_setup(&last_model_output->res[0], i, &cas_input_param);
			} else {
				cas_input_param.img.roi.xmin = 0;
				cas_input_param.img.roi.ymin = 0;
				cas_input_param.img.roi.xmax = cas_input_param.img.width;
				cas_input_param.img.roi.ymax = cas_input_param.img.height;
			}

			input_data = last_model_output->input_data;
			input_param = &cas_input_param;
		} else {
			input_param->priv = (void *)input_data;
		}


		dprintf(LOG_INF, "input %x w %d h %d\n\r", input_data, input_param->img.width, input_param->img.height);

		if (ctx->params.model->preprocess) {
			if (ctx->params.in_param) {
				ctx->params.in_param->size_in_byte = input_size;
			}
			preprocess_status = ctx->params.model->preprocess(input_data, input_param, (void *)in_tensor, &ctx->params.model->input_param);
		} else {
			memcpy(data, input_data, buff_size > input_size ? input_size : buff_size);
			preprocess_status = 0;
		}

		CHK_NN(preprocess_status == 0);

		vipnn_mark();
		CHK_NN((status = vip_set_input(ctx->network, 0, ctx->input_buffers[0])) == VIP_SUCCESS);
		// set output

		vipnn_mark();
		for (int i = 0; i < ctx->output_count; i++) {
			CHK_NN((status = vip_set_output(ctx->network, i, ctx->output_buffers[i])) == VIP_SUCCESS);
		}

		vipnn_mark();

		rtw_mutex_get(&nn_mutex);
		// run network
		NN_MEASURE_START(0);
#if 0
		CHK_NN((status = vip_trigger_network(ctx->network)) == VIP_SUCCESS);
		CHK_NN((status = vip_wait_network(ctx->network)) == VIP_SUCCESS);
#else
		CHK_NN((status = vip_run_network(ctx->network)) == VIP_SUCCESS);
#endif
		NN_MEASURE_STOP(0);
		NN_MEASURE_PRINT(0);

		rtw_mutex_put(&nn_mutex);

		vipnn_mark();
		for (int i = 0; i < ctx->output_count; i++) {
			CHK_NN((status = vip_flush_buffer(ctx->output_buffers[i], VIP_BUFFER_OPER_TYPE_INVALIDATE)) == VIP_SUCCESS);
		}

		vipnn_mark();
		// get output and do postprocessing
		for (int i = 0; i < ctx->output_count; i++) {
			out_tensor[i] = (void *)vip_map_buffer(ctx->output_buffers[i]);
		}

		vipnn_mark();

		if (ctx->params.model->postprocess) {
			dprintf(LOG_INF, "out %p, res %p\n\r", out, &out->res[0]);

			// cascade mode: pass last model result to post-processing
			if (ctx->cas_mode != 0) {
				ctx->params.model->output_param.priv = (void *)(((uint8_t *)&last_model_output->res[0]) + i * last_model_output->res_size);
			} else {
				ctx->params.model->output_param.priv = NULL;
			}

			//post_res_cnt = ctx->params.model->postprocess(out_tensor, &ctx->params.model->output_param, &out->res[0], out->res_cnt);
			dprintf(LOG_INF, "out %p, res %p\n\r", &out->res[0], (void *)((uint8_t *)&out->res[0] + out->res_cnt * ctx->params.out_res_size));
			post_res_cnt = ctx->params.model->postprocess(out_tensor, &ctx->params.model->output_param,
						   (void *)((uint8_t *)&out->res[0] + out->res_cnt * ctx->params.out_res_size));

			out->res_cnt += post_res_cnt;
		}

		vipnn_mark();

		if (ctx->module_out_en) {
			out->input_data = input_data;
			if (ctx->cas_mode != 0) {
				out->input_param = last_model_output->input_param;
			} else {
				out->input_param = input_param;
			}
		}

		if (ctx->module_out_en && ctx->params.save_out_tensor) {
			out->tensors.vipnn_out_tensor_num = ctx->output_count;
			for (int i = 0; i < ctx->output_count; i++) {
				out->tensors.vipnn_out_tensor[i] = out_tensor[i];
				switch (ctx->vip_param_out[i].quant_format) {
				case VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT:
					out->tensors.quant_format[i] = VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT;
					out->tensors.quant_data[i].dfp.fixed_point_pos = ctx->vip_param_out[i].quant_data.dfp.fixed_point_pos;
					out->tensors.vipnn_out_tensor_size[i] = sizeof(int16_t);
					break;
				case VIP_BUFFER_QUANTIZE_TF_ASYMM:
					out->tensors.quant_format[i] = VIP_BUFFER_QUANTIZE_TF_ASYMM;
					out->tensors.quant_data[i].affine.scale = ctx->vip_param_out[i].quant_data.affine.scale;
					out->tensors.quant_data[i].affine.zeroPoint = ctx->vip_param_out[i].quant_data.affine.zeroPoint;
					out->tensors.vipnn_out_tensor_size[i] = sizeof(uint8_t);
					break;
				default:
					//printf(", none-quant\n\r");
					out->tensors.vipnn_out_tensor_size[i] = sizeof(__fp16);
				}
				for (int k = 0; k < ctx->params.model->output_param.dim[i].num; k++) {
					out->tensors.vipnn_out_tensor_size[i] = out->tensors.vipnn_out_tensor_size[i] * ctx->params.model->output_param.dim[i].size[k];
				}
			}
		}
	}

	vipnn_mark();

	if (ctx->disp_postproc) {
		ctx->disp_postproc(out, input_param);
	}

	vipnn_mark();

	ctx->measure.count++;
	if (ctx->measure.count % 16 == 0) {
		float nn_fps = (float)ctx->measure.count * 1000.0 / (float)(xTaskGetTickCount() - ctx->measure.time0);
		dprintf(LOG_MSG, ">>> %s FPS = %0.2f\n\r", ctx->params.model->name, nn_fps);
		dprintf(LOG_INF, ">>> %s count %d tick %d\n\r", ctx->params.model->name, nn_fps, ctx->measure.count, xTaskGetTickCount() - ctx->measure.time0);
	}
	vipnn_mark();

	/*------------------------------------------------------*/
	if (ctx->module_out_en && (out->res_cnt > 0 || ctx->params.save_out_tensor)) {
		output_item->size = sizeof(vipnn_out_buf_t) + ctx->params.out_res_size * ctx->params.out_res_max_cnt;
		//memcpy(output_item->data_addr, &tensor_out_next, output_item->size);
		output_item->timestamp = input_item->timestamp;
		output_item->type = AV_CODEC_ID_NN_RAW;
		memcpy(output_item->name, input_item->name, sizeof(output_item->name));

		return output_item->size;
	}

	vipnn_mark();
	return 0;
}

int vipnn_control(void *p, int cmd, int arg)
{
	int ret = 0;
	vipnn_ctx_t *ctx = (vipnn_ctx_t *)p;

	switch (cmd) {
	case CMD_VIPNN_SET_MODEL:
		ctx->params.model = (nnmodel_t *)arg;
		break;
	case CMD_VIPNN_SET_IN_PARAMS:
		ctx->params.in_param = (nn_data_param_t *)arg;
		if (!ctx->params.in_param->codec_type) {
			ctx->params.in_param->codec_type = AV_CODEC_ID_UNKNOWN;
		}
		break;
	case CMD_VIPNN_SET_DISPPOST:
		ctx->disp_postproc = (disp_postprcess_t)arg;
		break;
	case CMD_VIPNN_GET_STATUS:
		*(vipnn_status_t *)arg = ctx->status;
		break;
	case CMD_VIPNN_SET_CONFIDENCE_THRES:
		if (ctx->params.model->set_confidence_thresh) {
			ctx->params.model->set_confidence_thresh((void *)arg);
		}
		break;
	case CMD_VIPNN_SET_NMS_THRES:
		if (ctx->params.model->set_nms_thresh) {
			ctx->params.model->set_nms_thresh((void *)arg);
		}
		break;
	case CMD_VIPNN_SET_OUTPUT:
		ctx->module_out_en = (bool)arg;
		((mm_context_t *)ctx->parent)->module->output_type = MM_TYPE_VSINK;
		break;
	case CMD_VIPNN_SET_OUTPUT_TYPE:
		ctx->module_out_type = (vipnn_out_type_t)arg;
		break;
	case CMD_VIPNN_SET_CASCADE:
		ctx->cas_mode = arg;
		break;
	case CMD_VIPNN_SET_RES_SIZE:
		ctx->params.out_res_size = arg;
		break;
	case CMD_VIPNN_SET_RES_MAX_CNT:
		ctx->params.out_res_max_cnt = arg;
		break;
	case CMD_VIPN_SET_SAVE_OUT_TENSOR:
		ctx->params.save_out_tensor = arg;
		break;
	case CMD_VIPNN_APPLY:
		ctx->tmp_item = vipnn_new_item(ctx);
		if (!ctx->tmp_item) {
			dprintf(LOG_ERR, "%s out of resource %d\n\r", ctx->params.model->name, __LINE__);
			ret = -1;
			break;
		}
		rtw_mutex_get(&nn_mutex);
		dprintf(LOG_MSG, "Deploy %s\n\r", ctx->params.model->name);
		ret = vipnn_deoply_network(ctx);
		rtw_mutex_put(&nn_mutex);
		if (ret == 0) {
			ctx->status = VIPNN_APPLIED;
		}
		break;
	}

	return ret;
}

void vipnn_deinited(void *p)
{
	vipnn_ctx_t *ctx = (vipnn_ctx_t *)p;

	vip_finish_network(ctx->network);

	vip_destroy_network(ctx->network);

	for (int i = 0; i < ctx->input_count; i++) {
		vip_unmap_buffer(ctx->input_buffers[i]);
		vip_destroy_buffer(ctx->input_buffers[i]);
	}

	for (int i = 0; i < ctx->output_count; i++) {
		vip_unmap_buffer(ctx->output_buffers[i]);
		vip_destroy_buffer(ctx->output_buffers[i]);
	}

	if (ctx->params.model->release) {
		ctx->params.model->release();
	}

	ctx->status = VIPNN_DEINITED;
}

void *vipnn_destroy(void *p)
{
	vipnn_ctx_t *ctx = (vipnn_ctx_t *)p;

	if (ctx) {
		if (ctx->status != VIPNN_DEINITED) {
			vipnn_deinited(ctx);
		}

		if (ctx->tmp_item) {
			vipnn_del_item(ctx, ctx->tmp_item);
		}

		free(ctx);

		vipnn_module_cnt--;
		if (vipnn_module_cnt == 0 && vipnn_inited == 1) {
			vip_destroy();  // destroy viplite engine if it is last opened vipnn module
			printf("vip_destroy done\r\n");
			rtw_mutex_free(&nn_mutex);
			vipnn_inited = 0;
		}
	}
	return NULL;
}

#define ONERROR(status, string, exit_anchor) \
if (status != VIP_SUCCESS) { \
	printf("error: %s\n", string); \
	goto exit_anchor; \
}

void vipnn_dump_network_io_params(void *p)
{
	vipnn_ctx_t *ctx = (vipnn_ctx_t *)p;

	printf("---------------------------------\n\r");
	printf("input count %d, output count %d\n\r", ctx->input_count, ctx->output_count);
	for (int i = 0; i < ctx->input_count; i++) {
		printf("input param %d\n\r", i);
		printf("\tdata_format  %x\n\r", ctx->vip_param_in[i].data_format);
		printf("\tmemory_type  %x\n\r", ctx->vip_param_in[i].memory_type);
		printf("\tnum_of_dims  %x\n\r", ctx->vip_param_in[i].num_of_dims);
		//printf("\tquant_data   %x\n\r", ctx->vip_param_in[i].quant_data);
		printf("\tquant_format %x\n\r", ctx->vip_param_in[i].quant_format);
		printf("\tquant_data  ");
		switch (ctx->vip_param_in[i].quant_format) {
		case VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT:
			printf(", dfp=%d\n\r", ctx->vip_param_in[i].quant_data.dfp.fixed_point_pos);
			break;
		case VIP_BUFFER_QUANTIZE_TF_ASYMM:
			printf(", scale=%f, zero_point=%d\n\r", ctx->vip_param_in[i].quant_data.affine.scale,
				   ctx->vip_param_in[i].quant_data.affine.zeroPoint);
			break;
		default:
			printf(", none-quant\n\r");
		}
		//printf("\tsizes        %x\n\r", ctx->vip_param_in[i].sizes);
		printf("\tsizes        ");
		for (int x = 0; x < 6; x++) {
			printf("%x ", ctx->vip_param_in[i].sizes[x]);
		}
		printf("\n\r");
	}
	for (int i = 0; i < ctx->output_count; i++) {
		printf("output param %d\n\r", i);
		printf("\tdata_format  %x\n\r", ctx->vip_param_out[i].data_format);
		printf("\tmemory_type  %x\n\r", ctx->vip_param_out[i].memory_type);
		printf("\tnum_of_dims  %x\n\r", ctx->vip_param_out[i].num_of_dims);
		//printf("\tquant_data   %x\n\r", ctx->vip_param_out[i].quant_data);
		printf("\tquant_format %x\n\r", ctx->vip_param_out[i].quant_format);
		printf("\tquant_data  ");
		switch (ctx->vip_param_out[i].quant_format) {
		case VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT:
			printf(", dfp=%d\n\r", ctx->vip_param_out[i].quant_data.dfp.fixed_point_pos);
			break;
		case VIP_BUFFER_QUANTIZE_TF_ASYMM:
			printf(", scale=%f, zero_point=%d\n\r", ctx->vip_param_out[i].quant_data.affine.scale,
				   ctx->vip_param_out[i].quant_data.affine.zeroPoint);
			break;
		default:
			printf(", none-quant\n\r");
		}
		//printf("\tsizes        %x\n\r", ctx->vip_param_out[i].sizes);
		printf("\tsizes        ");
		for (int x = 0; x < 6; x++) {
			printf("%x ", ctx->vip_param_out[i].sizes[x]);
		}
		printf("\n\r");
	}
	printf("---------------------------------\n\r");
}

static vip_int32_t init_io_buffers(void *p)
{
	vipnn_ctx_t *ctx = (vipnn_ctx_t *)p;
	vip_network network = ctx->network;

	int i = 0;

	vip_query_network(network, VIP_NETWORK_PROP_INPUT_COUNT, &ctx->input_count);
	if (ctx->input_count > MAX_IO_NUM) {
		dprintf(LOG_ERR, "error, input count is more than max value=%d\n\r", MAX_IO_NUM);
		return -1;
	}

	vip_query_network(network, VIP_NETWORK_PROP_OUTPUT_COUNT, &ctx->output_count);
	if (ctx->output_count > MAX_IO_NUM) {
		dprintf(LOG_ERR, "error, output count is more than max value=%d\n\r", MAX_IO_NUM);
		return -1;
	}

	for (i = 0; i < ctx->input_count; i++) {
		vip_buffer_create_params_t *param = &ctx->vip_param_in[i];
		memset(param, 0, sizeof(vip_buffer_create_params_t));
		param->memory_type = VIP_BUFFER_MEMORY_TYPE_DEFAULT;
		vip_query_input(network, i, VIP_BUFFER_PROP_DATA_FORMAT, &param->data_format);
		vip_query_input(network, i, VIP_BUFFER_PROP_NUM_OF_DIMENSION, &param->num_of_dims);
		vip_query_input(network, i, VIP_BUFFER_PROP_SIZES_OF_DIMENSION, param->sizes);
		vip_query_input(network, i, VIP_BUFFER_PROP_QUANT_FORMAT, &param->quant_format);
		switch (param->quant_format) {
		case VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT:
			vip_query_input(network, i, VIP_BUFFER_PROP_FIXED_POINT_POS,
							&param->quant_data.dfp.fixed_point_pos);
			break;
		case VIP_BUFFER_QUANTIZE_TF_ASYMM:
			vip_query_input(network, i, VIP_BUFFER_PROP_TF_SCALE,
							&param->quant_data.affine.scale);
			vip_query_input(network, i, VIP_BUFFER_PROP_TF_ZERO_POINT,
							&param->quant_data.affine.zeroPoint);
		default:
			break;
		}

		dprintf(LOG_MSG, "input %d dim %d %d %d %d, data format=%d, quant_format=%d",
				i, param->sizes[0], param->sizes[1], param->sizes[2], param->sizes[3], param->data_format,
				param->quant_format);

		switch (param->quant_format) {
		case VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT:
			dprintf(LOG_MSG, ", dfp=%d\n\r", param->quant_data.dfp.fixed_point_pos);
			break;
		case VIP_BUFFER_QUANTIZE_TF_ASYMM:
			dprintf(LOG_MSG, ", scale=%f, zero_point=%d\n\r", param->quant_data.affine.scale,
					param->quant_data.affine.zeroPoint);
			break;
		default:
			dprintf(LOG_MSG, ", none-quant\n\r");
		}
		vip_create_buffer(param, sizeof(vip_buffer_create_params_t), &ctx->input_buffers[i]);
		//dprintf(LOG_MSG, "input buffer %d = %p, vid memory %x \n\r", i, ctx->input_buffers[i], ctx->input_buffers[i]->memory.physical);
	}

	for (i = 0; i < ctx->output_count; i++) {
		vip_buffer_create_params_t *param = &ctx->vip_param_out[i];
		memset(param, 0, sizeof(vip_buffer_create_params_t));
		param->memory_type = VIP_BUFFER_MEMORY_TYPE_DEFAULT;
		vip_query_output(network, i, VIP_BUFFER_PROP_DATA_FORMAT, &param->data_format);
		vip_query_output(network, i, VIP_BUFFER_PROP_NUM_OF_DIMENSION, &param->num_of_dims);
		vip_query_output(network, i, VIP_BUFFER_PROP_SIZES_OF_DIMENSION, param->sizes);
		vip_query_output(network, i, VIP_BUFFER_PROP_QUANT_FORMAT, &param->quant_format);
		switch (param->quant_format) {
		case VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT:
			vip_query_output(network, i, VIP_BUFFER_PROP_FIXED_POINT_POS,
							 &param->quant_data.dfp.fixed_point_pos);
			break;
		case VIP_BUFFER_QUANTIZE_TF_ASYMM:
			vip_query_output(network, i, VIP_BUFFER_PROP_TF_SCALE,
							 &param->quant_data.affine.scale);
			vip_query_output(network, i, VIP_BUFFER_PROP_TF_ZERO_POINT,
							 &param->quant_data.affine.zeroPoint);
			break;
		default:
			break;
		}
		dprintf(LOG_MSG, "ouput %d dim %d %d %d %d, data format=%d",
				i, param->sizes[0], param->sizes[1], param->sizes[2], param->sizes[3], param->data_format);

		switch (param->quant_format) {
		case VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT:
			dprintf(LOG_MSG, ", dfp=%d\n\r", param->quant_data.dfp.fixed_point_pos);
			break;
		case VIP_BUFFER_QUANTIZE_TF_ASYMM:
			dprintf(LOG_MSG, ", scale=%f, zero_point=%d\n\r", param->quant_data.affine.scale,
					param->quant_data.affine.zeroPoint);
			break;
		default:
			dprintf(LOG_MSG, ", none-quant\n\r");
		}
		vip_create_buffer(param, sizeof(vip_buffer_create_params_t), &ctx->output_buffers[i]);
		//dprintf(LOG_MSG, "output buffer %d = %p, vid memory %x \n\r", i, ctx->output_buffers[i], ctx->output_buffers[i]->memory.physical);
	}

	vipnn_dump_network_io_params(ctx);
	return 0;
}

int vipnn_deoply_network(void *p)
{
	vipnn_ctx_t *ctx = (vipnn_ctx_t *)p;
	vip_status_e status = VIP_SUCCESS;

	if (ctx->params.model->model_src == MODEL_SRC_MEM) {
		void *nn_model = ctx->params.model->nb();
		status = vip_create_network(nn_model, ctx->params.model->nb_size(), VIP_CREATE_NETWORK_FROM_MEMORY, &ctx->network);
		ctx->params.model->freemodel(nn_model);
	} else if (ctx->params.model->model_src == MODEL_SRC_FILE) {
		status = vip_create_network(ctx->params.model->nb(), 0, VIP_CREATE_NETWORK_FROM_FILE, &ctx->network);
	}
	ONERROR(status, "error: vip_create_network.", vipnn_deploy_error);

	dprintf(LOG_INF, "network %p\n\r", ctx->network);

	status = vip_query_network(ctx->network, VIP_NETWORK_PROP_NETWORK_NAME, ctx->network_name);
	ONERROR(status, "error: vip_query_network VIP_NETWORK_PROP_NETWORK_NAME.", vipnn_deploy_error);
	dprintf(LOG_INF, "network name:%s\n\r", ctx->network_name);

	if (init_io_buffers(ctx) < 0) {
		dprintf(LOG_ERR, "error: init_io_buffers.\n\r");
		goto vipnn_deploy_error;
	}

	status = vip_prepare_network(ctx->network);
	ONERROR(status, "error: vip_prepare_network.", vipnn_deploy_error);

	for (int i = 0; i < ctx->output_count; i++) {
		status = vip_set_output(ctx->network, i, ctx->output_buffers[i]);
		ONERROR(status, "error: vip_set_output.", vipnn_deploy_error);
	}

	for (int i = 0; i < ctx->input_count; i++) {
		status = vip_set_input(ctx->network, i, ctx->input_buffers[i]);
		ONERROR(status, "error: vip_set_input.", vipnn_deploy_error);
	}

	// fill model info
	ctx->params.model->input_param.count = ctx->input_count;
	for (int i = 0; i < ctx->input_count; i++) {
		ctx->params.model->input_param.dim[i].num = ctx->vip_param_in[i].num_of_dims;
		memcpy(ctx->params.model->input_param.dim[i].size, ctx->vip_param_in[i].sizes, 6 * sizeof(uint32_t));
		ctx->params.model->input_param.format[i].type = ctx->vip_param_in[i].quant_format;
		switch (ctx->vip_param_in[i].quant_format) {
		case VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT:
			ctx->params.model->input_param.format[i].fix_point_pos = ctx->vip_param_in[i].quant_data.dfp.fixed_point_pos;
			break;
		case VIP_BUFFER_QUANTIZE_TF_ASYMM:
			ctx->params.model->input_param.format[i].scale = ctx->vip_param_in[i].quant_data.affine.scale;
			ctx->params.model->input_param.format[i].zero_point = ctx->vip_param_in[i].quant_data.affine.zeroPoint;
			break;
		}
		dprintf(LOG_INF, "in %d, size %d %d\n\r", i, ctx->params.model->input_param.dim[i].size[0], ctx->params.model->input_param.dim[i].size[1]);
	}

	ctx->params.model->output_param.count = ctx->output_count;
	for (int i = 0; i < ctx->output_count; i++) {
		ctx->params.model->output_param.dim[i].num = ctx->vip_param_out[i].num_of_dims;
		memcpy(ctx->params.model->output_param.dim[i].size, ctx->vip_param_out[i].sizes, 6 * sizeof(uint32_t));
		ctx->params.model->output_param.format[i].buf_type = ctx->vip_param_out[i].data_format;
		ctx->params.model->output_param.format[i].type = ctx->vip_param_out[i].quant_format;
		switch (ctx->vip_param_out[i].quant_format) {
		case VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT:
			ctx->params.model->output_param.format[i].fix_point_pos = ctx->vip_param_out[i].quant_data.dfp.fixed_point_pos;
			break;
		case VIP_BUFFER_QUANTIZE_TF_ASYMM:
			ctx->params.model->output_param.format[i].scale = ctx->vip_param_out[i].quant_data.affine.scale;
			ctx->params.model->output_param.format[i].zero_point = ctx->vip_param_out[i].quant_data.affine.zeroPoint;
			break;
		}
	}

	return 0;
vipnn_deploy_error:
	vipnn_deinited(ctx);
	return -1;
}

void vipnn_hardware_init(void)
{
	hal_sys_peripheral_en(NN_SYS, ENABLE);
	hal_sys_set_clk(NN_SYS, NN_500M);
	//hal_sys_set_clk(NN_SYS, NN_250M);
	dprintf(LOG_INF, "hal_rtl_sys_get_clk %x \n", hal_sys_get_clk(NN_SYS));
}

void *vipnn_create(void *parent)
{
	vipnn_ctx_t *ctx = (vipnn_ctx_t *)malloc(sizeof(vipnn_ctx_t));
	memset(ctx, 0, sizeof(vipnn_ctx_t));

	ctx->parent = parent;

	vip_status_e status = VIP_SUCCESS;

	if (vipnn_inited == 0) {
		vipnn_hardware_init();

		status = vip_init();
		ONERROR(status, "VIP Init failed.", vipnn_error);

		rtw_mutex_init(&nn_mutex);
		vipnn_inited = 1;
	}

	dprintf(LOG_MSG, "VIPLite Drv version %x\n\r", vip_get_version());

	ctx->params.fps = 1;
	ctx->params.out_res_max_cnt = 16;
	ctx->params.out_res_size = 128;

	ctx->status = VIPNN_INITED;
	ctx->tmp_item = NULL;

	vipnn_module_cnt++;

	return ctx;

vipnn_error:

	return NULL;
}

void *vipnn_new_item(void *p)
{
	vipnn_ctx_t *ctx = (vipnn_ctx_t *)p;
	// out buffer structure + result array
	//dprintf(LOG_INF, "new item res size %d cnt %d\n\r", ctx->params.out_res_size, ctx->params.out_res_max_cnt);
	vipnn_out_buf_t *tmp = (vipnn_out_buf_t *)malloc(sizeof(vipnn_out_buf_t) + ctx->params.out_res_size * ctx->params.out_res_max_cnt);
	if (tmp) {
		tmp->res_size = ctx->params.out_res_size;
		tmp->res_max_cnt = ctx->params.out_res_max_cnt;
	}
	return (void *)tmp;
}

void *vipnn_del_item(void *p, void *d)
{
	(void)p;
	if (d) {
		free(d);
	}
	return NULL;
}


mm_module_t vipnn_module = {
	.create = vipnn_create,
	.destroy = vipnn_destroy,
	.control = vipnn_control,
	.handle = vipnn_handle,

	.new_item = vipnn_new_item,
	.del_item = vipnn_del_item,

	.output_type = MM_TYPE_NONE,
	.module_type = MM_TYPE_VDSP,
	.name = "vip nn"
};