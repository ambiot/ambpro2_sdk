#ifndef SAMPLE_CONFIG_H
#define SAMPLE_CONFIG_H

#include "kvs/mkv_generator.h"

/* KVS general configuration */
#define AWS_ACCESS_KEY                  "xxxxxxxxxxxxxxxxxxxx"
#define AWS_SECRET_KEY                  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"

/* KVS stream configuration */
#define KVS_STREAM_NAME                 "kvs_example_camera_stream"
#define AWS_KVS_REGION                  "us-east-1"
#define AWS_KVS_SERVICE                 "kinesisvideo"
#define AWS_KVS_HOST                    AWS_KVS_SERVICE "." AWS_KVS_REGION ".amazonaws.com"

/* KVS optional configuration */
#define ENABLE_AUDIO_TRACK              0
#define ENABLE_IOT_CREDENTIAL           0

/* Buffering options */
#define RING_BUFFER_MEM_LIMIT           (2 * 1024 * 1024)

/* Video configuration */
/* https://www.matroska.org/technical/codec_specs.html */
#define VIDEO_CODEC_NAME                    "V_MPEG4/ISO/AVC"
#define VIDEO_NAME                          "my video"

/* Audio configuration */
#if ENABLE_AUDIO_TRACK
#define USE_AUDIO_AAC                   1   /* Set to 1 to use AAC as audio track */
#define USE_AUDIO_G711                  0   /* Set to 1 to use G711 as audio track */
#if USE_AUDIO_AAC == USE_AUDIO_G711
#error KVS producer audio format setting error! USE_AUDIO_AAC and USE_AUDIO_G711 cannot be both 1 or both 0.
#endif
#if USE_AUDIO_AAC
#define AUDIO_CODEC_NAME                "A_AAC"
#elif USE_AUDIO_G711
#define AUDIO_CODEC_NAME                "A_MS/ACM"
#endif
#define AUDIO_MPEG_OBJECT_TYPE          MPEG4_AAC_LC
#define AUDIO_PCM_OBJECT_TYPE           PCM_FORMAT_CODE_MULAW
#define AUDIO_SAMPLING_RATE             8000
#define AUDIO_CHANNEL_NUMBER            1
#define AUDIO_NAME                      "my audio"
#endif /* ENABLE_AUDIO_TRACK */

/* IoT credential configuration */
#if ENABLE_IOT_CREDENTIAL
#define CREDENTIALS_HOST                "xxxxxxxxxxxxxx.credentials.iot.us-east-1.amazonaws.com"
#define ROLE_ALIAS                      "KvsCameraIoTRoleAlias"
#define THING_NAME                      KVS_STREAM_NAME

#define ROOT_CA \
"-----BEGIN CERTIFICATE-----\n" \
"......\n" \
"-----END CERTIFICATE-----\n"

#define CERTIFICATE \
"-----BEGIN CERTIFICATE-----\n" \
"......\n" \
"-----END CERTIFICATE-----\n"

#define PRIVATE_KEY \
"-----BEGIN RSA PRIVATE KEY-----\n" \
"......\n" \
"-----END RSA PRIVATE KEY-----\n"
#endif

#endif /* SAMPLE_CONFIG_H */