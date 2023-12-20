#ifndef _SERVO_HPP_
#define _SERVO_HPP_


#include "struct_typedef.h"




class Servo
{
public:
    Servo(){};
    virtual ~Servo(){};
    virtual void write(void * data, int len) = 0;
    virtual void set_servo_angle(int16_t angle_set) = 0;
    virtual int16_t get_servo_angle() = 0;

    //考虑到自研水下云台 可进行水平方向的旋转，新增加 这两个 horizontal 的函数，普通单轴的舵机无需重写
    virtual void set_servo_angle_horizontal(int16_t angle_set){ };
    virtual int16_t get_servo_angle_horizontal(){return 0;};
private:

};

/**
 * 霸勒斯舵机，485通讯
*/
class AUSServo : public Servo
{
public:
    AUSServo(uint8_t id);
    ~AUSServo(){};

    virtual void write(void * data, int len);
    /**
     * @brief 设置舵机控制角度，-180~180为单圈，在-180~180外，函数内部调用set_servo_angle_multi_turn
     * @param[in] angle_set:  舵机角度
     * @retval none
    */
    void set_servo_angle(int16_t angle_set);
    /**
     * @brief 设置舵机控制角度，-180~180为单圈，在-180~180外，函数内部调用set_servo_angle_multi_turn
     * @param[in] angle_set:  舵机角度
     * @param[in] time_ms:    舵机运行时间
     * @retval none
    */
    void set_servo_angle(int16_t angle_set, uint16_t time_ms);
    /**
     * @brief 设置舵机控制角度(多圈)
     * @param[in] angle_set:  舵机角度
     * @retval none
    */
    void set_servo_angle_multi_turn(int angle_set);
    /**
     * @brief 设置舵机控制角度(多圈)
     * @param[in] angle_set:  舵机角度
     * @param[in] time_ms:    舵机运行时间
     * @retval none
    */
    void set_servo_angle_multi_turn(int angle_set, unsigned int time_ms);


    /**
     * @brief 获取舵机角度
     * @retval none
    */
    int16_t get_servo_angle(void);

    /**
     * @brief 修改舵机ID
     * @param[in] new_id:     舵机新ID
     * @retval none
    */
    void modify_servo_id(uint8_t new_id);
    /**
     * @brief          计算前n字节累加校验和
     * @param[in]      pch_message: 数据
     * @param[in]      dw_length: 数据和校验的长度
     * @retval         计算完的校验和
     */
    uint8_t get_check_sum(uint8_t *pch_message,uint32_t dw_length);
    /**
    * @brief     发送内容打包
    * @param[in] cmd_type:  命令内容ID
    * @param[in] *p_data: 数据段
    * @param[in] len:     数据段长度
    * @retval			返回要发送的数据大小
    */
    uint16_t send_servo_pack(uint8_t cmd_type, uint8_t *p_data, uint8_t len);

public:
    uint8_t servo_id;
    static constexpr auto HEAD_SOF = 0x124C; //帧头
    uint8_t servo_tx_buffer[20];
    typedef enum
    {
        Ping_id = 0x01,      //检测舵机通讯
        Angle_set_id = 0x08, //舵机角度控制
        Angle_get_id = 0x0A, //舵机角度获取
        Modify_Id_id = 0x04, //修改舵机ID
        Angle_set_multi_id = 0x0D,//舵机角度控制 多圈
    }servo_cmd_e;
};

/**
 * 舵机，加了can转485，所以是can通讯的
*/
class AUSServo_Can : public AUSServo
{
public:
    AUSServo_Can(int _can_id, uint8_t _servo_id);
    ~AUSServo_Can(){};

    virtual void write(void * data, int len);

private:
    int CAN_ID;

};



/**
 * 自研水下云台，485通讯
 * 比普通舵机多了横向旋转，基类中的 set_servo_angle、get_servo_angle 都针对【和原先单轴云台相同的】俯仰角方向的旋转。
 * 新增 set_servo_angle_horizontal、get_servo_angle_horizontal 来针对 水平方向的旋转
*/
class UnderWater_PT : public Servo
{
private:
    typedef enum
    {
        pitch_angle_set_id = 0x4D, //俯仰 角度控制
        horizontal_angle_set_id = 0x4B, //水平 角度控制
        pitch_angle_get_id = 0x53, //俯仰 角度获取
        horizontal_angle_get_id = 0x51 // 水平 角度获取
    }PT_cmd_e;
public:
    UnderWater_PT(/* args */);
    ~UnderWater_PT();

    virtual void write(void * data, int len);
    
    /**
     * @brief 设置俯仰角 控制角度，范围 -180~180
     * @param[in] angle_set:  舵机角度
     * @retval none
    */
    virtual void set_servo_angle(int16_t angle_set);
    virtual int16_t get_servo_angle();

    /**
     * @brief 设置水平面角度 控制角度，范围 -180~180
     * @param[in] angle_set:  舵机角度
     * @retval none
    */
    void set_servo_angle_horizontal(int16_t angle_set);
    int16_t get_servo_angle_horizontal();

        /**
    * @brief     发送内容打包
    * @param[in] cmd_type:  命令内容ID
    * @param[in] *p_data: 数据段
    * @param[in] len:     数据段长度
    * @retval			返回要发送的数据大小
    */
    uint16_t send_PT_pack(uint8_t cmd_type, uint8_t *p_data, uint8_t len);

    uint8_t get_check_sum(uint8_t *pch_message,uint32_t dw_length);

public:
    uint8_t PT_tx_buffer[20];

};






#endif /* _SERVO_HPP_ */
