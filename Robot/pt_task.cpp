/** 
 *****************************Copyright (c) 2023  ZJU****************************
 * @file      : pt_task.cpp
 * @brief     : 履带电机执行任务
 * @history   :
 *  Version     Date            Author          Note
 *  V1.0.0    2023-12-19         zyj          1. <note>
 *******************************************************************************
 * @verbatim :
 *==============================================================================
 *                                                                              
 *                                                                              
 *==============================================================================
 * @endverbatim :
 *****************************Copyright (c) 2023  ZJU****************************
 */


#include "pt_task.hpp"
#include "servo.hpp"
#include "RobotConfig.h"
#include "communication_task.hpp"

osThreadId pt_task_handle;

static UnderWater_PT underwater_pt;

/**
 * @brief          双轴云台执行任务
 * @param[in]      argument: Not used
 * @retval         None
*/
static void pt_task(void const * argument)
{
    Servo* servo_pt = &underwater_pt;
    usart2_tx_dma_init();
    const int32_t* pt_datasPtr = NULL;
    
    pt_datasPtr = get_pt_anglesPrt();
    for(;;)
    {
        //等待控制指令
        while (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) != pdPASS)
        {
        }
        
        servo_pt->set_servo_angle(pt_datasPtr[0]);
        osDelay(5);//防止串口发的太快，前面的数据被后面的覆盖了
        servo_pt->set_servo_angle_horizontal(pt_datasPtr[1]);
    }
}



const osThreadDef_t os_thread_def_pt = {
    .name = (char*)"pt",
    .pthread = pt_task,
    .tpriority = osPriorityAboveNormal,
    .instances = 0,
    .stacksize = 256
};

void pt_task_start(void)
{
    pt_task_handle = osThreadCreate(&os_thread_def_pt, NULL);
}
