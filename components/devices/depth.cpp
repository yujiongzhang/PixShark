/** 
 *****************************Copyright (c) 2023  ZJU****************************
 * @file      : depth.cpp
 * @brief     : 深度计设备驱动文件
 * @history   :
 *  Version     Date            Author          Note
 *  V1.0.0    2023-08-14         WPJ        1. <note>
 *  V1.0.1    2023-9-30          Hao Lion   1. 重构为c++类
 *******************************************************************************
 * @verbatim :
 *==============================================================================
 * 深度计的数据读取————————中断读取                                                                             
 * 深度计数据解析——————————需要发送指令才能得到回传                                                   
 *==============================================================================
 * @endverbatim :
 *****************************Copyright (c) 2023  ZJU****************************
 */


#include "depth.hpp"
#include "bsp_usart.h"
#include "DeviceConfig.h"
#include "CRC8_CRC16.h"
#include "common.hpp"


/**
 * @brief 深度计基类的构造函数，初始化depth_data
 * @retval none
*/
Depth::Depth(){
    memset(&depth_data,0,sizeof(depth_data));
}



void KELLER::write(void * data, uint16_t len)
{
   bsp_usart_transmit(DEPTH_UART_CHANNEL, (uint8_t *)data, len);
}


/**
 * @brief          深度计协议解析，直接读取压力值，不进行modbus协议解析
 * @param[in]      depth_frame: 原生数据指针
 * @return  	   none
*/
void KELLER::depth_data_solve(volatile const uint8_t *depth_frame)
{
  static int i = 5;
  static float sum = 0;
  float pressure1 = 0;
	/*数据校验*/
    if (depth_frame == NULL)
    {
        return;
    }
    //TODO:进行深度计数据解析
    else if (depth_frame[0] == 0x01 && depth_frame[1] == 0x03 &&  depth_frame[2] == 0x04 && verify_CRC16_check_sum((uint8_t *)depth_frame, 9))
    {
      if(!restart_flag) // 深度前5次读取的气压值求平均作为大气压参考值
      {
        //深度计发上来的为大端存储
        sum += Byte_to_Float((uint8_t *)&depth_frame[3], true);
        i--;
        if (i == 0)
        {
          restart_flag = true;
          atmosphere = sum/5.0f;
        }
      }
      else
      {
        pressure1 = Byte_to_Float((uint8_t *)&depth_frame[3], true);
        depth_data = (pressure1 - atmosphere) * 100000.0f / (g * WATER_DENSITY);
      }
    }
}

/**
 * @brief          深度计协议解析，采用状态机方式解析modbus，比较规范
 * @param[in]      none
 * @return  	   none
*/
void KELLER::depth_data_solve(void)
{
  
}

/**
 * @brief          读取深度计寄存器
 * @param[in]      StAdd: 寄存器地址
 * @param[in]      len: 需要读取的寄存器个数
 * @return  	   none
*/
void KELLER::read_depth_register(uint16_t StAdd, uint8_t reg_len)
{
    uint8_t byte_len = reg_len * 2; //一个寄存器两个字节
    depth_tx_buf[0] = this->dev_addr_;
    depth_tx_buf[1] = Register_Read;
    depth_tx_buf[2] = StAdd >> 8;
    depth_tx_buf[3] = StAdd & 0xFF;
    depth_tx_buf[4] = byte_len >> 8;
    depth_tx_buf[5] = byte_len & 0xFF;
    append_CRC16_check_sum(depth_tx_buf, 8);
    write(depth_tx_buf, sizeof(depth_tx_buf));

}

/**
 * @brief          写入深度计单个寄存器
 * @param[in]      StAdd: 寄存器地址
 * @param[in]      data: 数据
*/
void KELLER::write_depth_single_register(uint16_t StAdd, uint16_t data)
{
    depth_tx_buf[0] = this->dev_addr_;
    depth_tx_buf[1] = Single_Register_Write;
    depth_tx_buf[2] = StAdd >> 8;
    depth_tx_buf[3] = StAdd & 0xFF;
    depth_tx_buf[4] = data >> 8;
    depth_tx_buf[5] = data & 0xFF;
    append_CRC16_check_sum(depth_tx_buf, 8);
    write(depth_tx_buf, sizeof(depth_tx_buf));
}

/**
 * @brief          请求读取一次压力值
 * @param[in]      none
 * @return  	   none
*/
void KELLER::request_depth_data(void)
{
  //读取P1传感器的压力值，P2实测有问题，所以未作处理
  read_depth_register(P1, 1);
}
