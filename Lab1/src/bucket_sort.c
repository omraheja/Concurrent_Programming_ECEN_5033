/*@Author       : Om Raheja
 *@File Name    : bucket_sort.c 
 *@Date         : 
 *@Tools        : Compiler:gcc ; Editor: Vim
 */


/* User defined header file */
#include "../inc/bucket_sort.h"
#include <pthread.h>

extern std::vector<std::vector<int>> buck;
extern int largest_element;
extern int num_of_threads;
extern pthread_mutex_t lock;


void bucket_sort(std::vector<int> bucket)
{
	printf("Hiiiiii\n");

}


void bucket_insert(int *array_base_address,int start_index,int end_index,int tid)
{
	printf("TID=%d\n",tid);
	printf("StartIndex = %d\n",start_index);
	printf("EndIndex = %d\n",end_index);
	printf("LARGEST ELEMENT = %d\n",largest_element);
	printf("NUMBER OF THREADS = %d\n",num_of_threads);

	for(int i=start_index;i<=end_index;i++)
	{
		int bucket_number = (int)floor(num_of_threads*(*(array_base_address+i))/largest_element);
	


		printf("Bucket Number = %d\n",bucket_number);
		 if(bucket_number == 0)
                        {
				buck[bucket_number].push_back(*(array_base_address + i));

                        }
                        else
                        {
				buck[bucket_number-1].push_back(*(array_base_address + i));

                        }
		//buck[bucket_number].push_back(*(array_base_address + i));
		//buck[tid].push_back(*(array_base_address + i));
	}

#if 0
	int *bucket_ptr = (int *)&buck[tid].front();

	printf("Bucket_ptr[%d] = %x\n",tid,&buck[tid].front());
	
	printf("BEFORE SORTING: ");
	for(int i = 0; i < buck[tid].size(); i++)
	{
		printf("%d ", *(bucket_ptr+i));
	}
	printf("\n");

	quick_sort(&buck[tid].front(),0,buck[tid].size()-1);

	 printf("AFTER SORTING: ");
        for(int i = 0; i < buck[tid].size(); i++)
        {
                printf("%d ", *(bucket_ptr+i));
        }
        printf("\n");
#endif

}



#if 0
                for(int i=0;i<count;i++)
                {
                        int bucket_number = (int)floor(num_of_threads*array[i]/largest_element);
                        //printf("Bucket Number = %d\n",bucket_number);

                        if(bucket_number == 0)
                        {
                                bucket[bucket_number].push_back(array[i]);
                                //printf("%d ",bucket[bucket_number]);
                        }
                        else
                        {
                                bucket[bucket_number-1].push_back(array[i]);
                                //printf("%d ",bucket[bucket_number-1]);
                        }
                }
#endif
