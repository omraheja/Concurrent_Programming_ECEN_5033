/* Standard C Library Header Files */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>


/* User defined header files */
#include "FG_Locking_BST.h"


/* External Global variables */
extern pthread_mutex_t tree_lock;
extern FG_BST_Node *g_root;
extern pthread_mutex_t dup_lock;
int dup = 0;



FG_BST_Node* create_new_node(int key,int value)
{
	/* Create a new node
	 * pointer returned by malloc will be typecasted with type (FG_BST_Node *)
	 * FG_BST_Node is a structure declared in FG_Locking_BST.h under inc/ subfolder. */
	FG_BST_Node *new_node = (FG_BST_Node *)malloc(sizeof(FG_BST_Node));

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


void insert(int key,int value,FG_BST_Node* root)
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
		printf("Duplicates Not Allowed\n");
	}
}


void inorder_traversal(FG_BST_Node *g_root)
{
	if(g_root == NULL)
	{
		return;
	}

	inorder_traversal(g_root->left);
	printf("Key = %d   Value = %d\n",g_root->key,g_root->value);
	inorder_traversal(g_root->right);
}




void search(int key,FG_BST_Node *root)
{
	if(root == NULL)
	{
		pthread_mutex_lock(&tree_lock);
		if(g_root == NULL)
		{
			printf("Search Failed for node with Key = %d\n",key);
			pthread_mutex_unlock(&tree_lock);
			return;
		}
		pthread_mutex_lock(&g_root->lock);
		root = g_root;
		pthread_mutex_unlock(&tree_lock);
	}

	if(key < root->key)
	{
		if(root->left == NULL)
		{
			printf("Search Failed for node with key = %d\n",key);
			pthread_mutex_unlock(&root->lock);
			return;
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
			printf("Search Failed for node with Key = %d\n",key);
			pthread_mutex_unlock(&root->lock);
			return;
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

		printf("Key = %d Data-------------------------------->%d\n",root->key,root->value);
		pthread_mutex_unlock(&root->lock);
	}
}




void range_query(int key1,int key2,FG_BST_Node *root)
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
