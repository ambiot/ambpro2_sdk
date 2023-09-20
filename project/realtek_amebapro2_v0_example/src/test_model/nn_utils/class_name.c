//------------------------------------------------------------------------------
// class name
//------------------------------------------------------------------------------
#include "class_name.h"

static const char *coco_names[80] = {"person", "bicycle", "car", "motorbike", "aeroplane", "bus", "train", "truck", "boat", "traffic light",
									 "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow",
									 "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
									 "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard", "tennis racket", "bottle",
									 "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple", "sandwich", "orange",
									 "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "sofa", "pottedplant", "bed",
									 "diningtable", "toilet", "tvmonitor", "laptop", "mouse", "remote", "keyboard", "cell phone", "microwave", "oven",
									 "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear", "hair drier", "toothbrush"
									};

static const char *voc_names_21[21] = {"__background__",
									   "aeroplane", "bicycle", "bird", "boat",
									   "bottle", "bus", "car", "cat", "chair",
									   "cow", "diningtable", "dog", "horse",
									   "motorbike", "person", "pottedplant",
									   "sheep", "sofa", "train", "tvmonitor"
									  };


static const char *voc_names_20[20] = {"aeroplane", "bicycle", "bird", "boat",
									   "bottle", "bus", "car", "cat", "chair",
									   "cow", "diningtable", "dog", "horse",
									   "motorbike", "person", "pottedplant",
									   "sheep", "sofa", "train", "tvmonitor"
									  };

#define voc_names   voc_names_20

const char *coco_name_get_by_id(int id)
{
	return coco_names[id];
}

const char *voc_name_get_by_id(int id)
{
	return voc_names[id];
}