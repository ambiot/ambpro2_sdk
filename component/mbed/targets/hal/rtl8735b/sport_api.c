/**************************************************************************//**
 * @file     sport_api.c
 * @brief    This file implements the AUDIO Mbed HAL API functions.
 *
 * @version  V1.00
 * @date     2017-05-03
 *
 * @note
 *
 ******************************************************************************
 * @attention
 *
 * Copyright(c) 2007 - 2022 Realtek Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */
#include "objects.h"
#include "sport_api.h"
#include "hal_sport.h"

void sport_init(sport_t *obj)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;
	hal_sport_page_intr_t page_intr;
	uint32_t I2S_S0[5] = {PF_13, PF_15, PF_14, PF_12, PF_11};
	uint32_t check_temp = 0;
	HAL_Status ret;
	uint8_t i;

	hal_sys_peripheral_en(AUDIO_CODEC_EXTE_EN, ENABLE);

	for (i = 0; i < 5; i++) {

		ret = hal_pinmux_register(I2S_S0[i], PID_I2S0);

		check_temp |= ret;
		if (ret != HAL_OK) {
			DBG_SPORT_ERR("sport_init : SPORT pin[%d] is invalid. \r\n", i);
		}
	}

	if (check_temp == HAL_OK) {
		ret = hal_sport_init(psport_adapter);
		if (ret != HAL_OK) {
			DBG_SPORT_ERR("sport_init is failure\r\n");
		} else {

			hal_sport_set_master(psport_adapter, SPORT_MASTER_MODE);
			hal_sport_set_loopback(psport_adapter, DISABLE);
			hal_sport_mode(psport_adapter, SPORT_DMA_MODE);
			hal_sport_tx_ch(psport_adapter, SPORT_L_R);
			hal_sport_rx_ch(psport_adapter, SPORT_L_R);
			hal_sport_format(psport_adapter, SPORT_I2S);
			hal_sport_rx_same_format(psport_adapter, ENABLE);
			hal_sport_rx_format(psport_adapter, SPORT_I2S);
			hal_sport_tx_data_dir(psport_adapter, SPORT_MSB_FIRST, SPORT_MSB_FIRST);
			hal_sport_rx_data_dir(psport_adapter, SPORT_MSB_FIRST, SPORT_MSB_FIRST);
			hal_sport_tx_lr_swap(psport_adapter, DISABLE, DISABLE);
			hal_sport_rx_lr_swap(psport_adapter, DISABLE, DISABLE);
			hal_sport_tx_byte_swap(psport_adapter, DISABLE, DISABLE);
			hal_sport_rx_byte_swap(psport_adapter, DISABLE, DISABLE);
			hal_sport_bclk_inverse(psport_adapter, DISABLE);
			hal_sport_set_mclk(psport_adapter, SPORT_SRC_DIV_1, DISABLE);

			//IRQ
			hal_sport_tx_fifo_th(psport_adapter, 16);
			hal_sport_rx_fifo_th(psport_adapter, 16);
			hal_sport_autoload_dma_burst(psport_adapter);

			hal_rtl_sport_mode(psport_adapter, SPORT_DMA_MODE);
			page_intr.sport_p0ok = 1;
			page_intr.sport_p1ok = 1;
			page_intr.sport_p2ok = 1;
			page_intr.sport_p3ok = 1;
			page_intr.sport_p0unava = 1;
			page_intr.sport_p1unava = 1;
			page_intr.sport_p2unava = 1;
			page_intr.sport_p3unava = 1;
			page_intr.sport_p0err = 1;
			page_intr.sport_p1err = 1;
			page_intr.sport_p2err = 1;
			page_intr.sport_p3err = 1;
			page_intr.sport_fifo_err = 1;
			hal_sport_set_dma_intr(psport_adapter, SPORT_TX0, &page_intr);
			hal_sport_set_dma_intr(psport_adapter, SPORT_TX1, &page_intr);
			hal_sport_set_dma_intr(psport_adapter, SPORT_RX0, &page_intr);
			hal_sport_set_dma_intr(psport_adapter, SPORT_RX1, &page_intr);
		}
	}

}

void sport_deinit(sport_t *obj)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;
	uint32_t I2S_S0[5] = {PF_13, PF_15, PF_14, PF_12, PF_11};
	uint32_t check_temp = 0;
	HAL_Status ret;
	uint8_t i;

	hal_sport_deinit(psport_adapter);

	for (i = 0; i < 5; i++) {

		ret = hal_pinmux_unregister(I2S_S0[i], PID_I2S0);

		check_temp |= ret;
		if (ret != HAL_OK) {
			DBG_SPORT_ERR("sport_deinit : SPORT pin[%d] is invalid. \r\n", i);
		}
	}
}

void sport_reset(sport_t *obj)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	hal_sport_reset(psport_adapter);
	hal_rtl_sport_reset_tx_fifo(psport_adapter);
	hal_rtl_sport_reset_rx_fifo(psport_adapter);
	hal_rtl_sport_dma_reset(psport_adapter);
	hal_rtl_sport_clean_tx_page_own(psport_adapter);
	hal_rtl_sport_clean_rx_page_own(psport_adapter);
}

void sport_set_master(sport_t *obj, sport_ms_mode ms_mode)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	hal_sport_set_master(psport_adapter, ms_mode);
}

void sport_set_loopback(sport_t *obj, BOOL loopback_en)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	hal_sport_set_loopback(psport_adapter, loopback_en);
}

void sport_set_lr_channel(sport_t *obj, sport_sel_chan tx_sel, sport_sel_chan rx_sel)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	hal_sport_tx_ch(psport_adapter, tx_sel);
	hal_sport_rx_ch(psport_adapter, rx_sel);
}

void sport_set_format(sport_t *obj, sport_format format)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	hal_sport_format(psport_adapter, format);
	hal_sport_rx_same_format(psport_adapter, ENABLE);
	hal_sport_rx_format(psport_adapter, format);
}

void sport_data_dir(sport_t *obj, sport_ml tx0_ml, sport_ml tx1_ml, sport_ml rx0_ml, sport_ml rx1_ml)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	hal_sport_tx_data_dir(psport_adapter, tx0_ml, tx1_ml);
	hal_sport_rx_data_dir(psport_adapter, rx0_ml, rx1_ml);
}

void sport_lr_swap(sport_t *obj, BOOL tx0_en, BOOL tx1_en, BOOL rx0_en, BOOL rx1_en)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	hal_sport_tx_lr_swap(psport_adapter, tx0_en, tx1_en);
	hal_sport_rx_lr_swap(psport_adapter, rx0_en, rx1_en);
}

void sport_byte_swap(sport_t *obj, BOOL tx0_en, BOOL tx1_en, BOOL rx0_en, BOOL rx1_en)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	hal_sport_tx_byte_swap(psport_adapter, tx0_en, tx1_en);
	hal_sport_rx_byte_swap(psport_adapter, rx0_en, rx1_en);
}

void sport_set_sck_inv(sport_t *obj, BOOL sck_inv_en)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	hal_sport_bclk_inverse(psport_adapter, sck_inv_en);
}

void sport_set_mclk(sport_t *obj, sport_mclk m_speed, BOOL mclk_en)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	hal_sport_set_mclk(psport_adapter, m_speed, mclk_en);
}

void sport_tx_params(sport_t *obj, sport_ch ch_num, sport_cl ch_len, sport_dl tx0_data_len, sport_dl tx1_data_len, sport_rate rate)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;
	hal_sport_tx_params_t tx_params;

	tx_params.sport_tx_ch = ch_num;
	tx_params.sport_tx_cl = ch_len;
	tx_params.sport_tx0_dl = tx0_data_len;
	tx_params.sport_tx1_dl = tx1_data_len;
	tx_params.sport_tx_rate = rate;
	hal_sport_tx_params(psport_adapter, &tx_params);
}

void sport_rx_params(sport_t *obj, sport_ch ch_num, sport_cl ch_len, sport_dl rx0_data_len, sport_dl rx1_data_len, sport_rate rate)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;
	hal_sport_rx_params_t rx_params;

	rx_params.sport_rx_ch = ch_num;
	rx_params.sport_rx_cl = ch_len;
	rx_params.sport_rx0_dl = rx0_data_len;
	rx_params.sport_rx1_dl = rx1_data_len;
	rx_params.sport_rx_rate = rate;
	hal_sport_tx_params(psport_adapter, &rx_params);
}

void sport_dma_buffer(sport_t *obj, u8 *ptx0_buf, u8 *ptx1_buf, u8 *prx0_buf, u8 *prx1_buf, u32 page_num, u32 page_size)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;
	hal_sport_buf_params_t buf_params;

	buf_params.tx0_buf = ptx0_buf;
	buf_params.rx0_buf = ptx1_buf;
	buf_params.tx1_buf = prx0_buf;
	buf_params.rx1_buf = prx1_buf;
	buf_params.page_num = page_num;
	buf_params.page_size = page_size;
	hal_sport_dma_buffer(psport_adapter, &buf_params);
}

void sport_fifo_th(sport_t *obj, u8 tx_fifo_th, u8 rx_fifo_th)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	hal_sport_tx_fifo_th(psport_adapter, tx_fifo_th);
	hal_sport_rx_fifo_th(psport_adapter, rx_fifo_th);
	hal_sport_autoload_dma_burst(psport_adapter);
}

void sport_fifo_cb_handler(sport_t *obj, sport_irq_cb_t handler, void *parg)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	hal_sport_fifo_cb_handler(psport_adapter, (sport_irq_user_cb_t)handler, parg);
}

void sport_tx0_dma_cb_handler(sport_t *obj, sport_irq_cb_t handler, void *parg)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	hal_sport_tx0_dma_cb_handler(psport_adapter, (sport_irq_user_cb_t)handler, parg);
}

void sport_tx1_dma_cb_handler(sport_t *obj, sport_irq_cb_t handler, void *parg)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	hal_sport_tx1_dma_cb_handler(psport_adapter, (sport_irq_user_cb_t)handler, parg);
}

void sport_rx0_dma_cb_handler(sport_t *obj, sport_irq_cb_t handler, void *parg)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	hal_sport_rx0_dma_cb_handler(psport_adapter, (sport_irq_user_cb_t)handler, parg);
}

void sport_rx1_dma_cb_handler(sport_t *obj, sport_irq_cb_t handler, void *parg)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	hal_sport_rx1_dma_cb_handler(psport_adapter, (sport_irq_user_cb_t)handler, parg);
}

int *sport_get_tx0_page(sport_t *obj)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;
	u8 page_idx;

	page_idx = hal_sport_get_tx0_page(psport_adapter);

	if (page_idx <= psport_adapter->base_addr->sp_dma_con_b.sp_page_num) {
		return ((int *)psport_adapter->ptx0_page_list[page_idx]);
	} else {
		DBG_SPORT_WARN("Tx_0_page is busy: \r\n");
		DBG_SPORT_WARN("page_idx: %d, PageNum: %d \r\n", page_idx, psport_adapter->base_addr->sp_dma_con_b.sp_page_num);
		return NULL;
	}
}

int *sport_get_tx1_page(sport_t *obj)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;
	u8 page_idx;

	page_idx = hal_sport_get_tx1_page(psport_adapter);

	if (page_idx <= psport_adapter->base_addr->sp_dma_con_b.sp_page_num) {
		return ((int *)psport_adapter->ptx1_page_list[page_idx]);
	} else {
		DBG_SPORT_WARN("Tx_1_page is busy: \r\n");
		DBG_SPORT_WARN("page_idx: %d, PageNum: %d \r\n", page_idx, psport_adapter->base_addr->sp_dma_con_b.sp_page_num);
		return NULL;
	}
}

void sport_tx0_page_send(sport_t *obj, u32 *pbuf)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;
	u32 page_num, i;

	page_num = psport_adapter->base_addr->sp_dma_con_b.sp_page_num + 1;
	for (i = 0; i < page_num; i++) {

		if (psport_adapter->ptx0_page_list[i] == pbuf) {
			hal_sport_tx0_page_send(psport_adapter, i);
			break;  // break the for loop
		}
	}

	if (i == page_num) {
		DBG_SPORT_WARN("sport_tx0_page_send: the pbuf(0x%x) is not a DMA buffer\r\n", pbuf);
	}

}

void sport_tx1_page_send(sport_t *obj, u32 *pbuf)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;
	u32 page_num, i;

	page_num = psport_adapter->base_addr->sp_dma_con_b.sp_page_num + 1;
	for (i = 0; i < page_num; i++) {

		if (psport_adapter->ptx1_page_list[i] == pbuf) {
			hal_sport_tx1_page_send(psport_adapter, i);
			break;  // break the for loop
		}
	}

	if (i == page_num) {
		DBG_SPORT_WARN("sport_tx1_page_send: the pbuf(0x%x) is not a DMA buffer\r\n", pbuf);
	}

}

u8 sport_get_tx0_error_cnt(sport_t *obj)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	return (psport_adapter->dma_err_sta & 0x01);
}

u8 sport_get_tx1_error_cnt(sport_t *obj)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	return ((psport_adapter->dma_err_sta & 0x02) >> 1);
}

u8 sport_get_rx0_error_cnt(sport_t *obj)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	return ((psport_adapter->dma_err_sta & 0x04) >> 2);
}

u8 sport_get_rx1_error_cnt(sport_t *obj)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	return ((psport_adapter->dma_err_sta & 0x08) >> 3);
}

void sport_clean_error_cnt(sport_t *obj)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	psport_adapter->dma_err_sta = psport_adapter->dma_err_sta & ~(0x0f);
}

void sport_tx_start(sport_t *obj)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	sport_clean_error_cnt(obj);
	hal_sport_tx_dma_start(psport_adapter, ENABLE);
}

void sport_rx_start(sport_t *obj)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	sport_clean_error_cnt(obj);
	hal_sport_rx_dma_start(psport_adapter, ENABLE);
}

void sport_trx_start(sport_t *obj)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	sport_clean_error_cnt(obj);
	hal_sport_tx_dma_start(psport_adapter, ENABLE);
	hal_sport_rx_dma_start(psport_adapter, ENABLE);
}

void sport_tx_stop(sport_t *obj)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	hal_sport_tx_dma_start(psport_adapter, DISABLE);
}

void sport_rx_stop(sport_t *obj)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	hal_sport_rx_dma_start(psport_adapter, DISABLE);
}

void sport_trx_stop(sport_t *obj)
{
	hal_sport_adapter_t *psport_adapter = &obj->sport_adapter;

	sport_clean_error_cnt(obj);
	hal_sport_tx_dma_start(psport_adapter, DISABLE);
	hal_sport_rx_dma_start(psport_adapter, DISABLE);
}


