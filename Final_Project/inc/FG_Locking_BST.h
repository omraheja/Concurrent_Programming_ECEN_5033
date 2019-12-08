#ifndef FG_LOCKING_BST_H
#define FG_LOCKING_BST_H

/*@Author       : Om Raheja
 *@File Name    : FG_Locking_BST.h
 *@Date         : 12/9/2019
 *@Tools        : Compiler:g++ ; Editor: Vim
 *@Brief        : This file includes all the standard c library header files
                  and function prototypes of the function used in FG_Locking_BST.c
		  and the structure for the BST node.
 * */

/* Standard C Library Header Files */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>


/* BST node structure */
typedef struct bst_node{
	int key;
	int value;
	struct bst_node *left;
	struct bst_node *right;
	pthread_mutex_t lock;
}BST_Node;


/*@Function Name: create_new_node
 *@Brief        : This function accepts keys and values as arguments. 
 *		  A new node is created by allocating memory equal to
 *		  the size of the node using malloc() function call.
 *		  After creation of the node,keys and values are inserted
 *		  along with initializing the mutex lock for that
 *		  respective thread.
 *@Param in [1]	: int key
 *@Param in [2]	: int value
 *@Return       : BST_Node* new_node (Address of the newly created node)
 * */
BST_Node* create_new_node(int key,int value);


/*@Function Name: insert
 *@Brief        : This function inserts a new node in the tree based on 
 *		  the keys. This function uses hand-over-hand locking
 *		  to allow multiple threads to insert simultaneously.
 *@Param in [1] : int key
 *@Param in [2] : int value 
 *@Param in [3]	: BST_Node* root
 *@Return       : void
 * */
void insert(int key,int value,BST_Node* root);


/*@Function Name: inorder_traversal
 *@Brief        : Traverses all the nodes in the Tree and prints the
 *		  key-value pair in ascending order of the key. This
 *		  function does not support multiple thread access.
 *		  This function is invoked after all the threads join
 *		  to display all the inserts(key-value pairs) by 
 *		  multiple threads. 
 *@Param in [1]	: BST_Node* root
 *@Return       : void
 * */
void inorder_traversal(BST_Node *root);


/*@Function Name: search
 *@Brief        : Takes key and the root as input parameters. Traverses the
 *		  Tree until it finds the key. If key is found, 1 is returned
 *		  successful search of that particular key. On failure to find
 *		  the key, -1 is returned indicating failure to search the key.
 *@Param in [1] : int key
 *@Param in [2]	: BST_Node* root
 *@Return       : 1 on success ; -1 on failure
 * */
BST_Node* search(int key,BST_Node *root);


void range_query(int key1,int key2,BST_Node *root);

void range(int key1,int key2,BST_Node *root,int thread_id);


#endif
