/**
 ******************************************************************************
 *This file contains sensor configurations for AmebaPro platform
 ******************************************************************************
*/


#ifndef __SENSOR_H
#define __SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

struct sensor_params_t {
	unsigned int sensor_width;
	unsigned int sensor_height;
	unsigned int sensor_fps;
};

#define SENSOR_DUMMY        0x00 //For dummy sensor, no support fast camera start
#define SENSOR_F37          0x01

static const struct sensor_params_t sensor_params[] = {
	{1920, 1080, 30}, //DUMMY
	{1920, 1080, 30}, //F37
};


#define SENSOR_MAX         2

static const unsigned char sen_id[SENSOR_MAX] = {
	SENSOR_DUMMY,
	SENSOR_F37
};
#define USE_SENSOR      	SENSOR_F37

static const      char manual_iq[SENSOR_MAX][64] = {
	"iq",
	"iq_f37"
};
#define MANUAL_SENSOR_IQ	0xFF

#define ENABLE_FCS      	0

#define MULTI_DISABLE       0x00
#define MULTI_ENABLE        0x01

#define MULTI_SENSOR  		MULTI_DISABLE
#define NONE_FCS_MODE       0
#define FW1_IQ_ADDR        0xF20000
#define FW2_IQ_ADDR        0xF60000
#define FW_IQ_SIZE         256*1024
#define FW_CAL_IQ_SIZE     16*1024
#define FW_SENSOR_SIZE     16*1024
#define FW_VOE_SIZE        600*1024
#define VIDEO_MPU_VOE_HEAP  0
#define SENSOR_SINGLE_DEFAULT_SETUP     0x00
#define SENSOR_MULTI_DEFAULT_SETUP      0X01
#define SENSOR_MULTI_SAVE_VALUE         0X02
#define SENSOR_MULTI_SETUP_PROCEDURE	0X03
#ifdef __cplusplus
}
#endif


#endif /* __AMEBAPRO_SENSOR_EVAL_H */
