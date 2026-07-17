#include "ffmpeg_init.h"
#include <pthread.h>
#include <queue>

using namespace std;

#define MAX_VIDEO_BUFFER_SIZE 1024*1024*3

typedef struct video_packet
{
    unsigned char buffer[MAX_VIDEO_BUFFER_SIZE];
    int video_frame_size;
}video_packet;



class video_queue
{
    public:
        queue<video_packet*> queue_packet;
        pthread_mutex_t queue_mutex;
        pthread_cond_t queue_cond;
    public:
        video_queue();

        int get_packet_count();
        int set_packet(video_packet* v_packeet);
        video_packet* get_packet();
        

        ~video_queue();

};