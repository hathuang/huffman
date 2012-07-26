/*
http://dungenessbin.diandian.com/post/2012-05-23/21949784
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "huffman.h"
#include "sort.h"

int add(int a, int b)
{
        return a + b;
}

// huffman encode
static int isnew(struct node *huffman_node, int size, char new)
{
        int i = 0;
        if (!huffman_node) {
                return -1;
        } 
        while (i < size) {
                if (new == huffman_node[i].data) {
                        huffman_node[i].priority++;
                        return 0; // the same
                } 
        }
        huffman_node[size].data = new;
        huffman_node[size].priority++;
        return 1;
}

static int make_tree(struct node *node, int num)
{
        int i, j;

        if (num <= 0) {
                return -1;
        } 
        if (num == 1) {
                return 0;
        } 

        // use malloc to do it 



        num--;
        return make_tree(node, num);
/*
        if (node == NULL || num < 0) {
                return -1; 
        }
        if (num <= 1) {
                return 0;
        } 

        for (i = 0; i < num - 1; i++) {
                for (j = i + 1; j < num; j++) {
                        if (node[i].priority > node[j].priority) {
                                // exchange 
                                node[j].priority = node[i].priority ^ node[j].priority;   
                                node[i].priority = node[i].priority ^ node[j].priority;  
                                node[j].priority = node[i].priority ^ node[j].priority; 
                                
                                node[j].data = node[i].data ^ node[j].data;
                                node[i].data = node[i].data ^ node[j].data;
                                node[j].data = node[i].data ^ node[j].data;
                                
                                node[j].rnext = node[i].rnext ^ node[j].rnext;
                                node[i].rnext = node[i].rnext ^ node[j].rnext;
                                node[j].rnext = node[i].rnext ^ node[j].rnext;
                                
                                node[j].lnext = node[i].lnext ^ node[j].lnext;
                                node[i].lnext = node[i].lnext ^ node[j].lnext;
                                node[j].lnext = node[i].lnext ^ node[j].lnext;
                        } 
                }
        }
        return 0;
*/
}

int init_huffman_line(struct huffman_node *node, struct huffman_line *line, char *str)
{
        unsigned int i;
        char ch;
        struct huffman_line *pline = NULL;
        struct huffman_line *preline = NULL;
        struct huffman_node *pnode = NULL;
        
        if (!node || !line || !str) {
                printf("ugly params\n");
                return -1;
        } 

        // first
        node->data = *(src + 0);
        node->newcode = 0;
        node->priority = 1;
        node->lnext = NULL;
        node->rnext = NULL;

        line->curr = node; 
        line->next = NULL; 
        
        i = 1;
        while (ch = *(src + i)) {
                pline = line;
                do {
                        if (pline->curr->data == ch) {
                                pline->curr->priority++;
                                break;
                        }
                        preline = pline;
                } while (pline = pline->next);
                if (!pline) {
                        // new
                        preline->next = (struct huffman_line *) malloc(sizeof(struct huffman_line));
                        if ((pline = preline->next) == NULL) {
                                perror("fail to malloc for new line");
                                // TODO
                        }
                        pline->next = NULL;
                        pline->curr = (struct huffman_node *) malloc(sizeof(struct huffman_node)); 
                        if ((pnode = pline->curr) == NULL) {
                                perror("fail to malloc for new node");
                                // TODO
                        }
                        pnode->data = ch;
                        pnode->newcode = 0;
                        pnode->priority = 1;
                        pnode->lnext = NULL;
                        pnode->rnext = NULL;
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

