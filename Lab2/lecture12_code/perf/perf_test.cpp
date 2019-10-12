#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <atomic>
#include <assert.h>

using namespace std;


// test admin
pthread_t* threads;
size_t* args;
size_t NUM_THREADS;
pthread_barrier_t bar;
int ITERATIONS = 5000;
int counter;
atomic<int> atomic_counter;
struct timespec start, end;
int TEST_NUM = 0;

// synchronization vars
pthread_mutex_t lock;
atomic<bool> tas_lock;
int* array;


struct padded_int{
	int contents;
	char padding[128-sizeof(int)];
};
padded_int* array_padded;
thread_local int local_cnt;

// functions
void empty(int);
void print(int);
void cnt_lock(int);
void cnt_tas_lock(int);
void cnt_test_and_tas_lock(int);
void cnt_fai(int);
void cnt_cas(int);
void cnt_array(int);
void cnt_array_padded(int);
void cnt_thread_local(int);
void cnt_stack(int);

const int NUM_FUNCS = 11;
void (*funcs[NUM_FUNCS])(int)  = {
	empty,
	print,
	cnt_lock,
	cnt_tas_lock,
	cnt_test_and_tas_lock,
	cnt_fai,
	cnt_cas,
	cnt_array,
	cnt_array_padded,
	cnt_thread_local,
	cnt_stack
};
	
const char* func_names[NUM_FUNCS] = {
	"empty",
	"print",
	"cnt_lock",
	"cnt_tas_lock",
	"cnt_test_and_tas_lock",
	"cnt_fai",
	"cnt_cas",
	"cnt_array",
	"cnt_array_padded",
	"cnt_thread_local",
	"cnt_stack"
};

void global_init(){
	threads = (pthread_t*) malloc(NUM_THREADS*sizeof(pthread_t));
	args = (size_t*) malloc(NUM_THREADS*sizeof(size_t));
	pthread_barrier_init(&bar, NULL, NUM_THREADS);
	counter = 0;
	atomic_counter.store(0);	

	pthread_mutex_init(&lock,NULL);
	tas_lock.store(false);
	array = (int*) malloc(NUM_THREADS*sizeof(int));
	array_padded = (padded_int*) malloc(NUM_THREADS*sizeof(padded_int));
}
void global_cleanup(){
	if(atomic_counter.load()!=0){counter = atomic_counter.load();}
	
	printf("Counter:%d\n",counter);
	
	free(array);
	free(array_padded);
	free(threads);
	free(args);
	pthread_barrier_destroy(&bar);
	pthread_mutex_destroy(&lock);
}

void local_init(int tid){
	local_cnt=0;
	array[tid-1]=0;
	array_padded[tid-1].contents=0;
}
void local_cleanup(){}


void empty(int tid){}

void print(int tid){
	printf("Thread %d reporting for duty.\n",tid);
}

void cnt_lock(int tid){
	for(int i = 0; i<ITERATIONS; i++){
		pthread_mutex_lock(&lock);
		counter++;
		pthread_mutex_unlock(&lock);
	}
}

void cnt_tas_lock(int tid){
	bool expected, changed;
	for(int i = 0; i<ITERATIONS; i++){
		do{
			expected = false;
			changed = true;
		}while(!tas_lock.compare_exchange_strong(expected,changed));
		counter++;
		tas_lock.store(false);
	}
}

void cnt_test_and_tas_lock(int tid){
	bool expected, changed;
	for(int i = 0; i<ITERATIONS; i++){
		do{
			expected = false;
			changed = true;
		}while(tas_lock.load()==true || !tas_lock.compare_exchange_strong(expected,changed));
		counter++;
		tas_lock.store(false);
	}
}

void cnt_fai(int tid){
	for(int i = 0; i<ITERATIONS; i++){
		atomic_counter.fetch_add(1);
	}
}

void cnt_cas(int tid){
	int expected, changed;
	for(int i = 0; i<ITERATIONS; i++){
		do{
			expected = atomic_counter.load();
			changed = expected+1;
		}while(!atomic_counter.compare_exchange_strong(expected,changed));
	}
}

void cnt_array(int tid){
	for(int i = 0; i<ITERATIONS; i++){
		array[tid-1]++;
	}
	atomic_counter.fetch_add(array[tid-1]);
}

void cnt_array_padded(int tid){
	for(int i = 0; i<ITERATIONS; i++){
		array_padded[tid-1].contents++;
	}
	atomic_counter.fetch_add(array[tid-1]);
}

void cnt_thread_local(int tid){
	for(int i = 0; i<ITERATIONS; i++){
		local_cnt++;
	}
	atomic_counter.fetch_add(local_cnt);
}

void cnt_stack(int tid){
	int stack_cnt = 0;
	for(int i = 0; i<ITERATIONS; i++){
		stack_cnt++;
	}
	atomic_counter.fetch_add(stack_cnt);
}

void* thread_main(void* args){
	size_t tid = *((size_t*)args);
	void (*test)(int) = funcs[TEST_NUM];
	
	local_init(tid);
	pthread_barrier_wait(&bar);
	if(tid==1){
		clock_gettime(CLOCK_MONOTONIC,&start);
	}
	pthread_barrier_wait(&bar);
	test(tid);
	pthread_barrier_wait(&bar);
	if(tid==1){
		clock_gettime(CLOCK_MONOTONIC,&end);
	}
	local_cleanup();
	
	return 0;
}




int main(int argc, const char* argv[]){
	
	// parse args
	if(argc!=4){
		printf("USAGE: perf_test [num_threads] [test_num] [iterations]\n");
		for(int i = 0; i<NUM_FUNCS; i++){
			printf("%d : %s\n",i,func_names[i]);
		}
		exit(-1);
	}
	else{
		NUM_THREADS = atoi( argv[1] );
		if(NUM_THREADS > 150){
			printf("ERROR; too many threads\n");
			exit(-1);
		}
		TEST_NUM = atoi( argv[2] );
		ITERATIONS = atoi(argv[3]);
		printf("Running %zu threads on %s for %d iterations\n",
		 NUM_THREADS,func_names[TEST_NUM],ITERATIONS);
	}
	
	
	global_init();
	
	// launch threads
	int ret; size_t i;
  for(i=1; i<NUM_THREADS; i++){
		args[i]=i+1;
		//printf("creating thread %zu\n",args[i]);
		ret = pthread_create(&threads[i], NULL, &thread_main, &args[i]);
		if(ret){
			printf("ERROR; pthread_create: %d\n", ret);
			exit(-1);
		}
	}
	i = 1;
	thread_main(&i); // master also calls thread_main
	
	// join threads
	for(size_t i=1; i<NUM_THREADS; i++){
		ret = pthread_join(threads[i],NULL);
		if(ret){
			printf("ERROR; pthread_join: %d\n", ret);
			exit(-1);
		}
		//printf("joined thread %zu\n",i+1);
	}
	
	global_cleanup();
	
	unsigned long long elapsed_ns;
	elapsed_ns = (end.tv_sec-start.tv_sec)*1000000000 + (end.tv_nsec-start.tv_nsec);
	printf("Elapsed (ns): %llu\n",elapsed_ns);
	double elapsed_s = ((double)elapsed_ns)/1000000000.0;
	printf("Elapsed (s): %lf\n",elapsed_s);
}
