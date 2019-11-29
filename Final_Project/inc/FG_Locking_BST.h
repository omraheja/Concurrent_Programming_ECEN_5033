#ifndef FG_LOCKING_BST_H
#define FG_LOCKING_BST_H


#include <pthread.h>


typedef struct bst_node{
	int key;
	int value;
	struct bst_node *left;
	struct bst_node *right;
	struct bst_node *parent;
	pthread_mutex_t lock;
}FG_BST_Node;



FG_BST_Node* create_new_node(int key,int value);
//FG_BST_Node* insert(int key,int value,FG_BST_Node* root);
void insert(int key,int value,FG_BST_Node* root);
void inorder_traversal(FG_BST_Node *root);



#endif
