#include <stdio.h>
#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"
#include "handler.c"
#include <stdbool.h>

#define KEY_BS 0x08
#define KEY_CR 0x0D
#define KEY_LF 0x0A
#define KEY_CTRL_U 0x15
#define KEY_CTRL_W 0x17
#define KEY_SPACE 0x20

#ifdef __cplusplus
extern "C" {
#endif 

/*=============================================================
                        GLOBAL VARIABLES
 ==============================================================*/

_pool_id g_InterruptMessagePool;	// A message pool for messages sent between from the UART event handler to the handler task
_pool_id g_SerialMessagePool;		// A message pool for messages sent between the handler task and its user tasks
HandlerPtr g_Handler;				// The global handler instance
MUTEX_STRUCT g_HandlerMutex;		// The mutex controlling access to the handler's internal state

/*=============================================================
                        HANDLER TASK
 ==============================================================*/

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
	printf("\r\nHandler task started.\r\n");

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

    // Wait for any incoming messages
	GenericMessagePtr receivedMessage = (GenericMessagePtr) _msgq_receive(MSGQ_ANY_QUEUE, 0);
	if (receivedMessage == NULL){
		   printf("Handler failed to receive a message.\n");
		   _task_block();
	}

	// Lock access to the handler while it processses the message
	if(_mutex_lock(&g_HandlerMutex) != MQX_OK){
		printf("Mutex lock failed.\n");
		_task_block();
	}

	// If the message is an serial message from the current writer, handle the writer input
	if(receivedMessage->HEADER.TARGET_QID == inputQueue){
		_handleWriteMessage((SerialMessagePtr) receivedMessage, g_Handler);
	}
	// If the message is an interrupt message from the UART event handler, handle the character press
	else if (receivedMessage->HEADER.TARGET_QID == interruptQueue){
		_handleInterruptMessage((InterruptMessagePtr) receivedMessage, g_Handler);
	}

	// Unlock the handler for user access
	_mutex_unlock(&g_HandlerMutex);

#ifdef PEX_USE_RTOS   
  }
#endif    
}

/*=============================================================
                        SERIAL TASK
 ==============================================================*/

void serial_task(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
  printf("\r\nSerialTask Created!\n\r");

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

/* END os_tasks */

#ifdef __cplusplus
}  /* extern "C" */
#endif

/*=============================================================
                        USER TASK 1
 ==============================================================*/

void UserTaskOne_task(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
  printf("UserTaskOne started!\n");
  OSA_TimeDelay(1000);
  	//tests
	//open r getline, receive lines
	//already openr return false
	//openw print out to console
	//already openw return false
	//if getline called before openr return false
	//if putline called before openw return false
	//close wipes read and write
	//close return false if no priv
	//check: readers read, writers write

	//T1
	//getline false

	char outputString[HANDLER_BUFFER_SIZE];
	if(!GetLine(outputString)){
		printf("Getline early check: PASS\n");
	}else{
		printf("Getline early check: FAIL");
	}
	//openr
	if(OpenR(_initializeQueue(10))){
		printf("OpenR privileges check: PASS\n");
	}else{
		printf("OpenR privileges check: FAIL\n");
	}
	//openr false
	if(!OpenR(_initializeQueue(11))){
		printf("OpenR twice check: PASS\n");
	}else{
		printf("OpenR twice check: FAIL\n");
	}
	//close
	if(Close()){
		printf("Close: PASS\n");
	}else{
		printf("Close: FAIL\n");
	}
	//close false
	if(!Close()){
		printf("Close twice: PASS\n");
	}else{
		printf("Close twice: FAIL\n");
	}
#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */

	  OSA_TimeDelay(10);

	  //getline should block
//	  	GetLine(outputString);
//	  	printf("UserTaskOne output string: %s\n", outputString);
#ifdef PEX_USE_RTOS   
  }
#endif
}

/*=============================================================
                       USER TASK 2
 ==============================================================*/

void UserTaskTwo_task(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
	 printf("UserTaskTwo started!\n");
	 OSA_TimeDelay(3000);
	  //T2
	  //delay(5sec)
	  //putline
	  char inputString[HANDLER_BUFFER_SIZE];
	  if(!PutLine(0, inputString)){
		  printf("PutLine early check: PASS\n");
	  }else{
		  printf("PutLine early check: FAIL\n");
	  }
	  //openw
	  _queue_id qID = OpenW();
	  //if(qID != )
	  //openw false
	  //putline should block
	  //close
	  //close false
#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */
	  OSA_TimeDelay(10);

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
