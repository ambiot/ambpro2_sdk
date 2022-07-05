#ifndef NN_OSD_DRAW_H_
#define NN_OSD_DRAW_H_

#include "mmf2_module.h"
#include "module_vipnn.h"

#define OSD_OBJ_MAX_NUM 6 //fix value
#define OSD_OBJ_TOTAL_MAX_NUM 18 //fix value
#define OSD_TXT_MAX_NUM 20
#define OSD_PT_MAX_NUM 5

#define DRAW_NOTHING   0
#define DRAW_RECT_ONLY 1
#define DRAW_TEXT_ONLY 2
#define DRAW_RECT_AND_TEXT 3

typedef struct osd_color_s {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} osd_color_t;

typedef struct nn_rect_s {
	int xmin, ymin;
	int xmax, ymax;
} nn_rect_t;

typedef struct nn_osd_pt_s {
	unsigned char pt_width;
	osd_color_t color;
	int x, y;
} nn_osd_pt_t;

typedef struct nn_osd_text_s {
	char text_str[OSD_TXT_MAX_NUM];
	int left;
	int top;
	osd_color_t color;
} nn_osd_text_t;

typedef struct nn_osd_rect_s {
	nn_rect_t rect;
	unsigned char line_width;
	osd_color_t color;
} nn_osd_rect_t;

typedef struct nn_osd_object_s {
	int idx;
	int ch;
	int draw_status;
	nn_osd_text_t text;
	nn_osd_rect_t rect;

	int point_num;
	nn_osd_pt_t pt[OSD_PT_MAX_NUM];
} nn_osd_object_t;

typedef struct nn_osd_ctx_s {
	int nn_osd_ready2draw;
	int nn_osd_channel_en[3];
	int nn_osd_channel_xmax[3], nn_osd_channel_ymax[3];
	nn_osd_object_t obj[OSD_OBJ_TOTAL_MAX_NUM];
} nn_osd_ctx_t;

typedef struct rgb_s {
	uint8_t r, g, b;
} rgb_t;

typedef union {
	uint32_t u32;	// u32 [x, b, g, r]
	rgb_t rgb;
} nn_color_t;

#define RGB(r,g,b) ((((b)&0xff) << 16) | (((g)&0xff) << 8) | ((r)&0xff))

#define COLOR_RED 		RGB(0xff,0x00,0x00)
#define COLOR_BLUE 		RGB(0x00,0x00,0xff)
#define COLOR_GREEN 	RGB(0x00,0xff,0x00)
#define COLOR_PURPLE 	RGB(0xff,0x00,0xff)
#define COLOR_YELLOW 	RGB(0xff,0xff,0x00)
#define COLOR_CYAN      RGB(0x00,0xff,0xff)
#define COLOR_WHITE 	RGB(0xff,0xff,0xff)
#define COLOR_BLACK 	RGB(0x00,0x00,0x00)
#define COLOR_GRAY 		RGB(0x7f,0x7f,0x7f)
#define COLOR_HORANGE	RGB(0xf3,0x70,0x21)
#define COLOR_TBLUE		RGB(0x0a,0xba,0xb5)
#define COLOR_BGREEN	RGB(0x00,0x92,0x5b)
#define COLOR_CRED		RGB(0xfe,0x00,0x1a)

typedef struct nn_osd_draw_obj_s {
	int obj_num;
	nn_rect_t rect[OSD_OBJ_MAX_NUM];
	int score[OSD_OBJ_MAX_NUM];
	int class[OSD_OBJ_MAX_NUM];
	char name[OSD_OBJ_MAX_NUM][128];
	landmark_t landmark[OSD_OBJ_MAX_NUM * 5];

	nn_color_t	color[OSD_OBJ_MAX_NUM];
	int invalid[OSD_OBJ_MAX_NUM];
} nn_osd_draw_obj_t;

void nn_osd_start(int v1_enable, int v1_w, int v1_h, int v2_enable, int v2_w, int v2_h,
				  int v3_enable, int v3_w, int v3_h, int txt_w, int txt_h);
int nn_osd_get_status(void);
int nn_osd_set_rect_with_text(int idx, int ch, nn_osd_text_t *text, nn_osd_rect_t *rect);
int nn_osd_set_rect(int idx, int ch, nn_osd_rect_t *rect);
int nn_osd_set_point(int idx, int ch, int point_num, nn_osd_pt_t *pt);
int nn_osd_clear_bitmap(int idx, int ch);
int nn_osd_block_occupied(int idx, int ch);
int nn_osd_update(void);

#endif //NN_OSD_DRAW_H_