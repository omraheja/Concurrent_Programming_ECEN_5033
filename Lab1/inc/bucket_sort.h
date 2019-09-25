#ifndef BUCKET_SORT_H
#define BUCKET_SORT_H

/*@Author       : Om Raheja
 *@File Name    : bucket_sort.h 
 *@Date         :  
 *@Tools        : Compiler:gcc ; Editor: Vim
 */


/* Standard C Library Headers */
#include <stdio.h>
#include <math.h>
#include <vector>

#include "quick_sort.h"

/*@Function Name: buckt_sort
 *@Brief        :
 *@Param in[1]  : int array[]           (Sorted Array)
 *@Param in[2]  : int element_count     (Number of elements in the array)
 *@Return       : void
 * */
void bucket_sort(std::vector<int> bucket);






void bucket_insert(int *array_base_address,int start_index,int end_index,int tid);

#endif

