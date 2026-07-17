#include <pthread.h>
extern "C"
{
#define __STDC_CONSTANT_MACROS
#include <libavutil/avutil.h>
#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include "libavutil/time.h"
}

#define FLV 0
#define TS 1
#define Width 1920
#define Height 1080
#define Gopsize 25

int init_ffmpeg_module_function(int Type , const char* ser_addr);
