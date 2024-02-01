/*
 * queue.c
 *
 *  Created on: Jan 25, 2024
 *      Author: hoanpx
 */
/*******************************************************/
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "app_queue.h"
/*******************************************************/
#define LEN     30
/*******************************************************/
uint8_t queue1[LEN];
uint8_t queue2[LEN];
uint8_t queue3[LEN];
uint8_t queue4[LEN];
uint8_t* queue_data[4] = {queue1,queue2,queue3,queue4};
/*******************************************************/
int8_t APP_QUEUE_Init(app_queue* queue){
    queue->front = queue->rear = -1;
    queue->size = 4;
    queue->data = queue_data;
    return 0;
}
int8_t APP_QUEUE_EnQueue(app_queue* queue,uint8_t* value){
    if(queue->level == queue->size)
    {
        queue->state = Queue_Full;
        return 1;
    }
    else if (queue->front == -1)
    {
        queue->front = queue->rear = 0;
        strcpy((char*)queue->data[queue->rear],(char*)value);
        queue->level++;
    }
    else if (queue->rear == queue->size-1 && queue->front != 0)
    {
        queue->rear = 0;
        strcpy((char*)queue->data[queue->rear],(char*)value);
        queue->level++;
    }
    else
    {
        queue->rear++;
        strcpy((char*)queue->data[queue->rear],(char*)value);
        queue->level++;
    }
    return 0;
}
uint8_t* APP_QUEUE_DeQueue(app_queue* queue){
    uint8_t* data = queue->data[queue->front];
    if (queue->front == -1)
    {
        queue->state = Queue_Empty;
        return 0;
    }
    if (queue->front == queue->rear)
    {
        queue->front = -1;
        queue->rear = -1;
    }
    else if (queue->front == queue->size-1)
    {
        queue->front = 0;
    }
    else
    {
        queue->front++;
    }
    queue->level--;
    return data;
}
int8_t APP_QUEUE_CheckFull(app_queue* queue){
    if(queue->level == queue->size)
    {
        queue->state = Queue_Full;
        return 1;
    }
    return 0;
}
