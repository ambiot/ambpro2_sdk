#include <platform_opts.h>
#include <FreeRTOS.h>
#include <stdlib.h>

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif
#include "mbedtls/platform.h"

extern int aws_main(void);

static void example_amazon_freertos_thread(void *pvParameters)
{
#if defined(MBEDTLS_PLATFORM_C)
	mbedtls_platform_set_calloc_free(calloc, free);
#endif

	aws_main();

	vTaskDelete(NULL);
}

void example_amazon_freertos(void)
{
	if (xTaskCreate(example_amazon_freertos_thread, ((const char *)"example_amazon_freertos_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		printf("\n\r%s xTaskCreate(example_amazon_freertos_thread) failed", __FUNCTION__);
	}
}
