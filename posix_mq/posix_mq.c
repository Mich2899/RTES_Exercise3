/****************************************************************************/
/* Function: Basic POSIX message queue demo                                 */
/*                                                                          */
/* Sam Siewert, 02/05/2011                                                  */
/****************************************************************************/

//   Mounting the message queue file system
//       On  Linux,  message queues are created in a virtual file system.
//       (Other implementations may also provide such a feature, but  the
//       details  are likely to differ.)  This file system can be mounted
//       (by the superuser) using the following commands:

//           # mkdir /dev/mqueue
//           # mount -t mqueue none /dev/mqueue


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#define SNDRCV_MQ "/send_receive_mq"
#define MAX_MSG_SIZE 128
#define ERROR (-1)

struct mq_attr mq_attr;
pthread_t sender_thread, receiver_thread;

void* receiver(void)
{
  mqd_t mymq;
  char buffer[MAX_MSG_SIZE];
  int prio;
  int nbytes;

  mymq = mq_open(SNDRCV_MQ, O_CREAT|O_RDWR, S_IRWXU, &mq_attr);

  if(mymq == (mqd_t)ERROR)
  {
    perror("receiver mq_open");
    exit(-1);
  }

  /* read oldest, highest priority msg from the message queue */
  if((nbytes = mq_receive(mymq, buffer, MAX_MSG_SIZE, &prio)) == ERROR)
  {
    perror("mq_receive");
  }
  else
  {
    buffer[nbytes] = '\0';
    printf("receive: msg %s received with priority = %d, length = %d\n",
           buffer, prio, nbytes);
  }
  
  mq_close(mymq);
  if(mymq==(mqd_t)ERROR)
	perror("mqclose");  

}

static char canned_msg[] = "this is a test, and only a test, in the event of a real emergency, you would be instructed ...";

void* sender(void)
{
  mqd_t mymq;
  int prio;
  int nbytes;

  mymq = mq_open(SNDRCV_MQ, O_CREAT|O_RDWR, S_IRWXU, &mq_attr);

  if(mymq < 0)
  {
    perror("sender mq_open");
    exit(-1);
  }
  else
  {
    printf("sender opened mq\n");
  }

  /* send message with priority=30 */
  if((nbytes = mq_send(mymq, canned_msg, sizeof(canned_msg), 30)) == ERROR)
  {
    perror("mq_send");
  }
  else
  {
    printf("send: message successfully sent\n");
  }
  mq_close(mymq);
  if(mymq==(mqd_t)ERROR)
      perror("mqclose");
  
}


void main(void)
{

  /* setup common message q attributes */
  mq_attr.mq_maxmsg = 100;
  mq_attr.mq_msgsize = MAX_MSG_SIZE;

  mq_attr.mq_flags = 0;

  pthread_create(&sender_thread,NULL,sender,NULL);
  pthread_create(&receiver_thread,NULL, receiver, NULL);

  pthread_join(sender_thread,NULL);
  pthread_join(receiver_thread,NULL);
}
