#include "../inc/locks.h"
#define RELAXED		std::memory_order_relaxed

/* Global variables */
std::atomic<int> now_serving;
std::atomic<int> next_num;
std::atomic<bool> tas_lock;

extern MCSLock mcs_lock;

void cnt_tas_lock()
{
	bool expected, changed;
	do
	{
		expected = false;
		changed = true;
	}while(!tas_lock.compare_exchange_strong(expected,changed));
}

void cnt_tas_unlock()
{
	tas_lock.store(false);
}



void test_and_tas_lock()
{

	bool expected, changed;
	do{
		expected = false;
		changed = true;
	}while(tas_lock.load()==true || !tas_lock.compare_exchange_strong(expected,changed));

}

void test_and_tas_unlock()
{
	tas_lock.store(false);

}


void ticket_lock()
{
	int my_num = std::atomic_fetch_add(&next_num,1);
	while(now_serving.load() != my_num){}
}

void ticket_unlock()
{
	std::atomic_fetch_add(&now_serving,1);
}

