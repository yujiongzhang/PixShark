/** 
 *****************************Copyright (c) 2023  ZJU****************************
 * @file      : communication_task.cpp
 * @brief     : 
 * @history   :
 *  Version     Date            Author          Note
 *  V1.0.0    2023-11-21       Hao Lion        1. <note>
 *******************************************************************************
 * @verbatim :
 *==============================================================================
 *                                                                              
 *                                                                              
 *==============================================================================
 * @endverbatim :
 *****************************Copyright (c) 2023  ZJU****************************
 */
#include "CJsonObject.hpp"
#include "communication_task.hpp"
#include "mqtt_client.hpp"
#include "RobotConfig.h"
#include "freertos_inc.h"

static osThreadId commuication_task_handle;
static std::string recv_topic;
static std::string pay_load;
static mqtt::Client* com_nodePtr = NULL;



static uint32 brightness;
static int32 servo_angle;
static int32 thruster_cmd[6] = {100};
static int32 crawler_cmd[2] = {0};
static int32 jet_servo_angle;
static int32 pt_angles_cmd[2] = {0};


using namespace std;

static void on_message(void *this_client, const char *topic, u32_t tot_len)
{
    recv_topic = std::string(topic);
}

static void on_data(void *this_client, const u8_t *data, u16_t len, u8_t flags)
{
    
    pay_load = std::string((const char*)data, len);
    
    xTaskNotifyGive(commuication_task_handle);
}

void communication_task(const void *arg)
{
    //初始化mqtt
    com_nodePtr = new mqtt::Client(ROBOT_NAME);
    com_nodePtr->on_message_ = on_message;
    com_nodePtr->on_data_ = on_data;

    com_nodePtr->connect(MQTT_SERVER_IP);

    // com_nodePtr->subscribe(LED_TOPIC);
    // com_nodePtr->subscribe("/crawler");
    // com_nodePtr->subscribe(SERVO_TOPIC);
    // com_nodePtr->subscribe(THRUSTER_TOPIC);
    com_nodePtr->subscribe(CRAWLER_TOPIC);

    com_nodePtr->subscribe(JET_SERVO_TOPIC);
    com_nodePtr->subscribe(PT_ANGLES_TOPIC);

    for(;;)
    {
        //等待控制指令
        while (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) != pdPASS)//
        {
        }
        neb::CJsonObject result(pay_load);
        
        if(recv_topic.compare(LED_TOPIC) == 0){
            result.Get("brightness", brightness);
            xTaskNotifyGive(led_task_handle);
        }
            
        else if(recv_topic.compare(SERVO_TOPIC) == 0){
            result.Get("servo_angle", servo_angle);
            xTaskNotifyGive(servo_task_handle);
        }
            
        else if(recv_topic.compare(THRUSTER_TOPIC) == 0)
        {
            result.Get("vl", thruster_cmd[0]);
            result.Get("vr", thruster_cmd[1]);
            result.Get("htl", thruster_cmd[2]);
            result.Get("htr", thruster_cmd[3]);
            result.Get("hbr", thruster_cmd[4]);
            result.Get("hbl", thruster_cmd[5]);
        }

        else if(recv_topic.compare(CRAWLER_TOPIC) == 0)
        {
            result.Get("left", crawler_cmd[0]);
            result.Get("right", crawler_cmd[1]);
            xTaskNotifyGive(crawler_task_handle);
        }

        else if(recv_topic.compare(JET_SERVO_TOPIC) == 0)
        {
            result.Get("jet_servo_angle", jet_servo_angle);
            xTaskNotifyGive(jet_servo_task_handle);
        }

        else if(recv_topic.compare(PT_ANGLES_TOPIC) == 0)
        {
            result.Get("pitch", pt_angles_cmd[0]);
            result.Get("horizontal", pt_angles_cmd[1]);
            xTaskNotifyGive(pt_task_handle);
        }

        
    }
}


const uint32_t* get_brightnessPrt() 
{
    return &brightness;

}

const int32_t* get_servo_anglePrt() 
{
    return &servo_angle;
}

const int32_t* get_thruster_cmdPrt()
{
    return thruster_cmd;
}

const int32_t *get_crawler_cmdPrt()
{
    return crawler_cmd;
}

const int32_t *get_jet_servo_anglePrt()
{
    return &jet_servo_angle;
}

const int32_t *get_pt_anglesPrt()
{
    return pt_angles_cmd;
}

bool publish(const char* topic, const char* payload, uint32_t payload_len)
{
    return com_nodePtr->publish(topic, payload, payload_len);
}

const osThreadDef_t os_thread_def_commuication = {
    .name = (char*)"commuication",
    .pthread = communication_task,
    .tpriority = osPriorityAboveNormal,
    .instances = 0,
    .stacksize = 512
};

void commuication_task_start(void)
{
    commuication_task_handle = osThreadCreate(&os_thread_def_commuication, NULL);
    
}