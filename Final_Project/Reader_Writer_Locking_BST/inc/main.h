#ifndef __MAIN_H__
#define __MAIN_H_

/*@Author       : Om Raheja
 *@File Name    : main.h
 *@Date         : 12/9/2019
 *@Tools        : Compiler:g++ ; Editor: Vim
 *@Brief        : This file includes all the standard c library header files
 		  and function prototypes of the function used in main.cpp
 * */

/* Standard C Library Headers */
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <vector>
#include <iostream>
using namespace std;


/*@Function Name: thread_func
 *@Brief	: All threads after their creation enter into this function.
 		  This function runs a for loop from 0 until the number of
		  iterations provided by the user. Inside the for loop, keys
		  and values are randomly generated using the rand() function.
		  These randomly genrated keys and values are used to build the
		  fine-grained locked BST by using insert() and search() functions.
 *@Param in	: void *args (This is the only argument passed to the thread_func)
 *@Return 	: void
 * */
void *thread_func(void *args);

#endif
