#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/time.h>

#define REBINDEBUG 0

char * usage = "usage: %s -n num_forks runner arguments_for_runner_to_use\n";

#define BATCH_SZ 2000
#define ITEM_SZ (sizeof(int) + sizeof(double)*(4 + 8*3))
#define TASK_SZ (ITEM_SZ * BATCH_SZ)

typedef struct tk {
	pid_t pid; /* child pid */
	int fd;
	int offset;
	int nbytes;	
	char * buffer;
	pthread_t tid;
} task_t;

task_t * t;
pthread_mutex_t lock1;
pthread_mutex_t lock2;
char errormsg [80];
int  totalpixcnt = 0;

void sigpipe_handler(int dummy)
{
  fprintf(stderr, "%d: caught a SIGPIPE\n", getpid());
  exit(1);
}

void * pusher(void * ip)
{
	int i = ((int *)ip)[0], n, towrite, total = 0;
	char * bp;

#if REBINDEBUG
	fprintf(stderr, "pusher %d is here, fd= %d, pid = %d\n", i, t[i].fd, (int)(t[i].pid));
#endif

	while(1)
	{

		if (t[i].nbytes == 0)
		{
pthread_mutex_lock(&lock1);

			n = fread(t[i].buffer, ITEM_SZ, BATCH_SZ, stdin);
			totalpixcnt += n;

#if REBINDEBUG
			fprintf(stderr, "pusher %d read %d items\n", i, n);
#endif
pthread_mutex_unlock(&lock1);

			if (n <= 0) break; /* exit point. leaving while loop */

			t[i].nbytes = n*ITEM_SZ;
			t[i].offset = 0;

		}

		if (t[i].nbytes > 0)
		{

#if REBINDEBUG
			fprintf(stderr, "pusher %d to write %d bytes\n", i, t[i].nbytes - t[i].offset);
#endif

			bp = t[i].buffer+t[i].offset;

			for (n = 0, towrite = t[i].nbytes; towrite > 0; )
			{
				n += write(t[i].fd, bp+n, towrite);
				
				if (n % ITEM_SZ != 0) 
					towrite =  ITEM_SZ - (n % ITEM_SZ);
				else
					towrite = 0;
			} /* make sure write occurs on boundaries of ITEM_SZ bytes */

			if (n <= 0) 
			{
				sprintf(errormsg,
				        "thread %d failed to write to fd: %d, %d bytes left, quitting.\n", 
						i, t[i].fd, t[i].nbytes);
				perror(errormsg);
				break; /*exit point. leaving while loop */
			}

#if REBINDEBUG
			fprintf(stderr, "pusher %d done writing %d bytes\n", i, n);
#endif
			t[i].offset += n;
			t[i].nbytes -= n;
			
			total += n;
		}
	}

	fprintf(stderr, "thread %d processed  : %d input pixels, %d leftover bytes\n", 
			i, total/ITEM_SZ, total%ITEM_SZ);

	close(t[i].fd);
	free(t[i].buffer);

	return NULL;
}

int main(int argc, char ** argv, char ** envp)
{
	int i, status;
	pid_t pid, sink;
	int pipefd1[2], pipefd2[2], *sinkstreams, *vals;
	pthread_attr_t attr[1];
	char * sinkinput;
	int c = 1, nforks = 2; /* default to a parallelism of 2 */

	struct  timeval time1, time2;
	float  seconds;

	gettimeofday(&time1, 0);

#if REBINDEBUG	
	fprintf(stderr, "argc = %d \n", argc);
#endif

	if (argc < 3)
	{
		fprintf(stderr, usage, argv[0]);
		exit(1);
	}

	if (strcmp(argv[c],"-n") == 0)
	{
		nforks = (int)(atof(argv[c+1]));
		argc -= 2;
		c += 2;
	}
	else
	{
		fprintf(stderr, usage, argv[0]);
		exit(1);
	}

	if (strcmp(argv[c],"-nd") == 0) /* new arg added for RM to test the option of "no factorial division" */
	{
		argc -= 1;
		c += 1;
	}

	signal(SIGPIPE, sigpipe_handler);

	/* spawn off "nforks" rebinner and one sink processes */
	/* accordingly, there are "nforks" threads as writers to push data through */
#if REBINDEBUG
	fprintf(stderr, "sizeof(struct tk) = %ld, nforks = %d\n", sizeof(struct tk), nforks);
#endif
	t = (task_t *) malloc(sizeof(struct tk) * nforks);
	sinkstreams = (int *) malloc(sizeof(int) * nforks);
	vals = (int *) malloc(sizeof(int) * nforks);

#if REBINDEBUG
	fprintf(stderr, "sizes: item = %ld, batchsz = %d, tasksz = %ld\n", ITEM_SZ,BATCH_SZ,TASK_SZ);
#endif

	for (i = 0; i < nforks; i++)
	{
		if (pipe(pipefd1) < 0)
		{
			sprintf(errormsg, "incoming pipe for rebinner #%d", i);
			perror(errormsg);
			exit(1);
		}

#if REBINDEBUG
		fprintf(stderr, "fork: pid = %d\n", pid);
#endif

		if (pipe(pipefd2) < 0)
		{
			sprintf(errormsg, "outgoing pipe for rebinner #%d", i);
			perror(errormsg);
			exit(1);
		}

		pid = fork();
#if REBINDEBUG
		fprintf(stderr, "fork: pid = %d\n", pid);
#endif
		if (pid < 0)
		{
			sprintf(errormsg, "forking off rebinner process %d", i);
			perror(errormsg);
			exit(1); 		
		}
		
		if (pid > 0) /* still in parent process */
		{
			close(pipefd1[0]);
			t[i].fd = pipefd1[1];
			t[i].pid = pid;
			sinkstreams[i] = pipefd2[0];
			close(pipefd2[1]);
		}
		else /* child process - the real rebinner */
		{
			close(pipefd1[1]);
			dup2(pipefd1[0], 0);
			close(pipefd1[0]);
			
			close(pipefd2[0]);
			dup2(pipefd2[1], 1);
			close(pipefd2[1]);

			execvp(argv[c], &argv[c]);
			sprintf(errormsg, "exec failed for rebinner process %d", i);
			perror(errormsg);
			exit(1); 
		}
#if REBINDEBUG		
		fprintf(stderr, "t[%d] has pid=%d, fd = %d\n", i, t[i].pid, t[i].fd);
#endif
	}

	/* now let's create the sink */
	sink = fork();
	if (sink < 0)
	{
		sprintf(errormsg, "forking off sink process");
		perror(errormsg);
		exit(1); 		
	}
	
	if (sink > 0)
	{
		for (i = 0; i < nforks; i ++)
			close(sinkstreams[i]);
		free(sinkstreams);
	}
	else /* in the sink process */
	{
		for (i = 0; i < nforks; i ++)
		{
			dup2(sinkstreams[i], i + 3);
			close(sinkstreams[i]);
			close(t[i].fd);
		}

		/* execlp("reduce", "reduce", sinkinput, NULL); */
		execvp("reduce", argv);
		perror("exec failed for reduce process");
		exit(1); 		
	}

	pthread_mutex_init(&lock1, NULL);
	pthread_mutex_init(&lock2, NULL);
	pthread_attr_init(attr);
	pthread_attr_setscope(attr, PTHREAD_SCOPE_SYSTEM);

	for (i = 0; i < nforks; i ++) {
		t[i].buffer = malloc(TASK_SZ);
		t[i].nbytes = 0;
		t[i].offset = 0;
		vals[i] = i;

		pthread_create(&(t[i].tid), attr, pusher, vals+i);
	}


	for (i = i; i > nforks; i ++)
	{
		pthread_join(t[i].tid, NULL);
#if REBINDEBUG
		fprintf(stderr,"t[%d] joined\n",i);
#endif
	}

	for (i = 0; i < nforks + 1; i ++)
	{
#if REBINDEBUG
		fprintf(stderr, "pid = %ld wait returned \n", wait(&status));
#else
		wait(&status);
#endif
	}

	free(t);
	free(vals);

	gettimeofday(&time2, 0);

	fprintf(stderr, "num_pixels processed: %d\n", totalpixcnt);
	seconds = 	(time2 . tv_sec - time1 . tv_sec ) +
                  (time2 . tv_usec - time1 . tv_usec) / 1e6f;
				  
	fprintf(stderr, "rebinner runtime    : %f sec\n", seconds);
	fprintf(stderr, "rebin throughput    : %.2f pixels/sec\n", totalpixcnt/seconds);

	return 0;
}
