/**
   \ingroup rebinner_sdk
   \file src/sorter.c
   $Id$
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <pthread.h>
#include "dllist.h"
#include "jval.h"
#include "socketfun.h"
#include "binnerio.h"


typedef struct sorter_thread {
    pthread_t tid;

	int socket;
	int offset;
	int nbytes;
	int status; /* 0: untouched, 1: in progress, 2: succeeded, -1: failed */
	
	char * buffer;
} task_t;

Dllist taskpool;
pthread_mutex_t lock1, lock2;
int tasksz, taskpool_sz;

int taskcnt[2];

/* read the next k paralleliped from fd, -1 ret means all done */
int read_nextask(int tasksz, void * buf)
{
	int i, n;
	char * v;
	static double vdata[1024];
	
	v = buf;
	for (n = 0; (n < tasksz) && ((i = get_polygond(vdata)) > 0); ) {

		memcpy(v+n, &i, sizeof(int)); 
		n += sizeof(int);
		memcpy(v+n, vdata, i*3*sizeof(double));
		n += i*3*sizeof(double);

	}
/*
	fprintf(stderr,"read_nextask: want %d bytes, got %d bytes\n",tasksz, n);
*/	
	return n; /* number of facets */

}

void * sorter_thread(void * sock)
{
	task_t * me;
	Dllist tmp;
	int s, n;

	s = *((int *) sock);

	fprintf(stderr,"sorter_thread %d tasks in pool ...", taskpool_sz);

	if (taskpool -> flink == taskpool) /* nothing in the pool */
	{
		pthread_mutex_lock(&lock1);
		taskcnt[1] = 1;
		write(s, taskcnt, sizeof(int)*2);
		pthread_mutex_unlock(&lock1);
		close(s);
		pthread_exit(NULL);
	}
	
	if (taskpool_sz <= 1) /* last task in the pool */
	{
		pthread_mutex_lock(&lock1);
		taskcnt[1] = 1;
		//write(s, taskcnt, sizeof(int)*2);
		pthread_mutex_unlock(&lock1);
	}
	
	fprintf(stderr,"%d %d ...\n", taskcnt[0], taskcnt[1]);

	/* otherwise, let's do something */
	pthread_mutex_lock(&lock1);
		tmp = taskpool->flink;
		me = (task_t *) jval_v(dll_val(tmp));
		dll_delete_node(tmp);
		taskpool_sz --;
		write(s, taskcnt, sizeof(int)*2);
		taskcnt[0] ++;
	pthread_mutex_unlock(&lock1);

	/*fprintf(stderr,"want to write %d bytes to fd: %d\n", me->nbytes, me->socket);*/
	n = write(s, me->buffer, me->nbytes);
	/*
	if (n < 0)
		perror("write n");
	*/
	/*fprintf(stderr,"wrote %d bytes, 0x%x\n", n, me->buffer);*/

	close(s);

	/* get mutax and do a read */
	pthread_mutex_lock(&lock2);
	n = read_nextask(tasksz, me->buffer);
	pthread_mutex_unlock(&lock2);

	if (n > 0)
	{
		me-> nbytes = n;
		pthread_mutex_lock(&lock1);
		dll_append(taskpool, new_jval_v(me));
		taskpool_sz ++;
		pthread_mutex_unlock(&lock1);
	}
	else
	{
		free(me->buffer);
		free(me);
	}

	pthread_exit(NULL);
}

int main(int argc, char ** argv)
{
	int sock1, sock2, port;
	int i, readin;
	task_t *t;
    pthread_t tid;
	pthread_attr_t attr[1];
	FILE * portnum_file;

	taskcnt[0] = taskcnt[1] = 0;

	sock1 = server_setup(argv[1], &port);

	fprintf(stderr, "%s server_setup: serving socket port. %d\n", argv[0], port);

	/* task buf size are pre-determined on cmd-line argument */
	/* size of task pool is also pre-determined on cmd-line */
	tasksz = (sizeof(int) + 4*3*sizeof(double))*30; /* *10*6; */
	taskpool_sz = 20;
	taskpool = new_dllist();

	pthread_mutex_init(&lock1, NULL);
	pthread_mutex_init(&lock2, NULL);
  
	/* buffered read in bunches. buffer sized to fill at least 10 tasks */
	for (i = 0, readin = 0; i < taskpool_sz; i ++) {
		t = malloc(sizeof(task_t));
		t -> buffer = malloc(tasksz);
		t -> nbytes = read_nextask(tasksz, t->buffer);
		readin += t->nbytes;
		if (t -> nbytes > 0)
			dll_append(taskpool, new_jval_v(t));
		else
		{
			free(t->buffer);
			free(t);
			break;
		}
	}
	taskpool_sz = i;
	/*
	fprintf(stderr,"done readin = %d bytes, taskpool_sz = %d\n", readin, taskpool_sz);

	dll_traverse(tmp, taskpool)
		fprintf(stderr,"taskpool 0x%x\n", jval_v(dll_val(tmp)));
	*/

	/* work as a web server. 
	 * upon request, spawn off a new thread to serve the request 
	 *   write() call according to the offset and length in the buffer
	 */
	portnum_file = fopen("sorterport.num","w");
	fprintf(portnum_file,"%d\n", port);
	fflush(portnum_file);
	fclose(portnum_file);

	for (; taskpool_sz > 0; )
	{
		sock2 = accept_connection(sock1);
		pthread_attr_init(attr);
		pthread_attr_setscope(attr, PTHREAD_SCOPE_SYSTEM);
		pthread_create(&tid, attr, sorter_thread, &sock2);
	}
	
	free_dllist(taskpool);
	
	close(sock1);
	return 0;
}
