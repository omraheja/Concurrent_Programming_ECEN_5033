




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


	FG_BST_Node *root = NULL;
	root = insert(100,10,root);
	root = insert(200,15,root);
	root = insert(50,20,root);
	root = insert(150,30,root);
	root = insert(25,40,root);
	inorder_traversal(root);



	/* Initialize the mutex lock */
	int rc = pthread_mutex_init(&tree_lock,NULL);
	if(rc != 0)
	{
		printf("[ERROR]:Mutex Initialization Failed!\n");
		exit(-1);
	}



	/* Free the resources allocated on mutex initialization */
	rc = pthread_mutex_destroy(&tree_lock);
	if(rc != 0)
	{
		printf("[ERROR]:Mutex Destroy Failed!\n");
	}
	return 0;
}
