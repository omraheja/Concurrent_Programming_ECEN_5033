/*@Author       : Om Raheja
 *@File Name    : mysort.c 
 *@Date         : 11/1/2019 
 *@Tools        : Compiler:gcc ; Editor: Vim
 *@References	: The concept for Quick Sort Algorithm was referenced from https://www.youtube.com/watch?v=3DV8GO9g7B4
		: This article gives a brief analysis for using openMP to parallelise quick sort
 		  [https://pushkar2196.wordpress.com/2017/02/26/quicksort/}
 		: The youtube series by Tim Mattson was really helpful to understand the OpenMP Library
 		  [https://www.youtube.com/watch?v=nE-xN4Bf8XI&list=PLLX-Q6B8xqZ8n8bwjGdzBJ25X2utwnoEG]
 		: Lastly, this code was reused from Lab0(implementation of quick sort) and openMP Library
 		  was then used to parallelise it.
 */

/* Standard C Library Headers */
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>

/* User Defined Header Files */
#include "../inc/print_on_console.h"
#include "../inc/quick_sort.h"
#include "../inc/print_in_file.h"

/* Global variabeles */
static char *name = "Om Raheja";

/* Timespec structures to calculate amount of time taken for algorithm to sort the array */
struct timespec start,end;


/* Main Function */
int main(int argc, char *argv[])
{
	int number;
	int count = 0;
	int z = 0;
	int option_index = 0;
	int name_flag = 0;
	int output_file_flag = 0;
	int opt;
	char *input_file = (char*)malloc(20);
	char *output_file =(char*)malloc(20);

	struct option long_options[] = {
		{ "name", no_argument, NULL,'n'},
		{ "output",required_argument, NULL,'o'},
		{ 0, 0, 0, 0}
	};

	char *optstring = "no:";

	while((opt = getopt_long(argc, argv, optstring, long_options, &option_index))!= -1)
	{
		if (opt == -1)
			break;

		switch (opt) {
			case 'n':
				name_flag = 1;
				break;
			case 'o':
				output_file_flag = 1;
				strcpy(output_file,optarg);
				//printf("Output File = %s\n",output_file);
				break;

			default:
				printf("Usage: %s [...]\n", argv[0]);
				exit(1);
		}
	}


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


			/***************************************************************************************************************************/
			/* Sort the array in ascending order using Quick Sort Algorithm */

			int pivot = partition(array,0,number_of_elements - 1);

			#pragma omp parallel sections
			{
				printf("Number of threads = %d\n",omp_get_num_threads());
				#pragma omp section
				{
					/* Sort the array in ascending order using Quick Sort Algorithm */

					printf("Section 1 ID = %d\n",omp_get_thread_num());
					quick_sort(array,0,pivot - 1);

				}

				#pragma omp section
				{

					printf("Section 2 ID = %d\n",omp_get_thread_num());
					quick_sort(array, pivot + 1, number_of_elements - 1);

				}

			}
			/************************************************************************************************************************/

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


		clock_gettime(CLOCK_MONOTONIC,&start);

/********************************************************************************************************************************/
		/* Sort the array in ascending order using Quick Sort Algorithm */
		int pivot = partition(array,0,number_of_elements - 1);

		#pragma omp parallel sections
		{
			#pragma omp section
			{
				/* Sort the array in ascending order using Quick Sort Algorithm */

				printf("Section 1 ID = %d\n",omp_get_thread_num());
				quick_sort(array,0,pivot - 1);

			}

			#pragma omp section
			{

				printf("Section 2 ID = %d\n",omp_get_thread_num());
				quick_sort(array, pivot + 1, number_of_elements - 1);

			}

		}
/********************************************************************************************************************************/

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

		clock_gettime(CLOCK_MONOTONIC,&end);

		/* Calculate time elapsed */
		unsigned long long elapsed_ns;
		elapsed_ns = (end.tv_sec-start.tv_sec)*1000000000 + (end.tv_nsec-start.tv_nsec);
		printf("Elapsed (ns): %llu\n",elapsed_ns);
		double elapsed_s = ((double)elapsed_ns)/1000000000.0;
		printf("Elapsed (s): %lf\n",elapsed_s);
	}

	return 0;
}
