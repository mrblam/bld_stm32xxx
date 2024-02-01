/*
 * app_queue.h
 *
 *  Created on: Jan 31, 2024
 *      Author: hoanpx
 */

#ifndef INC_APP_APP_QUEUE_H_
#define INC_APP_APP_QUEUE_H_

/*******************************************************/
typedef enum {
    Queue_Empty = 0,
    Queue_Full = 1,
    Queue_Available = 2,
    Queue_State_default = 0xff
}Queue_State;
typedef struct app_queue {
    int8_t front;
    int8_t rear;
    uint8_t size;
    uint8_t** data;
    uint8_t level;
    Queue_State  state;
}app_queue;
/*******************************************************/
/*
 * @brief
 * @param
 * @param
 * @retval
 */
int8_t APP_QUEUE_Init(app_queue* queue);

/*
 * @brief
 * @param
 * @param
 * @retval
 */
int8_t APP_QUEUE_CheckFull(app_queue* queue);

/*
 * @brief
 * @param
 * @param
 * @retval
 */
int8_t APP_QUEUE_EnQueue(app_queue* queue,uint8_t* value);

/*
 * @brief
 * @param
 * @param
 * @retval
 */
uint8_t* APP_QUEUE_DeQueue(app_queue* queue);

#endif /* INC_APP_APP_QUEUE_H_ */
