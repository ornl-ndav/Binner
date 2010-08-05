/**
   \ingroup rebinner_sdk

   \file src/collector.c

   \brief CURRENT sdk executable to collect results in distributed rebinner runs.

   $Id$
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "socketfun.h"
#include "binnerio.h"
#include "vcbutils.h"



typedef struct collector_thread {
    pthread_t tid;

	int socket;
	int offset;
	int nbytes;
	int status; /* 0: untouched, 1: in progress, 2: succeeded, -1: failed */
	
	char buffer[8000];
} task_t;

int sock1; 
int orig[3], xyzsize[3];
double * voxels;

int maxtaskid, alreadyfinal, curtaskcnt;
pthread_mutex_t lock1, lock2;
clock_t time1, time2;

void * collector_thread(void * in)
{
	task_t * me;
	char * p;
	int i, n, x, y, z, vid, toread;
	int taskcnt[2];
	int nvoxel, orig[3], blksize[3];
	char * netbuf;

	me = (task_t *) in;
	fprintf(stderr,"collector_thread start reading ...");
	fflush(stderr);

	n = read(me->socket, taskcnt, sizeof(int)*2);
	if (n > 0) 
	{
		pthread_mutex_lock(&lock1);
		curtaskcnt ++;
		pthread_mutex_unlock(&lock1);
	}

	pthread_mutex_lock(&lock1);
	if (taskcnt[1] > 0) alreadyfinal = 1; /* this means you know the final total cnt */
	if (taskcnt[0] > maxtaskid) maxtaskid = taskcnt[0]; /* which is maxtaskid */
	pthread_mutex_unlock(&lock1);

	fprintf(stderr,"done reading max/cur/final: %d %d %d\n", maxtaskid, curtaskcnt, alreadyfinal);
	fflush(stderr);
	
	toread = 7 *sizeof(int);
	for (p = me->buffer, n = 0; (i = read(me->socket, p, toread)) > 0; toread -= i, p+=i)
		n += i;

	p = me->buffer;
	memcpy(&nvoxel, p, sizeof(int));   p+= sizeof(int);
	memcpy(orig, p, sizeof(int)*3);    p+= sizeof(int)*3;
	memcpy(blksize, p, sizeof(int)*3); p+= sizeof(int)*3;

	toread = nvoxel * (3*sizeof(int)+sizeof(double));
	netbuf = malloc(toread);
	for (p = netbuf, n = 0; (i = read(me->socket, p, toread)) > 0; toread -= i, p+=i)
		n += i;
	
	close(me->socket);

	/* write into the volume */
	for (i = 0, p = netbuf; i < nvoxel; p+=20, i ++)
	{
		x = *((int*)p) + orig[0];
		y = *((int*)(p+4)) + orig[1];
		z = *((int*)(p+8)) + orig[2];
		vid = (x*xyzsize[1] + y)*xyzsize[2] + z;
		voxels[vid] = *((double*)(p+12));
	}

	free(netbuf);
	free(me);

	if ((alreadyfinal > 0) && ((curtaskcnt - 1) >= maxtaskid))
	{
		pthread_mutex_lock(&lock2);
		close(sock1);
time2 = clock();
		printf("total collector up time: %.3f sec\n",(float)(time2-time1)/CLOCKS_PER_SEC);

		orig[0] = orig[1] = orig[2] = 0;
		vcbGenBinm("400.bin", VCB_DOUBLE, 3, orig, xyzsize, 1, voxels);
		free(voxels);
		exit(maxtaskid);
		pthread_mutex_unlock(&lock2);
	}	
	pthread_exit(NULL);
}

int main(int argc, char ** argv)
{
	int sock2, i;
	int nvoxel;
	task_t *t;
	int port;
	FILE * portnum_file;
	pthread_attr_t attr[1];


	xyzsize[0] = atoi(argv[1]);
	xyzsize[1] = atoi(argv[2]);
	xyzsize[2] = atoi(argv[3]);
	//ntasks = atoi(argv[4]);
	sock1 = server_setup(argv[4], &port);

	fprintf(stderr, "%s server_setup: serving socket port. %d\n", argv[0], port);

time1 = clock();

	nvoxel = xyzsize[0]*xyzsize[1]*xyzsize[2];
	voxels = malloc(nvoxel * sizeof(double));
	for (i = 0; i < nvoxel; voxels[i] = 0.0, i ++);

	pthread_mutex_init(&lock1, NULL);
	pthread_mutex_init(&lock2, NULL);
	
	maxtaskid = 0;
	alreadyfinal = 0; 

	portnum_file = fopen("collectorport.num","w");
	fprintf(portnum_file,"%d\n", port);
	fflush(portnum_file);
	fclose(portnum_file);

	for (curtaskcnt = 0; (alreadyfinal == 0) || ((curtaskcnt - 1) != maxtaskid); )
	{
		sock2 = accept_connection(sock1);
		if (sock2 < 0) break;

	fprintf(stderr, "%s got a new connection\n", argv[0]);

		t = (task_t *) malloc(sizeof(task_t));
		t -> socket = sock2;
		pthread_attr_init(attr);
		pthread_attr_setscope(attr, PTHREAD_SCOPE_SYSTEM);
		pthread_create(&t->tid, NULL, collector_thread, t);
	}

	pthread_mutex_lock(&lock2);
	close(sock1);
	orig[0] = orig[1] = orig[2] = 0;
	//vcbGenBinm("400.bin", VCB_DOUBLE, 3, orig, xyzsize, 1, voxels);
	free(voxels);
	pthread_mutex_unlock(&lock2);

	return 0;
}
