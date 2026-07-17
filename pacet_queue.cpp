#include "pacet_queue.h"
extern video_queue* Queue_vedio;

video_queue::video_queue()
{
    pthread_mutex_init(&queue_mutex,NULL);
    pthread_cond_init(&queue_cond,NULL);

}

int video_queue::set_packet(video_packet* v_packeet)
{
    pthread_mutex_lock(&queue_mutex);
    this->queue_packet.push(v_packeet);
    pthread_cond_broadcast(&queue_cond);
    pthread_mutex_unlock(&queue_mutex);
    return 0;
}

int video_queue::get_packet_count()
{
    int num = Queue_vedio->queue_packet.size();
    return num;
}


video_packet* video_queue::get_packet()
{

    pthread_mutex_lock(&queue_mutex);
    while(Queue_vedio->get_packet_count() == 0)
    {
        pthread_cond_wait(&queue_cond,&queue_mutex);
    }
    video_packet* tmp_packet = Queue_vedio->queue_packet.front();
    Queue_vedio->queue_packet.pop();
    pthread_mutex_unlock(&queue_mutex);
    return tmp_packet;
}


video_queue::~video_queue()
{
    pthread_mutex_destroy(&queue_mutex);
    pthread_cond_destroy(&queue_cond);
}

