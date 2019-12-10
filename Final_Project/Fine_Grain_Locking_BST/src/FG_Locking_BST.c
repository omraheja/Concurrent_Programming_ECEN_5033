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


/*-----------------------------------------------------------------------
 *@Function: create_new_node
 *-----------------------------------------------------------------------
 *@brief: Takes key and value to be inserted as input parameters,
 *	  allocates memory for new node, inserts key, value and
 *	  initializes the mutex lock for that node.
 *-----------------------------------------------------------------------
 *@returns: Address of newly created node (BST_Node*)
 *-----------------------------------------------------------------------
 * */
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


/*-----------------------------------------------------------------------
 *@Function: insert
 *-----------------------------------------------------------------------
 *@brief: Inserts new node in the tree based on key values. Uses hand-
 *	  over-hand locking to allow multiple threads to insert a node
 *	  simultaneously.
 *-----------------------------------------------------------------------
 *@returns: void
 *-----------------------------------------------------------------------
 * */
void insert(int key,int value,BST_Node* root)
{
	/* Check if root is NULL */
	if(root == NULL)
	{
		pthread_mutex_lock(&tree_lock);	//Lock the tree
		/* Check if root of Tree exists or not */
		if(g_root == NULL)
		{
			g_root = create_new_node(key,value);	//Create root node if it doesn't exist
			pthread_mutex_unlock(&tree_lock);	//Unlock the Tree
			return;
		}

		pthread_mutex_lock(&g_root->lock);	//Lock the parent node
		root = g_root;				//Assign parent node to root(local to this function)
		pthread_mutex_unlock(&tree_lock);	//Unlock the tree
	}

	/* Check if key to be inserted is greater than the key of the root */
	if(key > root->key)
	{
		/* Check if the node at which the new node will get inserted is NULL or not */
		if(root->right == NULL)
		{
			root->right = create_new_node(key,value);	//Create new node, link it to the right of parent 
			pthread_mutex_unlock(&root->lock);		//Unlock the parent node
		}
		else
		{
			pthread_mutex_lock(&root->right->lock);	//If node exists, traverse further down the tree and lock the right child
			pthread_mutex_unlock(&root->lock);	//Unlock parent node
			insert(key,value,root->right);		//Call insert with root as right child
		}

	}
	/* Check if key to be inserted is less than the key of the root */
	else if(key < root->key)
	{
		/* Check if the node at which the new node will get inserted is NULL or not */
		if(root->left == NULL)
		{
			root->left = create_new_node(key,value);	//Create new node, link it to the left of parent
			pthread_mutex_unlock(&root->lock);		//Unlock the parent node
		}
		else
		{
			pthread_mutex_lock(&root->left->lock);	//If node exists, traverse further down the tree and lock the left child
			pthread_mutex_unlock(&root->lock);	//Unlock parent node
			insert(key,value,root->left);		//Call insert with root as left child
		}
	}
	/* This condition is entered when a key that is already existing
	 * in the tree is being tried to insert again  */
	else
	{
		pthread_mutex_lock(&dup_lock);
		dup++;
		pthread_mutex_unlock(&dup_lock);
		pthread_mutex_unlock(&root->lock);
#if DEBUG
		printf("Duplicates Key = %d Not Allowed\n",root->key);
#endif
	}
}



/*-----------------------------------------------------------------------
 *@Function: inorder_traversal
 *-----------------------------------------------------------------------
 *@brief: This function prints all the nodes in the tree in ascending
 *	  order. This function is not thread safe. It is used after
 *	  all the threads are joined.
 *-----------------------------------------------------------------------
 *@returns: void
 *-----------------------------------------------------------------------
 * */
void inorder_traversal(BST_Node *g_root)
{
	/* Check if the root passed as input to the function is NULL or not */
	if(g_root == NULL)
	{
		//If root passed is null that means the tree is empty. In that case you return
		return;
	}
	inorder_traversal(g_root->left);	//Traverse recursively till the leaf node on left half of tree
	printf("[INORDER TRAVERSAL] [Key = %d] [Value = %d]\n",g_root->key,g_root->value);	//Print the node
	inorder_traversal(g_root->right);	//Traverse recursively on the right half of the tree
}


/*-----------------------------------------------------------------------
 *@Function: search
 *-----------------------------------------------------------------------
 *@brief: This function takes a node as input, and keeping that as the
 *	  reference searches the key. If the key is found, the node is
 *	  returned, else NULL is returned.
 *-----------------------------------------------------------------------
 *@returns: BST_Node*
 *-----------------------------------------------------------------------
 * */
BST_Node* search(int key,BST_Node *root)
{
	/* Check if the input passed is NULL or not */
	if(root == NULL)
	{
		pthread_mutex_lock(&tree_lock);	//Lock the tree
		
		/* Check if tree exixts or not by checking if global root is NULL or not */
		if(g_root == NULL)	
		{
#if DEBUG
			printf("Search Failed for node with Key = %d\n",key);
#endif
			pthread_mutex_unlock(&tree_lock);	//Unlock the tree
			return NULL;				//return NULL indicating node with requested key not found
		}
		pthread_mutex_lock(&g_root->lock);	//Lock the parent node
		root = g_root;				//Assign parent node to root(local to this function)
		pthread_mutex_unlock(&tree_lock);	//Unlock the tree	
	}

	/* Check if key to be searched is less than the key of the parent */
	if(key < root->key)
	{
		/* Check if the node at which the key is searched is NULL or not */
		if(root->left == NULL)
		{
#if DEBUG
			printf("Search Failed for node with key = %d\n",key);
#endif
			pthread_mutex_unlock(&root->lock);	//Unlock the parent
		}
		/* If parent node exists, we traverse down further to the left half of the tree */
		else
		{
			pthread_mutex_lock(&root->left->lock);	//Lock the left child
			pthread_mutex_unlock(&root->lock);	//Unlock the parent
			search(key,root->left);			//Search key at left child
		}
	}
	/* Check if key to be searched is greater than the key of the parent */
	else if(key > root->key)
	{
		/* Check if the node at which the key is searched is NULL or not */
		if(root->right == NULL)
		{
#if DEBUG
			printf("Search Failed for node with Key = %d\n",key);
#endif
			pthread_mutex_unlock(&root->lock);	//Unlock the parent
			return NULL;				//return NULL indicating node with requested key not found
		}
		else
		{
			pthread_mutex_lock(&root->right->lock);	//Lock the right child
			pthread_mutex_unlock(&root->lock);	//Unlock the parent
			search(key,root->right);		//Search key at right child
		}
	}
	/* Key found */
	else
	{
		pthread_mutex_unlock(&root->lock);	//Unlock the parent
		return root;				//Return the root
	}
}


#if 0
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
#endif



/*-----------------------------------------------------------------------
 *@Function: range
 *-----------------------------------------------------------------------
 *@brief: Takes key1 and key2 along with root as input arguments and
 *	  prints all nodes between those range at that instant. Multiple
 *	  threads can access this function simultaneously to search for
 *	  different ranges. Hand-over-hand locking mechanism is used to
 *	  protect multiple threads from data races.
 *-----------------------------------------------------------------------
 *@returns: void
 *-----------------------------------------------------------------------
 * */
void range(int key1,int key2,BST_Node *root,int thread_id)
{
	int temp;		//temporary variable to store key

	/* Ensure that key1 is always less than key2
	 * If key1 is greater than key2, interchange
	 * them */
	if(key1 > key2)
	{
		temp = key1;
		key1 = key2;
		key2 = temp;
	}

	/* Check if the input passed is NULL or not */
	if(root == NULL)
	{
		pthread_mutex_lock(&tree_lock); //Lock tree
		if(g_root == NULL)		//Check if global root is NULL or not
		{
			//printf("Search Failed for node with Key = %d\n",key1);
			pthread_mutex_unlock(&tree_lock);	//Unlock tree
			return;					//return in that case
		}
		pthread_mutex_lock(&g_root->lock);	//Lock parent node
		root = g_root;				//Assign parent node to root(local to this function)
		pthread_mutex_unlock(&tree_lock);	//Unlock parent node
	}
	
	/* Check if key to be searched is less than the key of the parent */
	if(key1 < root->key)
	{
		/* If left child is not NULL */
		if(root->left != NULL)
		{
			pthread_mutex_lock(&root->left->lock);	//Lock the left child
			pthread_mutex_unlock(&root->lock);	//Unlock the parent
			range(key1,key2,root->left,thread_id);	//Call range recursively to reach the leaf node
			pthread_mutex_lock(&root->lock);	//Lock the parent
		}
	}

	/* if root's data lies in range, then prints root's data */
        if ((key1 <= root->key) && (key2 >= root->key))
        {
#if DEBUG
                printf("[RANGE QUERY] [THREAD ID -> %d] [RANGE KEY -> %d]\n", thread_id, root->key);
#endif
        }



	/* If root->data is smaller than k2, then only we can get o/p keys
	 * in right subtree.Check if key to be searched is less than the
	 * key of the parent
	 * */
	if (key2 > root->key)
	{
		/* If right child is not NULL */
		if(root->right != NULL)
		{
			pthread_mutex_lock(&root->right->lock);	//Lock the right child
			pthread_mutex_unlock(&root->lock);	//Unlock the parent
			range(key1,key2,root->right,thread_id);	//Call range recursively to access right half of tree
			pthread_mutex_lock(&root->lock);	//Lock the parent
		}
	}

	pthread_mutex_unlock(&root->lock);		//After the entire operation, unlock the parent
}
