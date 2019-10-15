/*@Author       : Om Raheja
 *@File Name    : quick_sort.c 
 *@Date         : 9/2/2019 
 *@Tools        : Compiler: g++ ; Editor: Vim
 *@References   : The concept for Quick Sort Algorithm was referenced from https://www.youtube.com/watch?v=3DV8GO9g7B4
 */


/* User defined header file */
#include "../inc/quick_sort.h"

int partition(int *array,int p,int r)
{
	int last_element;
	int temp;
	int i;

	last_element = *(array+r);		//Store last element in a varible (This is the Pivot)

	i = p-1;

	/* Quick sort algorithm */
	for(int j=p;j<=(r-1);j++)
	{
		if(*(array+j) < last_element)
		{
			i++;
			temp = *(array+i);
			*(array+i) = *(array+j);
			*(array+j) = temp;
		}

	}

	/* Assign the last element of the partition in its correct position */
	temp = *(array+r);
	*(array+r) = *(array+i+1);
	*(array+i+1) = temp;

	return (i+1);

}


void quick_sort(int *array,int p,int r)
{
	int q;
	if(p < r)
	{
		q = partition(array,p,r);
		quick_sort(array,p,q-1);
		quick_sort(array,q+1,r);

	}

}
