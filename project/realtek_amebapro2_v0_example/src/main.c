#include "FreeRTOS.h"
#include "task.h"
#include "diag.h"
#include "hal.h"
#include "log_service.h"
#include "video_api.h"
#include <platform_opts.h>
#include <platform_opts_bt.h>
#include "video_boot.h"
#include "mmf2_mediatime_8735b.h"

#if CONFIG_WLAN
#include <wifi_fast_connect.h>
extern void wlan_network(void);
#endif

extern void console_init(void);
extern void mpu_rodata_protect_init(void);

// tick count initial value used when start scheduler
uint32_t initial_tick_count = 0;

#ifdef _PICOLIBC__
int errno;
#endif

#if defined(CONFIG_FTL_ENABLED)
#include <ftl_int.h>

const u8 ftl_phy_page_num = 3;	/* The number of physical map pages, default is 3: BT_FTL_BKUP_ADDR, BT_FTL_PHY_ADDR1, BT_FTL_PHY_ADDR0 */
const u32 ftl_phy_page_start_addr = BT_FTL_BKUP_ADDR;

void app_ftl_init(void)
{
	ftl_init(ftl_phy_page_start_addr, ftl_phy_page_num);
}
#endif

/* entry for the example*/
_WEAK void app_example(void)
{

}

void setup(void)
{
#if CONFIG_WLAN
#if ENABLE_FAST_CONNECT
	wifi_fast_connect_enable(1);
#else
	wifi_fast_connect_enable(0);
#endif
	wlan_network();
#endif

#if defined(CONFIG_FTL_ENABLED)
	app_ftl_init();
#endif
}

void set_initial_tick_count(void)
{
	// Check DWT_CTRL(0xe0001000) CYCCNTENA(bit 0). If DWT cycle counter is enabled, set tick count initial value based on DWT cycle counter.
	if ((*((volatile uint32_t *) 0xe0001000)) & 1) {
		(*((volatile uint32_t *) 0xe0001000)) &= (~((uint32_t) 1)); // stop DWT cycle counter
		uint32_t dwt_cyccnt = (*((volatile uint32_t *) 0xe0001004));
		uint32_t systick_load = (configCPU_CLOCK_HZ / configTICK_RATE_HZ) - 1UL;
		initial_tick_count = dwt_cyccnt / systick_load;
	}

	// Auto set the media time offset
	video_boot_stream_t *isp_fcs_info;
	video_get_fcs_info(&isp_fcs_info);  //Get the fcs info
	uint32_t media_time_ms = initial_tick_count + isp_fcs_info->fcs_start_time;
	mm_set_mediatime_in_ms(media_time_ms);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
	/* for debug, protect rodata*/
	//mpu_rodata_protect_init();
	console_init();

	voe_t2ff_prealloc();

	setup();

	/* Execute application example */
	app_example();

	/* set tick count initial value before start scheduler */
	set_initial_tick_count();
	vTaskStartScheduler();
	while (1);
}
