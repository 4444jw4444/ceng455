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



void myUART_RxCallback(uint32_t instance, void * uartState)
{
	//UART_DRV_SendData(myUART_IDX, myRxBuff, sizeof(myRxBuff));

	// Allocate an interrupt message
	InterruptMessagePtr interruptMessage = (InterruptMessagePtr)_msg_alloc(g_InterruptMessagePool);
	if (interruptMessage == NULL) {
	 _task_block();
	}

	interruptMessage->HEADER.TARGET_QID = _msgq_get_id(0, HANDLER_INTERRUPT_QUEUE_ID);
	interruptMessage->HEADER.SIZE = sizeof(InterruptMessage);
	interruptMessage->character = myRxBuff[0];

	// Send the message to the handler
	bool result = _msgq_send(interruptMessage);
	if (result != TRUE) {
		_task_block();
	}
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
