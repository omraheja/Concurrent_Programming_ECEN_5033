/*@Author       : Om Raheja
 *@File Name    : bucket_sort.c 
 *@Date         : 9/25/2019
 *@Tools        : Compiler: g++ ; Editor: Vim
 */


/* User defined header file */
#include "../inc/bucket_sort.h"
#include <pthread.h>

/* Global external variables */
extern std::vector<std::vector<int>> buck;
extern int largest_element;
extern int num_of_threads;
extern pthread_mutex_t lock;


void bucket_insert(int *array_base_address,int start_index,int end_index,int tid)
{
	/* For Debugging */
	//printf("TID=%d\n",tid);
	//printf("StartIndex = %d\n",start_index);
	//printf("EndIndex = %d\n",end_index);
	//printf("LARGEST ELEMENT = %d\n",largest_element);
	//printf("NUMBER OF THREADS = %d\n",num_of_threads);

	for(int i=start_index;i<=end_index;i++)
	{
		int bucket_number = (int)floor(num_of_threads*(*(array_base_address+i))/largest_element);



		if(bucket_number == 0)
		{
			buck[bucket_number].push_back(*(array_base_address + i));

		}
		else
		{
			buck[bucket_number-1].push_back(*(array_base_address + i));

		}
	}
}
