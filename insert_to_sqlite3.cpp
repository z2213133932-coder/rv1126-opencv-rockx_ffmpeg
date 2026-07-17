#include "sqlite3_operation.h"

rockx_handle_t detect_handle;
rockx_handle_t recognize_handle;
rockx_handle_t land5mark_handle;


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

int face_recognize(rockx_image_t* input_image , rockx_object_array_t face_arry , char* name)
{
    rockx_object_t max_face = get_MaxFace(face_arry);
    
    rockx_image_t out_image;
    rockx_ret_t rockx_ret = rockx_face_align(land5mark_handle,input_image,&(max_face.box),NULL,&out_image);
    if(rockx_ret != 0)
    {
        printf("rockx_face_align error\n");
        return -1;
    }
    
    rockx_face_feature_t face_feature = {0};
    rockx_ret = rockx_face_recognize(recognize_handle,&out_image,&face_feature);
    if(rockx_ret != 0)
    {
        printf("rockx_face_recognize error\n");
        return -1;
    }
    
    int ret = InsertInToSqlite3(face_feature,name);
    if(ret == -1)
    {
        printf("insert error\n");
        return -1;
    }

    return 0 ;
}


int main(int argc , char* argv[])
{
    if(argc != 3)
    {
        printf("Please usse: './insert to sqlite3+Name+Path'\n");
    }
    
    int ret = 0;
    char* name = argv[1];
    FILE* path = fopen(argv[2],"rb");

    ret = fseek(path,0L,SEEK_END);
    if(ret != 0)
    {
        printf("fseek error\n");
        exit(-1);
    }
    int file_size = ftell(path);
    rewind(path);

    
    ret = ConnectToSqilte3();
    if(ret == 0)
    {
        printf("you already connect to sqlite\n");
    }
    else
    {
        return -1;
    }


    rockx_ret_t rockx_ret;

    rockx_image_t input_image;
    rockx_ret = rockx_image_read(argv[2],&input_image,1);
    if(rockx_ret != 0)
    {
        printf("rockx_image_read error\n");
        exit(-1);
    }

   
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
    
    
    rockx_object_array_t face_arry = {0,0};
    rockx_ret = rockx_face_detect(detect_handle,&input_image,&face_arry,NULL);
    if(rockx_ret != 0)
    {
        printf("rockx_face_detect error\n");
        exit(-1);
    }
    
    if(face_arry.count > 0)
    {
        ret = face_recognize(&input_image,face_arry,name);
        if(ret != 0)
        {
            printf("face_reconize error\n");
            exit(-1);
        }
    }

    else
    {
        printf("no face \n");
        return -1;
    }

    fclose(path);

    return 0;
}
