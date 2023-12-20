#ifndef _PT_TASK_HPP_
#define _PT_TASK_HPP_

#ifdef __cplusplus


extern "C" {
#endif

#include "cmsis_os.h"
#include "struct_typedef.h"
void pt_task_start(void);
extern osThreadId pt_task_handle;


#ifdef __cplusplus
}
#endif

#endif /* _PT_TASK_HPP_ */
