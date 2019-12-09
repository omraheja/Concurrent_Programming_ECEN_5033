/* User defined header files */
#include "RW_Locking_BST.h"


/* External Global variables */
extern pthread_rwlock_t tree_lock;
extern BST_Node *g_root;
extern pthread_mutex_t dup_lock;
int dup = 0;


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
	pthread_rwlock_init(&new_node->lock, NULL);

	/* Return the address of the newly created node */
	return new_node;
}

/* Insert */
void insert(int key,int value,BST_Node* root)
{
	if(root == NULL)
	{
		pthread_rwlock_wrlock(&tree_lock);
		if(g_root == NULL)
		{
			g_root = create_new_node(key,value);
			pthread_rwlock_unlock(&tree_lock);
			return;
		}

		pthread_rwlock_wrlock(&g_root->lock);
		root = g_root;
		pthread_rwlock_unlock(&tree_lock);
	}


	if(key > root->key)
	{
		if(root->right == NULL)
		{
			root->right = create_new_node(key,value);
			pthread_rwlock_unlock(&root->lock);
		}
		else
		{
			pthread_rwlock_wrlock(&root->right->lock);
			pthread_rwlock_unlock(&root->lock);
			insert(key,value,root->right);
		}

	}
	else if(key < root->key)
	{
		if(root->left == NULL)
		{
			root->left = create_new_node(key,value);
			pthread_rwlock_unlock(&root->lock);
		}
		else
		{
			pthread_rwlock_wrlock(&root->left->lock);
			pthread_rwlock_unlock(&root->lock);
			insert(key,value,root->left);
		}
	}

	else
	{
		pthread_mutex_lock(&dup_lock);
		dup++;
		pthread_mutex_unlock(&dup_lock);
		pthread_rwlock_unlock(&root->lock);
		printf("Duplicates Not Allowed\n");
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
		pthread_rwlock_rdlock(&tree_lock);
		if(g_root == NULL)
		{
			printf("Search Failed for node with Key = %d\n",key);
			pthread_rwlock_unlock(&tree_lock);
			//return -1;
			return NULL;
		}
		pthread_rwlock_rdlock(&g_root->lock);
		root = g_root;
		pthread_rwlock_unlock(&tree_lock);
	}

	if(key < root->key)
	{
		if(root->left == NULL)
		{
			printf("Search Failed for node with key = %d\n",key);
			pthread_rwlock_unlock(&root->lock);
			//return -1;
			//return NULL;
		}
		else
		{
			pthread_rwlock_rdlock(&root->left->lock);
			pthread_rwlock_unlock(&root->lock);
			search(key,root->left);
		}
	}
	else if(key > root->key)
	{
		if(root->right == NULL)
		{
			printf("Search Failed for node with Key = %d\n",key);
			pthread_rwlock_unlock(&root->lock);
			//return -1;
			return NULL;
		}
		else
		{
			pthread_rwlock_rdlock(&root->right->lock);
			pthread_rwlock_unlock(&root->lock);
			search(key,root->right);
		}
	}
	else
	{

		//printf("[SEARCH] [Key-------------------------------->%d] [Data-------------------------------->%d]\n",root->key,root->value);
		pthread_rwlock_unlock(&root->lock);
		//return 1;
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
			pthread_rwlock_rdlock(&tree_lock);
			if(g_root == NULL)
			{
				//printf("Search Failed for node with Key = %d\n",key);
				pthread_rwlock_unlock(&tree_lock);
				return;
			}
			pthread_rwlock_rdlock(&g_root->lock);
			root = g_root;
			pthread_rwlock_unlock(&tree_lock);
		}

		if(key1 < root->key)
		{
			if(root->left == NULL)
			{
				//printf("Search Failed for node with key = %d\n",key);
				pthread_rwlock_unlock(&root->lock);
				return;
			}
			else
			{
				pthread_rwlock_rdlock(&root->left->lock);
				pthread_rwlock_unlock(&root->lock);
				search(key1,root->left);
			}
		}
		else if(key1 > root->key)
		{
			if(root->right == NULL)
			{
				//printf("Search Failed for node with Key = %d\n",key);
				pthread_rwlock_unlock(&root->lock);
				return;
			}
			else
			{
				pthread_rwlock_rdlock(&root->right->lock);
				pthread_rwlock_unlock(&root->lock);
				search(key1,root->right);
			}
		}
		else
		{

			printf("Key = %d Data-------------------------------->%d\n",root->key,root->value);
			pthread_rwlock_unlock(&root->lock);
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
		pthread_rwlock_rdlock(&tree_lock);
		if(g_root == NULL)
		{
			//printf("Search Failed for node with Key = %d\n",key1);
			pthread_rwlock_unlock(&tree_lock);
			return;
		}
		pthread_rwlock_rdlock(&g_root->lock);
		root = g_root;
		pthread_rwlock_unlock(&tree_lock);
	}

	if(key1 < root->key)
	{
		if(root->left == NULL)
		{
			//printf("Search Failed for node with key = %d\n",key1);
			//pthread_mutex_unlock(&root->lock);
		}
		else
		{
			pthread_rwlock_rdlock(&root->left->lock);
			pthread_rwlock_unlock(&root->lock);
			range(key1,key2,root->left,thread_id);
			pthread_rwlock_rdlock(&root->lock);
		}
	}

	/* if root's data lies in range, then prints root's data */
        if ( key1 <= root->key && key2 >= root->key )
        {
                printf("[RANGE QUERY] [THREAD ID -> %d] [RANGE VALUE -> %d]\n", thread_id, root->key);
                //pthread_mutex_unlock(&root->lock);
        }



	/* If root->data is smaller than k2, then only we can get o/p keys
	   in right subtree */
	if (key2 > root->key)
	{
		if(root->right == NULL)
		{
			//printf("Search Failed for node with Key = %d\n",key2);
			//pthread_mutex_unlock(&root->lock);
		}
		else
		{
			pthread_rwlock_rdlock(&root->right->lock);
			pthread_rwlock_unlock(&root->lock);
			range(key1,key2,root->right,thread_id);
			pthread_rwlock_rdlock(&root->lock);
		}
	}

	pthread_rwlock_unlock(&root->lock);
}
