#ifndef ARBOL_H
#define ARBOL_H

typedef struct Node {
    int value;
    int num_sons;
    struct Node *sons;
}Node;


extern void insert(Node*,int*,int);
extern Node* createNode(int,int);

#endif
