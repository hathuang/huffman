/*
http://dungenessbin.diandian.com/post/2012-05-23/21949784
*/
#include <stdio.h>
#include <string.h>

#include "huffman.h"
#include "sort.h"
#include "syslog.h"

#ifdef DEBUG
#define print_debug(x)     printf(x)
#else
#define print_debug(x)
#endif

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

int huffman_sort(struct huffman_node **head, int flag)
{
        // only 255 elements in the line at most;
        struct huffman_node *p = *head;
        struct huffman_node *q = NULL;
        struct huffman_node *pre = NULL;
        
        if (!p) {
                syslog(LOG_USER | LOG_ERR , "%s : Ugly params.", __func__);
                return -1;
        } else if (p->next == NULL) {
                return 0; // one element ONLY
        } 

        // bubble sort
        do {
                q = p;
                while (q = q->next) {
                        switch (flag) {
                        case HUFFMAN_SORT_SMALL_FIRST:
                                if (p->priority > q->priority) {
                                        p->data = (p->data) ^ (q->data);
                                        q->data = (p->data) ^ (q->data);
                                        p->data = (p->data) ^ (q->data);
                                       
                                        p->bits = (p->bits) ^ (q->bits);
                                        q->bits = (p->bits) ^ (q->bits);
                                        p->bits = (p->bits) ^ (q->bits);
                                        
                                        p->newcode = (p->newcode) ^ (q->newcode);
                                        q->newcode = (p->newcode) ^ (q->newcode);
                                        p->newcode = (p->newcode) ^ (q->newcode);

                                        p->priority = (p->priority) ^ (q->priority);
                                        q->priority = (p->priority) ^ (q->priority);
                                        p->priority = (p->priority) ^ (q->priority);
                                }
                                break;
                        case HUFFMAN_SORT_BIG_FIRST:
                                if (p->priority < q->priority) {
                                        p->data = (p->data) ^ (q->data);
                                        q->data = (p->data) ^ (q->data);
                                        p->data = (p->data) ^ (q->data);
                                        
                                        p->bits = (p->bits) ^ (q->bits);
                                        q->bits = (p->bits) ^ (q->bits);
                                        p->bits = (p->bits) ^ (q->bits);
                                        
                                        p->newcode = (p->newcode) ^ (q->newcode);
                                        q->newcode = (p->newcode) ^ (q->newcode);
                                        p->newcode = (p->newcode) ^ (q->newcode);

                                        p->priority = (p->priority) ^ (q->priority);
                                        q->priority = (p->priority) ^ (q->priority);
                                        p->priority = (p->priority) ^ (q->priority);
                                }
                                break;
                        default: break;
                        }


                } 
        } while (p = p->next);

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
                        syslog(LOG_USER | LOG_ERR , "%s : Fail to right hand huffman_code.", __func__);
                        return -1;
                }
        }
        if (p->lnext) {
                p->lnext->bits = p->bits + 1;
                p->lnext->newcode = (p->newcode << 1) & 0xfe;  // left  : 0 
                if (huffman_code(&(p->lnext))) {
                        syslog(LOG_USER | LOG_ERR , "%s : Fail to left hand huffman_code.", __func__);
                        return -1;
                }
        }
        return 0;
}

int huffman_insert_sort(struct huffman_node **_head, struct huffman_node *new)
{
        struct huffman_node *q = *_head;
        struct huffman_node *p = NULL;

        if (!q || !new) {
                return -1;
        }
        // small first
        if (q->priority >= new->priority) {
                new->next = q;
                *_head = new;
                return 0;
        } 
        p = q;
        while (q = p->next) {
                if (p->priority <= new->priority && new->priority <= q->priority) {
                        // insert
                        new->next = p->next;
                        p->next = new;
                        return 0;
                }
                p = q;
        }
        p->next = new;
        new->next = NULL;
        
        return 0;
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
                        syslog(LOG_USER | LOG_ERR , "%s : Fail to malloc.", __func__);
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
                                syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_insert_sort.", __func__);
                                return -1;
                        } else {
                                return huffman_tree(root);
                        }
                } 
        }
        syslog(LOG_USER | LOG_ERR , "%s : Unkown error, out of control.", __func__);
        
        return -1;
}

int node_distory(struct huffman_node **head)
{
        struct huffman_node *p = *head;
        struct huffman_node *q = NULL;

        if (!p) {
                syslog(LOG_USER | LOG_ERR , "%s : Ugly params.", __func__);
                return -1;
        }
        q = p;
        while (q = p->next) {
                free(p);
                p = q;
        }
        free(p);
        *head = NULL;
        return 0;
}

int print_newcode(struct huffman_node **head)
{
        struct huffman_node *p = *head;
        if (!p) {
                printf("Ugly head\n");
                return -1;
        }
        do {
                printf("Huffman : char = %c = 0x%02x, bits = %d, newcode = 0x%02x\n", p->data, p->data, p->bits, p->newcode);
        } while (p = p->next);
}

int huffman_encode(char *_str)
{
        struct huffman_node **head = NULL;
        struct huffman_node *node = (struct huffman_node *)malloc(sizeof(struct huffman_node));
        char *src = _str;

        if (!(*head = node)) {
                syslog(LOG_SYSTEM | LOG_ERR , "%s : Fail to malloc for head", __func__);
                return -1;
        } else if (!src) {
                syslog(LOG_USER | LOG_ERR , "%s : Ugly params.", __func__);
                node_distory(head);
                return -1;
        }
        // make the src in the huffman line
        print_debug("start to init_huffman_line\n");
        if (init_huffman_line(head, src)) {
                syslog(LOG_USER | LOG_ERR , "%s : Fail to init_huffman_line", __func__);
                node_distory(head);
                return -1;
        }
        // sort the line
        print_debug("start to huffman_sort, small first\n");
        if (huffman_sort(head, HUFFMAN_SORT_SMALL_FIRST)) {
                syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_sort small first", __func__);
                node_distory(head);
                return -1;
        }

        print_debug("start to huffman_tree\n");
        if (huffman_tree(head)) {
                syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_tree", __func__);
                node_distory(head);
                return -1;
        }
        
        print_debug("start to huffman_code\n");
        if (huffman_code(head)) {
                syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_code", __func__);
                node_distory(head);
                return -1;
        }
        
        print_debug("start to huffman_sort, big first\n");
        if (huffman_sort(head, HUFFMAN_SORT_BIG_FIRST)) {
                syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_sort big first", __func__);
                node_distory(head);
                return -1;
        }
        print_debug("start to print_newcode\n");
        print_newcode(head); 
        print_debug("All goes well...\n");
        node_distory(head);
        return 0;
}
