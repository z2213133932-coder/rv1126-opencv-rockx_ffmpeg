#include "rockx.h"
#include "mappeoplefeature.h"
#include <assert.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "rkmedia_api.h"
#include <stdio.h>
#include <time.h>


int init_vi(int VI_CHN,VI_CHN_ATTR_S* vi_chn_attr);

int init_venc(int VENC_Chn,VENC_CHN_ATTR_S* venc_chn_attr);

int init_rkmedia_module_function();

int start_vi_mod();
