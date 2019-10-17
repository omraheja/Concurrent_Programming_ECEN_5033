#include <stdio.h>
#include <atomic>
#include <pthread.h>


/* Standard C++ Library Headers */
#include <iostream>
#include <vector>



/* Mutexes for Synchronization */
extern pthread_mutex_t lock;

void cnt_tas_lock();
void cnt_tas_unlock();
void test_and_tas_lock();
void test_and_tas_unlock();
void ticket_lock();
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
