/* Standard C Library Headers */
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* User defined header files */
#include "main.h"
#include "FG_Locking_BST.h"


/* Global variables */
pthread_mutex_t tree_lock;		//Mutex lock to lock the root of the BST
FG_BST_Node *g_root = NULL;		//Root of the BST
size_t *args;				//Variable to be passed to each thread
pthread_t *threads;			//Thread variable
extern int dup;				//variable to count numbe of duplicate inserts
pthread_mutex_t dup_lock;		//Mutex lock to increment count of duplicate occurances
int find_key[50];			//Array to store 50 keys to find in the BST
int num_threads = 0;			//Stores number of threads [Command line argument]
int fine_grain_lock = 0;		//Made 1 when fine grain locking is selected
int reader_writer_lock = 0;		//Made 1 when reader-writer lock is selected
int iterations_per_thread = 0;		//Stores number of times each thread will perform either insert,search of range query operation


void *thread_func(void *args)
{
	size_t tid = *((size_t *)args);
	int key;
	int value;
	printf("[THREAD_FUNC]: tid = %ld\n",tid);
	
	for(int i=0;i<(num_threads*iterations_per_thread);i++)
	{
		key = rand()%rand();
		find_key[i%48] = key;
		value = rand()%rand();
		printf("[THREAD_FUNC]: Key inserted = %d\nValue inserted = %d\n",key,value);
		insert(key,value,NULL);
	
		/* If thread id is a multiple of 3, it will search after inserting elements */
		if(tid % 3 == 0)
		{
			search(find_key[i%num_threads],NULL);
		}
	}

}


int main(int argc,char* argv[])
{
	int opt;
	int option_index = 0;

	struct option long_options[] = {
		{"lock",required_argument,NULL,'l'},
		{"iterations",required_argument,NULL,'i'},
		{"num_of_threads",required_argument,NULL,'t'},
		{0,0,0,0}
	};

	char *optstring = "l:i:t:";

	srand(time(NULL));

	while((opt = getopt_long(argc,argv,optstring,long_options,&option_index))!= -1)
	{
		if(opt == -1)
		{
			break;
		}


		switch(opt)
		{
			case 'l':
				if(!strcmp(optarg,"fine-grain"))
				{	
					fine_grain_lock = 1;
				}
				if(!strcmp(optarg,"reader-writer"))
				{
					reader_writer_lock = 1;
				}

				if(fine_grain_lock == 1)
				{
					printf("Fine-Grain Lock Selected\n");
				}
				
				if(reader_writer_lock == 1)
				{
					printf("Reader-Writer Lock Selected\n");
				}

				break;

			case 'i':
				iterations_per_thread = atoi(optarg);
				printf("Number of iterations per thread = %d\n",iterations_per_thread);
				break;

			case 't':
				num_threads = atoi(optarg);
				printf("Number of threads = %d\n",num_threads);
				break;
		}
	}

	/* Allocate resources for threads */
	threads = (pthread_t *)malloc(num_threads*sizeof(pthread_t));
	args    = (size_t *)malloc(num_threads*sizeof(size_t));

	/* Initialize the mutex lock */
	int rc = pthread_mutex_init(&tree_lock,NULL);
	if(rc != 0)
	{
		printf("[ERROR]:Mutex Initialization Failed!\n");
		exit(-1);
	}

	/* Initialize the mutex lock */
	rc = pthread_mutex_init(&dup_lock,NULL);
	if(rc != 0)
	{
		printf("[ERROR]:Mutex Initialization Failed!\n");
		exit(-1);
	}


	/* Create Number of threads mentioned in command line argument */
	for(size_t i =0;i<num_threads;i++)
	{
		args[i] = i;
		printf("Creating Thread = %zu\n",args[i]);
		rc = pthread_create(&threads[i],NULL,&thread_func,&args[i]);

		if(rc)
		{
			printf("[ERROR]:Failed to create %d\n",rc);
			exit(-1);
		}
	}

	/* Join threads */
	for(size_t i=0;i<num_threads;i++)
	{
		rc = pthread_join(threads[i],NULL);
		if(rc)
		{
			printf("[ERROR]:pthread_join: %d\n",rc);
			exit(-1);
		}
		printf("Joined thread %zu\n",i);
	}





	inorder_traversal(g_root);

	printf("[LOG_INFO]:Number of duplicates = %d\n",dup);
	//	printf("/**********************************************************Range between keys************************************************/\n");
	//	range_query(3,685,NULL);

	/* Free the resources allocated on mutex initialization */
	rc = pthread_mutex_destroy(&tree_lock);
	if(rc != 0)
	{
		printf("[ERROR]:Mutex Destroy Failed!\n");
	}
	return 0;
}
