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

/*=============================================================
                        USER TASK 1
 ==============================================================*/

void UserTaskOne_task(os_task_param_t task_init_data)
{
  printf("User Task 1 started!\n");

  // Sleep for some time to allow for handler initialization
  OSA_TimeDelay(20);

  	// Create a buffer for received messages
	char outputString[HANDLER_BUFFER_SIZE + 1];
	memset(outputString, 0, HANDLER_BUFFER_SIZE + 1);

	// Create a queue to receive messages
	_queue_id receiveQueue = _initializeQueue(10);

	// Test that a call to GetLine() before a call to OpenR() returns false
	if(GetLine(outputString) == false){
		printf("TEST - GetLine() fails if task has not called OpenR(): PASS\n");
	} else{
		printf("TEST - GetLine() fails if task has not called OpenR(): FAIL");
	}
	
	// Test that a valid call to OpenR() returns true
	if(OpenR(receiveQueue)){
		printf("TEST - OpenR() returns true: PASS\n");
	} else{
		printf("TEST - OpenR() returns true: FAIL\n");
	}
	
	// Test that OpenR() returns false if a task already has read privileges
	if(OpenR(receiveQueue) == false){
		printf("TEST - OpenR() returns false if task already has read access: PASS\n");
	} else{
		printf("TEST - OpenR() returns false if task already has read access: FAIL\n");
	}
	
	// Test that Close() returns true if a task has read access
	if(Close()){
		printf("TEST - Close() returns true if task has read access: PASS\n");
	} else{
		printf("TEST - Close() returns true if task has read access: FAIL\n");
	}
	
	// Test that Close() returns false if a task does not have read access
	if(Close() == false){
		printf("TEST - Close() returns false if task does not have read access: PASS\n");
	} else{
		printf("TEST - Close() returns false if task does not have read access: FAIL\n");
	}

	// Re-register task to receive future messages
	if(!OpenR(receiveQueue)){
		printf("Failed to aquire handler read access.\n");
		_task_block();
	}

#ifdef PEX_USE_RTOS
  while (1) {
#endif
	
	GetLine(outputString);
  	printf("User Task 1 received string: %s\n", outputString);
  	memset(outputString, 0, HANDLER_BUFFER_SIZE + 1);

#ifdef PEX_USE_RTOS   
  }
#endif
}

/*=============================================================
                       USER TASK 2
 ==============================================================*/

void UserTaskTwo_task(os_task_param_t task_init_data)
{
	printf("User Task 2 started.\n");
	 
	// Delay so that user tasks can run tests
	OSA_TimeDelay(3000);

	// Test that a call to PutLine() before a call to OpenW() returns false
	if(PutLine(0, "Hello World!") == false){
		printf("TEST - PutLine() fails if task has not called OpenW(): PASS\n");
	} else{
	 	printf("TEST - PutLine() fails if task has not called OpenW(): FAIL\n");
	}
	
	// Test that a valid call to OpenW() returns a valid queue ID
	_queue_id inputQueue = OpenW();
	if(inputQueue != 0){
		printf("TEST - OpenW() returns a non-zero queue ID: PASS\n");
	} else{
		printf("TEST - OpenW() returns a non-zero queue ID: FAIL\n");
	}
	
	// Test that OpenW() returns false if task already has write privileges
	if(OpenW() == 0){
		printf("TEST - OpenW() returns false if task already has write access: PASS\n");
	} else{
		printf("TEST - OpenW() returns false if task already has write access: FAIL\n");
	}

	// Test that PutLine() returns true
	if(PutLine(inputQueue, "Hello World!\n")){
		printf("TEST - PutLine() returns true when task has write access: PASS\n");
	} else{
		printf("TEST - PutLine() returns true when task has write access: FAIL\n");
	}

	// Test that Close() returns true if a task has write access
	if(Close()){
		printf("TEST - Close() returns true if task has write access: PASS\n");
	} else{
		printf("TEST - Close() returns true if task has write access: FAIL\n");
	}

	// Test that Close() returns false if a task does not have write access 
	if(!Close()){
		printf("TEST - Close() returns false if task does not have write access: PASS\n");
	} else{
		printf("TEST - Close() returns false if task does not have write access: FAIL\n");
	}

	printf("User Task 2 complete.\n");
	_task_block();  
}

#ifdef __cplusplus
}  /* extern "C" */
#endif 

/*!
** @}
*/
