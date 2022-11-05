#include "equal-paths.h"
#include <algorithm>
using namespace std;


// You may add any prototypes of helper functions here

// Helper function used to find the height of a subtree
int calculatingPaths(Node * root)
{
	if (root == nullptr)
	{
		return 0;
	}
	
	// Post order traversal
	int lHeight = calculatingPaths(root -> left);
	int rHeight = calculatingPaths(root -> right);

	return max(lHeight, rHeight) + 1;

}

bool equalPaths(Node * root)
{
  // Add your code below
	if (root == nullptr)
	{
		return true;
	}

	// If both left and right nodes exists, then check if both subtrees have equal paths
	if (root->left != nullptr && root->right != nullptr)
	{
		return calculatingPaths(root->left) == calculatingPaths(root->right);
	}
	
	// If only one child node exists, then check if that nodes' subtree has equal paths
	if (root->left == nullptr)
	{
		return equalPaths(root->right);
	}
	else
	{
		return equalPaths(root->left);
	}
}

