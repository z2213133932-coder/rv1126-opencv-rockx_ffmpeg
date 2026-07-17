#include "sqlite3_operation.h"
#include "rkmedia_init.h"
#include "ffmpeg_init.h"
#include "core_function.h"

video_queue* Queue_vedio = NULL;


int get_sqlite_data()
{
    int index = 0;

    Sthread_map sthread_map;
    map<string,rockx_face_feature_t> name_map;
    int ret = ConnectToSqilte3();
    if(ret == 0)
    {
        printf("alredy connect to sqlite\n");
    }
    else 
    {
        return -1;
    }
    name_map = GetSquliteData();
    if(name_map.empty())
    {
        printf("get data from squlite error\n");
    }
    else
    {
        printf("get data from squlite sucess\n");
    }
    
    ret = sthread_map.set_map(index,name_map);
    if(ret != 0)
    {
        printf("set map error\n");
        exit(-1);
    }
    return 0;
}


int main(int argc , char* argv[])
{
    if(argc != 3)
    {
        printf("Please usage:./face_recognize Type ServiceAddress  Type: 0-->flv 1-->ts\n");
        return 0;
    }

    Queue_vedio =  new video_queue();


    char* ser_addr = argv[2];
    int T_type = atoi(argv[1]);


    int ret = 0;
    ret = get_sqlite_data();
    if(ret == 0)
    {
        printf("set map sucess\n ");
    }
    else
    {
        return -1;
    }
     
    ret = init_rkmedia_module_function();
    if(ret == 0)
    {
        printf("set map sucess\n ");
    }
    else
    {
        return -1;
    }

    ret = init_ffmpeg_module_function(T_type,ser_addr);
    if(ret != 0)
    {   
        printf("init_ffmpeg_module_function error\n");
        return -1;
    }
    else
    {
        printf("init_ffmpeg_module_function sucess\n");
    }

    ret = start_vi_mod();
    if(ret == 0)
    {
        printf("start_vi_mod sucess\n ");
    }
    else
    {   
        printf("start_vi_mod sucess\n");
        return -1;
    }

    core_infunction();
    

    return 0;
}