#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#if tskKERNEL_VERSION_MAJOR >= 10

BaseType_t xTaskGenericCreate(TaskFunction_t pxTaskCode, const char *const pcName, const uint16_t usStackDepth, void *const pvParameters,
							  UBaseType_t uxPriority, TaskHandle_t *const pxCreatedTask, StackType_t *const puxStackBuffer, const MemoryRegion_t *const xRegions)
{
	return xTaskCreate(pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask);
}

BaseType_t xQueueGenericReceive(QueueHandle_t xQueue, void *const pvBuffer, TickType_t xTicksToWait, const BaseType_t xJustPeek)
{
	return xQueueReceive(xQueue, pvBuffer, xTicksToWait);
}

#endif