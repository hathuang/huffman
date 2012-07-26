/*
http://dungenessbin.diandian.com/post/2012-05-23/21949784
*/
#include <stdio.h>
#include <string.h>

#include "huffman.h"
#include "sort.h"
#include "syslog.h"

int add(int a, int b)
{
        return a + b;
}

int init_huffman_line(struct huffman_node **head, char *str)
{
        unsigned int i;
        char ch;
        char *src = str;
        struct huffman_node *node = *head;
        struct huffman_node *pnode = NULL;
        struct huffman_node *prenode= NULL;
        
        if (!node || !src) {
                syslog(LOG_USER | LOG_ERR , "%s : Ugly params.", __func__);
                return -1;
        } 

        // first node
        node->data = *(src + 0);
        node->bits = 0;
        node->newcode = 0;
        node->priority = 1;
        node->lnext = NULL;
        node->rnext = NULL;
        node->next = NULL;

        i = 1;
        while (ch = *(src + i)) {
                pnode = node;
                do {
                        if (pnode->data == ch) {
                                pnode->priority++;
                                break;
                        }
                        prenode = pnode;
                } while (pnode = pnode->next);
                if (!pnode) {
                        // new
                        pnode = (struct huffman_node *) malloc(sizeof(struct huffman_node)); 
                        if ((prenode->next = pnode) == NULL) {
                                syslog(LOG_USER | LOG_ERR , "%s : fail to malloc for new node.", __func__);
                                // TODO
                        }
                        pnode->data = ch;
                        pnode->bits = 0;
                        pnode->newcode = 0;
                        pnode->priority = 1;
                        pnode->lnext = NULL;
                        pnode->rnext = NULL;
                        pnode->next = NULL;
                }
                ++i;
        }
        return 0;
}

int huffman_sort(struct huffman_line *line)
{
        // only 255 elements in the line at most;
        struct huffman_line *xline = NULL;
        struct huffman_line *yline = NULL;
        struct huffman_line *tline = NULL;
        struct huffman_line *xpreline = NULL;
        struct huffman_line *ypreline = NULL;
       
        if (line == NULL) {
                return -1;
        } else if (line->next == NULL) {
                return 0; // one element ONLY
        } 

        xline = line;
       
        // bubble sort
        do {
                yline = xline->next;
                ypreline = xline;
                do {
                        if (xline->curr->priority > yline->curri->priority) {
                                // exchange Be careful about it . FIXME
                                tline = xline;
                                xline->curr = yline->curr;
                                xline->next = yline->next;
                                if (xpreline) {
                                        xpreline->next = yline;
                                } 
                                yline->curr = tline->curr;
                                yline->next = tline->next;
                                ypreline->next = tline;
                        } 
                        ypreline = yline;
                } while (yline = yline->next);
                xpreline = xline;
        } while (xline = xline->next);

        return 0;
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
