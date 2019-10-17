/*@Author       : Om Raheja
 *@File Name    : locks.c 
 *@Date         : 10/16/2019
 *@Tools        : Compiler: g++ ; Editor: Vim
 */

/* User defined header files */
#include "../inc/locks.h"

/* Defines */
#define RELAXED		std::memory_order_relaxed

/* Global variables */
std::atomic<int> now_serving;
std::atomic<int> next_num;
std::atomic<bool> tas_lock;

/* External global variables */
extern MCSLock mcs_lock;


/* Test and set Lock */
void cnt_tas_lock()
{
	bool expected, changed;
	do
	{
		expected = false;
		changed = true;
	}while(!tas_lock.compare_exchange_strong(expected,changed));
}

/* Test and set unlock */
void cnt_tas_unlock()
{
	tas_lock.store(false);
}

/* Test and test and set lock */
void test_and_tas_lock()
{
	bool expected, changed;
	do{
		expected = false;
		changed = true;
	}while(tas_lock.load()==true || !tas_lock.compare_exchange_strong(expected,changed));
}


/* Test and Test and set unlock */
void test_and_tas_unlock()
{
	tas_lock.store(false);
}

/* Ticket lock */
void ticket_lock()
{
	int my_num = std::atomic_fetch_add(&next_num,1);
	while(now_serving.load() != my_num){}
}

/* Ticket unlock */
void ticket_unlock()
{
	std::atomic_fetch_add(&now_serving,1);
}

