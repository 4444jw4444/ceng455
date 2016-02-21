#include <stdio.h>
#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"
#include "handler.c"

#define KEY_BS 0x08
#define KEY_CR 0x0D
#define KEY_LF 0x0A
#define KEY_CTRL_U 0x15
#define KEY_CTRL_W 0x17
#define KEY_SPACE 0x20

#ifdef __cplusplus
extern "C" {
#endif 

_pool_id interrupt_message_pool;

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

// Handler Globals
_pool_id g_InterruptMessagePool;
_pool_id g_SerialMessagePool;
HandlerPtr g_Handler;
MUTEX_STRUCT g_HandlerMutex;

/*
** ===================================================================
**     Callback    : RunHandler
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/

void _initializeMessagePools(){
	// Initialize interrupt message pool
	g_InterruptMessagePool = _msgpool_create(sizeof(InterruptMessage),
			INTERRUPT_MESSAGE_POOL_INITIAL_SIZE,
			INTERRUPT_MESSAGE_POOL_GROWTH_RATE,
			INTERRUPT_MESSAGE_POOL_MAX_SIZE);
	if(g_InterruptMessagePool == MSGPOOL_NULL_POOL_ID){
		printf("Failed to create the interrupt message pool.\n");
		_task_block();
	}

	// Initialize serial message pool
	g_SerialMessagePool = _msgpool_create(sizeof(SerialMessage),
			SERIAL_MESSAGE_POOL_INITIAL_SIZE,
			SERIAL_MESSAGE_POOL_GROWTH_RATE,
			SERIAL_MESSAGE_POOL_MAX_SIZE);
	if(g_SerialMessagePool == MSGPOOL_NULL_POOL_ID){
		printf("Failed to create the serial message pool.\n");
		_task_block();
	}
}

_queue_id _initializeQueue(int queueNum){
	// Initialize interrupt queue
	_queue_id queueId = _msgq_open(queueNum, 0);
	if(queueId == 0){
		printf("Failed to open queue %d.\n", queueNum);
		_task_block();
	}
	return queueId;
}

void RunHandler(os_task_param_t task_init_data)
{
	printf("Handler task started.\n");

	// Initialize queues and message pools
	_queue_id interruptQueue = _initializeQueue(HANDLER_INTERRUPT_QUEUE_ID);
	_queue_id inputQueue = _initializeQueue(HANDLER_INPUT_QUEUE_ID);
	_initializeMessagePools();

	// Initialize Handler
	Handler handler;
	g_Handler = &handler;
	_initializeHandlerMutex(&g_HandlerMutex);
	_initializeHandler(g_Handler, interruptQueue, inputQueue, myUART_IDX);

#ifdef PEX_USE_RTOS
  while (1) {
#endif
    
	  	InterruptMessagePtr interruptMessage = _msgq_receive(interruptQueue, 0);

		if (interruptMessage == NULL) {
		   printf("Handler failed to receive an interrupt message.\n");
		   _task_block();
		}
		unsigned char inputChar = interruptMessage->character;
		_handleCharacterInput(inputChar, g_Handler);
		_msg_free(interruptMessage);

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
