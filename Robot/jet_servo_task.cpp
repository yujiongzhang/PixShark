/**
  ****************************(C) COPYRIGHT 2022 ZJU****************************
  * @file       servo_task.cpp
  * @brief      servo控制任务         
  * @note       
  * @history
  *  Version    Date            Author                Modification
  *  V1.0.0     OCT-7-2023     HaoLion(郝亮亮) zyj       1. done
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2022 ZJU****************************
  */

#include "jet_servo_task.hpp"
#include "servo.hpp"
#include "RobotConfig.h"
#include "communication_task.hpp"

#include "bsp_can.h"

osThreadId jet_servo_task_handle;

static AUSServo_Can jet_aus(Jet_Servo_ID, 0x00);

static void jet_servo_task(void const * argument)
{
    Servo* jet_servo = &jet_aus;

    //初始化can
    can_filter_init();

    const int32_t* jet_servo_dataPtr = NULL;
    
    jet_servo_dataPtr = get_jet_servo_anglePrt();
    for(;;)
    {
        //等待控制指令
        while (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) != pdPASS)
        {
        }
        jet_servo->set_servo_angle(*jet_servo_dataPtr);
    }
}


const osThreadDef_t os_thread_def_jet_servo = {
    .name = (char*)"jet_servo",
    .pthread = jet_servo_task,
    .tpriority = osPriorityNormal,
    .instances = 0,
    .stacksize = 128
};

void jet_servo_task_start(void)
{
    jet_servo_task_handle = osThreadCreate(&os_thread_def_jet_servo, NULL);
}