#include "ffmpeg_init.h"

AVFormatContext* oc = NULL;
AVStream* av_stream = NULL;
AVCodecContext* codec_context = NULL;
AVPacket* av_packet = NULL;

int init_ffmpeg_module_function(int Type , const char* ser_addr)
{
    int ret = 0;
    if(Type != 0)
    {
        printf("Not is FLV\n");
        return -1;
    }
    ret = avformat_alloc_output_context2(&oc,NULL,"flv", ser_addr);
    if(ret != 0)
    {
        printf("avformat_alloc_output_context2 error\n");
        return -1;
    }
    else
    {
        printf("avformat_alloc_output_context2 sucess\n");
    }

    av_stream = avformat_new_stream(oc,NULL);
    if(av_stream == NULL)
    {
        printf("new stream error\n");
        avformat_free_context(oc);
        return -1;
    }
    av_stream->id = oc->nb_streams-1;


    AVCodec * avcodec = NULL;
    avcodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if( avcodec == NULL)
    {
        printf("find_encoder error\n");
        avformat_free_context(oc);
            
        return -1;
    }


    
    codec_context = avcodec_alloc_context3(avcodec);
    if( codec_context == NULL)
    {
        printf("avcodec_alloc_context3 error\n");
        avformat_free_context(oc);
        return -1;
    }

    if(avcodec->type != AVMEDIA_TYPE_VIDEO)
    {
        printf("avcodec->type != AVMEDIA_TYPE_VIDEO\n");
        avcodec_close(codec_context); //就是需要关闭的编码器的AVCodecContext
        avcodec_free_context(&codec_context); //释放AVCodecContext上下文

        avformat_free_context(oc);
        avformat_free_context(oc);
        return -1;
    }
    
    codec_context->bit_rate = Width * Height * 3; //FFMPEG视频码率
    codec_context->width = Width; //FFMPEG视频宽度
    codec_context->height = Height;//FFMPEG视频高度

    av_stream->r_frame_rate.den = 1; //FFMPEG帧率,分母
    av_stream->r_frame_rate.num = 25;//FFMPEG帧率,分子
    av_stream->time_base = (AVRational){1, 25};//Stream视频时间基，默认情况下等于帧率

    codec_context->time_base = av_stream->time_base; //编码器时间基
    codec_context->gop_size = Gopsize;//GOPSIZE
    codec_context->pix_fmt = AV_PIX_FMT_NV12;//图像格式
        
    ret = avcodec_parameters_from_context(av_stream->codecpar,  codec_context);
    if(ret != 0)
    {
        printf("avcodec_parameters_from_context error\n");

        avcodec_close(codec_context); //就是需要关闭的编码器的AVCodecContext
        avcodec_free_context(&codec_context); //释放AVCodecContext上下文
        avformat_free_context(oc);
        return -1;
    }
    
    if(oc->oformat->flags & AVFMT_GLOBALHEADER)
    {
        codec_context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        printf("Add sps.pps sucess\n");
    }

    
    av_packet = av_packet_alloc();
    if(av_packet == NULL)
    {
        printf("av_packet_alloc error\n");
        avcodec_close(codec_context); //就是需要关闭的编码器的AVCodecContext
        avcodec_free_context(&codec_context); //释放AVCodecContext上下文
        avformat_free_context(oc);
        return -1;
    }


    av_dump_format(oc, 0, ser_addr , 1);

    if (!(oc->oformat->flags & AVFMT_NOFILE))
    {
        //打开输出文件
        ret = avio_open(&oc->pb,ser_addr, AVIO_FLAG_WRITE);
        if (ret < 0)
        {
            printf("avio open error\n");
            avcodec_close(codec_context); //就是需要关闭的编码器的AVCodecContext
            avcodec_free_context(&codec_context); //释放AVCodecContext上下文
            av_buffer_unref(&(av_packet->buf));//释放AVBufferRef
            av_packet_unref(av_packet); //释放AVPacket
            av_packet_free(&av_packet); //释放AVPacket
            avformat_free_context(oc);
            return -1;
        }
    }

    ret = avformat_write_header(oc, NULL);
    if(ret < 0 )
    {
        printf("writer header error\n");
        avcodec_close(codec_context); //就是需要关闭的编码器的AVCodecContext
        avcodec_free_context(&codec_context); //释放AVCodecContext上下文
        av_buffer_unref(&(av_packet->buf));//释放AVBufferRef
        av_packet_unref(av_packet); //释放AVPacket
        av_packet_free(&av_packet); //释放AVPacket
        avformat_free_context(oc);
        return -1;
    }
    
    
    
    
    
    return 0;
}

