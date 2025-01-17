/** 
 *****************************Copyright (c) 2023  ZJU****************************
 * @file      : servo.cpp
 * @brief     : 舵机设备驱动文件
 * @history   :
 *  Version     Date            Author          Note
 *  V1.0.0    2023-08-12       Hao Lion        1. <note>
 *******************************************************************************
 * @verbatim :
 *==============================================================================
 *                                                                              
 *                                                                              
 *==============================================================================
 * @endverbatim :
 *****************************Copyright (c) 2023  ZJU****************************
 */

#include "servo.hpp"
#include "bsp_usart.h"
#include "DeviceConfig.h"

#include "bsp_can.h"
#include <assert.h>



AUSServo::AUSServo(uint8_t id = 0x00)
{
    this->servo_id = id;
}

void AUSServo::write(void * data, int len)
{
    //TODO: 底层通讯方式
    bsp_usart_transmit(SERVO_UART_CHANNEL, (uint8_t *)data, len);
    
}

/**
  * @brief          计算前n字节累加校验和
  * @param[in]      pch_message: 数据
  * @param[in]      dw_length: 数据和校验的长度
  * @retval         计算完的校验和
  */
uint8_t AUSServo::get_check_sum(uint8_t *pch_message,uint32_t dw_length)
{
   uint8_t check_sum = 0;
    if (pch_message == NULL)
    {
        return 0xFF;
    }
    while(--dw_length)
    {
        check_sum += *pch_message++;
    }
    return check_sum % 256;
}

/**
* @brief     发送内容打包
* @param[in] cmd_type:  命令内容ID
* @param[in] *p_data: 数据段
* @param[in] len:     数据段长度
* @retval			返回要发送的数据大小
*/
uint16_t AUSServo::send_servo_pack(uint8_t cmd_type, uint8_t *p_data, uint8_t len)
{
    memset(servo_tx_buffer, 0, sizeof(servo_tx_buffer));
    uint16_t send_len = 5 + len;
    servo_tx_buffer[0] = HEAD_SOF >> 8;
    servo_tx_buffer[1] = HEAD_SOF & 0xFF;
    servo_tx_buffer[2] = cmd_type;
    servo_tx_buffer[3] = len;
    memcpy(&servo_tx_buffer[4], p_data, len);
    servo_tx_buffer[send_len - 1] =  get_check_sum(servo_tx_buffer, send_len);
    write(servo_tx_buffer, send_len);
    return send_len;
}

/**
 * @brief 设置舵机控制角度
 * @param[in] servo_id:   舵机ID
 * @param[in] angle_set:  舵机角度
 * @param[in] time_ms:    舵机运行时间
 * @retval none
*/
void AUSServo::set_servo_angle(int16_t angle_set, uint16_t time_ms)
{
    if(angle_set<=180 && angle_set > -180){
        //单圈模式
        uint8_t data[7];
        data[0] = servo_id;  
        data[1] = (angle_set*10) & 0xff;
        data[2] = (angle_set*10) >> 8;
        data[3] = time_ms & 0xff;
        data[4] = time_ms >> 8;
        data[5] = 0x00;
        data[6] = 0x00;

        send_servo_pack(Angle_set_id, data, sizeof(data));
    }
    else
    {
        //调用多圈模式的函数
        set_servo_angle_multi_turn((int)angle_set,(unsigned int)time_ms);
    }
    

}


/**
 * @brief 设置舵机控制角度
 * @param[in] servo_id:   舵机ID
 * @param[in] angle_set:  舵机角度
 * @retval none
*/
void AUSServo::set_servo_angle(int16_t angle_set)
{
    if(angle_set<=180 && angle_set > -180){
        //单圈模式
        uint16_t time_ms = 500;
        uint8_t data[7];
        data[0] = servo_id;  
        data[1] = (angle_set*10) & 0xff;
        data[2] = (angle_set*10) >> 8;
        data[3] = time_ms & 0xff;
        data[4] = time_ms >> 8;
        data[5] = 0x00;
        data[6] = 0x00;
        
        send_servo_pack(Angle_set_id, data, sizeof(data));
    }
    else
    {
        //调用多圈模式的函数
        set_servo_angle_multi_turn((int)angle_set);
    }
    
}


void AUSServo::set_servo_angle_multi_turn(int angle_set)
{
    unsigned int time_ms = 0;
    uint8_t data[11];
    data[0] = servo_id;
    //角度
    data[1] = (angle_set*10) & 0xff;
    data[2] = (angle_set*10) >> 8;
    data[3] = (angle_set*10) >> 16;;
    data[4] = (angle_set*10) >> 24;
    data[5] = time_ms & 0xff;
    data[6] = time_ms >> 8;
    data[7] = time_ms >> 16;  
    data[8] = time_ms >> 24;
    data[9] = 0;
    data[10] = 0;
    
    send_servo_pack(Angle_set_multi_id, data, sizeof(data));
}

void AUSServo::set_servo_angle_multi_turn(int angle_set, unsigned int time_ms)
{
    uint8_t data[11];
    data[0] = servo_id;
    //角度
    data[1] = (angle_set*10) & 0xff;
    data[2] = (angle_set*10) >> 8;
    data[3] = (angle_set*10) >> 16;;
    data[4] = (angle_set*10) >> 24;
    data[5] = time_ms & 0xff;
    data[6] = time_ms >> 8;
    data[7] = time_ms >> 16;  
    data[8] = time_ms >> 24;
    data[9] = 0;
    data[10] = 0;
    
    send_servo_pack(Angle_set_multi_id, data, sizeof(data));
}


/**
 * @brief 获取舵机角度
 * @param[in] servo_id:   舵机ID
 * @retval none
*/
int16_t AUSServo::get_servo_angle(void)
{
    uint8_t data[1];
    data[0] = servo_id;  
    send_servo_pack(Angle_get_id, data, sizeof(data));
    return  0;
}

/**
 * @brief 修改舵机ID
 * @param[in] servo_id:   舵机原来ID
 * @param[in] new_id:     舵机新ID
 * @retval none
*/
void AUSServo::modify_servo_id(uint8_t new_id)
{
    uint8_t data[3];
    data[0] = servo_id;  
    data[1] = 0x22; 
    data[2] = new_id; 
    servo_id = new_id;
    send_servo_pack(Modify_Id_id, data, sizeof(data));
}


AUSServo_Can::AUSServo_Can(int _can_id, uint8_t _servo_id)
{
    this->CAN_ID = _can_id;
    this->servo_id = _servo_id;
}

void AUSServo_Can::write(void *data, int len)
{
    // assert(len <= 8);
    // bsp_can_transmit(this->CAN_ID, (uint8_t *)data, len);
    uint8_t *data_temp =  (uint8_t *)data;
    int len_temp = len;
    while (len_temp > 8)
    {
        bsp_can_transmit(this->CAN_ID, data_temp, 8);
        len_temp = len_temp - 8;
        data_temp = data_temp + 8;
    }
    bsp_can_transmit(this->CAN_ID, data_temp, len_temp);
}

UnderWater_PT::UnderWater_PT()
{
}

UnderWater_PT::~UnderWater_PT()
{
}

void UnderWater_PT::write(void *data, int len)
{
    //TODO: 底层通讯方式
    bsp_usart_transmit(SERVO_UART_CHANNEL, (uint8_t *)data, len);
}

void UnderWater_PT::set_servo_angle(int16_t angle_set)
{
        uint8_t data[2];
        int16_t angle_set_data = 0; // 往设备具体发送的值
        if (angle_set >= 0)
        {
            angle_set_data = angle_set * 100;
        }
        else{
            angle_set_data = angle_set * 100 + 36000;
        }
        data[0] = angle_set_data >> 8;  // 数据高位
        data[1] = angle_set_data & 0xFF;// 数据低位
        send_PT_pack(pitch_angle_set_id, data, sizeof(data));
}

int16_t UnderWater_PT::get_servo_angle()
{
    uint8_t data[2];
    data[0] = 0;
    data[1] = 0;
    send_PT_pack(pitch_angle_get_id, data, sizeof(data));
    return  0;
}

void UnderWater_PT::set_servo_angle_horizontal(int16_t angle_set)
{
        uint8_t data[2];
        int16_t angle_set_data = 0; // 往设备具体发送的值
        if (angle_set >= 0)
        {
            angle_set_data = angle_set * 100;
        }
        else{
            angle_set_data = angle_set * 100 + 36000;
        }
        data[0] = angle_set_data >> 8;  // 数据高位
        data[1] = angle_set_data & 0xFF;// 数据低位
        send_PT_pack(horizontal_angle_set_id, data, sizeof(data));
}

int16_t UnderWater_PT::get_servo_angle_horizontal()
{
    uint8_t data[2];
    data[0] = 0;
    data[1] = 0;
    send_PT_pack(horizontal_angle_get_id, data, sizeof(data));
    return  0;
}

uint16_t UnderWater_PT::send_PT_pack(uint8_t cmd_type, uint8_t *p_data, uint8_t len)
{
    memset(PT_tx_buffer, 0, sizeof(PT_tx_buffer));
    uint16_t send_len = 5 + len;
    PT_tx_buffer[0] = 0xFF;
    PT_tx_buffer[1] = 0x01;
    PT_tx_buffer[2] = 0x00;
    PT_tx_buffer[3] = cmd_type;
    memcpy(&PT_tx_buffer[4], p_data, len);
    PT_tx_buffer[send_len - 1] =  get_check_sum(PT_tx_buffer, send_len);
    write(PT_tx_buffer, send_len);
    return send_len;
}

uint8_t UnderWater_PT::get_check_sum(uint8_t *pch_message, uint32_t dw_length)
{
    uint8_t check_sum = 0;
    if (pch_message == NULL)
    {
        return 0xFF;
    }
    // 最前面的0xFF不算，从第二个字节 加到 倒数第二个（共5个字节）
    for (uint8_t i = 1; i < dw_length-1; i++)
    {
        check_sum += pch_message[i];
    }
    return check_sum;
}

