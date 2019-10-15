#ifndef BUCKET_SORT_H
#define BUCKET_SORT_H

/*@Author       : Om Raheja
 *@File Name    : bucket_sort.h 
 *@Date         : 9/25/2019
 *@Tools        : Compiler: g++ ; Editor: Vim
 */


/* Standard C Library Headers */
#include <stdio.h>
#include <math.h>
#include <vector>


/* User defined header files */
#include "quick_sort.h"

/*@Function Name: buckt_insert
 *@Brief        : Insert the element from the array into an appropriate
 *@Param in[1]  : int *array_base_address (Base address of the array in which all
		  the elemnts to be sorted are present)
 *@Param in[2]  : int start_index	(index from which sorting starts) 
 *@Param in[3]	: int end_index		(index at which sorting ends)
 *@Param in[4]	: int tid		(thread number)
 *@Return       : void
 * */
void bucket_insert(int *array_base_address,int start_index,int end_index,int tid);

#endif

