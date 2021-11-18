#include "pc_buffer.h"

//*****************************************************************************
// Initializes the producer-consumer buffer structure.  
// 
// Assume the array passed in has already been allocated by the calling
// function
//
// Both the producer and consumer counts should be set to 0.
//*****************************************************************************
void pc_buffer_init(PC_Buffer *buffer, char* array, uint16_t buffer_size)
{
  buffer->BUFFER_SIZE = buffer_size;
	buffer->array = array;
	buffer->consume_count = 0;
	buffer->produce_count = 0;
}

//*****************************************************************************
// Adds a character to the producer consumer buffer
//
// Add the new data to array[produce_count % BUFFER_SIZE].  
//
// Increment produce_count.
//*****************************************************************************
void pc_buffer_add(PC_Buffer *buffer, char data)
{
  buffer->array[buffer->produce_count % buffer->BUFFER_SIZE] = data;
	buffer->produce_count++;
}

//*****************************************************************************
// Removes a character from the producer consumer buffer.  Returned in data
//
// Return array[consume_count % BUFFER_SIZE] in 'data'  
//
// Increment consume_count.
//*****************************************************************************
// Returns the data at array[consume_count].  Increments  consume_count
//*****************************************************************************
void pc_buffer_remove(PC_Buffer *buffer, char *data)
{
  *data = buffer->array[buffer->consume_count % buffer->BUFFER_SIZE];
	buffer->consume_count++;
}

//*****************************************************************************
// Returns if the producer consumer buffer is empty
//*****************************************************************************
bool pc_buffer_empty(PC_Buffer *buffer)
{
  if (buffer->produce_count - buffer->consume_count == 0)
		return true;
	return false;
}

//*****************************************************************************
// Returns if the producer consumer buffer is full
//*****************************************************************************
bool pc_buffer_full(PC_Buffer *buffer)
{
  if (buffer->produce_count - buffer->consume_count == buffer->BUFFER_SIZE)
		return true;
	return false;
}
