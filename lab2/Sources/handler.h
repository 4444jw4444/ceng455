#include <stdio.h>
#include <stdbool.h>
#include <mqx.h>
#include <message.h>
#include <mutex.h>

#ifndef SOURCES_HANDLER_H_
#define SOURCES_HANDLER_H_

/*=============================================================
                         CONSTANTS
 ==============================================================*/

#define SERIAL_MESSAGE_POOL_INITIAL_SIZE 16
#define SERIAL_MESSAGE_POOL_GROWTH_RATE 16
#define SERIAL_MESSAGE_POOL_MAX_SIZE 2048

/*=============================================================
                      EXPORTED TYPES
 ==============================================================*/

typedef struct HandlerBuffer {
	int currentSize;
	int maxSize;
	char* buffer;
} HandlerBuffer, * HandlerBufferPtr;

typedef struct HandlerReader{
	_task_id taskId;
	_queue_id queueId;
} HandlerReader, *HandlerReaderPtr;

typedef struct HandlerReaderList{
	int count;
	int maxSize;
	HandlerReaderPtr* readers;
} HandlerReaderList;

typedef struct Handler{
	HandlerReaderList readerList;
	HandlerBuffer buffer;
	_task_id currentWriter;
	MUTEX_STRUCT accessMutex;
	_queue_id charInputQueue;
	_queue_id bufferOutputQueue;
} Handler, * HandlerPtr;

typedef struct SerialMessage{
	MESSAGE_HEADER_STRUCT HEADER;
	int length;
	char* content;
} SerialMessage, * SerialMessagePtr;

/*=============================================================
                      GLOBAL VARIABLES
 ==============================================================*/

extern HandlerPtr g_Handler;
extern _pool_id g_SerialMessagePool;

/*=============================================================
                      USER TASK INTERFACE
 ==============================================================*/

bool OpenR(uint16_t streamNumber);
bool GetLine(char* outputString);
_queue_id OpenW(void);
bool PutLine(_queue_id queueId, char* inputString);
bool Close(void);

#endif
