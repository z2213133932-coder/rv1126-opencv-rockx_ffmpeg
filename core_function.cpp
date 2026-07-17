#include "core_function.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/legacy/constants_c.h>
extern video_queue* Queue_vedio;
extern AVFormatContext* oc ;
extern AVStream* av_stream ;
extern AVCodecContext* codec_context ;
extern AVPacket* av_packet ;

using namespace cv;


rockx_object_array_t Face_arry = {0,0};
pthread_mutex_t face_arry_mutex;
pthread_mutex_t name_mutex;

string Name;

void init_facearry_name()
{
    pthread_mutex_init(&face_arry_mutex,NULL);
    pthread_mutex_init(&name_mutex,NULL);

    return;
}


map<string,rockx_face_feature_t> Get_map()
{
    int index = 0;
    Sthread_map thread_map;
    map<string,rockx_face_feature_t> tmp_map = thread_map.get_map(index);
    printf("get map sucess\n");
    return tmp_map;
}


void set_face_arry(rockx_object_array_t handle_face_arry)
{
    pthread_mutex_lock(&face_arry_mutex);
    Face_arry = handle_face_arry;
    pthread_mutex_unlock(&face_arry_mutex);
    return;
}


rockx_object_array_t get_face_arry()
{
    rockx_object_array_t tmp_face_arry = {0,0};
    pthread_mutex_lock(&face_arry_mutex);
    tmp_face_arry = Face_arry;
    pthread_mutex_unlock(&face_arry_mutex);
    return tmp_face_arry;

}


void set_name(string handle_name)
{
    pthread_mutex_lock(&name_mutex);
    Name = handle_name;
    pthread_mutex_unlock(&name_mutex);
    return;
}


string get_name()
{
    string tmp_name;
    pthread_mutex_lock(&name_mutex);
    tmp_name = Name;
    pthread_mutex_unlock(&name_mutex);
    return tmp_name;
}


rockx_object_t get_MaxFace(rockx_object_array_t face_arry)
{   
    int i = 0;
    rockx_object_t max_object ={0};
    
    double area = 0;
    double max_area = 0;
    for(i;i<face_arry.count;i++)
    {
        area =(face_arry.object[i].box.bottom-face_arry.object[i].box.top)*(face_arry.object[i].box.right-face_arry.object[i].box.left);
        if(area>max_area)
        {
            max_area = area;
            max_object = face_arry.object[i];
        }
    }
    return max_object;

}


void* vi_handle_thread(void* args)
{
    Name.clear();
    pthread_detach(pthread_self());

    map<string,rockx_face_feature_t> string_map = Get_map();
    if(string_map.empty())
    {
        printf("Get string_mao error\n");
    }
    

    rockx_handle_t detect_handle;
    rockx_handle_t recognize_handle;
    rockx_handle_t land5mark_handle;
    rockx_ret_t rockx_ret;

    rockx_config_t *rockx_congfig;
    rockx_congfig = rockx_create_config();
    rockx_ret = rockx_add_config(rockx_congfig,ROCKX_CONFIG_DATA_PATH,"/userdata/rockx_data");
    if(rockx_ret != 0)
    {
        printf("rockx_add_config error\n");
        exit(-1);
    }
    
    rockx_ret = rockx_create(&detect_handle,ROCKX_MODULE_FACE_DETECTION_V2,rockx_congfig,0);
    if(rockx_ret != 0)
    {
        printf("detect rockx_create error\n");
        exit(-1);
    }
    
    rockx_ret = rockx_create(&recognize_handle,ROCKX_MODULE_FACE_RECOGNIZE,rockx_congfig,0);
    if(rockx_ret != 0)
    {
        printf("recognize rockx_create error\n");
        exit(-1);
    }
    
    rockx_ret = rockx_create(&land5mark_handle,ROCKX_MODULE_FACE_LANDMARK_5,rockx_congfig,0);
    if(rockx_ret != 0)
    {
        printf("land5mark rockx_create error\n");
        exit(-1);
    }


    rockx_image_t input_image,out_img;
    input_image.width = Width;
    input_image.height = Height;
    input_image.pixel_format = ROCKX_PIXEL_FORMAT_YUV420SP_NV12;
    rockx_object_array_t handle_face_arry ;
    rockx_object_t Max_face = {0};


    MEDIA_BUFFER mb = NULL;
    float similarity = 0;
    bool is_recognize;
    string handle_name;

    while(1)
    {
        map<string, rockx_face_feature_t>::iterator database_iter;
        memset(&out_img, 0, sizeof(out_img));
        handle_name.clear();
        memset(&handle_face_arry, 0, sizeof(handle_face_arry));
        mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_VI,0,-1);
        input_image.data = (unsigned char *)RK_MPI_MB_GetPtr(mb);
        input_image.size = RK_MPI_MB_GetSize(mb);


        rockx_ret = rockx_face_detect(detect_handle,&input_image,&handle_face_arry,NULL);
        if(rockx_ret != 0)
        {
            printf("rockx_face_detect error\n");
            exit(-1);
        }

        if(handle_face_arry.count>0)
        {
            set_face_arry(handle_face_arry);
            Max_face = get_MaxFace(handle_face_arry);
            rockx_ret = rockx_face_align(land5mark_handle, &input_image,&(Max_face.box), NULL, &out_img);
            if (rockx_ret != ROCKX_RET_SUCCESS)
            {
                printf("face_align failed\n");
            }

            rockx_face_feature_t out_feature;
            rockx_face_recognize(recognize_handle, &out_img, &out_feature);

            for(database_iter = string_map.begin();database_iter != string_map.end();database_iter++)
            {
                rockx_face_feature_similarity(&out_feature,&database_iter->second,&similarity);
                if (similarity <= 1.2)
                    {
                        is_recognize = true;
                        break;
                    }
                    else
                    {
                        is_recognize = false;
                        continue;
                    }
            }

            if(is_recognize)
            {
                handle_name = database_iter->first;
                set_name(handle_name);
                printf("name: %s\n",handle_name.c_str());

            }
            else
            {
                handle_name = "UNkonw!";
                set_name(handle_name);
                printf("name: %s\n",handle_name.c_str());
            }

        }

    RK_MPI_MB_ReleaseBuffer(mb);
    }
    int ret = RK_MPI_VI_DisableChn(0, 0);
    if (ret)
    {
        printf("Disable Chn Venc error! ret=%d\n", ret);
        return NULL; ;
    }
    return NULL;
}


void* vi_show_thread(void* args)
{
    pthread_detach(pthread_self());
    
    int ret = 0;


    MEDIA_BUFFER mb = NULL ;
    
    Point point_1 = {0,0};
    Point point_2 = {0,0};
    Point point_count,point_name;
    point_count.x = 300;
    point_count.y = 300;
    point_name.x = 800;
    point_name.y = 800;


    while(1)
    {
        int i = 0;
        mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_VI,1,-1);
        Mat show_image = Mat(1080, 1920, CV_8UC1, RK_MPI_MB_GetPtr(mb));
        rockx_object_array_t show_face_arry = get_face_arry();

        string text_count = to_string(show_face_arry.count);
        string show_name = get_name();
        putText(show_image,text_count,point_count,FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 0), 3);
        putText(show_image,show_name,point_name,FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 255, 255), 3);
        

        for(i;i<show_face_arry.count;i++)
        {
            point_1.y = show_face_arry.object[i].box.top;
            point_1.x = show_face_arry.object[i].box.left;
            point_2.y = show_face_arry.object[i].box.bottom;
            point_2.x = show_face_arry.object[i].box.right;

            rectangle(show_image,point_1,point_2,Scalar(255, 255, 255),1,8,0);
        }

        RK_MPI_SYS_SendMediaBuffer(RK_ID_VENC, 0, mb);
        RK_MPI_MB_ReleaseBuffer(mb);

    }

   ret = RK_MPI_VI_DisableChn(0, 0);
    if (ret)
    {
        printf("Disable Chn Venc error! ret=%d\n", ret);
        return NULL; ;
    }

    return NULL;
}


void* venc_thread(void* args)
{
    pthread_detach(pthread_self());
    MEDIA_BUFFER mb = NULL;
    video_packet* v_packet = (video_packet*)malloc(sizeof(video_packet));
    while(1)
    {
        mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_VENC,0,-1);
        memcpy(v_packet->buffer, RK_MPI_MB_GetPtr(mb), RK_MPI_MB_GetSize(mb));
        v_packet->video_frame_size = RK_MPI_MB_GetSize(mb);

        Queue_vedio->set_packet(v_packet);
        RK_MPI_MB_ReleaseBuffer(mb);
    }

    int ret = RK_MPI_VENC_DestroyChn(0);
    if (ret)
    {
        printf("Destroy Venc error! ret=%d\n", ret);
        return NULL; ;
    }    


}


void* push_thread(void* args)
{
    pthread_detach(pthread_self());
    av_packet->pts = 0;
    while(1)
    {
        video_packet* push_packet = Queue_vedio->get_packet();
        av_packet = av_packet_alloc();
        int ret = av_buffer_realloc(&av_packet->buf, push_packet->video_frame_size + 70);
        if (ret < 0)
        {
            return NULL;
        }

        av_packet->size = push_packet->video_frame_size;                                        // rv1126的视频长度赋值到AVPacket Size
        memcpy(av_packet->buf->data, push_packet->buffer, push_packet->video_frame_size); // rv1126的视频数据赋值到AVPacket data
        av_packet->data = av_packet->buf->data;                                                             // 把pkt->buf->data赋值到pkt->data
        av_packet->flags |= AV_PKT_FLAG_KEY;
        av_packet->pts++;

        av_packet_rescale_ts(av_packet, codec_context->time_base, av_stream->time_base);
        av_packet->stream_index = av_stream->index;
        av_interleaved_write_frame(oc, av_packet);
    }
    av_write_trailer(oc);                        
    avcodec_close(codec_context); 
    avcodec_free_context(&codec_context); 
    av_buffer_unref(&(av_packet->buf));
    av_packet_unref(av_packet); 
    av_packet_free(&av_packet);
    avio_closep(&oc->pb);                       
    avformat_free_context(oc);          
    return NULL;       
}


int core_infunction()
{
    pthread_t pid;
    pthread_create(&pid,NULL,vi_handle_thread,NULL);

    pthread_create(&pid,NULL,vi_show_thread,NULL);

    pthread_create(&pid,NULL,venc_thread,NULL);

    pthread_create(&pid,NULL,push_thread,NULL);

    while(1)
    {
        sleep(20);
    }


    return 0;
}