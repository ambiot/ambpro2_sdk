#include "platform_opts.h"

/* Headers for example */
#include "sample_config.h"
#include "module_kvs_producer.h"

#include "wifi_conf.h"
#include "sntp/sntp.h"

/* Headers for video */
#include "avcodec.h"

/* Headers for KVS */
#include "kvs/port.h"
#include "kvs/nalu.h"
#include "kvs/restapi.h"
#include "kvs/stream.h"

#include "mbedtls/config.h"
#include "mbedtls/platform.h"
#include "mbedtls/threading.h"
#include "threading_alt.h"

#define ERRNO_NONE      0
#define ERRNO_FAIL      __LINE__

#define EN_SEND_END_OF_FRAMES   1

static int gProducerStop = 0;

typedef enum doWorkExType {
	DO_WORK_DEFAULT = 0,  /* default behavior */
	DO_WORK_SEND_END_OF_FRAMES = 1  /* It's similar to doWork, except that it also sends out the end of frames */
} doWorkExType_t;

typedef struct doWorkExParamter {
	doWorkExType_t eType;
} doWorkExParamter_t;

static int kvsVideoInitTrackInfo(Kvs_t *pKvs, uint8_t *pData, size_t uDataLen)
{
	int res = ERRNO_NONE;
	uint8_t *pVideoCpdData = NULL;
	size_t uCpdLen = 0;
	uint8_t *pSps = NULL;
	size_t uSpsLen = 0;
	uint16_t uWidth = 0;
	uint16_t uHeight = 0;

	if (pKvs == NULL || pData == NULL || uDataLen == 0) {
		printf("Invalid argument\r\n");
		res = ERRNO_FAIL;
	} else if (pKvs->pVideoTrackInfo != NULL) {
		printf("VideoTrackInfo is not NULL\r\n");
		res = ERRNO_FAIL;
	} else if (Mkv_generateH264CodecPrivateDataFromAnnexBNalus(pData, uDataLen, &pVideoCpdData, &uCpdLen) != ERRNO_NONE) {
		printf("Fail to get Codec Private Data from AnnexB nalus\r\n");
		res = ERRNO_FAIL;
	} else if (NALU_getNaluFromAnnexBNalus(pData, uDataLen, NALU_TYPE_SPS, &pSps, &uSpsLen) != ERRNO_NONE) {
		printf("Fail to get SPS from AnnexB nalus\r\n");
		res = ERRNO_FAIL;
	} else if (NALU_getH264VideoResolutionFromSps(pSps, uSpsLen, &uWidth, &uHeight) != ERRNO_NONE) {
		printf("Fail to get Resolution from SPS\r\n");
		res = ERRNO_FAIL;
	} else if ((pKvs->pVideoTrackInfo = (VideoTrackInfo_t *)malloc(sizeof(VideoTrackInfo_t))) == NULL) {
		printf("Fail to allocate memory for Video Track Info\r\n");
		res = ERRNO_FAIL;
	} else {
		memset(pKvs->pVideoTrackInfo, 0, sizeof(VideoTrackInfo_t));

		pKvs->pVideoTrackInfo->pTrackName = (char *)VIDEO_NAME;
		pKvs->pVideoTrackInfo->pCodecName = (char *)VIDEO_CODEC_NAME;
		pKvs->pVideoTrackInfo->uWidth = uWidth;
		pKvs->pVideoTrackInfo->uHeight = uHeight;
		pKvs->pVideoTrackInfo->pCodecPrivate = pVideoCpdData;
		pKvs->pVideoTrackInfo->uCodecPrivateLen = uCpdLen;
		printf("\r[Video resolution from SPS] w: %d h: %d\r\n", pKvs->pVideoTrackInfo->uWidth, pKvs->pVideoTrackInfo->uHeight);
	}

	return res;
}

#if ENABLE_AUDIO_TRACK
static void kvsAudioInitTrackInfo(Kvs_t *pKvs)
{
	int res = ERRNO_NONE;
	uint8_t *pCodecPrivateData = NULL;
	size_t uCodecPrivateDataLen = 0;

	pKvs->pAudioTrackInfo = (AudioTrackInfo_t *)malloc(sizeof(AudioTrackInfo_t));
	memset(pKvs->pAudioTrackInfo, 0, sizeof(AudioTrackInfo_t));
	pKvs->pAudioTrackInfo->pTrackName = (char *)AUDIO_NAME;
	pKvs->pAudioTrackInfo->pCodecName = (char *)AUDIO_CODEC_NAME;
	pKvs->pAudioTrackInfo->uFrequency = AUDIO_SAMPLING_RATE;
	pKvs->pAudioTrackInfo->uChannelNumber = AUDIO_CHANNEL_NUMBER;

#if USE_AUDIO_AAC
	res = Mkv_generateAacCodecPrivateData(AUDIO_MPEG_OBJECT_TYPE, AUDIO_SAMPLING_RATE, AUDIO_CHANNEL_NUMBER, &pCodecPrivateData, &uCodecPrivateDataLen);
#endif
#if USE_AUDIO_G711
	res = Mkv_generatePcmCodecPrivateData(AUDIO_PCM_OBJECT_TYPE, AUDIO_SAMPLING_RATE, AUDIO_CHANNEL_NUMBER, &pCodecPrivateData, &uCodecPrivateDataLen);
#endif
	if (res == ERRNO_NONE) {
		pKvs->pAudioTrackInfo->pCodecPrivate = pCodecPrivateData;
		pKvs->pAudioTrackInfo->uCodecPrivateLen = (uint32_t)uCodecPrivateDataLen;
	} else {
		printf("Failed to generate codec private data\r\n");
	}
}
#endif

static int kvsInitialize(Kvs_t *pKvs)
{
	int res = ERRNO_NONE;
	char *pcStreamName = (char *)KVS_STREAM_NAME;

	if (pKvs == NULL) {
		res = ERRNO_FAIL;
	} else {
		memset(pKvs, 0, sizeof(Kvs_t));

		pKvs->xServicePara.pcHost = (char *)AWS_KVS_HOST;
		pKvs->xServicePara.pcRegion = (char *)AWS_KVS_REGION;
		pKvs->xServicePara.pcService = (char *)AWS_KVS_SERVICE;
		pKvs->xServicePara.pcAccessKey = (char *)AWS_ACCESS_KEY;
		pKvs->xServicePara.pcSecretKey = (char *)AWS_SECRET_KEY;

		pKvs->xDescPara.pcStreamName = pcStreamName;

		pKvs->xCreatePara.pcStreamName = pcStreamName;
		pKvs->xCreatePara.uDataRetentionInHours = 2;

		pKvs->xGetDataEpPara.pcStreamName = pcStreamName;

		pKvs->xPutMediaPara.pcStreamName = pcStreamName;
		pKvs->xPutMediaPara.xTimecodeType = TIMECODE_TYPE_ABSOLUTE;

#if ENABLE_IOT_CREDENTIAL
		pKvs->xIotCredentialReq.pCredentialHost = CREDENTIALS_HOST;
		pKvs->xIotCredentialReq.pRoleAlias = ROLE_ALIAS;
		pKvs->xIotCredentialReq.pThingName = THING_NAME;
		pKvs->xIotCredentialReq.pRootCA = ROOT_CA;
		pKvs->xIotCredentialReq.pCertificate = CERTIFICATE;
		pKvs->xIotCredentialReq.pPrivateKey = PRIVATE_KEY;
#endif
		pKvs->inited = true;
	}
	return res;
}

static void kvsTerminate(Kvs_t *pKvs)
{
	if (pKvs != NULL) {
		if (pKvs->xServicePara.pcPutMediaEndpoint != NULL) {
			free(pKvs->xServicePara.pcPutMediaEndpoint);
			pKvs->xServicePara.pcPutMediaEndpoint = NULL;
		}
	}
	pKvs->inited = false;
}

static int setupDataEndpoint(Kvs_t *pKvs)
{
	int res = ERRNO_NONE;
	unsigned int uHttpStatusCode = 0;

	if (pKvs == NULL) {
		res = ERRNO_FAIL;
	} else {
		if (pKvs->xServicePara.pcPutMediaEndpoint != NULL) {
		} else {
			printf("Try to describe stream\r\n");
			if (Kvs_describeStream(&(pKvs->xServicePara), &(pKvs->xDescPara), &uHttpStatusCode) != ERRNO_NONE || uHttpStatusCode != 200) {
				printf("Failed to describe stream\r\n");
				printf("Try to create stream\r\n");
				if (Kvs_createStream(&(pKvs->xServicePara), &(pKvs->xCreatePara), &uHttpStatusCode) != ERRNO_NONE || uHttpStatusCode != 200) {
					printf("uHttpStatusCode != 200 = %d\r\n", uHttpStatusCode);
					printf("Failed to create stream\r\n");
					res = ERRNO_FAIL;
				}
			}

			if (res == ERRNO_NONE) {
				if (Kvs_getDataEndpoint(&(pKvs->xServicePara), &(pKvs->xGetDataEpPara), &uHttpStatusCode, &(pKvs->xServicePara.pcPutMediaEndpoint)) != ERRNO_NONE ||
					uHttpStatusCode != 200) {
					printf("Failed to get data endpoint\r\n");
					res = ERRNO_FAIL;
				}
			}
		}
	}

	if (res == ERRNO_NONE) {
		printf("PUT MEDIA endpoint: %s\r\n", pKvs->xServicePara.pcPutMediaEndpoint);
	}

	return res;
}

static void kvsStreamFlush(StreamHandle xStreamHandle)
{
	DataFrameHandle xDataFrameHandle = NULL;
	DataFrameIn_t *pDataFrameIn = NULL;

	while ((xDataFrameHandle = Kvs_streamPop(xStreamHandle)) != NULL) {
		pDataFrameIn = (DataFrameIn_t *)xDataFrameHandle;
		free(pDataFrameIn->pData);
		Kvs_dataFrameTerminate(xDataFrameHandle);
	}
}

static void kvsStreamFlushToNextCluster(StreamHandle xStreamHandle)
{
	DataFrameHandle xDataFrameHandle = NULL;
	DataFrameIn_t *pDataFrameIn = NULL;

	while (1) {
		xDataFrameHandle = Kvs_streamPeek(xStreamHandle);
		if (xDataFrameHandle == NULL) {
			sleepInMs(50);
		} else {
			pDataFrameIn = (DataFrameIn_t *)xDataFrameHandle;
			if (pDataFrameIn->xClusterType == MKV_CLUSTER) {
				break;
			} else {
				xDataFrameHandle = Kvs_streamPop(xStreamHandle);
				pDataFrameIn = (DataFrameIn_t *)xDataFrameHandle;
				free(pDataFrameIn->pData);
				Kvs_dataFrameTerminate(xDataFrameHandle);
			}
		}
	}
}

static void kvsStreamFlushHeadUntilMem(StreamHandle xStreamHandle, size_t uMemLimit)
{
	DataFrameHandle xDataFrameHandle = NULL;
	DataFrameIn_t *pDataFrameIn = NULL;
	size_t uMemTotal = 0;

	while (Kvs_streamMemStatTotal(xStreamHandle, &uMemTotal) == 0 && uMemTotal > uMemLimit && (xDataFrameHandle = Kvs_streamPop(xStreamHandle)) != NULL) {
		pDataFrameIn = (DataFrameIn_t *)xDataFrameHandle;
		free(pDataFrameIn->pData);
		Kvs_dataFrameTerminate(xDataFrameHandle);
	}
}

static void kvsAddMediaFrame(Kvs_t *pKvs, uint8_t *pData, size_t uDataLen, size_t uDataSize, TrackType_t xTrackType)
{
	int res = ERRNO_NONE;
	DataFrameIn_t xDataFrameIn = {0};
	size_t uMemTotal = 0;

	if (pKvs == NULL || pData == NULL || uDataLen == 0) {
		res = ERRNO_FAIL;
	} else {
		if (pKvs->xStreamHandle != NULL) {
			if (xTrackType == TRACK_VIDEO && NALU_isAnnexBFrame(pData, uDataLen) &&
				(res = NALU_convertAnnexBToAvccInPlace(pData, uDataLen, uDataSize, (uint32_t *)&uDataLen)) != ERRNO_NONE) {
				printf("Failed to convert Annex-B to AVCC\r\n");
				res = ERRNO_FAIL;
			} else if (Kvs_streamMemStatTotal(pKvs->xStreamHandle, &uMemTotal) != ERRNO_NONE) {
				printf("Failed to get stream mem state\r\n");
			} else {
				xDataFrameIn.pData = (char *)pData;
				xDataFrameIn.uDataLen = uDataLen;
				xDataFrameIn.bIsKeyFrame = (xTrackType == TRACK_VIDEO) ? isKeyFrame(pData, uDataLen) : false;
				xDataFrameIn.uTimestampMs = getEpochTimestampInMs();
				xDataFrameIn.xTrackType = xTrackType;
				xDataFrameIn.xClusterType = (xDataFrameIn.bIsKeyFrame) ? MKV_CLUSTER : MKV_SIMPLE_BLOCK;

				kvsStreamFlushHeadUntilMem(pKvs->xStreamHandle, RING_BUFFER_MEM_LIMIT);
				Kvs_streamAddDataFrame(pKvs->xStreamHandle, &xDataFrameIn);
			}
		}
	}
}

static void kvsVideoTrackInfoTerminate(VideoTrackInfo_t *pVideoTrackInfo)
{
	if (pVideoTrackInfo != NULL) {
		if (pVideoTrackInfo->pCodecPrivate != NULL) {
			free(pVideoTrackInfo->pCodecPrivate);
		}
		memset(pVideoTrackInfo, 0, sizeof(VideoTrackInfo_t));
		free(pVideoTrackInfo);
	}
}

static void kvsAudioTrackInfoTerminate(AudioTrackInfo_t *pAudioTrackInfo)
{
	if (pAudioTrackInfo != NULL) {
		if (pAudioTrackInfo->pCodecPrivate != NULL) {
			free(pAudioTrackInfo->pCodecPrivate);
		}
		memset(pAudioTrackInfo, 0, sizeof(VideoTrackInfo_t));
		free(pAudioTrackInfo);
	}
}

static void kvsProducerModuleStop(Kvs_t *pKvs)
{
	if (pKvs->xStreamHandle != NULL) {
		kvsStreamFlush(pKvs->xStreamHandle);
		Kvs_streamTermintate(pKvs->xStreamHandle);
		pKvs->xStreamHandle = NULL;
	}
	if (pKvs->pVideoTrackInfo != NULL) {
		kvsVideoTrackInfoTerminate(pKvs->pVideoTrackInfo);
		pKvs->pVideoTrackInfo = NULL;
	}
	if (pKvs->pAudioTrackInfo != NULL) {
		kvsAudioTrackInfoTerminate(pKvs->pAudioTrackInfo);
		pKvs->pAudioTrackInfo = NULL;
	}
}

static int putMediaSendData(Kvs_t *pKvs, int *pxSendCnt, bool bForceSend)
{
	int res = 0;
	DataFrameHandle xDataFrameHandle = NULL;
	DataFrameIn_t *pDataFrameIn = NULL;
	uint8_t *pData = NULL;
	size_t uDataLen = 0;
	uint8_t *pMkvHeader = NULL;
	size_t uMkvHeaderLen = 0;
	int xSendCnt = 0;

	if (pKvs->xStreamHandle != NULL &&
		Kvs_streamAvailOnTrack(pKvs->xStreamHandle, TRACK_VIDEO) &&
		(!bForceSend || !pKvs->pAudioTrackInfo || Kvs_streamAvailOnTrack(pKvs->xStreamHandle, TRACK_AUDIO))) {

		if ((xDataFrameHandle = Kvs_streamPop(pKvs->xStreamHandle)) == NULL) {
			printf("Failed to get data frame\r\n");
			res = ERRNO_FAIL;
		} else if (Kvs_dataFrameGetContent(xDataFrameHandle, &pMkvHeader, &uMkvHeaderLen, &pData, &uDataLen) != ERRNO_NONE) {
			printf("Failed to get data and mkv header to send\r\n");
			res = ERRNO_FAIL;
		} else if (Kvs_putMediaUpdate(pKvs->xPutMediaHandle, pMkvHeader, uMkvHeaderLen, pData, uDataLen) != ERRNO_NONE) {
			printf("Failed to update\r\n");
			res = ERRNO_FAIL;
		} else {
			xSendCnt++;
		}

		if (xDataFrameHandle != NULL) {
			pDataFrameIn = (DataFrameIn_t *)xDataFrameHandle;
			free(pDataFrameIn->pData);
			Kvs_dataFrameTerminate(xDataFrameHandle);
		}
	}

	if (pxSendCnt != NULL) {
		*pxSendCnt = xSendCnt;
	}

	return res;
}

static int kvsPutMediaDoWorkDefault(Kvs_t *pKvs)
{
	int res = ERRNO_NONE;
	int xSendCnt = 0;

	do {
		if (Kvs_putMediaDoWork(pKvs->xPutMediaHandle) != ERRNO_NONE) {
			res = ERRNO_FAIL;
			break;
		}

		if (putMediaSendData(pKvs, &xSendCnt, false) != ERRNO_NONE) {
			res = ERRNO_FAIL;
			break;
		}
	} while (false);

	if (xSendCnt == 0) {
		sleepInMs(50);
	}

	return res;
}

static int kvsPutMediaDoWorkSendEndOfFrames(Kvs_t *pKvs)
{
	int res = ERRNO_NONE;
	int xSendCnt = 0;

	do {
		if (Kvs_putMediaDoWork(pKvs->xPutMediaHandle) != ERRNO_NONE) {
			res = ERRNO_FAIL;
			break;
		}

		if (putMediaSendData(pKvs, &xSendCnt, true) != ERRNO_NONE) {
			res = ERRNO_FAIL;
			break;
		}
	} while (xSendCnt > 0);

	return res;
}

static int kvsDoWorkEx(Kvs_t *pKvs, doWorkExParamter_t *pPara)
{
	int res = ERRNO_NONE;

	if (pKvs == NULL) {
		res = ERRNO_FAIL;
	} else {
		if (pPara == NULL || pPara->eType == DO_WORK_DEFAULT) {
			res = kvsPutMediaDoWorkDefault(pKvs);
		} else if (pPara->eType == DO_WORK_SEND_END_OF_FRAMES) {
			res = kvsPutMediaDoWorkSendEndOfFrames(pKvs);
		} else {
			res = ERRNO_FAIL;
		}
	}

	return res;
}

static int putMedia(Kvs_t *pKvs)
{
	int res = 0;
	unsigned int uHttpStatusCode = 0;
	uint8_t *pEbmlSeg = NULL;
	size_t uEbmlSegLen = 0;
	doWorkExParamter_t xDoWorkExParamter = { .eType = DO_WORK_DEFAULT };

	printf("Try to put media\r\n");
	if (pKvs == NULL) {
		printf("Invalid argument: pKvs\r\n");
		res = ERRNO_FAIL;
	} else if (Kvs_putMediaStart(&(pKvs->xServicePara), &(pKvs->xPutMediaPara), &uHttpStatusCode, &(pKvs->xPutMediaHandle)) != ERRNO_NONE ||
			   uHttpStatusCode != 200) {
		printf("Failed to setup PUT MEDIA\r\n");
		res = ERRNO_FAIL;
	} else if (Kvs_streamGetMkvEbmlSegHdr(pKvs->xStreamHandle, &pEbmlSeg, &uEbmlSegLen) != ERRNO_NONE ||
			   Kvs_putMediaUpdateRaw(pKvs->xPutMediaHandle, pEbmlSeg, uEbmlSegLen) != ERRNO_NONE) {
		printf("Failed to upadte MKV EBML and segment\r\n");
		res = ERRNO_FAIL;
	} else {
		/* The beginning of a KVS stream has to be a cluster frame. */
		kvsStreamFlushToNextCluster(pKvs->xStreamHandle);

		while (1) {
			if (gProducerStop == 1) {
				break;
			}
			if (kvsDoWorkEx(pKvs, &xDoWorkExParamter) != ERRNO_NONE) {
				break;
			}
		}
	}

#if EN_SEND_END_OF_FRAMES
	if (gProducerStop == 1) {
		printf("--> Sending out the end of frames in stream buffer \r\n");
		xDoWorkExParamter.eType = DO_WORK_SEND_END_OF_FRAMES;
		kvsDoWorkEx(pKvs, &xDoWorkExParamter);
		printf("--> Sending end of frames done! \r\n");
	}
#endif

	printf("Leaving put media\r\n");
	Kvs_putMediaFinish(pKvs->xPutMediaHandle);
	pKvs->xPutMediaHandle = NULL;

	return res;
}

static int initVideo(Kvs_t *pKvs)
{
	int res = ERRNO_NONE;

	while (pKvs->pVideoTrackInfo == NULL) {
		sleepInMs(50);
	}
	return res;
}

#if ENABLE_AUDIO_TRACK
static int initAudio(Kvs_t *pKvs)
{
	int res = ERRNO_NONE;

	kvsAudioInitTrackInfo(pKvs); // Initialize audio track info

	while (pKvs->pAudioTrackInfo == NULL) {
		sleepInMs(50);
	}

	return res;
}
#endif

static void Kvs_run(Kvs_t *pKvs)
{
	int res = ERRNO_NONE;

#if ENABLE_IOT_CREDENTIAL
	IotCredentialToken_t *pToken = NULL;
#endif
	if (kvsInitialize(pKvs) != ERRNO_NONE) {
		printf("Failed to initialize KVS\r\n");
		res = ERRNO_FAIL;
	} else {
		while (1) {
			if (pKvs->pVideoTrackInfo == NULL) {
				if (initVideo(pKvs) != ERRNO_NONE) {
					printf("Failed to init camera\r\n");
					res = ERRNO_FAIL;
				}
			}
#if ENABLE_AUDIO_TRACK
			if (pKvs->pAudioTrackInfo == NULL) {
				if (initAudio(pKvs) != ERRNO_NONE) {
					printf("Failed to init audio\r\n");
					res = ERRNO_FAIL;
				}
			}
#endif
			if (pKvs->xStreamHandle == NULL) {
				if ((pKvs->xStreamHandle = Kvs_streamCreate(pKvs->pVideoTrackInfo, pKvs->pAudioTrackInfo)) == NULL) {
					printf("Failed to create stream\r\n");
					res = ERRNO_FAIL;
				}
			}
#if ENABLE_IOT_CREDENTIAL
			Iot_credentialTerminate(pToken);
			if ((pToken = Iot_getCredential(&(pKvs->xIotCredentialReq))) == NULL) {
				printf("Failed to get Iot credential\r\n");
				sleepInMs(100);
				continue; //break;
			} else {
				pKvs->xServicePara.pcAccessKey = pToken->pAccessKeyId;
				pKvs->xServicePara.pcSecretKey = pToken->pSecretAccessKey;
				pKvs->xServicePara.pcToken = pToken->pSessionToken;
			}
#endif
			if (setupDataEndpoint(pKvs) != ERRNO_NONE) {
				printf("Failed to get PUT MEDIA endpoint");
			} else if (putMedia(pKvs) != ERRNO_NONE) {
				printf("End of PUT MEDIA\r\n");
				break;
			}

			if (gProducerStop == 1) {
				kvsProducerModuleStop(pKvs);
			}

			while (gProducerStop == 1) {
				printf("Producer is paused/stopped...\r\n");
				sleepInMs(500);
			}

			sleepInMs(100); /* Wait for retry */
		}
	}
}

void aws_mbedtls_mutex_init(mbedtls_threading_mutex_t *mutex)
{
	mutex->mutex = xSemaphoreCreateMutex();

	if (mutex->mutex != NULL) {
		mutex->is_valid = 1;
	} else {
		mutex->is_valid = 0;
		printf("Failed to initialize mbedTLS mutex.\r\n");
	}
}

void aws_mbedtls_mutex_free(mbedtls_threading_mutex_t *mutex)
{
	if (mutex->is_valid == 1) {
		vSemaphoreDelete(mutex->mutex);
		mutex->is_valid = 0;
	}
}

int aws_mbedtls_mutex_lock(mbedtls_threading_mutex_t *mutex)
{
	int ret = MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;

	if (mutex->is_valid == 1) {
		if (xSemaphoreTake(mutex->mutex, portMAX_DELAY)) {
			ret = 0;
		} else {
			ret = MBEDTLS_ERR_THREADING_MUTEX_ERROR;
			printf("Failed to obtain mbedTLS mutex.\r\n");
		}
	}

	return ret;
}

int aws_mbedtls_mutex_unlock(mbedtls_threading_mutex_t *mutex)
{
	int ret = MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;

	if (mutex->is_valid == 1) {
		if (xSemaphoreGive(mutex->mutex)) {
			ret = 0;
		} else {
			ret = MBEDTLS_ERR_THREADING_MUTEX_ERROR;
			printf("Failed to unlock mbedTLS mutex.\r\n");
		}
	}

	return ret;
}

static void ameba_platform_init(void)
{
#if defined(MBEDTLS_PLATFORM_C)
	mbedtls_platform_set_calloc_free(calloc, free);
#endif

#if defined(MBEDTLS_THREADING_ALT)
	/* Configure mbedtls to use FreeRTOS mutexes. */
	mbedtls_threading_set_alt(aws_mbedtls_mutex_init,
							  aws_mbedtls_mutex_free,
							  aws_mbedtls_mutex_lock,
							  aws_mbedtls_mutex_unlock);
#endif

	sntp_init();
	while (getEpochTimestampInMs() < 100000000ULL) {
		vTaskDelay(200 / portTICK_PERIOD_MS);
		printf("waiting get epoch timer\r\n");
	}
}

static void kvs_producer_thread(void *param)
{
	Kvs_t *pKvs = (Kvs_t *)param;

	// Ameba platform init
	ameba_platform_init();

	// kvs produccer init
	platformInit();

	Kvs_run(pKvs);

	vTaskDelete(NULL);
}

static int kvs_producer_handle(void *p, void *input, void *output)
{
	kvs_producer_ctx_t *ctx = (kvs_producer_ctx_t *)p;
	mm_queue_item_t *input_item = (mm_queue_item_t *)input;

	Kvs_t *pKvs = ctx->xKvs;
	TrackType_t track_type;

	size_t uDataLen = input_item->size;
	uint8_t *pData = (uint8_t *)malloc(uDataLen);
	if (pData == NULL) {
		printf("Fail to allocate memory for producer media frame\r\n");
		return 0;
	}
	memcpy(pData, (uint8_t *)input_item->data_addr, uDataLen);

	if (input_item->type == AV_CODEC_ID_H264) {
		if (pKvs->pVideoTrackInfo == NULL && pKvs->inited && isKeyFrame(pData, uDataLen)) {
			kvsVideoInitTrackInfo(pKvs, pData, uDataLen);
		}
		track_type = TRACK_VIDEO;
	} else if (input_item->type == AV_CODEC_ID_MP4A_LATM || input_item->type == AV_CODEC_ID_PCMU || input_item->type == AV_CODEC_ID_PCMA) {
		track_type = TRACK_AUDIO;
	}

	if (pKvs->pVideoTrackInfo
#if ENABLE_AUDIO_TRACK
		&& pKvs->pAudioTrackInfo
#endif
	   ) {
		kvsAddMediaFrame(pKvs, pData, uDataLen, uDataLen, track_type);
	} else {
		free(pData);
	}

	return 0;
}

static int kvs_producer_control(void *p, int cmd, int arg)
{
	kvs_producer_ctx_t *ctx = (kvs_producer_ctx_t *)p;
	Kvs_t *pKvs = ctx->xKvs;

	switch (cmd) {

	case CMD_KVS_PRODUCER_SET_APPLY:
		if (xTaskCreate(kvs_producer_thread, ((const char *)"kvs_producer_thread"), 4096, (void *)pKvs, tskIDLE_PRIORITY + 1,
						&ctx->kvs_producer_module_task) != pdPASS) {
			printf("\n\r%s xTaskCreate(kvs_producer_thread) failed", __FUNCTION__);
		}
		break;
	case CMD_KVS_PRODUCER_STOP:
	case CMD_KVS_PRODUCER_PAUSE:
		gProducerStop = 1;
		while (pKvs->xStreamHandle != NULL) {
			sleepInMs(50);
		}
		break;
	case CMD_KVS_PRODUCER_RECONNECT:
		gProducerStop = 0;
		break;
	}
	return 0;
}

static void *kvs_producer_destroy(void *p)
{
	kvs_producer_ctx_t *ctx = (kvs_producer_ctx_t *)p;

	kvsTerminate(ctx->xKvs);

	if (ctx && ctx->kvs_producer_module_task) {
		vTaskDelete(ctx->kvs_producer_module_task);
	}
	if (ctx && ctx->xKvs) {
		free(ctx->xKvs);
	}
	if (ctx) {
		free(ctx);
	}

	return NULL;
}

static void *kvs_producer_create(void *parent)
{
	kvs_producer_ctx_t *ctx = malloc(sizeof(kvs_producer_ctx_t));
	if (!ctx) {
		return NULL;
	}
	memset(ctx, 0, sizeof(kvs_producer_ctx_t));

	ctx->xKvs = (Kvs_t *)malloc(sizeof(Kvs_t));
	if (!ctx->xKvs) {
		return NULL;
	}
	memset(ctx->xKvs, 0, sizeof(Kvs_t));

	ctx->parent = parent;

	printf("kvs_producer_create...\r\n");

	return ctx;
}

mm_module_t kvs_producer_module = {
	.create = kvs_producer_create,
	.destroy = kvs_producer_destroy,
	.control = kvs_producer_control,
	.handle = kvs_producer_handle,

	.new_item = NULL,
	.del_item = NULL,

	.output_type = MM_TYPE_NONE,    // output for video sink
	.module_type = MM_TYPE_VDSP,    // module type is video algorithm
	.name = "KVS_Producer"
};
