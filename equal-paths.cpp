#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

// helper function to find height
int checkHeights(Node* node, int heightCount, int* heightExpec){

    // nothing to check here
    if (node == nullptr){
        return true;
    }

    // check for leaf
    if (node->left == nullptr && node->right == nullptr) {

        if (*heightExpec == -1){
            // set expected
            *heightExpec = heightCount;
        }

        // check if current height matches expected
        return (heightCount== *heightExpec);
    }

    // recursion, check each substree
    bool leftTree = checkHeights(node->left, heightCount + 1, heightExpec);
    bool rightTree = checkHeights(node->right, heightCount + 1, heightExpec);
    return leftTree && rightTree;
}

bool equalPaths(Node * root)
{
    // Add your code below

    // if tree empty
    if (root == nullptr){
        // the heights are equal
        return true;
    }

    int startingHeight = -1;
    return checkHeights(root, 0, &startingHeight);

}
