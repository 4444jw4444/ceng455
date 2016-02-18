/* ###################################################################
**     Filename    : os_tasks.h
**     Project     : lab2
**     Processor   : MK64FN1M0VLL12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-02-03, 16:17, # CodeGen: 1
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         serial_task - void serial_task(os_task_param_t task_init_data);
**
** ###################################################################*/
/*!
** @file os_tasks.h
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup os_tasks_module os_tasks module documentation
**  @{
*/         

#ifndef __os_tasks_H
#define __os_tasks_H
/* MODULE os_tasks */

#include "fsl_device_registers.h"
#include "clockMan1.h"
#include "pin_init.h"
#include "osa1.h"
#include "mqx_ksdk.h"
#include "uart1.h"
#include "fsl_mpu1.h"
#include "fsl_hwtimer1.h"
#include "MainTask.h"
#include "SerialTask.h"
#include "myUART.h"
#include "HandlerTask.h"
#include <message.h>
#ifdef __cplusplus
extern "C" {
#endif 

//Global Queues
#define HANDLER_INPUT_QUEUE 1
//#define HANDLER_OUTPUT_QUEUE 2

//Message Pool Definitions
/* This structure contains a data field and a message header structure */
typedef struct interrupt_message
{
   MESSAGE_HEADER_STRUCT   HEADER;
   unsigned char           CHARACTER;
} INTERRUPT_MESSAGE, * INTERRUPT_MESSAGE_PTR;

//Message Pool
extern _pool_id interrupt_message_pool;

//Initial number of messages in the pool
#define INITIAL_POOL_SIZE 5


/*
** ===================================================================
**     Callback    : serial_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void serial_task(os_task_param_t task_init_data);


/*
** ===================================================================
**     Callback    : RunHandler
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void RunHandler(os_task_param_t task_init_data);

/* END os_tasks */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

#endif 
/* ifndef __os_tasks_H*/
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
