#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <atomic>
#include <assert.h>
#include <string.h>
#include <getopt.h>

using namespace std;

/* Global variables */
int NUM_THREADS;		//stores number of threads
int NUM_ITERATIONS;		//stores number of iterations
int TEST_NUM = 0;		//stores which function to invoke
int counter=0;			//variable to be incremented by each thread
char output_file_name[20];	//stores output file name
char input_file_name[20];	//stores input file name


atomic<int> atomic_counter;
struct timespec start, end;	//stores start time and end time



/* Synchronization variables */                                                                                                                                                                          
pthread_mutex_t lock;
atomic<bool> tas_lock;





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

void cnt_ticket_lock(int tid)
{
	printf("Ticket lock to be implemented\n");
}

void cnt_mcs_lock(int tid)
{

	printf("MCS lock to be implemented\n");
}


const int NUM_FUNCS = 7;
void (*funcs[NUM_FUNCS])(int)  = {
	empty,				//0 -> empty
	print,				//1 -> print
	cnt_lock,			//2 -> pthread_mutex_lock and pthread_mutex_unlock
	cnt_tas_lock,			//3 -> test and set lock
	cnt_test_and_tas_lock,		//4 -> test and test and set lock
	cnt_ticket_lock,		//5 -> ticket lock
	cnt_mcs_lock			//6 -> mcs lock
};

const char* func_names[NUM_FUNCS] = {
	"empty",
	"print",
	"cnt_lock",
	"cnt_tas_lock",
	"cnt_test_and_tas_lock",
	"cnt_ticket_lock",
	"cnt_mcs_lock"
};





/* Main Function */
int main(int argc, char *argv[])
{
	int option_index = 0;
	int opt;
	int ret;		//to check return status
	char lock_selected[10];	//to store lock selected
	bool b = false;
	bool l = false;

	struct option long_options[] = {
		{ "name", no_argument, NULL,'n'},
		{ "output",required_argument, NULL,'o'},
		{ "threads",required_argument,NULL,'t' },
		{ "iterations",required_argument,NULL,'i'},
		{ "bar",required_argument,NULL,'b'},
		{ "lock",required_argument,NULL,'l'},
		{ 0, 0, 0, 0}
	};

	char optstring[25] = "no:t:i:bar:lock:";

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
				NUM_ITERATIONS = atoi(optarg);
				printf("Number of iterations: %d\n",NUM_ITERATIONS);
				break;

			case 'b':
				/* Condition to check if barrier were specified in the command line arguments first */
				if((b == false) && (l == false))
				{
					b = true;
					printf("Barrier selected = %s\n",optarg);
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

	return 0;
}

