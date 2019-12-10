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


/* Defines */
#define MODULO		65535
#define MAX_SIZE	1200
#define DEBUG		0
#define LOW_CONTENTION	10000
#define HIGH_CONTENTION	10000

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
bool low_contention_flag = false;	//condition variable for low contention
bool high_contention_flag = false;	//condition variable for high_contention
struct timespec start,stop;		//Stores start time and end time





/*-----------------------------------------------------------------------------
 *@Function: delete_bst
 *-----------------------------------------------------------------------------
 *@brief: De-allocate memory allocated for BST nodes 
 *-----------------------------------------------------------------------------
 *@Reference: https://www.geeksforgeeks.org/write-a-c-program-to-delete-a-tree/
 *-----------------------------------------------------------------------------
 *@returns: void
 *-----------------------------------------------------------------------------
 * */
void delete_bst(BST_Node* root)
{
	if(root == NULL)
	{
		return;
	}

	delete_bst(root->left);
	delete_bst(root->right);
#if DEBUG
	printf("Deleting Node: %d\n",root->key);
#endif
	/* Free the resources allocated on mutex initialization */
        int rc = pthread_mutex_destroy(&root->lock);
        if(rc != 0)
        {
                printf("[ERROR]:Mutex Destroy Failed!\n");
        }

	if(root->left != NULL)
	{
		free(root->left);
	}
	if(root->right != NULL)
	{
		free(root->right);
	}
}






/*-----------------------------------------------------------------------
 *@Function: high_contention_test
 *-----------------------------------------------------------------------
 *@brief: Multiple threads try to contest to search a particular key. 
 *	  This is done to demonstrate maximum contention. The key chosen
 *	  to be searched is 500 in this case. 
 *	  *NOTE: 500 has been chosen as an arbitray value to demonstrate
 *	  high contention case between multiple threads. Any other value
 *	  can also be chosen. 
 *-----------------------------------------------------------------------
 *@returns: void
 *-----------------------------------------------------------------------
 * */
void *high_contention_test(void *args)
{
	size_t tid = *((size_t *)args);         //Extract the thread id
#if DEBUG
	printf("[HIGH CONTENTION TEST]: Thread ID = %zu\n",tid);
#endif
	/* For loop to iterate for the number of times the user mentions and
	 * let all threads contest for a particular key in the concurrent 
	 * tree */
	for(int i=0;i<iterations_per_thread;i++)
	{
		//search(500,NULL);		//Search Key=500
	
		if(search(500,NULL))		//Search Key=500
		{
			printf("Key Found\n");
		}
		else
		{
			printf("Key Not Found\n");
		}
	}
}


/*-----------------------------------------------------------------------
 *@Function: low_contention_test
 *-----------------------------------------------------------------------
 *@brief: Multiple threads try to contest to search a random keys. 
 *	  This is done to demonstrate minimum contention. The key chosen
 *	  to be searched is generated using a random number generator
 *	  function in this case. 
 *	  *NOTE: The random number generated has been bounded between
 *	  0-65535. Any other range can also be examined. 
 *-----------------------------------------------------------------------
 *@returns: void
 *-----------------------------------------------------------------------
 * */

void *low_contention_test(void *args)
{
	size_t tid = *((size_t *)args);         //Extract the thread id
#if DEBUG
	printf("[LOW CONTENTION TEST]: Thread ID = %zu\n",tid);
#endif

	/* For loop to iterate for the number of times the user mentions and
	 * let all threads contest for random keys in the concurrent tree */
	for(int i=0;i<iterations_per_thread;i++)
	{
		search(rand()%MODULO,NULL);	//Search random keys between 0-65535
	}

}


/*------------------------------------------------------------------------
 *@Function: thread_func
 *------------------------------------------------------------------------
 *@brief: All threads enter this thread handler if none of the low or high
 *	  contention flag is set in the command line arguments. This
 *	  function is used to do the following 3 operations: insert,search
 *	  and finf range between the two keys provided. 
 *------------------------------------------------------------------------
 *@returns: void
 *------------------------------------------------------------------------
 * */
void *thread_func(void *args)
{
	size_t tid = *((size_t *)args);		//Extract the thread id
	int key;				//Stores key to be inserted
	int value;				//Stores value to be inserted

	/* Loop to insert, search and range query multiple key-values */
	for(int i=0;i<iterations_per_thread;i++)
	{
		key = rand() % MODULO;		//Generate random keys between 0-65535

		find_key[tid] = key;		//Store the keys in an array

		value = rand() % MODULO;	//Generate random values to be inserted

		find_value[tid] = value;	//Store the value in an array

#if DEBUG		
		printf("[THREAD_FUNC]: [Thread id %ld] [Key inserted = %d] [Value inserted = %d]\n",tid,key,value);
#endif	
		insert(find_key[tid],value,NULL);	//Insert keys in BST

		BST_Node* return_node = search(find_key[tid],NULL);	//Search the inserted key in the concurrent Tree


		/* The search() function will return the node with identical key
		 * Check if the value in the node is the same as stored
		 * If value for the keys match, then the key is successfully
		 * found, else the search() operation fails
		 * */
		if(return_node->value == find_value[tid])
		{
#if DEBUG
			printf("SUCCESS: KEY = %d FOUND, VALUE = %d\n",return_node->key,find_value[tid]);
#endif
		}

		/* If thread id is a multiple of 3, it will search after inserting elements */
		if(tid % 3 == 0)
		{
			search(find_key[tid%num_threads],NULL);
		}


#if 0
		/* Last thread will call range query */
		if(tid == (num_threads - 1))
		{
			if(find_key[tid] < find_key[i])
			{
				printf("[FIND_RANGE] [THREAD ID = %zu] [RANGE %d - %d]\n",tid,find_key[tid],find_key[i]);
			}
			else if(find_key[tid] > find_key[i])
			{
				printf("[FIND_RANGE] [THREAD ID = %zu] [RANGE %d - %d]\n",tid,find_key[i],find_key[tid]);
			}
			range(find_key[i],find_key[num_threads - i],NULL,tid);
		}
#endif


		/* Last thread will call the range query function */
		if(tid == (num_threads - 1))
		{
			if(find_key[tid] < find_key[0])
			{
#if DEBUG
				printf("[FIND_RANGE] [THREAD ID = %zu] [RANGE %d - %d]\n",tid,find_key[tid],find_key[0]);
#endif
				range(find_key[0],find_key[num_threads - 1],NULL,tid);

			}
			else if(find_key[tid] > find_key[0])
			{
#if DEBUG
				printf("[FIND_RANGE] [THREAD ID = %zu] [RANGE %d - %d]\n",tid,find_key[0],find_key[tid]);
#endif
				range(find_key[0],find_key[num_threads - 1],NULL,tid);
			}
			else
			{
				printf("Key1 %d = Key2 (%d), No node between them\n",find_key[0],find_key[num_threads-1]);
			}
			//range(find_key[0],find_key[num_threads - 1],NULL,tid);
		}

	}

#if 0

	/* Last thread will call the range query function */
	if(tid == (num_threads - 1))
	{
		if(find_key[tid] < find_key[0])
		{
#if DEBUG
			printf("[FIND_RANGE] [THREAD ID = %zu] [RANGE %d - %d]\n",tid,find_key[tid],find_key[0]);
#endif
		}
		else if(find_key[tid] > find_key[0])
		{
#if DEBUG
			printf("[FIND_RANGE] [THREAD ID = %zu] [RANGE %d - %d]\n",tid,find_key[0],find_key[tid]);
#endif
		}
		range(find_key[0],find_key[num_threads - 1],NULL,tid);
	}

#endif
}



/*------------------------------------------------------------------------
 *@Function: main
 *------------------------------------------------------------------------
 *@brief: Takes command line arguments, spawns threads and waits for them
 *	  to join. Calculates the time elapsed by the code and various 
 *	  operations.
 *------------------------------------------------------------------------
 *@returns: 0 on success
 *------------------------------------------------------------------------
 * */
int main(int argc,char* argv[])
{
	printf("\n***********************************************************FINE-GRAIN LOCKING BST***********************************************************\n");
	int opt;
	int rc;
	int option_index = 0;

	struct option long_options[] = {
		{"iterations",required_argument,NULL,'i'},
		{"num_of_threads",required_argument,NULL,'t'},
		{"help",no_argument,NULL,'m'},
		{"low_contention",no_argument,NULL,'l'},
		{"high_contention",no_argument,NULL,'h'},
		{0,0,0,0}
	};

	char optstring[20] = "i:t:mlh";

	srand(time(NULL));

	while((opt = getopt_long(argc,argv,optstring,long_options,&option_index))!= -1)
	{
		if(opt == -1)
		{
			break;
		}


		switch(opt)
		{
			/* Iterations per thread  */
			case 'i':
				iterations_per_thread = atoi(optarg);
				printf("Number of iterations per thread = %d\n",iterations_per_thread);
				break;

				/* Number of threads */
			case 't':
				num_threads = atoi(optarg);
				printf("Number of threads = %d\n",num_threads);
				break;

				/* Help option */
			case 'm':
				printf("\t\t\tCONCURRENT TREE HELP SECTION\n");
				printf("1. -t = <number of threads> Specify the number of threads\n");
				printf("2. -i = <number of iterations> Specify the number of iterations for insert/search/range query\n");
				printf("3. -l = <lock selection> Specify locking mechanism to be used, 'fine-grain' or 'reader-writer'\n\n");
				printf("\t\t\tINSTRUCTIONS TO RUN THE CODE\n>>  To run the program,type 'make all'\n");
				printf(">>  ./Concurrent_tree -t [number_of_threads] -i [number_of_iterations] -l [lock]\n\n");
				exit(1);
				break;

				/* Set low contention flag to test low contention between multiple threads */
			case 'l':
				if(high_contention_flag == true)
				{
					low_contention_flag = false;
				}
				else
				{
					printf("LOW CONTENTION TEST\n");
					low_contention_flag = true;
				}
				break;

				/* Set high contention flag to test high contention between multiple threads */
			case 'h':
				if(low_contention_flag == true)
				{
					high_contention_flag = false;
				}
				else
				{
					printf("HIGH CONTENTION TEST\n");
					high_contention_flag = true;
				}
				break;


				/* Default case */
			default:
				printf("[ERROR]: Invalid arguments, type './concurrent_tree --help' for help'\n");
				break;
		}
	}

	/* If number of arguments are less than two, throw an error */
	if(argc < 2)
	{
		printf("[ERROR]: Invalid arguments, type './concurrent_tree --help' for help'\n");
		exit(-1);
	}


	/* Allocate resources for threads */
	threads = (pthread_t *)malloc(num_threads*sizeof(pthread_t));
	args    = (size_t *)malloc(num_threads*sizeof(size_t));


	/* Initialize the mutex lock */
	rc = pthread_mutex_init(&tree_lock,NULL);
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

	/************************************************************************************************************************/
	/*					INSERT | SEARCH | RANGE QUERY OPERATION 					*/
	/************************************************************************************************************************/

	/* If none of the low or high contention cases are being tested */
	if((low_contention_flag == false) && (high_contention_flag == false))
	{
		clock_gettime(CLOCK_MONOTONIC,&start);		//Start the timer to calculate time taken for high contention test
		
		/* Create Number of threads mentioned in command line argument */
		for(size_t i =0;i<num_threads;i++)
		{
			args[i] = i;		//store argument array to be passed to thread handler
			//printf("Creating Thread = %zu\n",args[i]);
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
			//printf("Joined thread %zu\n",i);
		}

		clock_gettime(CLOCK_MONOTONIC,&stop);		//Start the timer to calculate time taken for high contention test
	}


	/************************************************************************************************************************/
	/*						HIGH CONTENTION TEST		 					*/
	/************************************************************************************************************************/
	/* Check high contention test */
	if(high_contention_flag == true)
	{
		printf("HIGH CONTENTION TEST\n");

		/* Create a tree
		 * This tree will be used as a base case for testing
		 * the high contention and low contention between
		 * multiple threads
		 * */
		int nodes = HIGH_CONTENTION;

		while(nodes != 100)
		{
			insert(nodes,rand()%65535,NULL);
			nodes--;
		}

		clock_gettime(CLOCK_MONOTONIC,&start);		//Start the timer to calculate time taken for high contention test

		/* Create Number of threads mentioned in command line argument */
		for(size_t i =0;i<num_threads;i++)
		{
			args[i] = i;
			//printf("Creating Thread = %zu\n",args[i]);
			rc = pthread_create(&threads[i],NULL,&high_contention_test,&args[i]);

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
			//printf("Joined thread %zu\n",i);
		}
		clock_gettime(CLOCK_MONOTONIC,&stop);		//Stop timer

	}


	/************************************************************************************************************************/
	/*                                              LOW CONTENTION TEST                                                     */
	/************************************************************************************************************************/
	if(low_contention_flag == true)
	{
		printf("LOW CONTENTION TEST\n");

		/* Create a tree
		 * This tree will be used as a base case for testing
		 * the high contention and low contention between
		 * multiple threads
		 * */
		int nodes = LOW_CONTENTION;
		while(nodes != 100)
		{
			insert(nodes,rand()%65535,NULL);
			nodes--;
		}

		clock_gettime(CLOCK_MONOTONIC,&start);

		/* Create Number of threads mentioned in command line argument */
		for(size_t i =0;i<num_threads;i++)
		{
			args[i] = i;
			//printf("Creating Thread = %zu\n",args[i]);
			rc = pthread_create(&threads[i],NULL,&low_contention_test,&args[i]);

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
			//printf("Joined thread %zu\n",i);
		}

		clock_gettime(CLOCK_MONOTONIC,&stop);
	}

	printf("********************************INORDER TRAVERSAL START******************************************\n");
	inorder_traversal(g_root);
	printf("********************************INORDER TRAVERSAL END******************************************\n");

	printf("[LOG_INFO]:Number of duplicates = %d\n",dup);

	/* Free the resources allocated on mutex initialization */
	rc = pthread_mutex_destroy(&tree_lock);
	if(rc != 0)
	{
		printf("[ERROR]:Mutex Destroy Failed!\n");
	}

	
	/* Free the resources allocated on mutex initialization */
        rc = pthread_mutex_destroy(&dup_lock);
        if(rc != 0)
        {
                printf("[ERROR]:Mutex Destroy Failed!\n");
        }



	/* Perform basic operations to display time calculated in seconds and nanpseconds */
	unsigned long long elapsed_ns;
	elapsed_ns = (stop.tv_sec-start.tv_sec)*1000000000 + (stop.tv_nsec- start.tv_nsec);
	printf("Elapsed (ns): %llu\n",elapsed_ns);
	double elapsed_s = ((double)elapsed_ns)/1000000000.0;
	printf("Elapsed (s): %lf\n",elapsed_s);



	delete_bst(g_root);

	free(g_root);
	free(threads);
	free(args);

	return 0;
}
