#ifndef _MODULE_KVS_PRODUCER_H
#define _MODULE_KVS_PRODUCER_H

#include "mmf2_module.h"
#include "sample_config.h"

#define CMD_KVS_PRODUCER_SET_PARAMS			MM_MODULE_CMD(0x00)
#define CMD_KVS_PRODUCER_GET_PARAMS			MM_MODULE_CMD(0x01)
#define CMD_KVS_PRODUCER_SET_APPLY			MM_MODULE_CMD(0x02)
#define CMD_KVS_PRODUCER_PAUSE              MM_MODULE_CMD(0x03)
#define CMD_KVS_PRODUCER_STOP               MM_MODULE_CMD(0x04)
#define CMD_KVS_PRODUCER_RECONNECT          MM_MODULE_CMD(0x05)

/* Headers for KVS */
#include "kvs/restapi.h"
#include "kvs/stream.h"

#if ENABLE_IOT_CREDENTIAL
#include "kvs/iot_credential_provider.h"
#endif /* ENABLE_IOT_CREDENTIAL */

typedef struct Kvs {
#if ENABLE_IOT_CREDENTIAL
	IotCredentialRequest_t xIotCredentialReq;
#endif
	KvsServiceParameter_t xServicePara;
	KvsDescribeStreamParameter_t xDescPara;
	KvsCreateStreamParameter_t xCreatePara;
	KvsGetDataEndpointParameter_t xGetDataEpPara;
	KvsPutMediaParameter_t xPutMediaPara;

	StreamHandle xStreamHandle;
	PutMediaHandle xPutMediaHandle;

	VideoTrackInfo_t *pVideoTrackInfo;
	AudioTrackInfo_t *pAudioTrackInfo;

	bool inited;
} Kvs_t;

typedef struct kvs_producer_ctx_s {
	void *parent;
	Kvs_t *xKvs;
	TaskHandle_t    kvs_producer_module_task;

} kvs_producer_ctx_t;

extern mm_module_t kvs_producer_module;

#endif