#include <stdio.h>
#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */

/*
** ===================================================================
**     Callback    : serial_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void serial_task(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
  printf("SerialTask Created!\n\r");

  char buf[50];
  sprintf(buf, "\r\nType here: \r\n");
  UART_DRV_SendDataBlocking(myUART_IDX, buf, sizeof(buf), 1000);


#ifdef PEX_USE_RTOS
  while (1) {
#endif

	  OSA_TimeDelay(10);

#ifdef PEX_USE_RTOS   
  }
#endif    
}

/*
** ===================================================================
**     Callback    : RunHandler
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/

_pool_id interrupt_message_pool;

void RunHandler(os_task_param_t task_init_data)
{
	printf("Handler task started.\r\n");

	_queue_id          input_qid;
	input_qid = _msgq_open(HANDLER_INPUT_QUEUE, 0);

	if (input_qid == 0) {
	      printf("\nCould not open the server message queue\n");
	      _task_block();
	}

	 /* create a message pool */
	interrupt_message_pool = _msgpool_create(sizeof(INTERRUPT_MESSAGE), INITIAL_POOL_SIZE, 1, 10);

	//check that the pool was created
	if (interrupt_message_pool == MSGPOOL_NULL_POOL_ID) {
		printf("\nCount not create a message pool\n");
		_task_block();
	}


#ifdef PEX_USE_RTOS
  while (1) {
#endif
    
	  	INTERRUPT_MESSAGE_PTR msg_ptr = _msgq_receive(input_qid, 0);

		if (msg_ptr == NULL) {
		   printf("\nCould not receive a message\n");
		   _task_block();
		}

		UART_DRV_SendData(myUART_IDX, msg_ptr->CHARACTER, sizeof(msg_ptr->CHARACTER));
		//printf(" %c \n", msg_ptr->CHARACTER);


#ifdef PEX_USE_RTOS   
  }
#endif    
}

/* END os_tasks */

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
