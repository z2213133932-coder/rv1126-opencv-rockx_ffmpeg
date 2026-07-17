#include "rkmedia_init.h"

#define VI_PIPE 0
#define VI_HANDLE_CHN 0
#define VI_SHOW_CHN 1
#define VENC_CHN 0
#define CAMERA_FILE "rkispp_scale0"

int init_vi(int VI_CHN,VI_CHN_ATTR_S* vi_chn_attr)
{
    int ret = 0;
    ret = RK_MPI_VI_SetChnAttr(VI_PIPE,VI_CHN,vi_chn_attr);
    if(ret != 0)
    {
        printf("RK_MPI_VI_SetChnAttr error\n ");
        return -1;
    }
    else
    {
        printf("RK_MPI_VI_SetChnAttr sucess\n ");
    }

    ret = RK_MPI_VI_EnableChn(VI_PIPE,VI_CHN);
    if(ret != 0)
    {
        printf("RK_MPI_VI_EnableChn error\n ");
        return -1;
    }
    else
    {
        printf("RK_MPI_VI_EnableChn sucess\n ");
    }


    return 0;
}

int init_venc(int VENC_chn,VENC_CHN_ATTR_S* venc_chn_attr)
{
    int ret = 0;
    ret = RK_MPI_VENC_CreateChn(VENC_chn,venc_chn_attr);
    if(ret != 0)
    {
        printf("RK_MPI_VENC_CreateChn error \n ");
        return -1;
    }
    else
    {
        printf("RK_MPI_VENC_CreateChn suess \n ");
    }
    return 0;
}

int init_rkmedia_module_function()
{
    RK_MPI_SYS_Init();

    int ret = 0;
    VI_CHN_ATTR_S vi_handle_attr,vi_show_attr;
    vi_handle_attr.enBufType = VI_CHN_BUF_TYPE_MMAP;
    vi_handle_attr.enPixFmt = IMAGE_TYPE_NV12;
    vi_handle_attr.enWorkMode = VI_WORK_MODE_NORMAL;
    vi_handle_attr.pcVideoNode = CAMERA_FILE;
    vi_handle_attr.u32BufCnt = 3;
    vi_handle_attr.u32Height = 1920;
    vi_handle_attr.u32Width = 1080;
    ret = init_vi(VI_HANDLE_CHN,&vi_handle_attr);
    if(ret != 0)
    {   
        RK_MPI_VI_DisableChn(VI_PIPE,VI_HANDLE_CHN);
        return -1;
    }
   
    vi_show_attr.enBufType = VI_CHN_BUF_TYPE_MMAP;
    vi_show_attr.enPixFmt = IMAGE_TYPE_NV12;
    vi_show_attr.enWorkMode = VI_WORK_MODE_NORMAL;
    vi_show_attr.pcVideoNode = CAMERA_FILE;
    vi_show_attr.u32BufCnt = 3;
    vi_show_attr.u32Height = 1080;
    vi_show_attr.u32Width = 1920;
    ret = init_vi(VI_SHOW_CHN,&vi_show_attr);
    if(ret != 0)
    {
        RK_MPI_VI_DisableChn(VI_PIPE,VI_SHOW_CHN);
        return -1;
    }


    VENC_CHN_ATTR_S venc_chn_attr;
    venc_chn_attr.stVencAttr.u32PicWidth = 1920;
    venc_chn_attr.stVencAttr.u32PicHeight = 1080;
    venc_chn_attr.stVencAttr.u32VirWidth = 1920;
    venc_chn_attr.stVencAttr.u32VirHeight = 1080;
    venc_chn_attr.stVencAttr.u32Profile = 66;
    venc_chn_attr.stVencAttr.enRotation = VENC_ROTATION_0;
    venc_chn_attr.stVencAttr.imageType = IMAGE_TYPE_NV12;
    venc_chn_attr.stVencAttr.enType = RK_CODEC_TYPE_H264;
    venc_chn_attr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
    venc_chn_attr.stRcAttr.stH264Cbr.u32Gop = 25;
    venc_chn_attr.stRcAttr.stH264Cbr.u32BitRate = 1920*1080*8*3;
    venc_chn_attr.stRcAttr.stH264Cbr.u32SrcFrameRateNum = 25;
    venc_chn_attr.stRcAttr.stH264Cbr.u32SrcFrameRateDen = 1;
    venc_chn_attr.stRcAttr.stH264Cbr.fr32DstFrameRateNum = 25;
    venc_chn_attr.stRcAttr.stH264Cbr.fr32DstFrameRateDen = 1;

    ret = init_venc(VENC_CHN,&venc_chn_attr);
    if(ret != 0)
    {
        RK_MPI_VENC_DestroyChn(VENC_CHN);
        return -1;
    }

    return 0;
}

int start_vi_mod()
{
    RK_MPI_VI_StartStream(VI_PIPE,VI_HANDLE_CHN);
    RK_MPI_VI_StartStream(VI_PIPE,VI_SHOW_CHN);
    return 0;
}
