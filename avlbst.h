#ifndef AVLBST_H
#define AVLBST_H

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
* An explicit constructor to initialize the elements by calling the base class constructor
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

    // rotations
    AVLNode<Key, Value>* leftRot(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* rightRot(AVLNode<Key, Value>* node);

    // balance
    AVLNode<Key, Value>* rebalance(AVLNode<Key, Value>* node);

    // removefix
    void fixRemove(AVLNode<Key, Value>* node, int8_t diff);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO

    // tree empty
    if(this->root_ == nullptr) {

        // create new root
        AVLNode<Key, Value>* newRoot = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        newRoot->setBalance(0);
        this->root_ = newRoot;
        return;
    }

    // bst isnertion
    AVLNode<Key, Value>* parent = nullptr;
    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);


    while(current != nullptr) {
        
        // set 
        parent = current;
        
        if(new_item.first < current->getKey()){
            current = current->getLeft();
        }

        else if(new_item.first > current->getKey()){
            current = current->getRight();
        }

        else { 
            // key exists
            current->setValue(new_item.second);
            return;
        }
    }

    // make node child of parent
    AVLNode<Key, Value>* newChild = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    if(new_item.first < parent->getKey()){
        parent->setLeft(newChild);
    }
    else{
        parent->setRight(newChild);
    }
    newChild->setBalance(0);

    // update balance
    AVLNode<Key, Value>* balanceNode = parent;

    while(balanceNode != nullptr) {
        if(new_item.first < balanceNode->getKey()){
            balanceNode->updateBalance(-1);
        }

        else{
            balanceNode->updateBalance(1);
        }

        
        // balance is 0
        if(balanceNode->getBalance() == 0){
            break;
        }

        
        // unbalanced
        if(balanceNode->getBalance() == -2 || balanceNode->getBalance() == 2) {
            balanceNode= rebalance(balanceNode);
            
            // root belancing
            if(balanceNode->getParent() == nullptr)
                this->root_ = balanceNode;
            break;
        }
       
        balanceNode = static_cast<AVLNode<Key, Value>*>(balanceNode->getParent());
    }

}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    
    // choose node to remove
    AVLNode<Key, Value>* removeNode = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
   
    if(removeNode == nullptr){
        return;
    }

    // 2 children case
    if(removeNode->getLeft() != nullptr && removeNode->getRight() != nullptr) {
        // swap the node w predeccesor 
        Node<Key, Value>* pred = BinarySearchTree<Key, Value>::predecessor(removeNode);
        this->nodeSwap(removeNode, static_cast<AVLNode<Key, Value>*>(pred));
    }

    // balance after removal
    int8_t change = 0;
   
    AVLNode<Key, Value>* parent = static_cast<AVLNode<Key, Value>*>(removeNode->getParent());
   
    if(parent != nullptr) {
        if(removeNode == parent->getLeft()){
            change = 1;
        }
        else{ 
            change = -1;
        }

    }

    // remove node
    if(removeNode->getLeft() == nullptr && removeNode->getRight() == nullptr) {
        if(removeNode == this->root_) {
            delete this->root_;
            this->root_ = nullptr;
        } 
        else {
            if(parent->getLeft() == removeNode){
                parent->setLeft(nullptr);
            }
            else{
                parent->setRight(nullptr);
            }
            
            delete removeNode;
        }
    } 
    else if(removeNode->getLeft() == nullptr) {
        
        // if only right child exists
        AVLNode<Key, Value>* child = removeNode->getRight();
        if(removeNode == this->root_) {
            this->root_ = child;
            child->setParent(nullptr);
        } 
        else {
            if(parent->getLeft() == removeNode){
                parent->setLeft(child);
            }
            else{
                parent->setRight(child);
            }
            child->setParent(parent);
        }
        delete removeNode;
    } 
    else if(removeNode->getRight() == nullptr) {
        // if only left child exist
        AVLNode<Key, Value>* child = removeNode->getLeft();
        
        if(removeNode == this->root_) {
            this->root_ = child;
            child->setParent(nullptr);
        } 
        else {
            if(parent->getLeft() == removeNode){
                parent->setLeft(child);
            }
            else{
                parent->setRight(child);
            }

            child->setParent(parent);
        }
        delete removeNode;
    }

    // removal path call for fixing
    fixRemove(parent, change);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

// rotate left helper
template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::leftRot(AVLNode<Key, Value>* node) {
    
    // left rotation
    AVLNode<Key, Value>* pivot = node->getRight();
    node->setRight(pivot->getLeft());


    if(pivot->getLeft() != nullptr){
        pivot->getLeft()->setParent(node);
    }
    pivot->setParent(node->getParent());

    if(node->getParent() == nullptr){
        this->root_ = pivot;
    }
    else if(node == node->getParent()->getLeft()){
        node->getParent()->setLeft(pivot);
    }
    else{
        node->getParent()->setRight(pivot);
    }

    pivot->setLeft(node);
    node->setParent(pivot);
    
    // fix
    balanceCheck(node);
    balanceCheck(pivot);

    return pivot;
}

// rotate right helper
template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rightRot(AVLNode<Key, Value>* node) {
    
    
    // right rotation
    AVLNode<Key, Value>* pivot = node->getLeft();
    node->setLeft(pivot->getRight());
   
    if(pivot->getRight() != nullptr){
        pivot->getRight()->setParent(node);
    }
    pivot->setParent(node->getParent());
    
    if(node->getParent() == nullptr){
        this->root_ = pivot;
    }

    else if(node == node->getParent()->getRight()){
        node->getParent()->setRight(pivot);
    }

    else{
        node->getParent()->setLeft(pivot);
    }

    // set right pibot
    pivot->setRight(node);
    // parent
    node->setParent(pivot);
    
    // resest balances
    balanceCheck(node);
    balanceCheck(pivot);
   
    return pivot;
}

// rebalance helper
template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rebalance(AVLNode<Key, Value>* node) {
   
    if(node->getBalance() == 2) {
       
        AVLNode<Key, Value>* rightChild = node->getRight();
        
        if(rightChild->getBalance() >= 0) {
            // right right
            return leftRot(node);
        } 
        else {
            // right left
            node->setRight(rightRot(rightChild));
            return leftRot(node);
        }
    } 
    else if(node->getBalance() == -2) {
       
        AVLNode<Key, Value>* leftChild = node->getLeft();
       
        if(leftChild->getBalance() <= 0) {
            // left left
            return rightRot(node);
        } 
        else {
            // left right
            node->setLeft(leftRot(leftChild));
            return rightRot(node);
        }
    }
    return node;
}

// fixRemove helper
template<class Key, class Value>
void AVLTree<Key, Value>::fixRemove(AVLNode<Key, Value>* node, int8_t diff) {
    
    while(node != nullptr) {

        int8_t oldBalance = node->getBalance();
        int8_t newBalance = oldBalance + diff;
        node->setBalance(newBalance);
        
        // unbalcned
        if(newBalance == 2 || newBalance == -2) {
            node = rebalance(node);
            // heigh doesnt change
            if(node->getBalance() != 0){
                break;
            }

        } 
        else if(newBalance != 0) {
            //stop
            break;
        }
        
        // find new dif for parent
        AVLNode<Key, Value>* parent = static_cast<AVLNode<Key, Value>*>(node->getParent());
        if(parent != nullptr) {
            if(node == parent->getLeft()){
                diff = 1;
            }
            else{
                diff = -1;
            }
        }
        node = parent;
    }
}


// bug fixing, more helpers
template<class Key, class Value>
int avlHeight(AVLNode<Key, Value>* node) {

    // null check
    if(node == nullptr){
        return 0;
    } 

    // add 1
    return 1 + std::max(avlHeight(node->getLeft()), avlHeight(node->getRight()));
}

template<class Key, class Value>
void balanceCheck(AVLNode<Key, Value>* node) {
    
    // null check
    if(node == nullptr){
        return;
    } 

    int heightLeft= avlHeight(node->getLeft());
    int heightRight = avlHeight(node->getRight());

    node->setBalance(static_cast<int8_t>(heightRight - heightLeft));
}

#endif
