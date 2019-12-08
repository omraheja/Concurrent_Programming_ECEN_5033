/*@Author       : Om Raheja
 *@File Name    : main.cpp 
 *@Date         : 12/9/2019 
 *@Tools        : Compiler:g++ ; Editor: Vim
 *@References	: https://github.com/swapnil-pimpale/lock-free-BST
 		: https://github.com/VasuAgrawal/418FinalProject
 		: The above codes were referred for implementing the
 		  Concurrent Tree. The logic and explanation of the code
 		  pieces used and referred from the above resources are 
		  explained in detail in the comment sections.
 */

/* User defined header files */
#include "main.h"
#include "FG_Locking_BST.h"
#include <bits/stdc++.h> 

/* Defines */
#define MODULO		65535
#define MAX_SIZE	1200

/* Global variables */
pthread_mutex_t tree_lock;		//Mutex lock to lock the root of the BST
BST_Node *g_root = NULL;		//Root of the BST
size_t *args;				//Variable to be passed to each thread
pthread_t *threads;			//Thread variable
extern int dup;				//variable to count number of duplicate inserts
pthread_mutex_t dup_lock;		//Mutex lock to increment count of duplicate occurances
int find_key[MAX_SIZE]={0};		//Array to store MAX_SIZE keys to find in the BST
int find_value[MAX_SIZE]={0};		//Array to store MAX_SIZE keys to verify insert functionality
int num_threads = 0;			//Stores number of threads [Command line argument]
int fine_grain_lock = 0;		//Made 1 when fine grain locking is selected
int reader_writer_lock = 0;		//Made 1 when reader-writer lock is selected
int iterations_per_thread = 0;		//Stores number of times each thread will perform either insert,search of range query operation
std::vector<BST_Node *> bst_vector;



/* Each threads entry point after creation */
void *thread_func(void *args)
{
	size_t tid = *((size_t *)args);		//Extract the thread id
	int key;				//Stores key to be inserted
	int value;				//Stores value to be inserted

	/* Loop to insert, search and range query multiple key-values */
	for(int i=0;i<iterations_per_thread;i++)
	{
		key = rand() % MODULO;					//Generate random keys between 0-65535
		find_key[tid] = key;					//Store the keys in an array
		value = rand() % MODULO;				//Store the value in an array
		find_value[tid] = value;				
		printf("[THREAD_FUNC]: [Thread id %ld] [Key inserted = %d] [Value inserted = %d]\n",tid,key,value);
		insert(find_key[tid],value,NULL);
		BST_Node* return_node = search(find_key[tid],NULL);
		if(return_node->value == find_value[tid])
		{
			printf("SUCCESS: KEY %d FOUND\n",find_value[tid]);
		}

		/* If thread id is a multiple of 3, it will search after inserting elements */
		if(tid % 3 == 0)
		{
			search(find_key[tid%num_threads],NULL);
		}

		if(tid == (num_threads - 1))
		{
			printf("[RANGE_QUERY] [THREAD ID = %zu] [RANGE %d - %d]\n",tid,find_key[MAX_SIZE-i],find_key[i]);
			range(find_key[i],find_key[num_threads - i],NULL,tid);
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
		{"help",no_argument,NULL,'h'},
		{0,0,0,0}
	};

	char optstring[20] = "l:i:t:h";

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

			case 'h':
				printf("\t\t\tCONCURRENT TREE HELP SECTION\n");
				printf("1. -t = <number of threads> Specify the number of threads\n");
				printf("2. -i = <number of iterations> Specify the number of iterations for insert/search/range query\n");
				printf("3. -l = <lock selection> Specify locking mechanism to be used, 'fine-grain' or 'reader-writer'\n\n");
				printf("\t\t\tINSTRUCTIONS TO RUN THE CODE\n>>  To run the program,type 'make all'\n");
				printf(">>  ./Concurrent_tree -t [number_of_threads] -i [number_of_iterations] -l [lock]\n\n");
				exit(1);
				break;

			default:
				printf("[ERROR]: Invalid arguments, type './concurrent_tree --help' for help'\n");
				break;
		}
	}

	/* If number of arguments are less than two, throw an error */
	if(argc <= 2)
	{
		printf("[ERROR]: Invalid arguments, type './concurrent_tree --help' for help'\n");
		exit(-1);
	}

	/* Allocate resources for threads */
	threads = (pthread_t *)malloc(num_threads*sizeof(pthread_t));
	args    = (size_t *)malloc(num_threads*sizeof(size_t));

#if 0
	std::vector<int>test;
	
	for(int i=0;i<10;i++)
	{
		test.insert(test.begin() + i, i*5);
		printf("Test[%d] = %d\n",i,test[i]);
	}

	for(int i=0;i<10;i++)
	{
		printf("Test[%d] = %d\n",i,test[i]);
	}
#endif

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
//		printf("Creating Thread = %zu\n",args[i]);
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
//		printf("Joined thread %zu\n",i);
	}

	printf("********************************INORDER TRAVERSAL START******************************************\n");
	inorder_traversal(g_root);
	printf("********************************INORDER TRAVERSAL END******************************************\n");


#if 0
	for(int i=0;i<50;i++)
	{
		printf("find_key[%d] = %d\n",i,find_key[i]);
	}

	if(find_key[0] < find_key[num_threads-1])
	{
		printf("<<<Thread Id = 0>>> <<<Range %d - %d>>>\n",find_key[0],find_key[num_threads-1]);
		range(find_key[0],find_key[num_threads-1],NULL,0);
	}
	else
	{
		printf("<<<Thread Id = 0>>> <<<Range %d - %d>>>\n",find_key[num_threads-1],find_key[0]);
		range(find_key[num_threads-1],find_key[0],NULL,0);

	}
#endif

	printf("[LOG_INFO]:Number of duplicates = %d\n",dup);

	/* Free the resources allocated on mutex initialization */
	rc = pthread_mutex_destroy(&tree_lock);
	if(rc != 0)
	{
		printf("[ERROR]:Mutex Destroy Failed!\n");
	}
	return 0;
}
