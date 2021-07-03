/****************************************************************************/
/*                                                                          */
/* Sam Siewert - 10/14/97                                                   */
/* Edited by Michelle Christian                                                                          */
/*                                                                          */
/****************************************************************************/
                                                                    
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define Q_NAME "/messagequeue"
#define ERROR (-1)

struct mq_attr mq_attr;
static mqd_t mymq;

pthread_t receiver_thread, sender_thread;

/* receives pointer to heap, reads it, and deallocate heap memory */

void* receiver()
{
  char buffer[sizeof(void*)+sizeof(int)];
  void *buffptr; 
  int prio;
  int nbytes;
  int count = 0;
  int id;
 
    /* read oldest, highest priority msg from the message queue */

    printf("Reading %ld bytes\n", sizeof(void *));
  
 mymq = mq_open(Q_NAME, O_CREAT|O_RDWR, S_IRWXU , &mq_attr);

  if(mymq == (mqd_t)ERROR)
    perror("mq_open");


    if((nbytes = mq_receive(mymq, buffer, (sizeof(void*)+sizeof(int)), &prio)) == ERROR)
/*
    if((nbytes = mq_receive(mymq, (void *)&buffptr, (size_t)sizeof(void *), &prio)) == ERROR)
*/
    {
      perror("mq_receive");
    }
    else
    {
      memcpy(&buffptr, buffer, sizeof(void *));
      memcpy((void *)&id, &(buffer[sizeof(void *)]), sizeof(int));
      printf("receive: ptr msg 0x%X received with priority = %d, length = %d, id = %d\n", buffptr, prio, nbytes, id);

      printf("contents of ptr = \n%s\n", (char *)buffptr);

      free(buffptr);

      printf("heap space memory freed\n");

   }
    mq_close(mymq);
  if(mymq == (mqd_t)ERROR)
    perror("mq_close");


}


static char imagebuff[4096];

void* sender()
{
  char buffer[sizeof(void *)+sizeof(int)];
  void *buffptr;
  int prio;
  int nbytes;
  int id = 999;

  /* send malloc'd message with priority=30 */

    buffptr = (void *)malloc(sizeof(imagebuff));
    strcpy(buffptr, imagebuff);
    printf("Message to send = %s\n", (char *)buffptr);

    printf("Sending %ld bytes\n", sizeof(buffptr));

    memcpy(buffer, &buffptr, sizeof(void *));
    memcpy(&(buffer[sizeof(void *)]), (void *)&id, sizeof(int));

 mymq = mq_open(Q_NAME, O_CREAT|O_RDWR, S_IRWXU , &mq_attr);

  if(mymq == (mqd_t)ERROR)
    perror("mq_open");


    if((nbytes = mq_send(mymq, buffer, (size_t)(sizeof(void *)+sizeof(int)), 30)) == ERROR)
    {
      perror("mq_send");
    }
    else
    {
      printf("send: message ptr 0x%X successfully sent\n", buffptr);
    }

    mq_close(mymq);
  if(mymq == (mqd_t)ERROR)
    perror("mq_close");


    sleep(3);

}


void main(void)
{
  int i, j, rc;
  char pixel = 'A';

  for(i=0;i<4096;i+=64) {
    pixel = 'A';
    for(j=i;j<i+64;j++) {
      imagebuff[j] = (char)pixel++;
    }
    imagebuff[j-1] = '\n';
  }
  imagebuff[4095] = '\0';
  imagebuff[63] = '\0';

  printf("buffer =\n%s", imagebuff);

  /* setup common message q attributes */
  mq_attr.mq_maxmsg = 100;
  mq_attr.mq_msgsize = sizeof(void *)+sizeof(int);

  mq_attr.mq_flags = 0;

  /* receiver runs at a higher priority than the sender */  
  rc=pthread_create(&sender_thread,NULL,sender,NULL);
  if(rc){
  	perror("thread creation failed");
  }

  pthread_join(sender_thread,NULL);

  /* receiver runs at a higher priority than the sender */
  rc=pthread_create(&receiver_thread,NULL,receiver,NULL);
  if(rc){
        perror("thread creation failed");
  }

  pthread_join(receiver_thread,NULL);

}

