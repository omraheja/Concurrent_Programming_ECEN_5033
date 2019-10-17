
/*
 *@References: en.cppreference.com/w/cpp/atomic/atomic_fetch_add
 * */


#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <atomic>
#include <assert.h>
#include <string.h>
#include <getopt.h>

#define RELAXED std::memory_order_relaxed

using namespace std;

/* Global variables */
int NUM_THREADS;		//stores number of threads
int NUM_ITERATIONS;		//stores number of iterations
int TEST_NUM = 0;		//stores which function to invoke
int counter=0;			//variable to be incremented by each thread
char output_file_name[20];	//stores output file name
char input_file_name[20];	//stores input file name
pthread_t* threads;		//pthreads
size_t* args;
pthread_barrier_t bar;		//pthread barrier
thread_local int local_cnt;


atomic<int> atomic_counter;
struct timespec start, end;	//stores start time and end time



/* Synchronization variables */                                                                                                                                                                          
pthread_mutex_t lock;
atomic<bool> tas_lock;

atomic<int> next_num;
atomic<int> now_serving;

atomic<int> cnt;
atomic<int> sense;



/* Function Prototypes */

void empty(int);
void print(int);
void cnt_lock(int);
void cnt_tas_lock(int);
void cnt_test_and_tas_lock(int);
void cnt_fai(int);
void cnt_cas(int);
void cnt_array(int);
void cnt_array_padded(int);
void cnt_thread_local(int);
void cnt_stack(int);
void local_init(int tid);
void local_cleanup();
void wait();


/* Class */
class Node{
	public:
		std::atomic<Node*> next;
		std::atomic<bool> wait;
};


std::atomic<Node*> tail{NULL};

class MCSLock{
	public:
		void acquire(Node* myNode)
		{
			Node* oldTail = tail.load();
			myNode->next.store(NULL);
			while(!tail.compare_exchange_strong(oldTail,myNode))
			{
				oldTail = tail.load();
			}

			if(oldTail != NULL)
			{
				myNode->wait.store(true);
				oldTail->next.store(myNode);
				while(myNode->wait.load()){}
			}
		}

		void release(Node* myNode)
		{
			Node* temp = myNode;
			if(tail.compare_exchange_strong(temp,NULL))
			{
				//no one is waiting, and we just freed the lock
			}
			else	//hand lock to next waiting thread
			{
				while(myNode->next.load() == NULL){}
				myNode->next.load()->wait.store(false);
			}
		}
};

MCSLock mcs_lock;

void wait()
{
	thread_local bool my_sense = 0;
	if(my_sense == 0)
	{
		my_sense = 1;	//Flip sense
	}
	else
	{
		my_sense = 0;
	}


	int cnt_cpy;
	cnt_cpy = std::atomic_fetch_add(&cnt,1);

	//	cnt_cpy = cnt.fetch_add(1);

	if(cnt_cpy == NUM_THREADS)	//Last thread to arrive
	{
		cnt.store(1,RELAXED);
		sense.store(my_sense);
	}
	else		//not the last
	{
		while(sense.load() !=my_sense){}
	}
}



void empty(int tid)
{
}

void print(int tid)
{
	printf("Thread %d reporting for duty.\n",tid);
}

void cnt_lock(int tid)
{
	for(int i = 0; i<NUM_ITERATIONS; i++)
	{
		pthread_mutex_lock(&lock);
		counter++;
		pthread_mutex_unlock(&lock);
	}
}

void cnt_tas_lock(int tid)
{
	bool expected, changed;
	for(int i = 0; i<NUM_ITERATIONS; i++)
	{
		do
		{
			expected = false;
			changed = true;
		}while(!tas_lock.compare_exchange_strong(expected,changed));
		counter++;
		tas_lock.store(false);
	}
}

void cnt_test_and_tas_lock(int tid)
{
	bool expected, changed;
	for(int i = 0; i<NUM_ITERATIONS; i++)
	{
		do{
			expected = false;
			changed = true;
		}while(tas_lock.load()==true || !tas_lock.compare_exchange_strong(expected,changed));
		counter++;
		tas_lock.store(false);
	}
}



void ticket_lock()
{
	int my_num = std::atomic_fetch_add(&next_num,1);
	//int my_num = next_num.fai(1);
	while(now_serving.load() != my_num){}
}

void ticket_unlock()
{
	std::atomic_fetch_add(&now_serving,1);
	//now_serving.fai(1);
}


void cnt_ticket_lock(int tid)
{
	//printf("Ticket lock to be implemented\n");
	//ticket_lock();
	for(int i=0;i<NUM_ITERATIONS;i++)
	{
		ticket_lock();
		counter++;
		ticket_unlock();
	}
	//ticket_unlock();
}

void cnt_mcs_lock(int tid)
{
	for(int i=0;i<NUM_ITERATIONS;i++)
	{
		Node* my_node = new Node;
		mcs_lock.acquire(my_node);
		counter++;
		printf("Counter =%d\n",counter);
		mcs_lock.release(my_node);
	}
}


void cnt_pthread_barrier(int tid)
{
	for(int i=0;i<NUM_ITERATIONS*NUM_THREADS;i++)
	{
		//printf("i=%d\tNUM_THREADS=%d\ttid=%d\n",i,NUM_THREADS,tid);
		if((i%NUM_THREADS==tid-1))
		{
			counter++;
			printf("Counter = %d\t TID=%d\n",counter,tid);
		}
		pthread_barrier_wait(&bar);	

	}
}




void cnt_sense_rev_barrier(int tid)
{

	for(int i=0;i<NUM_ITERATIONS*NUM_THREADS;i++)
	{
		//printf("i=%d\tNUM_THREADS=%d\ttid=%d\n",i,NUM_THREADS,tid);
		if((i%NUM_THREADS==tid-1))
		{
			counter++;
			printf("Counter = %d\t TID=%d\n",counter,tid);
		}
		wait();
	}

}



const int NUM_FUNCS = 9;
void (*funcs[NUM_FUNCS])(int)  = {
	empty,				//0 -> empty
	print,				//1 -> print
	cnt_lock,			//2 -> pthread_mutex_lock and pthread_mutex_unlock
	cnt_tas_lock,			//3 -> test and set lock
	cnt_test_and_tas_lock,		//4 -> test and test and set lock
	cnt_ticket_lock,		//5 -> ticket lock
	cnt_mcs_lock,			//6 -> mcs lock
	cnt_pthread_barrier,		//7 -> pthread_barrier
	cnt_sense_rev_barrier		//8 -> sense reversal barrier
};

const char* func_names[NUM_FUNCS] = {
	"empty",
	"print",
	"cnt_lock",
	"cnt_tas_lock",
	"cnt_test_and_tas_lock",
	"cnt_ticket_lock",
	"cnt_mcs_lock",
	"cnt_pthread_barrier",
	"cnt_sense_rev_barrier"
};


void* thread_main(void* args)
{
	size_t tid = *((size_t*)args);
	void (*test)(int) = funcs[TEST_NUM];

	local_init(tid);
	pthread_barrier_wait(&bar);
	if(tid==1)
	{
		clock_gettime(CLOCK_MONOTONIC,&start);
	}
	pthread_barrier_wait(&bar);
	test(tid);
	pthread_barrier_wait(&bar);
	if(tid==1)
	{
		clock_gettime(CLOCK_MONOTONIC,&end);
	}
	local_cleanup();

	return 0;
}


void global_init(){
	threads = (pthread_t*) malloc(NUM_THREADS*sizeof(pthread_t));
	args = (size_t*) malloc(NUM_THREADS*sizeof(size_t));
	pthread_barrier_init(&bar, NULL, NUM_THREADS);
	counter = 0;
	atomic_counter.store(0);

	pthread_mutex_init(&lock,NULL);
	tas_lock.store(false);
}
void global_cleanup(){
	if(atomic_counter.load()!=0){counter = atomic_counter.load();}

	printf("Counter:%d\n",counter);

	free(threads);
	free(args);
	pthread_barrier_destroy(&bar);
	pthread_mutex_destroy(&lock);
}

void local_init(int tid){
	local_cnt=0;
}
void local_cleanup(){}



/* Main Function */
int main(int argc, char *argv[])
{
	int option_index = 0;
	int opt;
	//int ret;		//to check return status
	char lock_selected[10];	//to store lock selected
	char bar_selected[25];	//to store barrier selected
	bool b = false;
	bool l = false;

	cnt=1;

	struct option long_options[] = {
		{ "name", no_argument, NULL,'n'},
		{ "output",required_argument, NULL,'o'},
		{ "threads",required_argument,NULL,'t' },
		{ "iterations=",required_argument,NULL,'i'},
		{ "bar",required_argument,NULL,'b'},
		{ "lock",required_argument,NULL,'l'},
		{ 0, 0, 0, 0}
	};

	char *optstring = "no:t:i:bar:lock:";


	while((opt = getopt_long(argc, argv, optstring, long_options, &option_index))!= -1)
	{
		if (opt == -1)
			break;

		switch (opt) 
		{
			case 'n':
				printf("Name: Om Raheja\n");
				exit(1);
				break;

			case 'o':
				strcpy(output_file_name,optarg);
				printf("Output Filename = %s\n",output_file_name);
				break;

			case 't':
				NUM_THREADS = atoi(optarg);
				printf("Number of threads = %d\n",NUM_THREADS);
				break;

			case 'i':
				NUM_ITERATIONS = atoi((optarg+1));
				printf("Number of iterations: %d\n",NUM_ITERATIONS);

				break;

			case 'b':
				/* Condition to check if barrier were specified in the command line arguments first */
				if((b == false) && (l == false))
				{
					b = true;
					strcpy(bar_selected,optarg);		//copy barrier selected
					if(!strcmp(bar_selected,"pthread"))	//pthread barrier
					{
						TEST_NUM = 7;
					}
					else if(!strcmp(bar_selected,"sense"))	//sense reversal barrier
					{
						TEST_NUM = 8;
					}
					printf("Barrier selected = %s\n",func_names[TEST_NUM]);
				}
				break;

			case 'l':
				/* Condition to check if locks were specified in the command line arguments first */
				if((b == false) && (l == false))
				{
					l = true;
					strcpy(lock_selected,optarg);			//copy lock selected
					if(!strcmp(lock_selected,"empty"))		//empty
					{
						TEST_NUM = 0;
					}
					else if(!strcmp(lock_selected,"print"))		//print
					{
						TEST_NUM = 1;
					}
					else if(!strcmp(lock_selected,"pthread"))	//pthread mutex lock and unlock
					{
						TEST_NUM = 2;
					}
					else if(!strcmp(lock_selected,"tas"))		//test and set lock
					{
						TEST_NUM = 3;
					}
					else if(!strcmp(lock_selected,"ttas"))		//test and test and set lock
					{
						TEST_NUM = 4;
					}
					else if(!strcmp(lock_selected,"ticket"))	//ticket lock
					{
						TEST_NUM = 5;
					}
					else if(!strcmp(lock_selected,"mcs"))		//MCS lock
					{
						TEST_NUM = 6;
					}				
					printf("Lock selected = %s\n",func_names[TEST_NUM]);
				}

				break;
		}
	}


	global_init();

	// launch threads
	int ret; size_t i;
	for(i=1; i<NUM_THREADS; i++){
		args[i]=i+1;
		//printf("creating thread %zu\n",args[i]);
		ret = pthread_create(&threads[i], NULL, &thread_main, &args[i]);
		if(ret){
			printf("ERROR; pthread_create: %d\n", ret);
			exit(-1);
		}
	}
	i = 1;
	thread_main(&i); // master also calls thread_main

	// join threads
	for(size_t i=1; i<NUM_THREADS; i++){
		ret = pthread_join(threads[i],NULL);
		if(ret){
			printf("ERROR; pthread_join: %d\n", ret);
			exit(-1);
		}
		//printf("joined thread %zu\n",i+1);
	}

	global_cleanup();

	/* Write Total iterations in a file */
	FILE *fptr;
	fptr = fopen(output_file_name,"w+");
	fprintf(fptr,"%d\n",counter);
	fclose(fptr);

	unsigned long long elapsed_ns;
	elapsed_ns = (end.tv_sec-start.tv_sec)*1000000000 + (end.tv_nsec-start.tv_nsec);
	printf("Elapsed (ns): %llu\n",elapsed_ns);
	double elapsed_s = ((double)elapsed_ns)/1000000000.0;
	printf("Elapsed (s): %lf\n",elapsed_s);

	return 0;
}

