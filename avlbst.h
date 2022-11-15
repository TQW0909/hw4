#ifndef RBBST_H
#define RBBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor and setting
* the color to red since every new node will be red when it is first inserted.
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO

protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
		void insert_fix (AVLNode<Key,Value>* p, AVLNode<Key,Value>* n);
		void rotateLeft (AVLNode<Key,Value>* g, AVLNode<Key,Value>* p);
		void rotateRight (AVLNode<Key,Value>* g, AVLNode<Key,Value>* p);
		bool isZigZig (AVLNode<Key,Value>* g, AVLNode<Key,Value>* p, AVLNode<Key,Value>* n);
		void remove_fix (AVLNode<Key,Value>* p, int diff);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
	// Checking for case where tree is empty
	if (BinarySearchTree<Key, Value>::empty())
	{
		BinarySearchTree<Key, Value>::root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
		return;
	}

	AVLNode<Key, Value> *current = static_cast<AVLNode<Key,Value>*>(BinarySearchTree<Key, Value>::root_);

	// Iterating through the AVL Tree to find where new node should go (Theta(h))
	while (current != nullptr)
	{
		// Checking if the key already exists
		if (current->getKey() == new_item.first)
		{
			current->setValue(new_item.second);
			return;
		}
		// Checking if key is bigger than current node
		else if(current->getKey() < new_item.first)
		{
			// If right node is empty, then insert new node
			if (current->getRight() == nullptr)
			{
				current->setRight(new AVLNode<Key, Value>(new_item.first, new_item.second, current));

				// Updating balance and maintaining balanced property
				if (current->getBalance() == 0)
				{
					current->updateBalance(+1);
					insert_fix(current, current->getRight());
				}
				// When b(p) = -1 or +1
				else
				{
					current->setBalance(0);
				}
				break;
			}

			current = current->getRight();
		}
		// Checking if key is smaller than current node
		else
		{
			// If left node is empty, then insert new node
			if (current->getLeft() == nullptr)
			{
				current->setLeft(new AVLNode<Key, Value>(new_item.first, new_item.second, current));

				// Updating balance
				if (current->getBalance() == 0)
				{
					current->updateBalance(-1);
					insert_fix(current, current->getLeft());
				}
				// When b(p) = -1 or +1
				else
				{
					current->setBalance(0);
				}
				break;
			}

			current = current->getLeft();
		}
	}
}

// Function that makes sure the tree is balanced after an insertion
template<class Key, class Value>
void AVLTree<Key, Value>::insert_fix (AVLNode<Key,Value>* p, AVLNode<Key,Value>* n)
{
	// Checking if p and parent(p) is nullptr
	if (p == nullptr)
	{
		return;
	}
	if (p->getParent() == nullptr)
	{
		return;
	}

	AVLNode<Key, Value> *g = p->getParent();

	// p is left child of g
	if (g->getLeft() == p)
	{
		g->updateBalance(-1);

		// Case 1
		if (g->getBalance() == 0)
		{
			return;
		}
		// Case 2
		if (g->getBalance() == -1)
		{
			insert_fix(g, p); // Recurse
		}
		// Case 3
		else
		{
			// If zig-zig
			if (isZigZig(g, p, n))
			{
				rotateRight(g, p);
				p->setBalance(0);
				g->setBalance(0);
			}
			// If ZigZag
			else
			{
				rotateLeft(p, n);
				rotateRight(g, n);
				
				// Case 3a
				if (n->getBalance() == -1)
				{
					p->setBalance(0);
					g->setBalance(+1);
					n->setBalance(0);
				}
				// Case 3b
				else if (n->getBalance() == 0)
				{
					p->setBalance(0);
					g->setBalance(0);
					n->setBalance(0);
				}
				// Case 3c (b(n)== +1)
				else
				{
					p->setBalance(-1);
					g->setBalance(0);
					n->setBalance(0);
				}
			}
		}
	}
	// p is right child of g
	else
	{
		g->updateBalance(+1);

		// Case 1
		if (g->getBalance() == 0)
		{
			return;
		}
		// Case 2
		if (g->getBalance() == +1)
		{
			insert_fix(g, p); // Recurse
		}
		// Case 3
		else
		{
			// If zig-zig
			if (isZigZig(g, p, n))
			{
				rotateLeft(g, p);
				p->setBalance(0);
				g->setBalance(0);
			}
			// if ZigZag
			else
			{
				rotateRight(p, n);
				rotateLeft(g, n);
				
				// Case 3a
				if (n->getBalance() == +1)
				{
					p->setBalance(0);
					g->setBalance(-1);
					n->setBalance(0);
				}
				// Case 3b
				else if (n->getBalance() == 0)
				{
					p->setBalance(0);
					g->setBalance(0);
					n->setBalance(0);
				}
				// Case 3c (b(n)== -1)
				else
				{
					p->setBalance(+1);
					g->setBalance(0);
					n->setBalance(0);
				}
			}
		}
	}
}

// Function that return true if the three nodes are in zig-zig scenerio
template<class Key, class Value>
bool AVLTree<Key, Value>::isZigZig (AVLNode<Key,Value>* g, AVLNode<Key,Value>* p, AVLNode<Key,Value>* n)
{
	return ((g->getLeft() == p) && (p->getLeft() == n)) || ((g->getRight() == p) && (p->getRight() == n));
}

// Rotating right
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight (AVLNode<Key,Value>* g, AVLNode<Key,Value>* p)
{
	// Checking is g is root node
	if (g->getParent() != nullptr)
	{
		// Finding Parent to node g
		AVLNode<Key, Value> *ggp = g->getParent();

		// Correcting child pointer for grandparent node
		if (ggp->getLeft() == g)
		{
			ggp->setLeft(p);
		}
		else
		{
			ggp->setRight(p);
		}
	
		p->setParent(ggp);
	}
	else
	{
		p->setParent(nullptr);
		BinarySearchTree<Key, Value>::root_ = p;
	}

	// Sorting out pointers with remaining nodes
	g->setLeft(p->getRight());
	if (p->getRight() != nullptr)
	{
		p->getRight()->setParent(g);
	}
	p->setRight(g);
	g->setParent(p);
}


template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft (AVLNode<Key,Value>* g, AVLNode<Key,Value>* p)
{
	// Checking is g is root node
	if (g->getParent() != nullptr)
	{
		// Finding Parent to node g
		AVLNode<Key, Value> *ggp = g->getParent();

		// Correcting child pointer for grandparent node
		if (ggp->getLeft() == g)
		{
			ggp->setLeft(p);
		}
		else
		{
			ggp->setRight(p);
		}

		p->setParent(ggp);
	}
	else
	{
		p->setParent(nullptr);
		BinarySearchTree<Key, Value>::root_ = p;
	}

	// Sorting out pointers with remaining nodes
	g->setRight(p->getLeft());
	if (p->getLeft() != nullptr)
	{
		p->getLeft()->setParent(g);
	}
	p->setLeft(g);
	g->setParent(p);
}


/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
	// TODO

	// Checking if AVLTree is empty
	if (BinarySearchTree<Key, Value>::empty())
	{
		return;
	}
	
	// Finding node with the key value
	AVLNode<Key, Value>* n = static_cast<AVLNode<Key,Value>*>(BinarySearchTree<Key, Value>::internalFind(key));
	if (n == nullptr)
	{
		return;
	}

	// Case 3: Two child nodes, swap with predecessor then Case 1 or Case 2
	if (n->getLeft() != nullptr && n->getRight() != nullptr)
	{
		AVLNode<Key, Value>* temp = static_cast<AVLNode<Key,Value>*>(BinarySearchTree<Key, Value>::predecessor(n));

		nodeSwap(temp, n);
	}
	
	// Obtaining p and diff before we alter the tree
	AVLNode<Key, Value>* p = n->getParent();
	int diff = 0;

	// Checking if parent node exists
	if (p != nullptr)
	{
		if (p->getLeft() == n)
		{
			diff = +1;
		}
		else
		{
			diff = -1;
		}
	}

	// Case 1: No child
	if (n->getLeft() == nullptr && n->getRight() == nullptr)
	{
		// Checking if node has parent
		if (p != nullptr)
		{
			if (p->getLeft() == n)
			{
				p->setLeft(nullptr);
			}
			else
			{
				p->setRight(nullptr);
			}
		}
		else // When node does not have a parent
		{
			this->root_ = nullptr; 
		}
	}

	// Case 2: 1 Child (Left child)
	else if(n->getLeft() != nullptr && n->getRight() == nullptr)
	{
		// Checking if node has parent
		if (p != nullptr)
		{
			// Promoting child node and removing pointers to selected node
			n->getLeft()->setParent(p);

			if (p->getLeft() == n)
			{
				p->setLeft(n->getLeft());
			}
			else
			{
				p->setRight(n->getLeft());
			}
		}
		else // When node does not have a parent
		{
			this->root_ = n->getLeft();
			n->getLeft()->setParent(nullptr);
		}
	}

	// Case 2: 1 Child (Right child)
	else if(n->getLeft() == nullptr && n->getRight() != nullptr)
	{
		// Checking if node has parent
		if (p != nullptr)
		{
			// Promoting child node and removing pointers to selected node
			n->getRight()->setParent(p);

			if (p->getLeft() == n)
			{
				p->setLeft(n->getRight());
			}
			else
			{
				p->setRight(n->getRight());
			}
		}
		else // When node does not have a parent
		{
			BinarySearchTree<Key, Value>::root_ = n->getRight();
			n->getRight()->setParent(nullptr);
		}
	}
	delete n;

	remove_fix (p, diff);
}

// Function that ensures tree is maintained after node is removed
template<class Key, class Value>
void AVLTree<Key, Value>::remove_fix (AVLNode<Key,Value>* n, int diff)
{
	// Checking if n is a nullptr
	if (n == nullptr)
	{
		return;
	}

	// Obtaining parent node and ndiff before we alter the tree
	AVLNode<Key, Value>* p = n->getParent();
	int ndiff = 0;

	// Checking if parent node exsits
	if (p != nullptr)
	{
		if (p->getLeft() == n)
		{
			ndiff = +1;
		}
		else
		{
			ndiff = -1;
		}
	}

	// Case 1
	if (n->getBalance() + diff == -2)
	{
		AVLNode<Key, Value>* c = n->getLeft();

		// Case 1a
		if (c->getBalance() == -1)
		{
			rotateRight(n, c);
			n->setBalance(0);
			c->setBalance(0);
			remove_fix(p, ndiff); // Recurse
		}

		// Case 1b
		else if (c->getBalance() == 0)
		{
			rotateRight(n, c);
			n->setBalance(-1);
			c->setBalance(+1);
			return;
		}

		// Case 1c
		else
		{
			AVLNode<Key, Value>* g = c->getRight();
			rotateLeft(c, g);
			rotateRight(n, g);
			
			if (g->getBalance() == +1)
			{
				n->setBalance(0);
				c->setBalance(-1);
				g->setBalance(0);
			}
			else if (g->getBalance() == 0)
			{
				n->setBalance(0);
				c->setBalance(0);
				g->setBalance(0);
			}
			else
			{
				n->setBalance(+1);
				c->setBalance(0);
				g->setBalance(0);
			}

			remove_fix(p, ndiff); // Recurse
		}
	}

	// Case 1
	else if (n->getBalance() + diff == +2)
	{
		AVLNode<Key, Value>* c = n->getRight();

		// Case 1a
		if (c->getBalance() == 1)
		{
			rotateLeft(n, c);
			n->setBalance(0);
			c->setBalance(0);
			remove_fix(p, ndiff); // Recurse
		}

		// Case 1b
		else if (c->getBalance() == 0)
		{
			rotateLeft(n, c);
			n->setBalance(+1);
			c->setBalance(-1);
			return;
		}

		// Case 1c
		else
		{
			AVLNode<Key, Value>* g = c->getLeft();
			rotateRight(c, g);
			rotateLeft(n, g);
			
			if (g->getBalance() == -1)
			{
				n->setBalance(0);
				c->setBalance(+1);
				g->setBalance(0);
			}
			else if (g->getBalance() == 0)
			{
				n->setBalance(0);
				c->setBalance(0);
				g->setBalance(0);
			}
			else
			{
				n->setBalance(-1);
				c->setBalance(0);
				g->setBalance(0);
			}

			remove_fix(p, ndiff); // Recurse
		}
	}

	// Case 2
	else if (n->getBalance() + diff == -1)
	{
		n->setBalance(-1);
		return;
	}

	// Case 2
	else if (n->getBalance() + diff == +1)
	{
		n->setBalance(1);
		return;
	}

	// Case 3
	else if (n->getBalance() + diff == 0)
	{
		n->setBalance(0);
		remove_fix(p, ndiff); // Recurse
	}
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
