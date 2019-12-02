/* Standard C Library Headers */
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include <time.h>

/* User defined header files */
#include "main.h"
#include "FG_Locking_BST.h"


/* Global variables */
pthread_mutex_t tree_lock;
FG_BST_Node *g_root = NULL;

extern int dup;

size_t *args;

pthread_mutex_t dup_lock;


void *thread_fun_1(void *args)
{
	size_t tid = *((size_t*)args);

	for(int i=0;i<2555;i++)
	{
		insert(rand()%2555, rand()%2555,NULL);
	}
#if 0
	insert(100,10,NULL);
	insert(200,15,NULL);
	insert(300,20,NULL);
	insert(400,30,NULL);
	insert(90,40,NULL);
	insert(97,10,NULL);
        insert(685,15,NULL);
        insert(125,20,NULL);
        insert(100,30,NULL);
        insert(92,40,NULL);
	insert(49,10,NULL);
	insert(64,15,NULL);
	insert(121,20,NULL);
	insert(144,30,NULL);
	insert(169,40,NULL);
	insert(196,10,NULL);
        insert(225,15,NULL);
        insert(256,20,NULL);
        insert(289,30,NULL);
        insert(215,40,NULL);
#endif
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
	insert(81,10,NULL);
        insert(3,15,NULL);
        insert(16,20,NULL);
        insert(25,30,NULL);
        insert(36,40,NULL);

}

void *thread_fun_3(void *args)
{
	size_t tid = *((size_t*)args);
	//search(100,NULL);
	//search(200,NULL);

}

void *thread_fun_4(void *args)
{

	size_t tid = *((size_t*)args);
	//search(80,NULL);
	//search(70,NULL);
	//printf("/**********************************************************Range between keys************************************************/\n");
        //range_query(3,685,NULL);
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

	srand(time(NULL));
	
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





	 /* Initialize the mutex lock */
        rc = pthread_mutex_init(&dup_lock,NULL);
        if(rc != 0)
        {
                printf("[ERROR]:Mutex Initialization Failed!\n");
                exit(-1);
        }

	args = (size_t *)malloc(num_threads*sizeof(size_t));


	pthread_t thread_1;
	pthread_t thread_2;
	pthread_t thread_3;
	pthread_t thread_4;

	args[1]=1;
	args[2]=2;
	args[3]=3;
	args[4]=4;

	pthread_create(&thread_1,NULL,thread_fun_1,&args[1]);
	pthread_create(&thread_2,NULL,thread_fun_2,&args[2]);
	pthread_create(&thread_3,NULL,thread_fun_3,&args[3]);
	pthread_create(&thread_4,NULL,thread_fun_4,&args[4]);

	pthread_join(thread_1,NULL);
	pthread_join(thread_2,NULL);
	pthread_join(thread_3,NULL);
	pthread_join(thread_4,NULL);
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

	printf("Number of duplicates = %d\n",dup);
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
