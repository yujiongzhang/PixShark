/** 
 *****************************Copyright (c) 2023  ZJU****************************
 * @file      : main.cpp
 * @brief     : 用户自定义主函数，会被freertos调用，无需关心原来的c文件，专注上层即可
 * @history   :
 *  Version     Date            Author          Note
 *  V1.0.0    2023-10-01       Hao Lion        1. <note>
 *******************************************************************************
 * @verbatim :
 *==============================================================================
 *                                                                              
 *                                                                              
 *==============================================================================
 * @endverbatim :
 *****************************Copyright (c) 2023  ZJU****************************
 */

#include "common_inc.h"

char s[][120] ={
    " ╔=============================================================================╗",
    " ║                         申 昊 科 技  -- 浙 江 大 学      🚢🚢🚢             ║",
    " ║                            _          _                _                    ║",
    " ║                           (_)        | |              | |                   ║",
    " ║                      _ __  ___  _____| |__   __ _ _ __| | __                ║",
    " ║                     | '_ \\| \\ \\/ / __| '_ \\ / _` | '__| |/ /                ║",
    " ║                     | |_) | |>  <\\__ | | | | (_| | |  |   <                 ║",
    " ║                     | .__/|_/_/\\_|___|_| |_|\\__,_|_|  |_|\\_\\                ║",
    " ║                     | |                                                     ║",   
    " ║                     |_|                                                     ║",
    " ╠======╦=========================================╦============================╣",
    " ║      ║                                         ║    🔧 设备设置             ║",
    " ║  配  ║  📞 通讯方式 :                          ╟- - - - - - - - - - - - - - ╢",
    " ║      ║                                         ║ 深度计频率 :               ║",
    " ║  置  ║  🦾 推进器数量 :                        ║ 推进器最大转速 :           ║",
    " ║      ║     履带数量 :                          ║ 履带电机回传频率 :         ║",
    " ╠======╩=========================================╩============================╣",
    " ║  机器人名称🪪:                                                               ║",
    " ║  机器人类型：                                                               ║",
    " ╚=============================================================================╝"
    };

void printf_logo(void)
{
    

    strncpy(s[12]+36, "TCP/IP", 6);
    strncpy(s[13]+80, "100", 3);
    strncpy(s[14]+38, "6", 1);
    strncpy(s[14]+88, "3000", 4);
    strncpy(s[15]+34, "2", 1);
    strncpy(s[15]+90, "100", 3);
    strncpy(s[17]+28, "YellowRobot", 11);
    strncpy(s[18]+27, "ROV", 3);
    for (int i=0; i<20; i++) {
        osDelay(100);
    }

}
                             

                                                                                       

/* Default Entry -------------------------------------------------------*/
void Main(void)
{
    {
      cJSON_Hooks hooks;
      hooks.malloc_fn = pvPortMalloc;
      hooks.free_fn = vPortFree;
      cJSON_InitHooks(&hooks);
    }/*初始化CJSON库的钩子函数，调用FreeRTOS的内存管理接口*/
    //初始化检测线程
    detetc_task_start();
    // Init all communication staff, including USB-VCP/UART/TCP etc.
    commuication_task_start();

    cabin_info_task_start();
    // imu_task_start();
    // altimeter_task_start();
    // depth_task_start();
    // led_task_start();
    // thruster_task_start();
    // servo_task_start();
    jet_servo_task_start();

    crawler_task_start();
    pt_task_start();


    // while (1)
    // {
    //     robot_printf("Hello World!\n");
    //     osDelay(1000);
    // }
    /**如果通信连接成功，打印logo*/
    printf_logo();
}
