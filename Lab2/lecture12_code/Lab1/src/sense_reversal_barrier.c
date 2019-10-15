#include "../inc/sense_reversal_barrier.h"
#include <atomic>
#define RELAXED         std::memory_order_relaxed


/* Sense Reversal Barrier variables */
extern std::atomic<int> cnt;
extern std::atomic<int> sense;
extern int num_of_threads;




void sense_rev_bar()
{
        thread_local bool my_sense = 0;
        if(my_sense == 0)
        {
                my_sense = 1;   //Flip sense
        }
        else
        {
                my_sense = 0;
        }


        int cnt_cpy;
        cnt_cpy = std::atomic_fetch_add(&cnt,1);

        //      cnt_cpy = cnt.fetch_add(1);

        if(cnt_cpy == num_of_threads)      //Last thread to arrive
        {
                cnt.store(1,RELAXED);
                sense.store(my_sense);
        }
        else            //not the last
        {
                while(sense.load() !=my_sense){}
        }
}

