/* Standard C Library includes */
#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>
#include <CUnit/Automated.h>

/* User defined header files */
#include "../inc/main.h"
#include "../inc/FG_Locking_BST.h"

/* Global variables */
pthread_mutex_t tree_lock;		//Mutex lock to lock the root of the BST
FG_BST_Node *g_root = NULL;		//Root of the BST
size_t *args;				//Variable to be passed to each thread
pthread_t *threads;			//Thread variable
extern int dup;				//variable to count number of duplicate inserts
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
		key = rand()%65535;
		find_key[i%48] = key;
		value = rand()%65535;
		printf("[THREAD_FUNC]: Key inserted = %d\nValue inserted = %d\n",key,value);
		insert(key,value,NULL);

		/* If thread id is a multiple of 3, it will search after inserting elements */
		if(tid % 3 == 0)
		{
			search(find_key[i%num_threads],NULL);
		}
	}

}



int init_suite(void)
{
	printf("INIT_SUITE\n");
	num_threads = 50;			//Stores number of threads [Command line argument]
	fine_grain_lock = 1;			//Made 1 when fine grain locking is selected
	reader_writer_lock = 0;			//Made 1 when reader-writer lock is selected
	iterations_per_thread = 5;		//Stores number of times each thread will perform either insert,search of range query operation

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


	srand(time(NULL));

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

	return 0;
}





int clean_suite(void)
{

	/* Free the resources allocated on mutex initialization */
	int rc = pthread_mutex_destroy(&tree_lock);
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

	return 0;
}


void test_create_new_node(void)
{
	CU_ASSERT_NOT_EQUAL(g_root = create_new_node(123,100),NULL);
}


void test_insert(void)
{
        CU_ASSERT_NOT_EQUAL(g_root = create_new_node(123,100),NULL);
}


void test_search(void)
{
        CU_ASSERT_NOT_EQUAL(g_root = create_new_node(123,100),NULL);
}




int register_test_suite(void) {

	printf("Registering test suites\n");
	CU_pSuite pSuite = NULL;

	pSuite = CU_add_suite("Functionality Test of Concurrent Tree", init_suite, clean_suite);
	if (NULL == pSuite) {
		return -1;
	}
	/*Adding tests to the suite */
	if ((NULL == CU_add_test(pSuite, "create_new_node", test_create_new_node))  ||
	    (NULL == CU_add_test(pSuite, "insert", test_insert)) ||
	    (NULL == CU_add_test(pSuite, "search", test_search))) 
	{
		return -1;
	}
	return 0;
}



int main(void)
{
	printf("In Main\n");
	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	//register_test_suite will add a test suite to the CUnit test registry
	if (register_test_suite() == -1) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* Run all tests using CUnit Basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();

	/* Run all tests using the CUnit Automated interface */
	CU_set_output_filename("Concurrent_Tree_Test");
	CU_automated_run_tests();

	CU_cleanup_registry();
	return CU_get_error();
}
