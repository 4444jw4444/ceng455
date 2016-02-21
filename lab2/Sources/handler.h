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

#define PRINT_DELAY_MS 10

#define HANDLER_INTERRUPT_QUEUE_ID 8
#define HANDLER_INPUT_QUEUE_ID 9

#define HANDLER_BUFFER_SIZE 256
#define HANDLER_READER_MAX 32

#define SERIAL_MESSAGE_POOL_INITIAL_SIZE 16
#define SERIAL_MESSAGE_POOL_GROWTH_RATE 16
#define SERIAL_MESSAGE_POOL_MAX_SIZE 2048

#define INTERRUPT_MESSAGE_POOL_INITIAL_SIZE 1
#define INTERRUPT_MESSAGE_POOL_GROWTH_RATE 1
#define INTERRUPT_MESSAGE_POOL_MAX_SIZE 16

/*=============================================================
                      EXPORTED TYPES
 ==============================================================*/

typedef struct HandlerBuffer {
	int currentSize;
	int maxSize;
	char* characters;
} HandlerBuffer, * HandlerBufferPtr;

typedef struct HandlerReader{
	_task_id taskId;
	_queue_id queueId;
} HandlerReader, *HandlerReaderPtr;

typedef struct HandlerReaderList{
	int count;
	int maxSize;
	HandlerReaderPtr* readers;
} HandlerReaderList, * HandlerReaderListPtr;

typedef struct Handler{
	HandlerReaderList readerList;
	HandlerBuffer buffer;
	_task_id currentWriter;
	_queue_id charInputQueue;
	_queue_id bufferInputQueue;
	uint32_t terminalInstance;
} Handler, * HandlerPtr;

typedef struct SerialMessage{
	MESSAGE_HEADER_STRUCT HEADER;
	int length;
	char* content;
} SerialMessage, * SerialMessagePtr;

typedef struct InterruptMessage
{
   MESSAGE_HEADER_STRUCT   HEADER;
   uint8_t character;
} InterruptMessage, * InterruptMessagePtr;

/*=============================================================
                      GLOBAL VARIABLES
 ==============================================================*/

extern HandlerPtr g_Handler;
extern MUTEX_STRUCT g_HandlerMutex;

extern _pool_id g_InterruptMessagePool;
extern _pool_id g_SerialMessagePool;

/*=============================================================
                      USER TASK INTERFACE
 ==============================================================*/

bool OpenR(uint16_t streamNumber);
bool GetLine(char* outputString);
_queue_id OpenW(void);
bool PutLine(_queue_id queueId, char* inputString);
bool Close(void);

/*=============================================================
                      INTERNAL INTERFACE
 ==============================================================*/

void _initializeHandler(HandlerPtr handler, _queue_id charInputQueue, _queue_id bufferInputQueue, uint32_t terminalInstance);
void _initializeHandlerMutex(MUTEX_STRUCT* mutex);
void _handleCharacterInput(char character, HandlerPtr handler);
void _handleWriteMessage(SerialMessagePtr serialMessage, HandlerPtr handler);

#endif
