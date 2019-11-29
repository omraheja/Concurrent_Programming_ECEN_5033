/* Standard C Library Headers */
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

/* User defined header files */
#include "main.h"
#include "FG_Locking_BST.h"


/* Global variables */
pthread_mutex_t tree_lock;
FG_BST_Node *g_root = NULL;



size_t *args;

void *thread_fun_1(void *args)
{
	size_t tid = *((size_t*)args);


	insert(100,10,NULL);
	insert(200,15,NULL);
	insert(300,20,NULL);
	insert(400,30,NULL);
	insert(90,40,NULL);

}

void *thread_fun_2(void *args)
{

	size_t tid = *((size_t*)args);
	insert(80,1650,NULL);
	insert(70,1525,NULL);
	insert(60,2560,NULL);
	insert(50,230,NULL);
	insert(40,140,NULL);
	insert(32,120,NULL);
}

int main(int argc,char* argv[])
{
	int opt;
	int option_index = 0;
	int fine_grain_lock = 0;
	int reader_writer_lock = 0;
	int num_threads = 0;

	struct option long_options[] = {
		{"fg_lock",required_argument,NULL,'f'},
		{"rw_lock",required_argument,NULL,'r'},
		{"threads",required_argument,NULL,'t'},
		{0,0,0,0}
	};

	char *optstring = "f:r:t:";

	while((opt = getopt_long(argc,argv,optstring,long_options,&option_index))!= -1)
	{
		if(opt == -1)
		{
			break;
		}


		switch(opt)
		{
			case 'f':
				fine_grain_lock = atoi(optarg);
				if(fine_grain_lock == 1)
				{
					printf("Fine Grain Lock Selected\n");
				}
				break;

			case 'r':
				reader_writer_lock = atoi(optarg);
				if(reader_writer_lock == 1)
				{
					printf("Reader Writer Lock Selected\n");
				}
				break;

			case 't':
				num_threads = atoi(optarg);
				printf("Number of threads = %d\n",num_threads);
				break;
		}
	}


	/* Initialize the mutex lock */
	int rc = pthread_mutex_init(&tree_lock,NULL);
	if(rc != 0)
	{
		printf("[ERROR]:Mutex Initialization Failed!\n");
		exit(-1);
	}

	args = (size_t *)malloc(num_threads*sizeof(size_t));


	pthread_t thread_1;
	pthread_t thread_2;

	args[1]=1;
	args[2]=2;

	pthread_create(&thread_1,NULL,thread_fun_1,&args[1]);
	pthread_create(&thread_2,NULL,thread_fun_2,&args[2]);

	pthread_join(thread_1,NULL);
	pthread_join(thread_2,NULL);
	/*
	   insert(100,10,NULL);
	   insert(200,15,NULL);
	   insert(300,20,NULL);
	   insert(400,30,NULL);
	   insert(90,40,NULL);
	   insert(80,1650,NULL);
	   insert(70,1525,NULL);
	   insert(60,2560,NULL);
	   insert(50,230,NULL);
	   insert(500,140,NULL);
	   insert(32,120,NULL);
	   */
	inorder_traversal(g_root);

	/* Free the resources allocated on mutex initialization */
	rc = pthread_mutex_destroy(&tree_lock);
	if(rc != 0)
	{
		printf("[ERROR]:Mutex Destroy Failed!\n");
	}
	return 0;
}
