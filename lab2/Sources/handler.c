#include "handler.h"

/*=============================================================
                      BUFFER MANAGEMENT
 ==============================================================*/

/*=============================================================
                      READER MANAGEMENT
 ==============================================================*/

/*=============================================================
                      WRITER MANAGEMENT
 ==============================================================*/

void _clearHandlerWriter(){

}

/*=============================================================
                      USER TASK INTERFACE
 ==============================================================*/

bool OpenR(uint16_t streamNumber){
	return false;
}

bool GetLine(char* outputString){
	return false;
}

_queue_id OpenW(void){
	if(_mutex_lock(&g_Handler->accessMutex) != MQX_OK){
		printf("Mutex lock failed.\n");
		_task_block();
	}

	_task_id writer = g_Handler->currentWriter;

	if (writer != 0){
		_mutex_unlock(&g_Handler->accessMutex);
		return 0;
	}

	g_Handler->currentWriter = _task_get_id();
	_queue_id outputQueue = g_Handler->bufferOutputQueue;

	_mutex_unlock(&g_Handler->accessMutex);
	return outputQueue;
}

bool PutLine(_queue_id queueId, char* inputString){

	// Ensure last char is a newline
	int stringLen = strlen(inputString);
	if (inputString[stringLen - 1] != '\n'){
		return false;
	}

	// Allocate and initialize serial message
	SerialMessagePtr writeMessage = (SerialMessagePtr)_msg_alloc(g_SerialMessagePool);
	if (writeMessage == NULL) {
	 printf("Could not allocate a message.\n");
	 _task_block();
	}

	writeMessage->HEADER.SIZE = sizeof(SerialMessage);
	writeMessage->HEADER.TARGET_QID = queueId;
	writeMessage->length = stringLen;
	writeMessage->message = inputString;

	// Write serial message to queue
	if (!_msgq_send(writeMessage)) {
	 printf("Could not send a message.\n");
	 _task_block();
	}

	return true;
}

bool Close(void){
	return false;
}

