#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>

#include "FG_Locking_BST.h"

pthread_mutex_t tree_lock;
FB_BST_Node *g_root = NULL;

FG_BST_Node* create_new_node(int key,int value,FG_BST_Node* root)
{
	printf("Creating New Node..........\n");

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



/*
FG_BST_Node* insert(int key,int value,FG_BST_Node* root)
{
	if(root == NULL)		//This is the root node
	{
		root = create_new_node(key,value,root);
	}
	else if(key <= root->key)
	{
		root->left = insert(key,value,root->left);
	}
	else if(key > root->key)
	{
		root->right = insert(key,value,root->right);
	}

	return root;
}
*/

void insert(int key,int value,FG_BST_Node* root)
{
	if(root == NULL)
	{
		pthread_mutex_lock(&tree_lock);
		if(g_root == NULL)
		{
			g_root = create_new_node(key,value,g_root);
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
			root->left = create_new_node(key,value,root);
			pthread_mutex_unlock(&root->lock);
		}
		else
		{
			pthread_mutex_lock(&root->left->lock);
			pthread_mutex_unlock(&root->lock);
		}
	}





}

void inorder_traversal(FG_BST_Node *root)
{
	if(root == NULL)
	{
		return;
	}

	inorder_traversal(root->left);
	printf("Key = %d   Value = %d\n",root->key,root->value);
	inorder_traversal(root->right);
}
