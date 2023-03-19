/**
 * @file aesd-circular-buffer.c
 * @brief Functions and data related to a circular buffer imlementation
 *
 * @author Dan Walkes
 * @date 2020-03-01
 * @copyright Copyright (c) 2020
 *
 */

#ifdef __KERNEL__
#include <linux/string.h>
#else
#include <string.h>
#endif

#include "aesd-circular-buffer.h"

static uint32_t size_of_aesd_circular_buffer(struct aesd_circular_buffer *buffer);

/**
 * @param buffer the buffer to search for corresponding offset.  Any necessary locking must be performed by caller.
 * @return the total size of all the contents in the circular buffer
 */
uint32_t size_of_aesd_circular_buffer(struct aesd_circular_buffer *buffer)
{
    uint32_t total_buffer_size = 0;
    uint8_t count = 0;

    // Storing the first element (buffer->out_offs) of the buffer
    // If the buffer full flag is not true then there is only this element in the buffer so we return else we will
    // proceed ahead to the next element
    total_buffer_size += buffer->entry[(buffer->out_offs) + count].size;
    if(((buffer->out_offs) + count) == (buffer->in_offs) && !(buffer->full))
        return total_buffer_size;

    count = 1;
    // Will loop through all the elements in the buffer and will break out when the in_offs and out_offs is equal 
    // (i.e. no more elements to read)
    while(count <= AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED)
    {
        // In normal case we will read incrementally until less than AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED
        // However, when the in_offs rolls over we will have to add count to the out_offs but subtract AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED
        // This is not the normal circular buffer read implementation because we do not want the out_offs value to change

        // Normal condition when the write pointer is ahead of the read pointer
        if(((buffer->out_offs) + count) < AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED)
        {
            // When the current reading node and the in_offs are equal then we have reached the end of the buffer
            if(((buffer->out_offs) + count) == (buffer->in_offs))
                break;

            // Add the size of the current node and loop back
            total_buffer_size += buffer->entry[(buffer->out_offs) + count].size;
        }
        // Roll over condition when the write pointer is behind the read pointer
        else
        {
            // When the current reading node and the in_offs are equal then we have reached the end of the buffer
            if(((buffer->out_offs) + (count-AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED)) == (buffer->in_offs))
                break;

            // Add the size of the current node and loop back
            total_buffer_size += buffer->entry[(buffer->out_offs) + (count-AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED)].size;
        }

        count++;
    }

    return total_buffer_size;
}

/**
 * @param buffer the buffer to search for corresponding offset.  Any necessary locking must be performed by caller.
 * @param char_offset the position to search for in the buffer list, describing the zero referenced
 *      character index if all buffer strings were concatenated end to end
 * @param entry_offset_byte_rtn is a pointer specifying a location to store the byte of the returned aesd_buffer_entry
 *      buffptr member corresponding to char_offset.  This value is only set when a matching char_offset is found
 *      in aesd_buffer.
 * @return the struct aesd_buffer_entry structure representing the position described by char_offset, or
 * NULL if this position is not available in the buffer (not enough data is written).
 */
struct aesd_buffer_entry *aesd_circular_buffer_find_entry_offset_for_fpos(struct aesd_circular_buffer *buffer,
            size_t char_offset, size_t *entry_offset_byte_rtn )
{
    /**
    * TODO: implement per description
    */

    uint8_t count = 0;
    uint32_t total_buffer_size = 0;

    // Obtaining the total size of the elements in the circular buffer
    total_buffer_size = size_of_aesd_circular_buffer(buffer);

    // If the require character offset char_offset is greater than the total size in the buffer we will return NULL
    if(char_offset >= total_buffer_size)
        return NULL;
    
    count = 0;
    // We will loop through and reduce the character offset until we are able to get the offset value in the size
    // (i.e. no more elements to read)
    while(count <= AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED)
    {             
        // Normal condition when the write pointer is ahead of the read pointer
        if(((buffer->out_offs) + count) < AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED)
        {
            // Check if the offset can be accomodated in the current node's size if yes then return the pointer to this node and laod the offset into entry_offset_byte_rtn
            int8_t temp = (char_offset - (buffer->entry[(buffer->out_offs) + count].size));
            if(temp < 0)
            {
                *entry_offset_byte_rtn = char_offset;
                return &(buffer->entry[(buffer->out_offs) + count]);
            }
            else
            {
                char_offset -= buffer->entry[(buffer->out_offs) + count].size;
            }
        }
        // Roll over condition when the write pointer is behind the read pointer
        else
        {
            // Check if the offset can be accomodated in the current node's size if yes then return the pointer to this node and laod the offset into entry_offset_byte_rtn
            int8_t temp = (char_offset - (buffer->entry[(buffer->out_offs) + (count-AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED)].size));
            if(temp < 0)
            {
                *entry_offset_byte_rtn = char_offset;
                return &(buffer->entry[(buffer->out_offs) + (count-AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED)]);
            }
            else
            {
                char_offset -= buffer->entry[(buffer->out_offs) + (count-AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED)].size;
            }
        }

        count++;
    }

    return NULL;
}

/**
* Adds entry @param add_entry to @param buffer in the location specified in buffer->in_offs.
* If the buffer was already full, overwrites the oldest entry and advances buffer->out_offs to the
* new start location.
* Any necessary locking must be handled by the caller
* Any memory referenced in @param add_entry must be allocated by and/or must have a lifetime managed by the caller.
*/
char *aesd_circular_buffer_add_entry(struct aesd_circular_buffer *buffer, const struct aesd_buffer_entry *add_entry)
{
    /**
    * TODO: implement per description
    */
    char* temp = NULL;

    if(buffer->full)
    {
        temp = (char*)buffer->entry[buffer->out_offs].buffptr;
        buffer->out_offs++;
        // Handling overflow condition
        if(buffer->out_offs >= AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED)
            buffer->out_offs = 0;
    }

    buffer->entry[buffer->in_offs].buffptr = add_entry->buffptr;
    buffer->entry[buffer->in_offs].size = add_entry->size;

    (buffer->in_offs)++;
    // Handling overflow condition
    if(buffer->in_offs >= AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED)
        buffer->in_offs = 0;

    // Checking if the buffer is full
    if(buffer->in_offs == buffer->out_offs)
        buffer->full = true;

    return temp;
}

/**
* Initializes the circular buffer described by @param buffer to an empty struct
*/
void aesd_circular_buffer_init(struct aesd_circular_buffer *buffer)
{
    memset(buffer,0,sizeof(struct aesd_circular_buffer));

    // Starting at index 0
    buffer->in_offs = 0;

    // Starting at index 0
    buffer->out_offs = 0;

    // Starting with an empty buffer
    buffer->full = false;
}
