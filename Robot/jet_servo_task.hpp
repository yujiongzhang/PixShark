#ifndef _JET_SERVO_TASK_HPP_
#define _JET_SERVO_TASK_HPP_

#ifdef __cplusplus


extern "C" {
#endif

#include "cmsis_os.h"
#include "struct_typedef.h"
void jet_servo_task_start(void);

extern osThreadId jet_servo_task_handle;

#ifdef __cplusplus
}
#endif



#endif /* _JET_SERVO_TASK_HPP_ */