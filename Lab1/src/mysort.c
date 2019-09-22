/*@Author       : Om Raheja
 *@File Name    : mysort.c 
 *@Date         : 
 *@Tools        : Compiler:gcc ; Editor: Vim
 *@References	: 
 */

/* Standard C Library Headers */
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

/* User Defined Header Files */
#include "../inc/print_on_console.h"
#include "../inc/quick_sort.h"
#include "../inc/print_in_file.h"


/* Defines */
#define MAX_THREADS	(60)

/* Global variabeles */
static char *name = "Om Raheja";
int num_of_threads = 4;		//Default value of the number of threads
int part = 0;			//To keep track of the indices of the array
int count = 0;

/* Thread related global variables */
pthread_t *threads;
size_t *args;
pthread_barrier_t bar;

/* Timespec structures to calculate amount of time taken for algorithm to sort the array */
struct timespec start,end;

/* Global variable to store base address of the array */
int *array_base_addr;

/* Global variables required for index tracking */
int elem_per_thread = 0;
int low_index_for_last_thread = 0;

/* Global arrays to store high and low indexes for merging arrays */
int lower_index[MAX_THREADS];
int higher_index[MAX_THREADS];


void merge(int low,int mid,int high)
{
	int left_array[mid-low+1];
	int right_array[high-mid];
	int sizeof_left_array  = mid-low+1;
	int sizeof_right_array = high-mid;
	int i,j;

	/* Storing Values in Left Part */
	for(i=0;i<sizeof_left_array;i++)
	{
		left_array[i] = *(array_base_addr + i + low);
	}

	/* Storing Values in Right Part */
	for(i=0;i<sizeof_right_array;i++)
	{
		right_array[i] = *(array_base_addr + i + mid + 1);
	}

	int k = low;
	i=0;
	j=0;

	/* Merge Left and right in ascending order */
	while((i < sizeof_left_array) && (j < sizeof_right_array))
	{
		if(left_array[i] <= right_array[j])
		{
			*(array_base_addr + (k++)) = left_array[i++];
		}
		else
		{
			*(array_base_addr + (k++)) = right_array[j++];
		}
	}

	/* Insert remaining values from left */
	while(i < sizeof_left_array)
	{
		*(array_base_addr + (k++)) = left_array[i++];
	}

	/* Insert remaining values from right */
	while(j < sizeof_right_array)
	{
		*(array_base_addr + (k++)) = right_array[j++];
	}
}



/* Merge sort function */
void merge_sort(int low,int high)
{
	/* Calculate mid point of array */
	int mid = low + (high - low)/2;

	if(low < high)
	{
		/* Call 1st half */
		merge_sort(low,mid);

		/* Call 2nd half */
		merge_sort(mid+1,high);

		/* merge two halves */
		merge(low,mid,high);
	}
}



/* Thread Main */
int i=1;
void *thread_main(void *args)
{
	size_t tid = *((size_t*)args);

	int thread_part = part++;
	int low,high;

	pthread_barrier_wait(&bar);
	if(tid == 1)
	{
		clock_gettime(CLOCK_MONOTONIC,&start);
	}
	pthread_barrier_wait(&bar);

	printf("Number of elements = %d\n",count);
	printf("Number of threads =  %d\n",num_of_threads);
	elem_per_thread = count/num_of_threads;


	if(thread_part == (num_of_threads-1))
	{
		//low = low_index_for_last_thread;
		//low = (elem_per_thread*(num_of_threads-1));
		low  = thread_part * (count/num_of_threads);
		high = count - 1;
		lower_index[thread_part] = low;
		higher_index[thread_part] = high;
		//printf("THREAD_PART = %d\t NUM_OF_ELEM_SORTED = %d\t LOW = %d\t HIGH = %d\n",thread_part,high-low+1,low,high);
	}
	else
	{
		low  = thread_part * (count/num_of_threads);
		high = (thread_part + 1) * (count/num_of_threads) - 1;

		lower_index[thread_part] = low;
		higher_index[thread_part] = high;
		//printf("THREAD_PART = %d\t NUM_OF_ELEM_SORTED = %d\t LOW = %d\t HIGH = %d\n",thread_part,high-low+1,low,high);
	}

	printf("THREAD_PART = %d\t NUM_OF_ELEM_SORTED = %d\t LOW = %d\t HIGH = %d\n",thread_part,high-low+1,low,high);


	int mid  = low + (high - low)/2;

	if(low < high)
	{
		merge_sort(low,mid);
		merge_sort(mid+1,high);
		merge(low,mid,high);
	}

	printf("Thread %zu: %d ; Thread_Part = %d\n",tid,i++,thread_part);
	printf("Thread %zu reporting for duty\n",tid);

	pthread_barrier_wait(&bar);
	if(tid==1)
	{
		clock_gettime(CLOCK_MONOTONIC,&end);
	}

	return 0;
}





/* Main Function */
int main(int argc, char *argv[])
{
	int number;
	//int count = 0;
	int z = 0;
	int option_index = 0;
	int name_flag = 0;
	//int output_file_flag = 0;
	int opt;
	int ret;		//to check return status
	char *input_file    = (char*)malloc(20);
	char *output_file   = (char*)malloc(20);
	char *algo_selected = (char*)malloc(10);

	struct option long_options[] = {
		{ "name", no_argument, NULL,'n'},
		{ "output",required_argument, NULL,'o'},
		{ "threads",required_argument,NULL,'t' },
		{ "alg",required_argument,NULL,'a'},
		{ 0, 0, 0, 0}
	};

	char *optstring = "no:t:alg:";

	while((opt = getopt_long(argc, argv, optstring, long_options, &option_index))!= -1)
	{
		if (opt == -1)
			break;

		switch (opt) {
			case 'n':
				name_flag = 1;
				break;

			case 'o':
				//output_file_flag = 1;
				strcpy(output_file,optarg);
				printf("Output Filename = %s\n",output_file);
				break;

			case 't':
				num_of_threads = atoi(optarg);
				printf("Number of threads = %d\n",num_of_threads);
				break;

			case 'a':
				strcpy(algo_selected,optarg);
				printf("Algorithm selected: %s\n",algo_selected);
				break;

			default:
				printf("Usage: %s [...]\n", argv[0]);
				exit(1);
		}
	}

	strcpy(input_file,argv[optind]);
	printf("After all inputs are parsed, %s\n",input_file);
	printf("Number of threads = %d\n",num_of_threads);
	printf("Arg count = %d\n",argc);
	printf("Arg[0]= %s\n",argv[0]);
	printf("Arg[1]= %s\n",argv[1]);
	printf("Arg[2]= %s\n",argv[2]);
	printf("Arg[3]= %s\n",argv[3]);
	printf("Arg[4]= %s\n",argv[4]);
	printf("Arg[5]= %s\n",argv[5]);
	printf("Arg[6]= %s\n",argv[6]);


	if(name_flag == 1)
	{
		printf("Name: %s\n",name);
		exit(1);
	}

	FILE *input_file_ptr;				//input file pointer	
	input_file_ptr = fopen(input_file,"r+");	//open file

	/* Check if file opened successfully or not */
	if(input_file_ptr == NULL)
	{
		printf("Error in Reading File\n");
		exit(1);
	}

	/* Calculate the number of integers in the file */
	while(fscanf(input_file_ptr,"%d",&number)== 1)
	{
		count++;

	}

	fclose(input_file_ptr);           //close file

	printf("Number of elements = %d\n",count);
	int array[count];       //declare array to store the elements from the file

	array_base_addr = array;

	printf("Array Base Address: %x\n",array_base_addr);
	printf("Array: %x\n",array);


	/* Initialize all elements of the array to zero */
	for(int i=0;i<count;i++)
	{
		array[i] = 0;
	}

	/* Open file */
	input_file_ptr = fopen(input_file,"r+");

	/* Check if the file opened successfully or not */
	if(input_file_ptr == NULL)
	{
		printf("Error in Reading File\n");
		exit(1);
	}

	/* Store the elements in the array from the file */
	for(int i=0;i<count;i++)
	{
		fscanf(input_file_ptr,"%d", &number);
		array[z] = number;
		z++;
	}


	/* Print the elements of the array */
	for(int i=0;i<count;i++)
	{
		printf("Array[%d] = %d\n",i,array[i]);
	}


	/* Spawn the number of threads mentioned in the command line */
	threads = malloc(num_of_threads*sizeof(pthread_t));
	args    = malloc(num_of_threads*sizeof(size_t));
	pthread_barrier_init(&bar, NULL, num_of_threads);
	size_t i;


	for(i=1;i<num_of_threads;i++)
	{
		args[i] = i+1;
		printf("Creating thread: %zu\n",args[i]);
		/* Create threads */
		ret = pthread_create(&threads[i],NULL,&thread_main,&args[i]);
		/* Check if threads are created successfully */
		if(ret)
		{
			printf("[ERROR]: Failed to create %d\n",ret);
			exit(-1);
		}
	}

	i=1;
	thread_main(&i);

	/* Join threads */
	for(size_t i=1; i<num_of_threads; i++)
	{
		ret = pthread_join(threads[i],NULL);
		if(ret)
		{
			printf("{ERROR]: pthread_join: %d\n", ret);
			exit(-1);
		}
		printf("Joined thread %zu\n",i+1);
	}

	/* Merge for 15 elements and 6 threads */
	//	merge(0,1,3);
	//	merge(0,3,5);
	//	merge(0,5,7);
	//	merge(0,7,9);
	//	merge(0,9,14);



	/* merge for 4 threads and element number = multiple of 4 */
	/*	merge(0, (count / 2 - 1) / 2, count / 2 - 1); 
		merge(count / 2, count/2 + (count-1-count/2)/2, count - 1); 
		merge(0, (count - 1)/2, count - 1); 
		*/

	/* merge for 17 elements 4 threads */
	//	merge(0,3,7);
	//	merge(0,7,11);
	//	merge(0,11,16);


	int high_temp = 0;
	int low_temp = 0;
	for (int i = 0; i < num_of_threads; ++i)
	{
		for (int j = i + 1; j < num_of_threads; ++j)
		{
			if (higher_index[i] > higher_index[j])
			{
				high_temp =  higher_index[i];
				higher_index[i] = higher_index[j];
				higher_index[j] = high_temp;
			}
		}
	}

	for(int i=0;i<20;i++)
	{
		printf("%d ",higher_index[i]);
	}

	for(int i=0;i<num_of_threads-1;i++)
	{
		merge(0,higher_index[i],higher_index[i+1]);
	}

	print_in_file(array,count,output_file);
	/* Print the elements of the array */
	for(int i=0;i<count;i++)
	{
		printf("Array[%d] = %d\n",i,array[i]);
	}


	/* Free the resources allocated */
	free(threads);
	free(args);
	pthread_barrier_destroy(&bar);

	/* Calculate time elapsed */
	unsigned long long elapsed_ns;
	elapsed_ns = (end.tv_sec-start.tv_sec)*1000000000 + (end.tv_nsec-start.tv_nsec);
	printf("Elapsed (ns): %llu\n",elapsed_ns);
	double elapsed_s = ((double)elapsed_ns)/1000000000.0;
	printf("Elapsed (s): %lf\n",elapsed_s);

}






























































































#if 0	
/* Case 1:  [./mysort --name]
 * Case 2:  [./mysort sourcefile.txt] */
if(argc == 2)
{
	/* Print name if -n or --name is mentioned as a command line argument */
	if(name_flag == 1)
	{
		printf("Name = %s\n",name);
		exit(1);
	}
	else
	{
		FILE *fptr;				//File pointer
		fptr = fopen(argv[1],"r+");		//open file

		/* Check if file opened successfully or not */
		if(fptr == NULL)
		{
			printf("Error in Reading File\n");
			exit(1);
		}

		/* Calculate the number of integers in the file */
		while(fscanf(fptr,"%d",&number)==1)
		{
			count++;

		}

		fclose(fptr);		//close file

		int array[count];	//declare array to store the elements from the file

		/* Initialize all elements of the array to zero */
		for(int i=0;i<count;i++)
		{
			array[i] = 0;
		}

		/* Open file */
		fptr = fopen(argv[1],"r+");

		/* Check if the file opened successfully or not */
		if(fptr == NULL)
		{
			printf("Error in Reading File\n");
			exit(1);
		}

		/* Store the elements in the array from the file */
		for(int i=0;i<count;i++)
		{
			fscanf(fptr,"%d", &number);
			array[z] = number;
			z++;
		}

		/* Calculate the number of elements in the array */
		int number_of_elements = sizeof(array)/sizeof(int);

		/* Sort the array in ascending order using Quick Sort Algorithm */
		quick_sort(array,0,number_of_elements - 1);

		/* Print Sorted Array on the output terminal */
		print_on_console(array,(sizeof(array)/sizeof(int)));
	}

	exit(1);
}

if(argc >= 3)
{
	/* Print name if -n or --name is mentioned as a command line argument */
	if(name_flag == 1)
	{
		printf("Name = %s\n",name);
		exit(1);
	}

	/* Copy input file name in a variable */
	strcpy(input_file,argv[optind]);

	/*
	   printf("Input File =  %s\n",input_file);
	   printf("Output File = %s\n",output_file);
	   */

	/*
	   printf("0->%s\n",argv[0]);
	   printf("1->%s\n",argv[1]);
	   printf("2->%s\n",argv[2]);
	   printf("3->%s\n",argv[3]);
	   printf("4->%s\n",argv[4]);
	   */

	FILE *fptr;			//File pointer

	fptr = fopen(input_file,"r+");	//open file

	/* Check if file opened successfully or not */
	if(fptr == NULL)
	{
		printf("Error in Reading File\n");
		exit(1);
	}

	/* Calculate the number of integers in the file */
	while(fscanf(fptr,"%d",&number)==1)
	{
		count++;

	}

	fclose(fptr);           //close file

	int array[count];       //declare array to store the elements from the file

	/* Initialize all elements of the array to zero */
	for(int i=0;i<count;i++)
	{
		array[i] = 0;
	}


	/* Open file */
	fptr = fopen(input_file,"r+");

	/* Check if the file opened successfully or not */
	if(fptr == NULL)
	{
		printf("Error in Reading File\n");
		exit(1);
	}

	/* Store the elements in the array from the file */
	for(int i=0;i<count;i++)
	{
		fscanf(fptr,"%d", &number);
		array[z] = number;
		z++;
	}

	/* Calculate the number of elements in the array */
	int number_of_elements = sizeof(array)/sizeof(int);

	/* Sort the array in ascending order using Quick Sort Algorithm */
	quick_sort(array,0,number_of_elements - 1);

	/* Check if the output file was mentioned */
	if(output_file_flag ==1)
	{
		/* Print Sorted Array on the output terminal */
		print_in_file(array,(sizeof(array)/sizeof(int)),output_file);
	}
	/* Else print the result on the console */
	else if(output_file_flag == 0)
	{
		/* Print Sorted Array on the output terminal */
		print_on_console(array,(sizeof(array)/sizeof(int)));

	}
}

return 0;
}
#endif
