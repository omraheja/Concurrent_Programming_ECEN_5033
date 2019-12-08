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
BST_Node *g_root = NULL;		//Root of the BST
size_t *args;				//Variable to be passed to each thread
pthread_t *threads;			//Thread variable
extern int dup;				//variable to count number of duplicate inserts
pthread_mutex_t dup_lock;		//Mutex lock to increment count of duplicate occurances
int find_key[50];			//Array to store 50 keys to find in the BST
int num_threads = 0;			//Stores number of threads [Command line argument]
int fine_grain_lock = 0;		//Made 1 when fine grain locking is selected
int reader_writer_lock = 0;		//Made 1 when reader-writer lock is selected
int iterations_per_thread = 0;		//Stores number of times each thread will perform either insert,search of range query operation



int init_suite(void)
{
//	printf("Initializing Suite...\n");
	return 0;
}



int clean_suite(void)
{
//	printf("Cleaning Suite...\n");
	return 0;
}


void test_create_new_node(void)
{
	CU_ASSERT_NOT_EQUAL(g_root = create_new_node(50,100),NULL);
}


void test_insert(void)
{
        insert(10,100,g_root);
	insert(20,200,g_root);
        insert(30,300,g_root);
        insert(40,400,g_root);
        insert(60,600,g_root);
	insert(70,700,g_root);
	insert(80,800,g_root);
	insert(90,900,g_root);
}


void test_search(void)
{
	BST_Node* test_node = NULL;
	
	test_node = search(10,g_root);
	CU_ASSERT(test_node->value == 100);
	
	test_node = search(20,g_root);
	CU_ASSERT(test_node->value == 200);
       
	
	test_node = search(30,g_root);
	CU_ASSERT(test_node->value == 300);
	
	
	test_node = search(40,g_root);
	CU_ASSERT(test_node->value == 400);
	

	test_node = search(60,g_root);
	CU_ASSERT(test_node->value == 600);
	
	test_node = search(70,g_root);
	CU_ASSERT(test_node->value == 700);
       
	
	test_node = search(80,g_root);
	CU_ASSERT(test_node->value == 800);
	
	
	test_node = search(90,g_root);
	CU_ASSERT(test_node->value == 900);

	test_node = search(150,g_root);
	CU_ASSERT_NOT_EQUAL(test_node,NULL);
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
	printf("Test Suites registered\n");
	return 0;
}



int main(void)
{
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
