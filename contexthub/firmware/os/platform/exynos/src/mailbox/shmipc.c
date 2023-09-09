/****************************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/

/*#include "RTOS.h"*/
//#include <sys/types.h>
#include <typedef.h>
#include <string.h>
#include <errno.h>
#include "mailboxGnssDrv.h"
#include "shmipc.h"
#include "csp_printf.h"

#define SHM_FRAME_DELIMITER 0xABCD

#define CHUB_SHMIPC_TOTAL_SIZE 0xC00

#define CHUB_SHMIPC_TX_BUFFER_ADDRESS ((uint8_t *)0x000BFC00)
#define CHUB_SHMIPC_TX_BUFFER_LENGTH  (0x400 - 0x18) /* -128 for the shared control registers area */

#define CHUB_SHMIPC_RX_BUFFER_ADDRESS ((uint8_t *)0x000BF400)
#define CHUB_SHMIPC_RX_BUFFER_LENGTH  (0x800)   

#define CHUB_SHMIPC_CONTROL_REGISTERS_ADDRESS    (0x000BFFE8)

/* Control register for SHMIPC */
#define CHUB_SHMIPC_RX_IPC_MSG   (CHUB_SHMIPC_CONTROL_REGISTERS_ADDRESS + 0x14)
#define CHUB_SHMIPC_TX_IPC_MSG   (CHUB_SHMIPC_CONTROL_REGISTERS_ADDRESS + 0x10)
#define CHUB_SHMIPC_RX_HEAD      (CHUB_SHMIPC_CONTROL_REGISTERS_ADDRESS + 0xC)
#define CHUB_SHMIPC_RX_TAIL      (CHUB_SHMIPC_CONTROL_REGISTERS_ADDRESS + 0x8)
#define CHUB_SHMIPC_TX_HEAD      (CHUB_SHMIPC_CONTROL_REGISTERS_ADDRESS + 0x4)
#define CHUB_SHMIPC_TX_TAIL      (CHUB_SHMIPC_CONTROL_REGISTERS_ADDRESS + 0x0)

#define SHM_MAILBOX_INT_IPC_MSG 2

/**
 * The frame sequence number and the channel sequence number have no exception
 * handling specified in the v1.4 SHMIPC document. Because of this, we are
 * assuming these values have no purpose outside of system debugging, and will
 * be ignored. Additionally fragmented packets are not supported. The description
 * of the multi-frame system is incomplete. It leaves out critical discussions
 * on the limits of the system, and doesn't describe how handle any failure cases.
 */

typedef struct ShmHeader_s
{
   uint16_t delimiter;
   uint16_t frameSeqNumber;
   uint16_t config;
   uint16_t length;
   uint8_t channelId;
   uint8_t chanSeqNumber;
   uint16_t reserved;
} ShmHeader_t;

static uint16_t shmChanSeqNumber;
static uint16_t shmFrameSeqNumber;

/* static OS_RSEMA shmTxBufferLock; */
/* static OS_RSEMA shmRxBufferLock; */

/**
 * Container for tracking buffer state
 */
typedef struct scbuffer_s
{
   size_t head;
   size_t tail;
   size_t size;
   char * data;
} SCBuffer_t;

size_t SCB_read(SCBuffer_t * scb, char * dst, size_t count);
size_t SCB_write(SCBuffer_t *scb, const char * src, size_t count);

/**
 * shm_calculatePadding
 *
 * @brief Calculate the padding to be added to the end of the frame to preserve
 * 32bit alignment.
 *
 * @param length The number of bytes in the message payload
 */
static uint16_t shm_calculatePadding(uint16_t length)
{
   uint16_t count = length % 4;
   if (count)
   {
      count = 4 - count;
   }
   return count;
}

/**
 * @fn shmipc_init
 * 
 * @brief zero out the shmipc memory block.
 * 
 */
void shmipc_init(void)
{  
     __raw_writel(0, CHUB_SHMIPC_RX_HEAD);
     __raw_writel(0, CHUB_SHMIPC_RX_TAIL);
     __raw_writel(0, CHUB_SHMIPC_TX_HEAD);
     __raw_writel(0, CHUB_SHMIPC_TX_TAIL);
     __raw_writel(0x1, CHUB_SHMIPC_TX_IPC_MSG);
     __raw_writel(0x1,CHUB_SHMIPC_RX_IPC_MSG);
}

/**
 * @fn shmipc_read
 *
 * @brief Read a shmipc message out of the shared memory region and return it's payload.
 *
 * @param payload The memory a shmipc payload will be copied in to.
 * @param length The number of bytes allocated for the payload.
 *
 * @return bytes read.
 */
int shmipc_read(void *payload, int length)
{
   int result = 0;
   ShmHeader_t hdr = { SHM_FRAME_DELIMITER, shmFrameSeqNumber, 0xC0, length, 0, 0, 0 };
   SCBuffer_t scb;

   scb.head = __raw_readl(CHUB_SHMIPC_RX_HEAD);
   scb.tail = __raw_readl(CHUB_SHMIPC_RX_TAIL);
   scb.size = CHUB_SHMIPC_RX_BUFFER_LENGTH;
   scb.data = (char*)CHUB_SHMIPC_RX_BUFFER_ADDRESS;
   
   if (scb.head != scb.tail)
   {
       size_t count = sizeof(ShmHeader_t);
       if (SCB_read(&scb, (char*)&hdr, count) == count && SHM_FRAME_DELIMITER == hdr.delimiter)
       {
          count = hdr.length + shm_calculatePadding(hdr.length) - count;
          if ((hdr.config >> 15) ^ (hdr.config >> 14 & 0x1))
          {
             /* someone is trying to pass multi-part messages. We don't support this. Throw the message away. */
             result = SHMIPC_READ_ERROR_MULTI_PART_MESSAGE;
             __raw_writel(scb.tail + count, CHUB_SHMIPC_RX_TAIL);
          }
          else if (length <= count)
          {
             /* The message is too big to fit in the buffer provided by the caller. Notify. Should we also purge the message? */
             result = SHMIPC_READ_ERROR_PAYLOAD_SIZE;
          }
          else if (SCB_read(&scb, payload, count) == count)
          {
             /* successful message processed. update the tail. */
             __raw_writel(scb.tail, CHUB_SHMIPC_RX_TAIL);
             result = hdr.length - sizeof(ShmHeader_t);
          }
       }
       else
       {
          /* failed to read in a header. unknown buffer state. reset and bail. */
          result = SHMIPC_READ_ERROR_UNKNOWN;
          __raw_writel(scb.head, CHUB_SHMIPC_RX_TAIL);
       }
    }

   /* OS_Unuse(&shmRxBufferLock); */

   return result;
}

/**
 * @fn shmipc_write
 *
 * @brief Write a payload to a shared memory region.
 *
 * @param payload The data to be packaged into a shmipc message and written to memory.
 * @param length The number of bytes in the payload.
 *
 * @return bytes written. This should match the length parameter.
 */
int shmipc_write(void *payload, int length)
{
   ShmHeader_t hdr = { SHM_FRAME_DELIMITER, shmFrameSeqNumber, 0xC0, length + sizeof(ShmHeader_t), 0, shmChanSeqNumber, 0 };
   SCBuffer_t scb;
   size_t count = sizeof(ShmHeader_t);
   int result = 0;
   
   /* OS_Use(&shmTxBufferLock); */
   scb.head = __raw_readl(CHUB_SHMIPC_TX_HEAD);
   scb.tail = __raw_readl(CHUB_SHMIPC_TX_TAIL);
   scb.size = CHUB_SHMIPC_TX_BUFFER_LENGTH;
   scb.data = (char*)CHUB_SHMIPC_TX_BUFFER_ADDRESS;

   if (SCB_write(&scb, (char*)&hdr, count) == count)
   {
      if (SCB_write(&scb, payload, length) == length)
      {
          char pad[3] = { 0, 0, 0 };
          size_t padding = shm_calculatePadding(length);
          if (SCB_write(&scb, pad, padding) == padding)
          {
             shmFrameSeqNumber++;
             shmChanSeqNumber++;
             __raw_writel(scb.head, CHUB_SHMIPC_TX_HEAD);
             mailboxGnssDrvGenInterupt(SHM_MAILBOX_INT_IPC_MSG);
             result = length; /* success */
          }
       }
    }

   return result;
}

/**
 * @fn SCB_read
 *
 * @brief Read from a circular buffer.
 *
 * @param channel The id of the desired channel.
 * @param dst The start address to use as a buffer.
 * @param count The number of bytes to write to the buffer.
 *
 * @return bytes read.
 *
 * This implementation leaves an unusedable element in the buffer.
 */
size_t SCB_read(SCBuffer_t * scb, char * dst, size_t count)
{
   size_t head = scb->head;
   size_t tail = scb->tail;
   size_t size = scb->size;
   size_t block = 0;
   size_t read = 0;
   size_t used = (head < tail) ? size + head - tail : head - tail;

   block = (head < tail) ? size - tail : head - tail;
   read = (count > block) ? block : count;
   memcpy(dst, &(scb->data[tail]), read);
   tail += read;

   if (count > block && block < used)
   {
      tail = count - read;
      tail = (tail > head) ? head : tail;
      memcpy(&dst[read], scb->data, tail);
      read += tail;
   }

   scb->tail = tail;

   return read;
}

/**
 * @fn SCB_write
 *
 * @brief Write to a circular buffer.
 *
 * @param scb The structure tracking the buffer state.
 * @param src The start address of the source data to be written.
 * @param count The number of bytes to write to the buffer.
 *
 * @return bytes written.
 */
size_t SCB_write(SCBuffer_t *scb, const char * src, size_t count)
{
   char * dst = scb->data;
   size_t head = scb->head;
   size_t tail = scb->tail;
   size_t block = 0;
   size_t write = 0;
   size_t remain = (head < tail) ? tail - head - 1 : scb->size + tail - head - 1;
   if (count > remain) { return 0; }

   block = (head < tail) ? tail - head - 1 : scb->size - head;
   write = (count > block) ? block : count;
   memcpy(&dst[head], src, write);
   head += write;

   if (count > block)
   {
      head = count - write;
      memcpy(dst, &src[write], head);
      write += head;
   }

   scb->head = head;

   return write;
}


