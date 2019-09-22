#ifndef PRINT_IN_FILE_H
#define PRINT_IN_FILE_H

/*@Author       : Om Raheja
 *@File Name    : print_in_file.h 
 *@Date         : 9/2/2019 
 *@Tools        : Compiler:gcc ; Editor: Vim
 */


/* Standard C Library Headers */
#include <stdio.h>


/*@Function Name: print_in_file
 *@Brief        : Takes the array, number of elements in the array and the filename as
 		: the input and prints the sorted array into the output file.
 *@Param in[1]	: int array[]		(Sorted Array)
 *@Param in[2]	: int element_count	(Number of elements in the array)
 *@Param in[3]	: char *filename	(Output File)
 *@Return       : void
 * */
void print_in_file(int array[],int element_count,char *filename);

#endif
