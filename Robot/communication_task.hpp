#ifndef COMMUNICATION_TASK_HPP
#define COMMUNICATION_TASK_HPP

#ifdef __cplusplus


extern "C" {
#endif

#include "cmsis_os.h"
#include "struct_typedef.h"
void commuication_task_start(void);

const uint32_t* get_brightnessPrt();
const int32_t* get_servo_anglePrt();
const int32_t* get_thruster_cmdPrt();
const int32_t* get_crawler_cmdPrt();
const int32_t* get_jet_servo_anglePrt();
const int32_t* get_pt_anglesPrt();

#ifdef __cplusplus
}
#endif


#include "servo_task.hpp"
#include "led_task.hpp"
#include "thruster_task.hpp"
#include "crawler_task.hpp"
#include "jet_servo_task.hpp"
#include "pt_task.hpp"
#include <string>

bool publish(const char* topic, const char* payload, uint32_t payload_len);

#endif /* COMMUNICATION_TASK_HPP */
