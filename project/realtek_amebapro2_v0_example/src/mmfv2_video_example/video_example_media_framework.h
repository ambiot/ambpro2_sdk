#ifndef MMF2_VIDEO_EXAMPLE_H
#define MMF2_VIDEO_EXAMPLE_H

#include "platform_opts.h"
#include "sensor.h"

#include "sys_api.h" 	// for system reset

#define USR_CMD_RTSP_PAUSE			    (u32)0x1
#define USR_CMD_RTSP1_PAUSE 			(u32)(0x1 << 1)
#define USR_CMD_RTSP2_PAUSE 			(u32)(0x1 << 2)
#define USR_CMD_RTSP3_PAUSE 			(u32)(0x1 << 3)
#define USR_CMD_AUDIO_PAUSE 			(u32)(0x1 << 4)
#define USR_CMD_VIDEO_PAUSE 			(u32)(0x1 << 8)
#define USR_CMD_VIDEO1_PAUSE 			(u32)(0x1 << 9)
#define USR_CMD_RECORD_PAUSE            (u32)(0x1 << 12)
#define USR_CMD_RTP_PAUSE               (u32)(0x1 << 16)
#define USR_CMD_MBNSSD_PAUSE            (u32)(0x1 << 20)
#define USR_CMD_FRC_PAUSE               (u32)(0x1 << 21)
#define USR_CMD_VIPNN_PAUSE             (u32)(0x1 << 22)
#define USR_CMD_EXAMPLE_DEINIT		    (u32)(0x1 << 31)

void video_example_media_framework(void);

void mmf2_video_example_v1_init(void);

void mmf2_video_example_vipnn_rtsp_init(void);

// ---- Plumerai change begin ----
void mmf2_video_plumerai_ffid_rtsp(void);
// ---- Plumerai change end ----

#endif /* MMF2_VIDEO_EXAMPLE_H */