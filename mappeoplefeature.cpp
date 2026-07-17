#include"mappeoplefeature.h"
map<string,rockx_face_feature_t> map_arry[MAX_NUM];

Sthread_map::Sthread_map()
{
    pthread_mutex_init(&mutex_map,NULL);
}


int Sthread_map::set_map(int index,map<string,rockx_face_feature_t> map_people)
{
    pthread_mutex_lock(&mutex_map);
    map_arry[index] = map_people;
    pthread_mutex_unlock(&mutex_map);
    return 0;
}

map<string,rockx_face_feature_t>Sthread_map:: get_map(int index)
{
    map<string,rockx_face_feature_t> tmp_map;
    pthread_mutex_lock(&mutex_map);
    tmp_map = map_arry[index];
    pthread_mutex_unlock(&mutex_map);
    return tmp_map;
}

Sthread_map::~Sthread_map()
{
    pthread_mutex_destroy(&mutex_map);
}