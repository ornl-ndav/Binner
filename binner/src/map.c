#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

#define REBINDEBUG 0

char * usage = "usage: %s [-n num_binners] [-t threshold] xmin xmax xspacing ymin ymax yspacing zmin zmax zspacing\n";

#define BATCH_SZ 1000
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
char errormsg [80];

void sigpipe_handler(int dummy)
{
  fprintf(stderr, "%d: caught a SIGPIPE\n", getpid());
  exit(1);
}

void * pusher(void * ip)
{
	int i = ((int *)ip)[0], n, k, total = 0;

#if REBINDEBUG
	fprintf(stderr, "pusher %d is here, fd= %d, pid = %d\n", i, t[i].fd, (int)(t[i].pid));
#endif

	while(1)
	{
		if (t[i].nbytes == 0)
		{
			pthread_mutex_lock(&lock1);
				n = fread(t[i].buffer, ITEM_SZ, BATCH_SZ, stdin);
			pthread_mutex_unlock(&lock1);
#if REBINDEBUG
			fprintf(stderr, "pusher %d read %d items\n", i, n);
#endif
			if (n <= 0) break; /* exit point. leaving while loop */

			t[i].nbytes = n*ITEM_SZ;
			t[i].offset = 0;
		}

		if (t[i].offset < t[i].nbytes)
		{
			k = write(t[i].fd, t[i].buffer+t[i].offset, t[i].nbytes);
			if (k <= 0) 
			{
				sprintf(errormsg,
				        "thread %d failed to write to fd: %d, %d bytes left, quitting.\n", 
						i, t[i].fd, t[i].nbytes);
				perror(errormsg);
				break; /*exit point. leaving while loop */
			}
#if REBINDEBUG
			fprintf(stderr, "pusher %d wrote %d bytes\n", i, k);
#endif
			t[i].offset += k;
			t[i].nbytes -= k;
			
			total += k;
		}
	}

	fprintf(stderr, "thread %d closing, wrote %d bytes\n", i, total);

	close(t[i].fd);
	free(t[i].buffer);

	pthread_join(t[i].tid, NULL);

}

int main(int argc, char ** argv, char ** envp)
{
	int i, status;
	pid_t pid, sink;
	int pipefd1[2], pipefd2[2], *sinkstreams, *vals;
	pthread_attr_t attr[1];
	char * sinkinput;


	int nbinners = 3, c = 1; /* default to 2 parallel rebinners */
	
	if (argc == 1)
	{
		fprintf(stderr, usage, argv[0]);
		exit(1);
	}

	if (strcmp(argv[c],"-n") == 0)
	{
		nbinners = (int)(atof(argv[c+1]));
		argc -= 2;
		c += 2;
	}

	signal(SIGPIPE, sigpipe_handler);

	/* spawn off "nbinners" rebinner and one sink processes */
	/* accordingly, there are "nbinners" threads as writers to push data through */
#if REBINDEBUG
	printf("sizeof(struct tk) = %ld, nbinners = %d\n", sizeof(struct tk), nbinners);
#endif
	t = (task_t *) malloc(sizeof(struct tk) * nbinners);
	sinkstreams = (int *) malloc(sizeof(int) * nbinners);
	vals = (int *) malloc(sizeof(int) * nbinners);

#if REBINDEBUG
	printf("sizes: item = %ld, batchsz = %d, tasksz = %ld\n", ITEM_SZ,BATCH_SZ,TASK_SZ);
#endif

	for (i = 0; i < nbinners; i++)
	{
		if (pipe(pipefd1) < 0)
		{
			sprintf(errormsg, "incoming pipe for rebinner #%d", i);
			perror(errormsg);
			exit(1);
		}

		if (pipe(pipefd2) < 0)
		{
			sprintf(errormsg, "outgoing pipe for rebinner #%d", i);
			perror(errormsg);
			exit(1);
		}

		pid = fork();
#if REBINDEBUG
		printf("fork: pid = %d\n", pid);
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

			// execve("./gmeshrebin3", argv+c, envp);
			execl("/bin/cat", "cat", NULL);
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
		for (i = 0; i < nbinners; i ++)
			close(sinkstreams[i]);
		free(sinkstreams);
	}
	else /* in the sink process */
	{
		for (i = 0; i < nbinners; i ++)
		{
			dup2(sinkstreams[i], i + 3);
			close(sinkstreams[i]);
			close(t[i].fd);
		}

		sinkinput = malloc(80);
		sprintf(sinkinput,"%d",nbinners); /*reusing errormsg for sink's argv */
		execl("/Users/huangj/huangj/coding/binner/bin/reduce", "reduce", sinkinput, NULL); 
		perror("exec failed for reduce process");
		exit(1); 		
	}

	pthread_mutex_init(&lock1, NULL);

	pthread_attr_init(attr);
	pthread_attr_setscope(attr, PTHREAD_SCOPE_SYSTEM);

	for (i = 0; i < nbinners; i ++) {
		t[i].buffer = malloc(TASK_SZ);
		t[i].nbytes = 0;
		t[i].offset = 0;
		vals[i] = i;

		pthread_create(&(t[i].tid), attr, pusher, vals+i);
	}

	for (i = 0; i < nbinners; i ++)
	{
		pthread_join(t[i].tid, NULL);
#if REBINDEBUG
		fprintf(stderr,"t[%d] joined\n",i);
#endif
	}

	for (i = 0; i < nbinners + 1; i ++)
	{
#if REBINDEBUG
		fprintf(stderr, "pid = %ld wait returned \n", wait(&status));
#endif
		wait(&status);
	}

	free(t);
	free(vals);

	return 0;
}
