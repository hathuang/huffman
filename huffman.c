/*
http://dungenessbin.diandian.com/post/2012-05-23/21949784
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "huffman.h"
#include "syslog.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef Debug
#define print_debug(x)     printf(x)
//#define syslog(x,y,z) Syslog(x,y,z) 
#else
#define print_debug(x)
//#define syslog(x,y,z) do{}while(0)
#endif

int huffman_line(struct huffman_node **head, char *str, unsigned int length)
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
        struct huffman_node *p = *head;
        struct huffman_node *q = NULL;
        
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
                /*
                        switch (flag) {
                        case HUFFMAN_SORT_SMALL_FIRST:
                        */
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
                                /*
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
                        */
                }
        } while (p = p->next);

        return 0;
}

int huffman_code(struct huffman_node *root)
{
        struct huffman_node *p = root;
        
        if (p == NULL) return -1;
        if (p->rnext) {
                p->rnext->bits &= 0x80; 
                if ((p->bits & 0x7f) >= 0x0f) {
                        //syslog(LOG_USER | LOG_ERR , "%s : Newcode bits limited R !", __func__);
                        return -1;
                }
                p->rnext->bits |= (p->bits & 0x0f) + 1;         // it is < 16 
                p->rnext->newcode = (p->newcode << 1) | 0x01;  // right : 1
                if (huffman_code(p->rnext)) {
                        //syslog(LOG_USER | LOG_ERR , "%s : Fail to right hand huffman_code.", __func__);
                        return -1;
                }
        }
        if (p->lnext) {
                p->lnext->bits &= 0x80;
                if ((p->bits & 0x7f) >= 0x0f) {
                        //syslog(LOG_USER | LOG_ERR , "%s : Newcode bits limited L !", __func__);
                        return -1;
                }
                p->lnext->bits |= (p->bits & 0xf) + 1;         // it is < 16
                p->lnext->newcode = (p->newcode << 1) & 0xfe;  // left  : 0 
                if (huffman_code(p->lnext)) {
                        //syslog(LOG_USER | LOG_ERR , "%s : Fail to left hand huffman_code.", __func__);
                        return -1;
                }
        }
        return 0;
}

int huffman_insert_tree(struct huffman_node **head, struct huffman_node *new)
{
        struct huffman_node *q = *head;
        struct huffman_node *p = NULL;

        if (!q || !new) return -1;
        if (q->priority >= new->priority) {
                new->next = q;
                *head = new;
                return 0;
        }
        p = q;
        while (q = p->next) {
                if (p->priority <= new->priority && new->priority <= q->priority) {
                        p->next = new;
                        new->next = q;
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
        
        if ((*root) == NULL) return -1;
        if ((*root)->next == NULL) {
                return 0;
        } else {
                if (!(node = (struct huffman_node *)malloc(sizeof(struct huffman_node)))) {
                        return -1;
                }
                node->next = NULL;
                node->data = '\0'; 
                node->bits = 0x80; // means not the code form the file or str 
                node->newcode = 0; 
                node->priority = (*root)->priority + (*root)->next->priority;
                
                if ((*root)->priority <= (*root)->next->priority) { // bigger right 
                        node->lnext = *root;
                        node->rnext = (*root)->next;
                } else {
                        // maybe do not enter here. forever !
                        return -1;
                        //node->rnext = *root;
                        //node->lnext = (*root)->next;
                }
                if ((*root)->next->next == NULL) { // two
                        (*root)->next->next = node;
                        (*root)->next = NULL;
                        *root = node;
                        return 0;
                } else { // three or more
                        p = (*root)->next->next;
                        (*root)->next->next = NULL;
                        (*root)->next = NULL;
                        *root = p;
                        if (!(huffman_insert_tree(root, node))) { // FIXME
                                return huffman_tree(root);
                        }
                }
        }
        
        return -1;
}

int node_distory(struct huffman_node **head)
{
        struct huffman_node *p = *head;
        struct huffman_node *q = NULL;

        if (!p) return -1;
        q = p;
        while (q = p->next) {
                //syslog(LOG_USER | LOG_INFO , "%s : free : %p.", __func__, p);
                free(p);
                p = q;
        }
        free(p);
        *head = NULL;
        return 0;
}

int huffman_root_distory(struct huffman_node *root)
{
        struct huffman_node *p = root;

        if (!p) return 0;
        if (p->rnext) {
                huffman_root_distory(p->rnext);
        }
        if (p->lnext) {
                huffman_root_distory(p->lnext);
        }
        free(p);

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
        //if (flag)
        //syslog(LOG_USER | LOG_DEBUG, "%s : place : %p\n", __func__, p);
        if (flag != 0 && flag != 1) {
                return 0;
        } 
        
        do {
                switch (flag) {
                case 0:
                        printf("Huffman : char = %c = 0x%02x, bits = %d, newcode = 0x%04x, priority = %d\n",
                                p->data, p->data, p->bits, p->newcode, p->priority);
                        break;
                case 1:
                        syslog(LOG_USER | LOG_DEBUG, "%s : char=%c=0x%02x,bits=%d,newcode=0x%04x,priority=%d,my=%p,next=%p,lnext=%p,rnext=%p",
                                __func__, p->data, p->data, p->bits, p->newcode, p->priority, p, p->next, p->lnext, p->rnext);
                        break;
                default:
                        break;
                }
        } while (p = p->next);
        
        return 0;
}

int huffman_fill_newcode(int fd, struct huffman_node *head, struct huffman_tags *tags, unsigned short (*arr)[2])
{
        struct huffman_node *q = head;
        char buf[4];
        
        if (fd < 0 || !q || !tags) return -1;

        syslog(LOG_USER | LOG_INFO, "%s : q[%p]", __func__, q);
        if (q->rnext) {
                syslog(LOG_USER | LOG_INFO, "%s : q[%p], q->rnext[%p]", __func__, q, q->rnext);
                huffman_fill_newcode(fd, q->rnext, tags, arr);
        }

        if (q->lnext) {
                syslog(LOG_USER | LOG_INFO, "%s : q[%p], q->lnext[%p]", __func__, q, q->lnext);
                huffman_fill_newcode(fd, q->lnext, tags, arr);
        }
        if (q->bits & 0x80) return 0;

        buf[0] = q->data;
        buf[1] = q->bits;
        buf[2] = (q->newcode >> 8) & 0xff;
        buf[3] = q->newcode & 0xff;
        arr[q->data & 0xff][0] = q->bits & 0xff;
        arr[q->data & 0xff][1] = q->newcode;
        syslog(LOG_USER | LOG_INFO, "%s : q[%p]. bits:%d, newcode:0x%x", __func__, q, q->bits, q->newcode & 0xffff);
        if (q->rnext) {
                syslog(LOG_USER | LOG_INFO, "%s : x q[%p], q->rnext[%p]", __func__, q, q->rnext);
        } 
        if (q->lnext) {
                syslog(LOG_USER | LOG_INFO, "%s : x q[%p], q->lnext[%p]", __func__, q, q->lnext);
        } 

        if (4 != write(fd, buf, 4)) return -1;
        if (q->bits <= ONE_CHAR) {
                tags->bytes1++;
        } else if (ONE_CHAR < q->bits && q->bits <= ONE_SHORT) {
                tags->bytes2++;
        } else if (ONE_SHORT < q->bits && q->bits <= ONE_INT) {
                syslog(LOG_USER | LOG_ERR, "%s : too large bits : %d", __func__, q->bits);
                tags->bytes4++;
                return -1;
        } else {
                syslog(LOG_USER | LOG_ERR, "%s : too large bits : %d", __func__, q->bits);
                return -1;
        }
        
        return 0;
}

int huffman_fill_file(const char *file, struct huffman_node *head, struct huffman_tags *tags, char *str, unsigned int length)
{
        unsigned char ch, newchar;
        unsigned int i;
        char newstr[4];
        int ret, n, fd;
        unsigned char bits, flag;
        struct huffman_node *p = NULL;
        struct huffman_node *q = NULL;
        unsigned short arr[256][2];
        int x = 0;

        if (!file || !head || !tags || !str || !length) return -1;
        for (i = 0; i < 256; i++) {
                arr[i][0] = 0;
                arr[i][1] = 0;
        }
        if ((fd = open(file, O_RDWR | O_CREAT | O_TRUNC, 0644)) < 0
                || write(fd, (char *)tags, HUFFMAN_TAGS_SIZE) != HUFFMAN_TAGS_SIZE
                || huffman_fill_newcode(fd, head, tags, arr)) {
                return -1;
        }
        if (tags->bytes1 == 0 && tags->bytes2 == 0 && tags->bytes4 == 0) {
                p = head;
                while (q = p->rnext);
                if (p == head || p->newcode == 0 || p->bits <= 0) return -1; 
                if (p->bits <= ONE_CHAR) {
                        tags->magic = ALL_CHAR;
                } else if (ONE_CHAR < p->bits && p->bits <= ONE_SHORT) {
                        tags->magic = ALL_SHORT;
                } else {
                        return -1; // not support right now !
                }
        }
        // write file body

        printf("file length = %d\n", length);
        i = 0;
        flag = 0;
        while (i < length) {
                ch = *(str + i++);
                n = ch & 0xff;
                bits = arr[n][0];
                flag += bits;
                if (flag < ONE_CHAR) {
                        newchar = newchar << bits;
                        newchar |= arr[n][1] & ((1 << bits) - 1);
                        continue;
                } else if (flag >= ONE_CHAR && flag < ONE_SHORT) { // FIXME
                        if (bits > ONE_CHAR) {
                                syslog(LOG_SYSTEM | LOG_ERR, "%s : == bits=%d, flag=%d, newchar=0x%x", __func__, bits, flag, newchar);
                        }
                        newchar = newchar << (ONE_CHAR - (flag - bits));
                        //newchar |= (arr[n][1] >> (flag - ((bits > ONE_CHAR) ? bits : ONE_CHAR))) & ((1 << (ONE_CHAR - (flag - bits))) - 1);
                        newchar |= (arr[n][1] >> (flag - ONE_CHAR)) & ((1 << (ONE_CHAR - (flag - bits))) - 1);
                        newstr[0] = newchar;
                        if (1 != write(fd, newstr, 1)) {
                                close(fd);
                                return -1;
                        }
                        syslog(LOG_SYSTEM | LOG_WARNING, "%s : out Compression 1 : x=%04d 0x%02x", __func__, x, newstr[0] & 0xff);
                        x++;
                        flag -= ONE_CHAR;
                        newchar = arr[n][1] & ((1 << flag) - 1);
                        if (bits > ONE_CHAR) {
                                syslog(LOG_SYSTEM | LOG_ERR, "%s : == bits=%d, flag=%d, newchar=0x%x, newcode=%c=0x%x", __func__, bits, flag, newchar, arr[n][1], arr[n][1]&0xffff);
                        }
                } else { // >= ONE_SHORT
                        newchar = newchar << (ONE_CHAR - (flag - bits));
                        newchar |= (arr[n][1] >> (flag - ONE_CHAR)) & ((1 << (ONE_CHAR - (flag - bits))) - 1);
                        newstr[0] = newchar;

                        flag -= ONE_SHORT;
                        newstr[1] = (arr[n][1] >> flag) & 0xff;
                        if (2 != write(fd, newstr, 2)) {
                                close(fd);
                                return -1;
                        }
                        x += 2;
                        newchar = arr[n][1] & ((1 << flag) - 1);
                }
        }
        if (flag) {
                if (flag >= ONE_CHAR) {
                        syslog(LOG_USER | LOG_ERR, "%s : unkown flag = %d", __func__, flag);
                        close(fd);
                        return -1;
                }
                newstr[0] = newchar;// << (ONE_CHAR - flag);
                if (1 != (ret = write(fd, newstr, 1))) {
                        syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to write to : %s, ret=%d,flag=%d", __func__, TMP_FILE, ret, flag);
                        close(fd);
                        return -1;
                }
                tags->magic &= 0x0f;
                tags->magic |= (flag << 4) & 0xf0;
                syslog(LOG_SYSTEM | LOG_WARNING, "%s : out Compression ~ : 0x%02x, flag = %d", __func__, newstr[0] & 0xff, flag);
                if (lseek(fd, 0, SEEK_SET) < 0) {
                        syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to lseek . errno=%d", __func__, errno);
                        close(fd);
                        return -1;
                }
                if (write(fd, (char *)tags, HUFFMAN_TAGS_SIZE) != HUFFMAN_TAGS_SIZE) {
                        syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to write to : %s, len=%d", __func__, TMP_FILE, HUFFMAN_TAGS_SIZE);
                        close(fd);
                        return -1;
                }
        }
        close(fd);
        printf("=========== HUFFMAN TAGS: ============\n");
        printf("tags : fillbits %c=0x%x\n", tags->fillbits[0], tags->fillbits[0]);
        printf("tags : fillbits %c=0x%x\n", tags->fillbits[1], tags->fillbits[1]);
        printf("tags : fillbits %c=0x%x\n", tags->fillbits[2], tags->fillbits[2]);
        printf("tags : fillbits %c=0x%x\n", tags->fillbits[3], tags->fillbits[3]);

        printf("tags : magic %d=0x%x\n", tags->magic, tags->magic);
        printf("tags : bytes1 %d=0x%x\n", tags->bytes1, tags->bytes1);
        printf("tags : bytes2 %d=0x%x\n", tags->bytes2, tags->bytes2);
        printf("tags : bytes4 %d=0x%x\n", tags->bytes4, tags->bytes4);
        printf("=========== HUFFMAN TAGS: ============\n");

        return 0;
}

#if 0
int huffman_compression(unsigned short (*arr)[2], char *src, unsigned int length, struct huffman_tags *tags)
{
        unsigned char ch, newchar;
        char *str = src;
        unsigned int i;
        char newstr[4];
        int ret, len, n, fd;
        unsigned char bits, flag;
        int x;

        //arr[ch][0] // bits
        //arr[ch][1] // newcode
        if (!length || !tags) {
                return -1;
        }
        fd = open(TMP_FILE, O_RDWR | O_TRUNC | O_CREAT, 0644); 
        if (fd < 0) {
                syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to open : %s", __func__, TMP_FILE);
                return -1;
        }
        // write huffman_tags
        tags->magic |= ONE_CHAR << 4;
        ret = write(fd, (char *)tags, len = sizeof(struct huffman_tags));
        if (ret != len) {
                close(fd);
                syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to write huffman_tags to : %s, ret=%d,len=%d", __func__, TMP_FILE, ret, len);
                return -1;
        }
        // write newcode 
        i = 0;
        while (i < 256) {
                if (arr[i][0]) {
                        newstr[0] = i & 0xff;                // oldcode 1 byte
                        newstr[1] = arr[i][0];               // bits    1 byte
                        newstr[2] = (arr[i][1] >> 8) & 0xff; // newcode 2 bytes
                        newstr[3] = arr[i][1] & 0xff;        // newcode 2 bytes
                        if (4 != write(fd, newstr, 4)) {
                                syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to write huffman newcode to : %s.", __func__, TMP_FILE);
                                close(fd);
                                return -1;
                        }
                        syslog(LOG_USER | LOG_ERR, "%s : I = %d, bits=%d", __func__, i, arr[i][0]);
                }
                ++i;
        }
        // write file body
        i = 0;
        flag = 0;
        x = 0;
        while (i < length) {
                ch = *(str + i++);
                n = ch & 0xff;
                bits = arr[n][0];
                flag += bits;
                if (flag < ONE_CHAR) {
                        newchar = newchar << bits;
                        newchar |= arr[n][1] & ((1 << bits) - 1);
                        continue;
                } else if (flag >= ONE_CHAR && flag < ONE_SHORT) { // FIXME
                        if (bits > ONE_CHAR) {
                                syslog(LOG_SYSTEM | LOG_ERR, "%s : == bits=%d, flag=%d, newchar=0x%x", __func__, bits, flag, newchar);
                        }
                        newchar = newchar << (ONE_CHAR - (flag - bits));
                        //newchar |= (arr[n][1] >> (flag - ((bits > ONE_CHAR) ? bits : ONE_CHAR))) & ((1 << (ONE_CHAR - (flag - bits))) - 1);
                        newchar |= (arr[n][1] >> (flag - ONE_CHAR)) & ((1 << (ONE_CHAR - (flag - bits))) - 1);
                        newstr[0] = newchar;
                        if (1 != (ret = write(fd, newstr, 1))) {
                                syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to write to : %s, ret=%d,len=%d", __func__, TMP_FILE, ret, len);
                                close(fd);
                                return -1;
                        }
                        syslog(LOG_SYSTEM | LOG_WARNING, "%s : out Compression 1 : x=%04d 0x%02x", __func__, x, newstr[0] & 0xff);
                        x++;
                        flag -= ONE_CHAR;
                        newchar = arr[n][1] & ((1 << flag) - 1);
                        if (bits > ONE_CHAR) {
                                syslog(LOG_SYSTEM | LOG_ERR, "%s : == bits=%d, flag=%d, newchar=0x%x, newcode=%c=0x%x", __func__, bits, flag, newchar, arr[n][1], arr[n][1]&0xffff);
                        }
                } else { // >= ONE_SHORT
                        newchar = newchar << (ONE_CHAR - (flag - bits));
                        newchar |= (arr[n][1] >> (flag - ONE_CHAR)) & ((1 << (ONE_CHAR - (flag - bits))) - 1);
                        newstr[0] = newchar;

                        flag -= ONE_SHORT;
                        newstr[1] = (arr[n][1] >> flag) & 0xff;
                        if (2 != (ret = write(fd, newstr, 2))) {
                                syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to write to : %s, ret=%d,len=%d", __func__, TMP_FILE, ret, len);
                                close(fd);
                                return -1;
                        }
                        x += 2;
                        newchar = arr[n][1] & ((1 << flag) - 1);
                }
        }
        if (flag) { // FIXME
                // write;
                if (flag >= ONE_CHAR) {
                        syslog(LOG_USER | LOG_ERR, "%s : unkown flag = %d", __func__, flag);
                        close(fd);
                        return -1;
                }
                newstr[0] = newchar;// << (ONE_CHAR - flag);
                if (1 != (ret = write(fd, newstr, 1))) {
                        syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to write to : %s, ret=%d,flag=%d", __func__, TMP_FILE, ret, flag);
                        close(fd);
                        return -1;
                }
                tags->magic &= 0x0f;
                tags->magic |= (flag << 4) & 0xf0;
                syslog(LOG_SYSTEM | LOG_WARNING, "%s : out Compression ~ : x=%04d 0x%02x, flag = %d", __func__, x, newstr[0] & 0xff, flag);
                x++;
                if (lseek(fd, 0, SEEK_SET) < 0) {
                        syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to lseek . errno=%d", __func__, errno);
                        close(fd);
                        return -1;
                }
                if (write(fd, (char *)tags, HUFFMAN_TAGS_SIZE) != HUFFMAN_TAGS_SIZE) {
                        syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to write to : %s, len=%d", __func__, TMP_FILE, HUFFMAN_TAGS_SIZE);
                        close(fd);
                        return -1;
                }
        }
        close(fd);
        //syslog(LOG_USER | LOG_INFO, "%s : xxxxxxxxxxxxxx = %d.", __func__, x);

        return 0;
}
#endif

#define BUFF_SIZE           2048
int get_header(const char *filename, struct huffman_header *header, int *file_len)
{
        if (!filename || !file_len) {
                syslog(LOG_USER| LOG_ERR, "%s : Ugly params", __func__);
                return -1;
        } 
        int fd = open(filename, O_RDWR);

        if (fd < 0) {
                syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to open : %s", __func__, filename);
                return -1;
        }
        int ret = read(fd, (char *)header, HUFFMAN_HEADER_SIZE);
        if (ret != HUFFMAN_HEADER_SIZE) {
                syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to get enough bytes : %d is wanna, but get %d", __func__, HUFFMAN_HEADER_SIZE, ret);
                return -1;
        }
        close(fd);
        FILE *fp = fopen(filename, "r"); 
        if (!fp) {
                perror("Fail to fopen TMP_FILE");
                return -1;
        }
        fseek(fp, 0, SEEK_END);
        *file_len = ftell(fp) - HUFFMAN_HEADER_SIZE;
        fclose(fp);

        return 0;
}

int get_file_buf(const char *filename, char *_buf, int file_len)
{
        int ret;
        char buf[1024];

        if (!filename || !_buf || file_len <= 0) {
                syslog(LOG_USER| LOG_ERR, "%s : Ugly params", __func__);
                return -1;
        } 
        int fd = open(filename, O_RDONLY); 
        if (fd < 0) {
                perror("Fail to open TMP_FILE");
                return -1;
        }
        int len = read(fd, buf, HUFFMAN_HEADER_SIZE);
        if (len != HUFFMAN_HEADER_SIZE) {
                syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to get enough bytes : %d is wanna, but get %d", __func__, HUFFMAN_HEADER_SIZE, len);
                return -1;
        }
        len = 0;
        while (len < file_len) {
                ret = read(fd, _buf + len, file_len - len);
                if (ret <= 0) {
                        perror("Fail to read TMP_FILE.");
                        close(fd);
                        return -1;
                }
                len += ret;
        }
        close(fd);
        return 0;
}

int get_root(char *buf, int n, struct tree *root)
{
        int i, bits, k;
        char oldcode, onebit;
        unsigned short newcode;
        struct tree *p = NULL;;
        struct tree *q = NULL;;
/*
        newstr[0] = i & 0xff;                // oldcode 1 byte
        newstr[1] = arr[i][0];               // bits    1 byte
        newstr[2] = (arr[i][1] >> 8) & 0xff; // newcode 2 bytes
        newstr[3] = arr[i][1] & 0xff;        // newcode 2 bytes
*/
        if (!buf || !n || !root) return -1;
        i = 0;
        while (i < n) {
                k = (i << 2) & (~0x03);
                oldcode = buf[k++];
                bits = buf[k++];
                newcode = buf[k++] << 8;
                newcode |= buf[k] & 0xff;
                p = root;
                if (bits > 16 || bits <= 0) {
                        syslog(LOG_USER | LOG_ERR, "%s : Ugly bits = %d = 0x%x, i=%d,n=%d", __func__, bits, bits, i, n); 
                        return -1;
                }
                while (bits--) {
                        onebit = (newcode >> bits) & 0x01;
                        if (onebit) {
                                if (!(q = p->rchild)) {
                                        if (p->oldcode) {
                                                syslog(LOG_USER | LOG_INFO, "%s : +++ oldcode=0x%x, i=%d,k=%d,buf[0]=0x%x=0x%x=0x%x=0x%x bits=%d", 
                                                        __func__, p->oldcode & 0xff, i,k,buf[k-3]&0xff, buf[k-2]&0xff, buf[k-1]&0xff, buf[k]&0xff, bits);
                                                //return -1;
                                        }
                                        if (!(q = (struct tree *)malloc(sizeof(struct tree)))) return -1;
                                        q->rchild = NULL;
                                        q->lchild = NULL;
                                        q->oldcode = 0;
                                        p->rchild = q;
                                }
                                p = q;
                        } else {
                                if (!(q = p->lchild)) {
                                        if (p->oldcode) {
                                                syslog(LOG_USER | LOG_INFO, "%s : +++ oldcode=0x%x, i=%d,k=%d,buf[0]=0x%x=0x%x=0x%x=0x%x bits=%d", 
                                                        __func__, p->oldcode & 0xff, i,k,buf[k-3]&0xff, buf[k-2]&0xff, buf[k-1]&0xff, buf[k]&0xff, bits);
                                                //return -1;
                                        } 
                                        if (!(q = (struct tree *)malloc(sizeof(struct tree)))) return -1;
                                        q->rchild = NULL;
                                        q->lchild = NULL;
                                        q->oldcode = 0;
                                        p->lchild = q;
                                }
                                p = q;
                        }
                }
                if (!q) return -1; 
                q->oldcode = oldcode;
                ++i;
        }

        return 0;
}

int huffman_decomp_tree(const char *filename, struct tree **tree, char **filebuf, unsigned int *_filelen, char *last_ch_bits)
{
        char buf[1024];
        int fd, ret, len, codenum;
        unsigned int filelen;
        struct huffman_tags *tags = NULL;
        struct tree *root = NULL;

        if (!filename || !tree || !_filelen || !last_ch_bits) {
                return -1;
        }
        fd = open(filename, O_RDONLY); 
        if (fd < 0) {
                perror("Fail to open TMP_FILE");
                return -1;
        }
        if ((filelen = lseek(fd, 0, SEEK_END)) <= 0 || lseek(fd, 0, SEEK_SET) < 0) {
                close(fd);
                return -1;
        }
        len = read(fd, buf, HUFFMAN_TAGS_SIZE);
        if (len != HUFFMAN_TAGS_SIZE) return -1;
        tags = (struct huffman_tags *)buf;
        switch (tags->magic & MAGIC_MAC) {
        case ALL_INT :
        case ALL_CHAR :
        case ALL_SHORT :
                len = 256;
                break;
        default :
                len = tags->bytes1 + tags->bytes2 + tags->bytes4;
                break;
        }
        *last_ch_bits = (tags->magic >> 4) & MAGIC_MAC;
        tags = NULL;
        codenum = len;
        len = (len << 2) & (~0x03); // * 4
        filelen -= HUFFMAN_TAGS_SIZE;
        if (len >= filelen || len > 1024 || len != read(fd, buf, len)) {
                close(fd);
                return -1;
        }
        filelen -= len;
        if (!(*filebuf = (char *)malloc(sizeof(char) * (filelen + 1)))) {
                perror("fail to malloc for filebuf");
                close(fd);
                return -1;
        }
        len = 0;
        while (len < filelen) {
                ret = read(fd, *filebuf + len, filelen - len);
                if (ret <= 0) {
                        perror("Fail to read TMP_FILE.");
                        free(*filebuf);
                        *filebuf = NULL;
                        close(fd);
                        return -1;
                }
                len += ret;
        }
        //ret = 0;
        //while (ret < filelen) {
        //syslog(LOG_USER | LOG_INFO, "%s : file buf %04d %c=0x%x", __func__, ret, *(*filebuf + ret), *(*filebuf + ret) & 0xff);
        //++ret;
        //}

        close(fd);
        // get the tree;
        if (!(root = (struct tree *)malloc(sizeof(struct tree)))) {
                perror("fail to malloc for root of tree");
                free(*filebuf);
                *filebuf = NULL;
                return -1;
        }
        root->oldcode = 0;
        root->rchild = NULL;
        root->lchild = NULL;
        *tree = root;
        *_filelen = filelen;
        printf("%s start to get_root\n", __func__);

        return get_root(buf, codenum, root); 
}

int free_tree(struct tree *root)
{
        struct tree *p = root;
        //static unsigned short newcode = 0;

        if (!p) return 0;
        if (p->rchild) {
                //newcode = (newcode << 1) | 0x01;
                free_tree(p->rchild);
        }
        if (p->lchild) {
                //newcode = (newcode << 1) & 0xfffe;
                free_tree(p->lchild);
        }
        //if (!(p->rchild)) syslog(LOG_USER | LOG_ERR, "%s : free oldcode = %c = 0x%x", __func__, p->oldcode, p->oldcode);
        free(p);
        return 0;
}

#if 1
int huffman_decompression()
{
        struct tree *root = NULL;
        struct tree *p = NULL;
        unsigned int i, file_len;
        char *buf = NULL;
        char preflag, last_ch_bits, abit;
        int fd, x;

        printf("%s : Before huffman_decomp_tree\n", __func__);
        if (huffman_decomp_tree(TMP_FILE, &root, &buf, &file_len, &last_ch_bits)) {
                syslog(LOG_USER | LOG_ERR, "%s : fail to huffman_decomp_tree", __func__);
                // distory root
                if (buf) free(buf);
                if (root) free_tree(root); 
                return -1;
        }
        printf("%s : After huffman_decomp_tree\n", __func__);
        fd = open("test.png", O_RDWR | O_TRUNC | O_CREAT, 0644);
        if (fd < 0) {
                if (buf) free(buf);
                if (root) free_tree(root); 
                return -1;
        }
        i = 0;
        x = 0;
        preflag = ONE_CHAR; // current buf[i] remain to use.
        do {
                p = root;
                while (p->lchild) {
                        printf("i = %d, file_len = %d\n", i, file_len);
                        abit = (buf[i] >> (--preflag)) & 0x01;
                        if (abit) {
                                syslog(LOG_USER | LOG_INFO, "%s : RR p : %p, p->rchild : %p, p->oldcode=%c=0x%x", __func__, p, p->rchild, p->oldcode,p->oldcode & 0xff);
                                syslog(LOG_USER | LOG_INFO, "%s : RR p : %p, p->lchild : %p", __func__, p, p->lchild);
                                p = p->rchild;
                        } else {
                                //syslog(LOG_USER | LOG_INFO, "%s : LL p : %p, p->rchild : %p", __func__, p, p->rchild);
                                syslog(LOG_USER | LOG_INFO, "%s : LL p : %p, p->rchild : %p, p->oldcode=%c=0x%x", __func__, p, p->rchild, p->oldcode,p->oldcode & 0xff);
                                syslog(LOG_USER | LOG_INFO, "%s : LL p : %p, p->lchild : %p", __func__, p, p->lchild);
                                if (p->oldcode == 0x0d) {
                                        while (p->lchild) {
                                                syslog(LOG_USER | LOG_INFO, "%s : xx p : %p, p->lchild : %p, p->rchild : %p, oldcode=0x%x", __func__, p, p->lchild, p->rchild, p->oldcode&0xff);
                                                if (p->lchild) {
                                                p=p->lchild;
                                                } else {
                                                p=p->rchild;
                                                }
                                        }
                                        while (p->rchild) {
                                                syslog(LOG_USER | LOG_INFO, "%s : yy p : %p, p->lchild : %p, p->rchild : %p, oldcode=0x%x", __func__, p, p->lchild, p->rchild, p->oldcode&0xff);
                                                p=p->rchild;
                                        }
                                        if (buf) free(buf);
                                        if (root) free_tree(root); 
                                        return -1;
                                }
                                p = p->lchild;
                        }
                        if (!p) {
                                syslog(LOG_USER | LOG_ERR, "%s : ------------------", __func__);
                        } 
                        if (!preflag) {
                                //syslog(LOG_USER | LOG_INFO, "%s : read buf[%d] = 0x%x", __func__, i, buf[i] & 0xff);
                                ++i;
                                if (i < (file_len - 1)) {
                                        preflag = ONE_CHAR;
                                } else if (i == (file_len - 1)) {
                                        preflag = last_ch_bits;
                                        printf("xxxxxxxxx = last_ch_bits %d\n", last_ch_bits);
                                } else {
                                        printf("xxxxxxxxx = %d\n", x);
                                        //goto THE_END;
                                        break;
                                }
                        }
                }
                //syslog(LOG_USER | LOG_INFO, "%s : ** read buf[%d] = 0x%x , preflag = %d, x = %d, ch=%c=0x%x. ", __func__, i, buf[i] & 0xff,preflag, x, p->oldcode, p->oldcode);
                if (!(p->lchild) && 1 != write(fd, &(p->oldcode), 1)) return -1;
                x++;
        } while (i < file_len || preflag > 0);
        syslog(LOG_USER | LOG_INFO, "%s : ------------", __func__);
        syslog(LOG_USER | LOG_INFO, "%s : ------------ preflag = %d", __func__, preflag);
        close(fd);

        if (buf) free(buf);
        if (root) free_tree(root); 

        return 0;
}

#else
int huffman_decompression()
{
        unsigned short decom_array[9][256];
        char filename[256];
        char rbuf[8];
        unsigned short bits, newcode;
        int n, m, fd, i;
        int file_len;
        unsigned char tmp, minbit, maxbit;
        char flag, preflag, last_ch_bits, oldcode;
        char str[2] = {0,0};
        char *buf = NULL;

        // 1. get the array
        if (get_header(TMP_FILE, _header, &file_len)) {
                syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to get_header", __func__);
                return -1;
        }

        //array[code][0] : bits
        //array[code][1] : newcode 
        minbit = _header->typeflag[0];
        maxbit = _header->typeflag[1];
        last_ch_bits = _header->typeflag[2];
        printf("last_ch_bits = %d\n", last_ch_bits);
        // init
        for (m = 0; m < 9; m++) {
                for (n = 0; n < 256; n++) {
                        decom_array[m][n] = 0;
                }
        }
        i = 0;
        // decom_array[bits][newcode]
        // bits : 1-16 ; newcode : 0x0000-0xffff
        while (i < 256) { // 0x00 - 0xff 
                bits = _header->bits[i];
                m = bits & 0xff;
                if (bits >= minbit && bits <= maxbit) {
                        if(i == 0) { // set it
                                decom_array[0][0] = bits;
                                decom_array[0][1] = _header->newcode[i];
                                decom_array[0][2] = 0x00;
                        } else {
                                n = (_header->newcode[i]) & 0xffff;
                                decom_array[m][n] = i & 0xff;
                        }
                }
                ++i;
        }
        syslog(LOG_USER | LOG_INFO, "%s : huffman file length : %d.", __func__, file_len);
        buf = (char *)malloc(sizeof(char) * (file_len + 1));
        if (buf == NULL) {
                syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to malloc for : %s, file buf", __func__, TMP_FILE);
                return -1;
        }
        if (get_file_buf(TMP_FILE, buf, file_len)) {
                syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to get_file_buf", __func__);
                free(buf);
                return -1;
        }
        // 2. read file to decompression .
        snprintf(filename, sizeof filename, "%s%s.png", "test", _header->name);
        fd = open(filename, O_RDWR | O_TRUNC | O_CREAT, 0644);
        if (fd < 0) {
                syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to open : %s", __func__, filename);
                free(buf);
                return -1;
        }
        printf("OKAY to open %s, buf : %p\n", filename, buf);
        printf("%s : minbit : %d, maxbit : %d\n", __func__, minbit, maxbit);
       
        //bits = 0;
        i = 0;
        flag = 0;
        preflag = ONE_CHAR;
        do {
                if (i < (file_len - 1)) {// fixme
                        tmp = (buf[i] << (ONE_CHAR - preflag)) | ((buf[1+i] >> preflag) & ((1 << (ONE_CHAR - preflag)) - 1)); 
                } else if (i == (file_len - 1) && preflag > 0) {
                        tmp = buf[i] << (last_ch_bits - preflag);
                        //printf("buf[%d]=0x%02x, last_ch_bits=%d, preflag=%d\n", i, (*(buf+i)) & 0xff, last_ch_bits, preflag);
                } else {
                        break;
                }
                printf("%s : Decompression : buf[%03d]=%c=0x%02x.\n", __func__, i, buf[i], buf[i]&0xff);
                for (bits = minbit; bits <= maxbit; bits++) {
                        flag = ONE_CHAR - bits;
                        newcode = (tmp >> flag) & ((1 << bits) - 1); 
                        if (decom_array[bits][newcode] == '\0') {
                                if (decom_array[0][0] == bits && decom_array[0][1] == newcode) {
                                        oldcode = 0;
                                        str[0] = oldcode;
                                        if (1 != write(fd, str, 1)) {
                                                syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to Decompression write : %s", __func__, str);
                                                close(fd);
                                                free(buf);
                                                return -1;
                                        }
                                        //printf("%s : Decompression : %c . 0x%02x. bits=%d, newcode=0x%02x\n", __func__, oldcode, oldcode, bits, newcode);
                                        break;
                                } else {
                                        continue;
                                }
                        } else {
                                oldcode = decom_array[bits][newcode] & 0xff; 
                                str[0] = oldcode;
                                if (1 != write(fd, str, 1)) {
                                        syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to Decompression write : %s", __func__, str);
                                        close(fd);
                                        free(buf);
                                        return -1;
                                }
                                //syslog(LOG_USER | LOG_INFO, "%s : Decompression : %c . 0x%02x", __func__, oldcode, oldcode);
                                //printf("%s : Decompression : %c . 0x%02x. bits=%d, newcode=0x%02x\n", __func__, oldcode, oldcode, bits, newcode);
                                break;
                        }
                }
                if (preflag <= bits) {
                        ++i;
                        if (i == (file_len - 1)) {
                                preflag = last_ch_bits - (bits - preflag);
                                continue;
                        } else {
                                preflag = ONE_CHAR - (bits - preflag);
                        }
                        if (i >= file_len && preflag == ONE_CHAR) {
                                break;
                        }
                } else {
                        preflag -= bits;
                }
        } while (i < file_len || preflag > 0);
        close(fd);
        printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx buf : %p\n", buf);
        free(buf);
        printf("==============================\n");

        return 0;
}
#endif

int huffman_encode(char *src, unsigned int length, struct huffman_tags *tags)
{
        struct huffman_node *head = NULL;
        struct huffman_node *node = NULL;
        //unsigned short array[256][2];
        
        head = (struct huffman_node *)malloc(sizeof(struct huffman_node));
        //char *src = _str;

        print_debug("enter huffman\n");
        if (!(node = head)) {
                syslog(LOG_SYSTEM | LOG_ERR , "%s : Fail to malloc for head", __func__);
                return -1;
        } else if (!src || !length) {
                syslog(LOG_USER | LOG_ERR , "%s : Ugly params.", __func__);
                node_distory(&node);
                return -1;
        }
        // make the src in the huffman line
        print_debug("start to huffman_line\n");
        syslog(LOG_SYSTEM | LOG_INFO, "%s : hello huffman line", __func__);
        if (huffman_line(&head, src, length)) {
                syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_line", __func__);
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

        printf("start to huffman_tree node : %p, head : %p\n", node, head);
        //node = head;
        syslog(LOG_USER | LOG_INFO, "%s : Before huffman_tree", __func__);
        if (huffman_tree(&head)) {
                syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_tree", __func__);
                node_distory(&node);
                return -1;
        }
        print_debug("After huffman_tree\n");
        syslog(LOG_USER | LOG_INFO, "%s : After huffman_tree", __func__);
        
        print_debug("start to huffman_code\n");
        if (huffman_code(head)) {
                syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_code", __func__);
                //node_distory(&node);
                huffman_root_distory(head);
                return -1;
        }
        
        //print_debug("start to huffman_array\n");
        //if (huffman_array(head, array, tags)) {
        //syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_array", __func__);
                //node_distory(&node);
                //huffman_root_distory(head);
                //return -1;
                //}
        print_debug("start to huffman_fill_file");
        //int huffman_fill_file(const char *file, struct huffman_node *head, struct huffman_tags *tags, char *str, unsigned int length)
        if (huffman_fill_file(TMP_FILE, head, tags, src, length)) {
                syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_fill_file", __func__);
                return -1;
        } 
        // distory the node, no need any more. 
        //node_distory(&node);
        huffman_root_distory(head);
        // Compression 
        //print_debug("start to huffman_compression\n");
        //if (huffman_compression(array, src, length, tags)) {
        //syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_compression", __func__);
        //return -1;
        //}
        // Decompression 
        print_debug("start to huffman_decompression\n");
        if (huffman_decompression()) {
                syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_decompression", __func__);
                return -1;
        }
        print_debug("All goes well...\n");
        return 0;
}
