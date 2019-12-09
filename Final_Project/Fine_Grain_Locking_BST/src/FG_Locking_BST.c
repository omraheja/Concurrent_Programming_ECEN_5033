/*@Author       : Om Raheja
 *@File Name    : FG_Locking_BST.c
 *@Date         : 12/9/2019
 *@Tools        : Compiler:g++ ; Editor: Vim
 *@Brief        : This file includes implementation insert(), search(),
 *		  range query() and inorder_traversal() functions.
 * */



/* User defined header files */
#include "FG_Locking_BST.h"
#define DEBUG 1			//To keep printf statements, DEBUG = 1,
				//Else to disable printf statement DEBUG = 0.


/* External Global variables */
extern pthread_mutex_t tree_lock;	//Mutex lock to lock tree
extern BST_Node *g_root;		//BST_Node to store address of global root
extern pthread_mutex_t dup_lock;	//Mutex lock to increment variable keeping track of duplicate entries
int dup = 0;				//Variable keeping track of duplicate entries


/* Create New Node */
BST_Node* create_new_node(int key,int value)
{
	/* Create a new node
	 * pointer returned by malloc will be typecasted with type (FG_BST_Node *)
	 * FG_BST_Node is a structure declared in FG_Locking_BST.h under inc/ subfolder. */
	BST_Node *new_node = (BST_Node *)malloc(sizeof(BST_Node));

	/* Check if memory has been successfully allocated */
	if(new_node == NULL)
	{
		printf("[ERROR]:Memory allocation failed for new node\n");
		return new_node;
	}

	/* Store key of the new node created */
	new_node->key = key;

	/* Store value of the new node created */
	new_node->value = value;

	/* Make the left and the right node of the newly crated node NULL */
	new_node->left = NULL;
	new_node->right = NULL;

	/* Initiaize the lock associated with the newly created node */
	pthread_mutex_init(&new_node->lock, NULL);

	/* Return the address of the newly created node */
	return new_node;
}

/* Insert */
void insert(int key,int value,BST_Node* root)
{
	if(root == NULL)
	{
		pthread_mutex_lock(&tree_lock);
		if(g_root == NULL)
		{
			g_root = create_new_node(key,value);
			pthread_mutex_unlock(&tree_lock);
			return;
		}

		pthread_mutex_lock(&g_root->lock);
		root = g_root;
		pthread_mutex_unlock(&tree_lock);
	}


	if(key > root->key)
	{
		if(root->right == NULL)
		{
			root->right = create_new_node(key,value);
			pthread_mutex_unlock(&root->lock);
		}
		else
		{
			pthread_mutex_lock(&root->right->lock);
			pthread_mutex_unlock(&root->lock);
			insert(key,value,root->right);
		}

	}
	else if(key < root->key)
	{
		if(root->left == NULL)
		{
			root->left = create_new_node(key,value);
			pthread_mutex_unlock(&root->lock);
		}
		else
		{
			pthread_mutex_lock(&root->left->lock);
			pthread_mutex_unlock(&root->lock);
			insert(key,value,root->left);
		}
	}

	else
	{
		pthread_mutex_lock(&dup_lock);
		dup++;
		pthread_mutex_unlock(&dup_lock);
		pthread_mutex_unlock(&root->lock);
#if DEBUG
		printf("Duplicates Not Allowed\n");
#endif
	}
}


void inorder_traversal(BST_Node *g_root)
{
	if(g_root == NULL)
	{
		return;
	}
	inorder_traversal(g_root->left);
	printf("[INORDER TRAVERSAL] [Key = %d] [Value = %d]\n",g_root->key,g_root->value);
	inorder_traversal(g_root->right);
}




BST_Node* search(int key,BST_Node *root)
{
	if(root == NULL)
	{
		pthread_mutex_lock(&tree_lock);
		if(g_root == NULL)
		{
#if DEBUG
			printf("Search Failed for node with Key = %d\n",key);
#endif
			pthread_mutex_unlock(&tree_lock);
			return NULL;
		}
		pthread_mutex_lock(&g_root->lock);
		root = g_root;
		pthread_mutex_unlock(&tree_lock);
	}

	if(key < root->key)
	{
		if(root->left == NULL)
		{
#if DEBUG
			printf("Search Failed for node with key = %d\n",key);
#endif
			pthread_mutex_unlock(&root->lock);
		}
		else
		{
			pthread_mutex_lock(&root->left->lock);
			pthread_mutex_unlock(&root->lock);
			search(key,root->left);
		}
	}
	else if(key > root->key)
	{
		if(root->right == NULL)
		{
#if DEBUG
			printf("Search Failed for node with Key = %d\n",key);
#endif
			pthread_mutex_unlock(&root->lock);
			return NULL;
		}
		else
		{
			pthread_mutex_lock(&root->right->lock);
			pthread_mutex_unlock(&root->lock);
			search(key,root->right);
		}
	}
	else
	{

		pthread_mutex_unlock(&root->lock);
		return root;
	}
}



/* This Function is not being used
 * Will me removed in the future commit */
void range_query(int key1,int key2,BST_Node *root)
{
	while(key1 <= key2)
	{
		if(root == NULL)
		{
			pthread_mutex_lock(&tree_lock);
			if(g_root == NULL)
			{
				//printf("Search Failed for node with Key = %d\n",key);
				pthread_mutex_unlock(&tree_lock);
				return;
			}
			pthread_mutex_lock(&g_root->lock);
			root = g_root;
			pthread_mutex_unlock(&tree_lock);
		}

		if(key1 < root->key)
		{
			if(root->left == NULL)
			{
				//printf("Search Failed for node with key = %d\n",key);
				pthread_mutex_unlock(&root->lock);
				return;
			}
			else
			{
				pthread_mutex_lock(&root->left->lock);
				pthread_mutex_unlock(&root->lock);
				search(key1,root->left);
			}
		}
		else if(key1 > root->key)
		{
			if(root->right == NULL)
			{
				//printf("Search Failed for node with Key = %d\n",key);
				pthread_mutex_unlock(&root->lock);
				return;
			}
			else
			{
				pthread_mutex_lock(&root->right->lock);
				pthread_mutex_unlock(&root->lock);
				search(key1,root->right);
			}
		}
		else
		{

			printf("Key = %d Data-------------------------------->%d\n",root->key,root->value);
			pthread_mutex_unlock(&root->lock);
		}
		key1++;
	}
}







void range(int key1,int key2,BST_Node *root,int thread_id)
{
	int temp;

	if(key1 > key2)
	{
		temp = key1;
		key1 = key2;
		key2 = temp;
	}

	if(root == NULL)
	{
		pthread_mutex_lock(&tree_lock);
		if(g_root == NULL)
		{
			//printf("Search Failed for node with Key = %d\n",key1);
			pthread_mutex_unlock(&tree_lock);
			return;
		}
		pthread_mutex_lock(&g_root->lock);
		root = g_root;
		pthread_mutex_unlock(&tree_lock);
	}

	if(key1 < root->key)
	{
		if(root->left != NULL)
		{
			pthread_mutex_lock(&root->left->lock);
			pthread_mutex_unlock(&root->lock);
			range(key1,key2,root->left,thread_id);
			pthread_mutex_lock(&root->lock);
		}
	}

	/* if root's data lies in range, then prints root's data */
        if ( key1 <= root->key && key2 >= root->key )
        {
#if DEBUG
                printf("[RANGE QUERY] [THREAD ID -> %d] [RANGE VALUE -> %d]\n", thread_id, root->key);
#endif
	 	//pthread_mutex_unlock(&root->lock);
        }



	/* If root->data is smaller than k2, then only we can get o/p keys
	   in right subtree */
	if (key2 > root->key)
	{
		if(root->right != NULL)
		{
			pthread_mutex_lock(&root->right->lock);
			pthread_mutex_unlock(&root->lock);
			range(key1,key2,root->right,thread_id);
			pthread_mutex_lock(&root->lock);
		}
	}

	pthread_mutex_unlock(&root->lock);
}
