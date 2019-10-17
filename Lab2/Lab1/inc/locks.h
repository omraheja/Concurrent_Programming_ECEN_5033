#ifndef LOCKS_H
#define LOCKS_H

/*@Author       : Om Raheja
 *@File Name    : locks.h 
 *@Date         : 10/16/2019
 *@Tools        : Compiler: g++ ; Editor: Vim
 */


/* Standard C Library headers */
#include <stdio.h>
#include <atomic>
#include <pthread.h>

/* Standard C++ Library Headers */
#include <iostream>
#include <vector>

/* Mutexes for Synchronization */
extern pthread_mutex_t lock;


/*@Function Name: cnt_tas_lock
 *@Brief        : Implements test and set lock
 *@Param	: void 
 *@Return       : void
 * */
void cnt_tas_lock();


/*@Function Name: cnt_tas_unlock
 *@Brief        : Implements test and set unlock
 *@Param	: void 
 *@Return       : void
 * */
void cnt_tas_unlock();


/*@Function Name: test_and_tas_lock
 *@Brief        : Implements test and test and set lock
 *@Param	: void 
 *@Return       : void
 * */
void test_and_tas_lock();


/*@Function Name: buckt_insert
 *@Brief        : Implements test and test and set unlock
 *@Param	: void 
 *@Return       : void
 * */
void test_and_tas_unlock();


/*@Function Name: ticket_lock
 *@Brief        : Implements ticket lock
 *@Param	: void 
 *@Return       : void
 * */
void ticket_lock();


/*@Function Name: ticket_unlock
 *@Brief        : Imlements ticket unlock
 *@Param	: void 
 *@Return       : void
 * */
void ticket_unlock();




/* Class */
class Node{
	public:
	std::atomic<Node*> next;
	std::atomic<bool> wait;
};


extern std::atomic<Node*> tail;

class MCSLock{
	public:
	void acquire(Node* myNode)
	{
		Node* oldTail = tail.load();
		myNode->next.store(NULL);
		while(!tail.compare_exchange_strong(oldTail,myNode))
		{
			oldTail = tail.load();
		}

		if(oldTail != NULL)
		{
			myNode->wait.store(true);
			oldTail->next.store(myNode);
			while(myNode->wait.load()){}
		}
	}

	void release(Node* myNode)
	{
		Node* temp = myNode;
		if(tail.compare_exchange_strong(temp,NULL))
		{
			//no one is waiting, and we just freed the lock
		}
		else	//hand lock to next waiting thread
		{
			while(myNode->next.load() == NULL){}
			myNode->next.load()->wait.store(false);
		}
	}
};

#endif
