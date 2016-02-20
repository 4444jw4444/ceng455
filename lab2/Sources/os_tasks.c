#include <stdio.h>
#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"


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

#define MAX_BUFFER_SIZE 128
unsigned char stringBuffer[MAX_BUFFER_SIZE];
int bufferIndex;


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
		unsigned char inputChar = msg_ptr->CHARACTER;

		if(inputChar == 0x08){//backspace

		}else if(inputChar == 0x07){//Bell

		}else if(inputChar == 0x09){//HTab

		}else if(inputChar == 0x0A){//LineFeed

		}else if(inputChar == 0x0D){//Carriage Return

		}else if(inputChar == 0x15){//CTRL U

		}else if(inputChar == 0x17){//CTRL W

		}else if(inputChar == 0x1B){//Esc

		}else{
			unsigned char holder[2];
			holder[0] = inputChar; //data
			holder[1] = '\0';//null

			UART_DRV_SendData(myUART_IDX, holder, sizeof(holder));
		}





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
