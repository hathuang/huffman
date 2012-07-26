/*
http://dungenessbin.diandian.com/post/2012-05-23/21949784
*/
#include <stdio.h>
#include <string.h>

#include "huffman.h"
#include "sort.h"

int add(int a, int b)
{
        return a + b;
}

int huffman_code(struct huffman_node **root)
{
        struct huffman_node *p = *root;
        
        if (p == NULL) {
                return -1;
        }
        if (p->rnext) {
                p->rnext->bits = p->bits + 1;
                p->rnext->newcode = (p->newcode << 1) | 0x01;  // right : 1
                if (huffman_code(&(p->rnext))) {
                        return -1;
                }
        }
        if (p->lnext) {
                p->lnext->bits = p->bits + 1;
                p->lnext->newcode = (p->newcode << 1) & 0xfe;  // left  : 0 
                if (huffman_code(&(p->lnext))) {
                        return -1;
                }
        }
        return 0;
}

int huffman_insert_sort(struct huffman_node **_head, struct huffman_node *new)
{
        struct huffman_node *head = *_head;

        if (!head) {
                return -1;
        }
        
        
}

int huffman_tree(struct huffman_node **root)
{
        struct huffman_node *node = NULL;
        
        if ((*root) == NULL) {
                return -1;
        } 
        if ((*root)->next == NULL) { // one
                return 0;
        } else {
                node = (struct huffman_node *)malloc(sizeof(struct huffman_node));
                if (new == NULL) {
                        return -1;
                }
                node->next = NULL;
                node->data = '\0'; 
                node->bits = 0; 
                node->newcode = '\0'; 
                node->priority = (*root)->priority + (*root)->next->priority;
                if ((*root)->priority > (*root)->next->priority) { // bigger left
                        node->lnext = (*root)->node;
                        node->rnext = (*root)->next->node;
                } else {
                        node->rnext = (*root)->node;
                        node->lnext = (*root)->next->node;
                }
                if ((*root)->next->next == NULL) { // two
                        *root = node;
                        return 0;
                } else { // three or more
                        *root = (*root)->next->next;
                        if (huffman_insert_sort(root, node)) {
                                return -1;
                        } else {
                                return huffman_tree(root);
                        }
                } 
        }
        
        return -1;
}

int huffman_encode(char *encode_s)
{









}
