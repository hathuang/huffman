/*
http://dungenessbin.diandian.com/post/2012-05-23/21949784
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "huffman.h"
#include "syslog.h"

#ifdef Debug
#define print_debug(x)     printf(x)
//#define syslog(x,y,z) Syslog(x,y,z) 
#else
#define print_debug(x)
//#define syslog(x,y,z) do{}while(0)
#endif

int add(int a, int b)
{
        return a + b;
}

int init_huffman_line(struct huffman_node **head, char *str, unsigned int length)
{
        unsigned int i;
        char ch;
        char *src = str;
        struct huffman_node *node = *head;
        struct huffman_node *pnode = NULL;
        struct huffman_node *prenode= NULL;
        
        if (!node || !src || !length) {
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
        while (i < length) {
                ch = *(src + i++);
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
                                return -1;
                        }
                        syslog(LOG_USER | LOG_INFO, "%s : new char = %c = 0x%02x", __func__, ch, ch);

                        pnode->data = ch;
                        pnode->bits = 0;
                        pnode->newcode = 0;
                        pnode->priority = 1;
                        pnode->lnext = NULL;
                        pnode->rnext = NULL;
                        pnode->next = NULL;
                }
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
                p->rnext->bits &= 0x80; 
                p->rnext->bits |= (p->bits & 0x07) + 1;         // it is < 7
                p->rnext->newcode = (p->newcode << 1) | 0x01;  // right : 1
                if (huffman_code(&(p->rnext))) {
                        syslog(LOG_USER | LOG_ERR , "%s : Fail to right hand huffman_code.", __func__);
                        return -1;
                }
        }
        if (p->lnext) {
                p->lnext->bits &= 0x80; 
                p->lnext->bits |= (p->bits & 0x07) + 1;         // it is < 7
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
        if (q->priority > new->priority) {
                new->next = q;
                *_head = new;
                return 0;
        } else if (q->priority == new->priority) {
                p = new->rnext;
                if (!p) {
                        // impossible
                        return -1;
                }
                new->next = p->next;
                p->next = new;
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
        struct huffman_node *p = NULL;
        int ret = -1;
        
        if ((*root) == NULL) {
                return -1;
        }
        if ((*root)->next == NULL) { // one
                return 0;
        } else {
                node = (struct huffman_node *)malloc(sizeof(struct huffman_node));
                if (node == NULL) {
                        syslog(LOG_USER | LOG_ERR , "%s : Fail to malloc.", __func__);
                        return -1;
                }
                node->next = NULL;
                node->data = '\0'; 
                node->bits = 0x80; // means not the code form the file or str 
                node->newcode = '\0'; 
                node->priority = (*root)->priority + (*root)->next->priority;
                
                if ((*root)->priority <= (*root)->next->priority) { // bigger right 
                        node->lnext = *root;
                        node->rnext = (*root)->next;
                } else {
                        node->rnext = *root;
                        node->lnext = (*root)->next;
                }
                if ((*root)->next->next == NULL) { // two
                        (*root)->next->next = node;
                        *root = node;
                        return 0;
                } else { // three or more
                        p = (*root)->next->next;
                        *root = p;
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

int print_newcode(struct huffman_node **head, int flag)
{
        struct huffman_node *p = *head;
        if (!p) {
                printf("Ugly head\n");
                return -1;
        }
        printf("\nplace : %p\n", p);
        if (flag)
                syslog(LOG_USER | LOG_DEBUG, "%s : place : %p\n", __func__, p);
        do {
                printf("Huffman : char = %c = 0x%02x, bits = %d, newcode = 0x%02x, priority = %d\n",
                        p->data, p->data, p->bits, p->newcode, p->priority);
                if (flag)
                        syslog(LOG_USER | LOG_DEBUG, "%s : char=%c=0x%02x,bits=%d,newcode=0x%02x,priority=%d,my=%p,next=%p,lnext=%p,rnext=%p",
                        __func__, p->data, p->data, p->bits, p->newcode, p->priority, p, p->next, p->lnext, p->rnext);
        } while (p = p->next);
        
        return 0;
}

int huffman_array(struct huffman_node **_head, char (*arr)[2])
{
        struct huffman_node *q = *_head;
        int n;

        if (!q || !arr) {
                syslog(LOG_USER | LOG_ERR , "%s : Ugly params", __func__);
                return -1;
        }
/*      
 array arch :
 ------------------------------------
 0 | bits     |
 ------------------------------------
 1 | new code |
 ------------------------------------
 */
        do {
                //if (q->data) {
                if ((q->bits) & 0x80) {
                        syslog(LOG_SYSTEM | LOG_INFO, "%s : is = 0x%x", __func__, q->bits & 0xff);
                        continue;
                }
                n = (q->data) & 0x00ff;
                arr[n][0] = q->bits;
                arr[n][1] = q->newcode;
                syslog(LOG_SYSTEM | LOG_INFO, "%s : arr[%c][0]=%d, newcode=0x%x", __func__, q->data, arr[n][0], arr[n][1]);
        } while (q = q->next);
        
        return 0;
}

int huffman_compression(char (*arr)[2], char *src, unsigned int length)
{
        syslog(LOG_SYSTEM | LOG_INFO, "%s : arr['b'][bits]=%d, newcode=0x%x", __func__, arr['b'][0], arr['b'][1]);
        syslog(LOG_SYSTEM | LOG_INFO, "%s : arr['e'][bits]=%d, newcode=0x%x", __func__, arr['e'][0], arr['e'][1]);
        
        char ch = '\0';
        char newchar = '\0';
        char *str = src;
        int n;
        unsigned int i = 0;
        char bits = 0;
        char flag = 0;

        #define ONE_CHAR 8
        //arr[ch][0] // bits
        //arr[ch][1] // newcode
        while (i < length) {
                ch = *(str + i++);
                n = ch & 0x00ff;
                bits = arr[n][0];
                flag += bits;
                syslog(LOG_SYSTEM | LOG_INFO, "%s : Compression # : %c, flag = %d, bits=%d", __func__, ch, flag, bits);
                if (flag < ONE_CHAR) {
                        newchar = newchar << bits;
                        newchar |= arr[n][1] & ((1 << bits) - 1);
                        continue;         
                } else if (flag == ONE_CHAR) {
                        newchar = newchar << bits;
                        newchar |= arr[n][1] & ((1 << bits) - 1);
                        // write newchar  ONE_CHAR
                        
                        syslog(LOG_SYSTEM | LOG_INFO, "%s : Compression = : 0x%x, flag = %d", __func__, newchar & 0xff, flag);
                        newchar = 0;
                        flag = 0;
                } else {
                        newchar = newchar << (ONE_CHAR - (flag - bits));
                        syslog(LOG_SYSTEM | LOG_INFO, "%s : Compression x :%d", __func__, ONE_CHAR - (flag - bits));
                        newchar |= (arr[n][1] >> (flag - ONE_CHAR)) & ((1 << (ONE_CHAR - (flag - bits))) - 1);
                        // write newchar  ONE_CHAR
                        
                        flag = flag - ONE_CHAR;
                        syslog(LOG_SYSTEM | LOG_INFO, "%s : Compression > : 0x%x, flag = %d", __func__, newchar & 0xff, flag);
                        newchar = arr[n][1] & ((1 << flag) - 1);
                }
        }
        if (flag) {
                // write;
                syslog(LOG_SYSTEM | LOG_INFO, "%s : Compression > : 0x%x, flag = %d", __func__, newchar & 0xff, flag);
        }
        
        return 0;
}

int huffman_decompression()
{
        return 0;
}

int huffman_encode(char *_str, unsigned int length)
{
        struct huffman_node *head = NULL;
        struct huffman_node *node = NULL;
        char array[256][2];
        
        head = (struct huffman_node *)malloc(sizeof(struct huffman_node));
        
        char *src = _str;

        print_debug("enter huffman\n");
        syslog(LOG_SYSTEM | LOG_INFO, "%s : hello huffman", __func__);
        if (!(node = head)) {
                syslog(LOG_SYSTEM | LOG_ERR , "%s : Fail to malloc for head", __func__);
                return -1;
        } else if (!src || !length) {
                syslog(LOG_USER | LOG_ERR , "%s : Ugly params.", __func__);
                node_distory(&node);
                return -1;
        }
        // make the src in the huffman line
        print_debug("start to init_huffman_line\n");
        syslog(LOG_SYSTEM | LOG_INFO, "%s : hello huffman line", __func__);
        if (init_huffman_line(&head, src, length)) {
                syslog(LOG_USER | LOG_ERR , "%s : Fail to init_huffman_line", __func__);
                node_distory(&node);
                return -1;
        }
        // sort the line
        print_debug("start to huffman_sort, small first\n");
        if (huffman_sort(&head, HUFFMAN_SORT_SMALL_FIRST)) {
                syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_sort small first", __func__);
                node_distory(&node);
                return -1;
        }

        node = head;
        print_debug("start to huffman_tree\n");
        syslog(LOG_USER | LOG_INFO, "%s : Before huffman_tree", __func__);
        print_newcode(&node, 1); 
        if (huffman_tree(&head)) {
                syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_tree", __func__);
                node_distory(&node);
                return -1;
        }
        print_debug("After huffman_tree\n");
        syslog(LOG_USER | LOG_INFO, "%s : After huffman_tree", __func__);
        print_newcode(&node, 1); 
        
        print_debug("start to huffman_code\n");
        if (huffman_code(&head)) {
                syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_code", __func__);
                node_distory(&node);
                return -1;
        }
        print_newcode(&node, 1); 
        
        print_debug("start to huffman_sort, big first\n");
        //if (huffman_sort(&head, HUFFMAN_SORT_BIG_FIRST)) {
        //syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_sort big first", __func__);
        //node_distory(&node);
        //return -1;
        //}
        
        if (huffman_array(&node, array)) {
                syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_array", __func__);
                node_distory(&node);
                return -1;
        }
        // distory the node, no need any more. 
        node_distory(&node);
        // Compression 
        if (huffman_compression(array, src, length)) {
                syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_compression", __func__);
                return -1;
        }
        // Decompression 
        if (huffman_decompression()) {
                syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_decompression", __func__);
                return -1;
        }

        print_debug("All goes well...\n");
        return 0;
}
