#ifndef MMF2_PRO2_VIDEO_CONFIG_H
#define MMF2_PRO2_VIDEO_CONFIG_H

/*ISP CHANNEL*/
//ISP channel 0/1/2:HEVC/JPEG/NV12 4:only RGB

/*ENCODE TYPE*/
//type : 0:HEVC 1:H264 2:JPEG 3:NV12 4:RGB 5:HEVC+JPEG 6:H264+JPEG
enum encode_type {
	VIDEO_HEVC = 0,
	VIDEO_H264,
	VIDEO_JPEG,
	VIDEO_NV12,
	VIDEO_RGB,
	VIDEO_NV16,
	VIDEO_HEVC_JPEG,
	VIDEO_H264_JPEG
};

#define	VIDEO_QCIF  0
#define	VIDEO_CIF   1
#define	VIDEO_WVGA  2
#define	VIDEO_VGA   3
#define	VIDEO_D1    4
#define	VIDEO_HD    5
#define	VIDEO_FHD   6
#define	VIDEO_3M    7
#define	VIDEO_5M    8
#define	VIDEO_2K    9

/*

static const int video_res_w[10] = {
	176, //QCIF
	352, // CIF
	640, // WVGA
	640, // VGA
	720, //D1
	1280, // HD
	1920, // FHD
	2048, // 3M
	2592, // 5M
	2560, // 2K
};

static const int video_res_h[10] = {
	144,  //QCIF
	288,  // CIF
	360,  // WVGA
	480,  // VGA
	480,  //D1
	720, // HD
	1080, // FHD
	1536, // 3M
	1944, // 5M
	1440, // 2K
};
*/

#endif /* MMF2_PRO2_VIDEO_CONFIG_H */