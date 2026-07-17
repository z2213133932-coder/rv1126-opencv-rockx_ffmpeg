#include "rkmedia_init.h"
#include "ffmpeg_init.h"
#include"mappeoplefeature.h"
#include "pacet_queue.h"


using namespace std;

void init_facearry();

map<string,rockx_face_feature_t> Get_map(int index);

void set_face_arry(rockx_object_array_t handle_face_arry);

rockx_object_array_t get_face_arry();

void set_name(string handle_name);

rockx_object_t get_MaxFace(rockx_object_array_t handle_face_arry);

void* vi_handle_thread(void* args);

void* vi_show_thread(void* args);

void* venc_thread(void* args);

void* push_thread(void* args);


int core_infunction();
