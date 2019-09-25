#ifndef BUCKET_SORT_H
#define BUCKET_SORT_H

/*@Author       : Om Raheja
 *@File Name    : bucket_sort.h 
 *@Date         :  
 *@Tools        : Compiler:gcc ; Editor: Vim
 */


/* Standard C Library Headers */
#include <stdio.h>
#include <vector>

/*@Function Name: buckt_sort
 *@Brief        :
 *@Param in[1]  : int array[]           (Sorted Array)
 *@Param in[2]  : int element_count     (Number of elements in the array)
 *@Return       : void
 * */
void bucket_sort(std::vector<int> bucket);

#endif

