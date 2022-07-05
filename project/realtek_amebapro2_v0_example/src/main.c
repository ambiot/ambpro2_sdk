#include "FreeRTOS.h"
#include "task.h"
#include "diag.h"
#include "hal.h"
#include "log_service.h"
#include "video_api.h"
#include <platform_opts.h>
#include <platform_opts_bt.h>

#if CONFIG_WLAN
#include <wifi_fast_connect.h>
extern void wlan_network(void);
#endif

extern void console_init(void);

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

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
	console_init();

	voe_t2ff_prealloc();

	setup();

	/* Execute application example */
	app_example();


	vTaskStartScheduler();
	while (1);
}
