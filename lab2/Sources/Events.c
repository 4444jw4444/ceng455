/* ###################################################################
**     Filename    : Events.c
**     Project     : lab2
**     Processor   : MK64FN1M0VLL12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-02-03, 15:57, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */

/*
** ===================================================================
**     Callback    : myUART_RxCallback
**     Description : This callback occurs when data are received.
**     Parameters  :
**       instance - The UART instance number.
**       uartState - A pointer to the UART driver state structure
**       memory.
**     Returns : Nothing
** ===================================================================
*/

_pool_id interrupt_message_pool;

void myUART_RxCallback(uint32_t instance, void * uartState)
{
  /* Write your code here ... */
	//UART_DRV_SendData(myUART_IDX, myRxBuff, sizeof(myRxBuff));
	//allocating a message

	printf("RxCallback received char: %c \r\n", myRxBuff[0]);

	/*allocate a message*/
	INTERRUPT_MESSAGE_PTR msg_ptr = (INTERRUPT_MESSAGE_PTR)_msg_alloc(interrupt_message_pool);

	if (msg_ptr == NULL) {
	 printf("\nCould not allocate a message\r\n");
	 _task_block();
	}

	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, HANDLER_INPUT_QUEUE);
	msg_ptr->HEADER.SIZE = sizeof(INTERRUPT_MESSAGE);
	msg_ptr->CHARACTER = myRxBuff[0];

	bool result = _msgq_send(msg_ptr);

	if (result != TRUE) {
	 printf("\nCould not send a message \r\n");
	 _task_block();
	}
	return;
}

/* END Events */

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
