/*@Author       : Om Raheja
 *@File Name    : print_in_file.c 
 *@Date         : 9/2/2019 
 *@Tools        : Compiler:gcc ; Editor: Vim
 */


/* User defined header file */
#include "../inc/print_in_file.h"


void print_in_file(int array[],int element_count,char *filename)
{
	FILE *fptr;			//File pointer
	
	fptr = fopen(filename,"w+");	//OPen the file
	
	/* Loop to print sorted array in the file */
	for(int i=0;i<element_count;i++)
	{
		fprintf(fptr,"%d\n",*(array + i));
	}

	fclose(fptr);			//Close file
}
