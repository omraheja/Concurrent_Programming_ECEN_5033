/*@Author       : Om Raheja
 *@File Name    : print_on_console.c 
 *@Date         : 9/2/2019 
 *@Tools        : Compiler:gcc ; Editor: Vim
 */


/* User defined header file */
#include "../inc/print_on_console.h"


void print_on_console(int *array,int num_of_elements)
{
	printf("\nSorted  Array:\n");

	/* Loop for printing sorted array on the console */
	for(int i=0;i<num_of_elements;i++)
	{
		printf("%d\n",*(array+i));
	}

	printf("\n");
}
