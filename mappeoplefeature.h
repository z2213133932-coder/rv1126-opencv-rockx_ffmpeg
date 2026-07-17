#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream> 
#include <fstream>
#include <map>
#include <vector>
#include <uchar.h>
#include "rockx.h"
#ifndef MAPPEOPLEFEATURE_H
#define MAPPEOPLEFEATURE_H
using namespace std;
#define MAX_NUM 10

extern map<string,rockx_face_feature_t> map_arry[MAX_NUM];


class Sthread_map 
{
    public:
        map<string,rockx_face_feature_t> thread_map;
        pthread_mutex_t mutex_map;
        
    public:
        Sthread_map();
        ~Sthread_map();
        int set_map(int index,map<string,rockx_face_feature_t> map_people);
        map<string,rockx_face_feature_t> get_map(int index);

};

#endif
