#include "handler.h"

/*=============================================================
                      BUFFER MANAGEMENT
 ==============================================================*/

/*=============================================================
                      READER MANAGEMENT
 ==============================================================*/

bool _addHandlerReader(_task_id taskId, _queue_id queue, HandlerPtr handler){

	int currentReaderCount = handler->readerList.count;

	if(currentReaderCount == handler->readerList.maxSize){
		return false;
	}

	HandlerReaderPtr reader;
	if(!(reader = (HandlerReaderPtr) malloc(sizeof(HandlerReader)))){
		printf("Unable to allocate memory for HandlerReader.");
		_task_block();
	}

	reader->queueId = queue;
	reader->taskId = taskId;

	handler->readerList.readers[currentReaderCount + 1] = reader;
	handler->readerList.count++;

	return true;
}

void _clearHandlerReader(_task_id taskId, HandlerPtr handler){
	int numReaders = handler->readerList.count;
	HandlerReaderPtr* readers = handler->readerList.readers;

	for(int i=0; i<numReaders; i++){
		if(readers[i]->taskId == taskId){

			// Deallocate the reader
			free(readers[i]);
			readers[i] = NULL;

			// Shift the remaining readers left by 1 index
			for (int j=i; j<numReaders-1; j++){
				readers[j] = readers[j+1];
			}
			handler->readerList.count--;
			break;
		}
	}
}

_queue_id _getReaderQueueNum(_task_id taskId, HandlerPtr handler){
	int numReaders = handler->readerList.count;
	HandlerReaderPtr* readers = handler->readerList.readers;

	for(int i=0; i<numReaders; i++){
		if(readers[i]->taskId == taskId){
			return readers[i]->queueId;
		}
	}

	return MSGQ_NULL_QUEUE_ID;
}

/*=============================================================
                      WRITER MANAGEMENT
 ==============================================================*/

void _clearHandlerWriter(_task_id taskId, HandlerPtr handler){
	if(handler->currentWriter == taskId){
		handler->currentWriter = 0;
	}
}

/*=============================================================
                      USER TASK INTERFACE
 ==============================================================*/

bool OpenR(uint16_t streamNumber){
	if(_mutex_lock(&g_Handler->accessMutex) != MQX_OK){
		printf("Mutex lock failed.\n");
		_task_block();
	}

	_task_id thisTask = _task_get_id();

	// Ensure this task does not already have read privileges
	if (_getReaderQueueNum(thisTask, g_Handler) != MSGQ_NULL_QUEUE_ID){
		_mutex_unlock(&g_Handler->accessMutex);
		return false;
	}

	// Register this task for reading with the handler
	bool result = _addHandlerReader(thisTask, streamNumber, g_Handler);

	_mutex_unlock(&g_Handler->accessMutex);
	return result;
}

bool GetLine(char* outputString){
	if(outputString == NULL){
		return false;
	}

	// Get the reader queue for this task
	if(_mutex_lock(&g_Handler->accessMutex) != MQX_OK){
		printf("Mutex lock failed.\n");
		_task_block();
	}
	_queue_id readerQueue = _getReaderQueueNum(_task_get_id(), g_Handler);
	_mutex_unlock(&g_Handler->accessMutex);

	// Ensure this task has read privileges
	if(readerQueue == MSGQ_NULL_QUEUE_ID){
		return false;
	}

	// Wait for the next message to arrive
  	SerialMessagePtr message = _msgq_receive(readerQueue, 0);
	if (message == NULL) {
	   printf("Could not receive a message\n");
	   _task_block();
	}

	// Copy message to output string
	strncpy(outputString, message->content, message->length);

	// Dispose of message
	_msg_free(message);

	return true;
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
	writeMessage->content = inputString;

	// Write serial message to queue
	if (!_msgq_send(writeMessage)) {
	 printf("Could not send a message.\n");
	 _task_block();
	}

	return true;
}

bool Close(void){
	if(_mutex_lock(&g_Handler->accessMutex) != MQX_OK){
		printf("Mutex lock failed.\n");
		_task_block();
	}

	_task_id thisTask = _task_get_id();
	_clearHandlerReader(thisTask, g_Handler);
	_clearHandlerWriter(thisTask, g_Handler);

	_mutex_unlock(&g_Handler->accessMutex);
	return true;
}

