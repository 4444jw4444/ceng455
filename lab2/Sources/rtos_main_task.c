/* ###################################################################
**     Filename    : rtos_main_task.c
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
**         main_task - void main_task(os_task_param_t task_init_data);
**
** ###################################################################*/
/*!
** @file rtos_main_task.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup rtos_main_task_module rtos_main_task module documentation
**  @{
*/         
/* MODULE rtos_main_task */

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"


#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */
//#include "handler.h"
//#include <stdio.h>
//#include <mqx.h>
//#include <bsp.h>
//
//#define USER_TASK_ONE 12345
//#define USER_TASK_TWO 2123
//#define USER_TASK_THREE 31234
//
//extern void user_task(uint32_t);
//
//const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
//{
//  /*Task Index,Function,Stack,Priority,Name,Attributes,Param,Time Slice*/
//    { USER_TASK_ONE, user_task, 700, 17U, "ut1", 1, 0, 0},
//    { USER_TASK_TWO, user_task, 700, 16U, "ut2", 2, 0, 0},
//    { USER_TASK_THREE, user_task, 700, 16U, "ut3", 3, 0, 0},
//	{0}
//};
//
//void user_task(uint32_t initial_data){
//	printf("%i: \n", initial_data);
//	if(initial_data == 1){
//		printf("User Task 1 Created\n");
//
//		//_task_block();
//	}else if(initial_data == 2){
//		printf("User Task 2 Created\n");
//
//		//_task_block();
//	}else if(initial_data == 3){
//		printf("User Task 3 Created\n");
//
//		//_task_block();
//	}
//	_task_block();
//}

/* Initialization of Processor Expert components function prototype */
#ifdef MainTask_PEX_RTOS_COMPONENTS_INIT
extern void PEX_components_init(void);
#endif 

/*
** ===================================================================
**     Callback    : main_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void main_task(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
//	_task_id user_task_id = _task_create(0, USER_TASK_ONE, 1);
//	if (user_task_id == MQX_NULL_TASK_ID) {
//		printf ("\n Could not create user_task_1\n");
//	}
	//	  _task_create(0, USER_TASK_TWO, 0);
	//	  _task_create(0, USER_TASK_THREE, 0);
  /* Initialization of Processor Expert components (when some RTOS is active). DON'T REMOVE THIS CODE!!! */
#ifdef MainTask_PEX_RTOS_COMPONENTS_INIT
  PEX_components_init(); 
#endif 
  /* End of Processor Expert components initialization.  */


#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */





    
    OSA_TimeDelay(10);                 /* Example code (for task release) */
   
    
    
    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

/* END rtos_main_task */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

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
