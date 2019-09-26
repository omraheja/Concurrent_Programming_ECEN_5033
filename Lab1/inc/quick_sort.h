#ifndef QUICK_SORT_H
#define QUICK_SORT_H

/*@Author       : Om Raheja
 *@File Name    : quick_sort.h
 *@Date         : 9/2/2019
 *@Tools        : Compiler: g++ ; Editor: Vim
 */

/* Standard C Library Header */
#include <stdio.h>

/*@Function Name: partition
 *@Brief        : Takes the pivot (last element) and places it in correct position.
		: All elements less than or equal to pivot are placed to its left.
	       	: All elements greater than pivot are placed to the right of it.	
 *@Param in[1]	: int array[] (Sorted Array)
 *@Param in[2]	: int p		(starting index)
 *@Param in[3]	: int r		(ending index)
 *@Return       : int
 * */
int partition(int *array,int p ,int r);

/*@Function Name: quick_sort
 *@Brief        : This function implements the quick sort
 *@Param in[1]	: int array[]	(Sorted Array)
 *@Param in[2]	: int p		(starting index)
 *@Param in[3]	: int r		(ending index)
 *@Return       : void
 * */
void quick_sort(int *array,int p,int r);

#endif
