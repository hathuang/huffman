/*
http://dungenessbin.diandian.com/post/2012-05-23/21949784
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "huffman.h"
#include "syslog.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef Debug
#define print_debug(x)     printf(x)
//#define syslog(x,y,z) Syslog(x,y,z) 
#else
#define print_debug(x)
//#define syslog(x,y,z) do{}while(0)
#endif

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
                        //syslog(LOG_USER | LOG_DEBUG, "%s : new char = %c = 0x%02x", __func__, ch, ch);

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
        // only 255 elements in the line at most; // 256
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
        //if (flag)
        //syslog(LOG_USER | LOG_DEBUG, "%s : place : %p\n", __func__, p);
        if (flag != 0 && flag != 1) {
                return 0;
        } 
        
        do {
                switch (flag) {
                case 0:
                        printf("Huffman : char = %c = 0x%02x, bits = %d, newcode = 0x%02x, priority = %d\n",
                                p->data, p->data, p->bits, p->newcode, p->priority);
                        break;
                case 1:
                        syslog(LOG_USER | LOG_DEBUG, "%s : char=%c=0x%02x,bits=%d,newcode=0x%02x,priority=%d,my=%p,next=%p,lnext=%p,rnext=%p",
                                __func__, p->data, p->data, p->bits, p->newcode, p->priority, p, p->next, p->lnext, p->rnext);
                        break;
                default:
                        break;
                }
        } while (p = p->next);
        
        return 0;
}

int huffman_array(struct huffman_node **_head, char (*arr)[2], struct huffman_header *header)
{
        struct huffman_node *q = *_head;
        int n;

        if (!q || !arr || !header) {
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
                if (q->bits & 0x80) {
                        continue;
                }
                n = (q->data) & 0x00ff;
                arr[n][0] = q->bits;
                arr[n][1] = q->newcode;
                header->bits[n] = q->bits; 
                header->newcode[n] = q->newcode; 
                syslog(LOG_SYSTEM | LOG_DEBUG, "%s : arr[%c][0]=%d, newcode=0x%x", __func__, q->data, arr[n][0], arr[n][1]);
                syslog(LOG_SYSTEM | LOG_INFO, "%s : oldcode:%c=0x%x, bits:%d, newcode=0x%x", __func__, q->data, q->data & 0xff, arr[n][0], arr[n][1]&0xff);
        } while (q = q->next);
        
        return 0;
}

int huffman_compression(char (*arr)[2], char *src, unsigned int length, struct huffman_header *header)
{
        char ch = '\0';
        char newchar = '\0';
        char *str = src;
        int n;
        unsigned int i = 0;
        char bits = 0;
        char flag = 0;
        char newstr[2];
        
        char maxbit, minbit;

        //arr[ch][0] // bits
        //arr[ch][1] // newcode

        if (!length || !header) {
                syslog(LOG_USER | LOG_ERR , "%s : Ugly params", __func__);
                return -1;
        } 
        int fd = open(TMP_FILE, O_RDWR | O_TRUNC | O_CREAT, 0644); 
        if (fd < 0) {
                syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to open : %s", __func__, TMP_FILE);
                return -1;
        }
        // write huffman_header
        int ret, len;
        i = 0;
        len = sizeof(header->newcode);
        flag = 1;
        while (i < len) {
                if (header->bits[i]) {
                        if (flag) {
                                flag = 0;
                                maxbit = header->bits[i]; 
                                minbit = maxbit;
                                //newstr[0] = i & 0xff;
                                //newstr[1] = i & 0xff;
                        } else if (maxbit < header->bits[i]) {
                                maxbit = header->bits[i];
                                //newstr[1] = i & 0xff;
                        } else if (minbit > header->bits[i]) {
                                minbit = header->bits[i];
                                //newstr[0] = i & 0xff;
                        }
                }
                ++i;
        }
        header->typeflag[1] = maxbit; 
        header->typeflag[0] = minbit; 
        //printf("%s : minbit : %d,minch:=%c=0x%x, maxbit : %d,maxch:=%c=0x%x\n", __func__, minbit, newstr[0], newstr[0]&0xff, maxbit, newstr[1], newstr[1]&0xff);
        len = sizeof(struct huffman_header);
        ret = write(fd, (char *)header, len);
        if (ret != len) {
                syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to write to : %s, ret=%d,len=%d", __func__, TMP_FILE, ret, len);
                return -1;
        }
        newstr[1] = 0;
        i = 0;
        flag = 0;
        while (i < length) {
                ch = *(str + i++);
                n = ch & 0x00ff;
                bits = arr[n][0];
                flag += bits;
                //syslog(LOG_SYSTEM | LOG_DEBUG, "%s : Compression # : %c, flag = %d, bits=%d", __func__, ch, flag, bits);
                if (flag < ONE_CHAR) {
                        newchar = newchar << bits;
                        newchar |= arr[n][1] & ((1 << bits) - 1);
                        continue;
                } else if (flag == ONE_CHAR) {
                        newchar = newchar << bits;
                        newchar |= arr[n][1] & ((1 << bits) - 1);
                        // write newchar  ONE_CHAR
                        newstr[0] = newchar;
                        if (1 != (ret = write(fd, newstr, 1))) {
                                syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to write to : %s, ret=%d,len=%d", __func__, TMP_FILE, ret, len);
                                close(fd);
                                return -1;
                        }
                        syslog(LOG_SYSTEM | LOG_INFO, "%s : Compression = : 0x%x, flag = %d", __func__, newchar & 0xff, flag);
                        newchar = 0;
                        flag = 0;
                } else {
                        newchar = newchar << (ONE_CHAR - (flag - bits));
                        syslog(LOG_SYSTEM | LOG_DEBUG, "%s : Compression x :%d", __func__, ONE_CHAR - (flag - bits));
                        newchar |= (arr[n][1] >> (flag - ONE_CHAR)) & ((1 << (ONE_CHAR - (flag - bits))) - 1);
                        // write newchar  ONE_CHAR
                        newstr[0] = newchar;
                        if (1 != (ret = write(fd, newstr, 1))) {
                                syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to write to : %s, ret=%d,len=%d", __func__, TMP_FILE, ret, len);
                                close(fd);
                                return -1;
                        }
                        syslog(LOG_SYSTEM | LOG_INFO, "%s : Compression > : 0x%x, flag = %d", __func__, newchar & 0xff, flag);
                        flag = flag - ONE_CHAR;
                        syslog(LOG_SYSTEM | LOG_DEBUG, "%s : Compression > : 0x%x, flag = %d", __func__, newchar & 0xff, flag);
                        newchar = arr[n][1] & ((1 << flag) - 1);
                }
        }
        if (flag) { // FIXME
                // write;
                newstr[0] = newchar;
                if (1 != (ret = write(fd, newstr, 1))) {
                        syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to write to : %s, ret=%d,flag=%d", __func__, TMP_FILE, ret, flag);
                        close(fd);
                        return -1;
                }
                syslog(LOG_SYSTEM | LOG_WARNING, "%s : Compression > : 0x%x, flag = %d", __func__, newchar & 0xff, flag);
        }
        
        close(fd);
        return 0;
}

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
        if (!filename || !_buf || !file_len) {
                syslog(LOG_USER| LOG_ERR, "%s : Ugly params", __func__);
                return -1;
        } 
        int fd = open(filename, O_RDWR); 
        if (!fd) {
                perror("Fail to open TMP_FILE");
                return -1;
        }
        int len = read(fd, _buf, HUFFMAN_HEADER_SIZE);
        if (len != HUFFMAN_HEADER_SIZE) {
                syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to get enough bytes : %d is wanna, but get %d", __func__, HUFFMAN_HEADER_SIZE, len);
                return -1;
        }
        len = 0;
        int ret;
        while (len < file_len) {
                ret = read(fd, _buf + len, 1024);
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

int huffman_decompression()
{
        char decom_array[9][256];
        char filename[256];
        char rbuf[1024];
        char bits, newcode;
        char *buf = NULL;
        int n, m, fd, i;
        int file_len;
        char minbit, maxbit;
        char flag, preflag;
        char oldcode;
        char tmp;
        char str[2] = {0,0};
        // 1. get the array
        //struct huffman_header *_header = (struct huffman_header *)malloc(sizeof(struct huffman_header));
        struct huffman_header *_header = (struct huffman_header *)rbuf;
        if (get_header(TMP_FILE, _header, &file_len)) {
                syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to get_header", __func__);
                //free(_header);
                return -1;
        }

        //array[code][0] : bits
        //array[code][1] : newcode 
        minbit = _header->typeflag[0];
        maxbit = _header->typeflag[1];
        // init
        for (m = 0; m < 9; m++) {
                for (n = 0; n < 256; n++) {
                        decom_array[m][n] = 0;
                }
        }
        i = 0;
        while (i < 256) { // 0x00 - 0xff 
                //decom_array[bits][newcode]
                //bits : 1-8 ; newcode : 0x00-0xff
                bits = _header->bits[i];
                m = bits & 0xff;
                if (bits >= minbit && bits <= maxbit) {
                        if(i == 0) { // set it
                                // set oldcode which=0x00 @ a special place,
                                // details as follow.
                                // decom_array[0][0]=bits of oldcode which = 0x00
                                // decom_array[0][1]=newcode of oldcode which = 0x00
                                // decom_array[0][2]=oldcode of oldcode which = 0x00, easily it's 0.
                                decom_array[0][0] = bits;
                                decom_array[0][1] = _header->newcode[i];
                                decom_array[0][1] = 0x00;
                        } else {
                                n = (_header->newcode[i]) & 0xff;
                                decom_array[m][n] = i & 0xff;
                        }
                }
                ++i;
        }
        syslog(LOG_USER | LOG_INFO, "%s : huffman file length : %d.", __func__, file_len);
        buf = (char *)malloc(file_len);
        if (buf == NULL) {
                syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to malloc for : %s, file buf", __func__, TMP_FILE);
                //free(_header);
                return -1;
        }
        if (get_file_buf(TMP_FILE, buf, file_len)) {
                syslog(LOG_SYSTEM | LOG_ERR, "%s : fail to get_file_buf", __func__);
                //free(_header);
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
        printf("OKAY to open %s\n", filename);
        printf("%s : minbit : %d, maxbit : %d\n", __func__, minbit, maxbit);
       
        bits = 0;
        i = 0;
        flag = 0;
        preflag = ONE_CHAR;
        int k = 0;
        do {
                // Decompression
                // TODO  there's a more easy method to do it 
                if (i < file_len) {
                        tmp = (buf[i] << (ONE_CHAR - preflag)) | ((buf[1+i] >> preflag) & ((1 << (ONE_CHAR - preflag)) - 1)); 
                } else {
                        break;

                        tmp = buf[i] << (ONE_CHAR - preflag); 
                }
                for (bits = minbit; bits <= maxbit; bits++) {
                        flag = ONE_CHAR - bits;
                        newcode = (tmp >> flag) & ((1 << bits) - 1); 
                        if (decom_array[bits][newcode] == 0) {
                                if (decom_array[0][0] == bits && decom_array[0][1] == newcode) {
                                        // write to file
                                        oldcode = 0x00;
                                        str[0] = oldcode;
                                        write(fd, str, 1);
                                        break;
                                } else {
                                        continue;
                                }
                        } else {
                                // write to file 
                                oldcode = decom_array[bits][newcode]; 
                                str[0] = oldcode;
                                write(fd, str, 1);
                                break;
                        }
                }
                if (preflag <= bits) {
                       preflag = ONE_CHAR - (bits - preflag);
                       ++i;
                       if (i > file_len && preflag == ONE_CHAR) {
                               printf("Decompression All goes well...\n");
                               break;
                       } 
                } else {
                        preflag = preflag - bits;
                }
                k++;
        } while (i < file_len || preflag);
        close(fd);
        printf("how many time in the loop : %d , file length : %d\n", k, file_len);

        return 0;
}

int huffman_encode(char *_str, unsigned int length, struct huffman_header *header)
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
        
        //if (huffman_sort(&head, HUFFMAN_SORT_BIG_FIRST)) {
        //syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_sort big first", __func__);
        //node_distory(&node);
        //return -1;
        //}
        
        print_debug("start to huffman_array\n");
        if (huffman_array(&node, array, header)) {
                syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_array", __func__);
                node_distory(&node);
                return -1;
        }
        // distory the node, no need any more. 
        node_distory(&node);
        // Compression 
        print_debug("start to huffman_compression\n");
        if (huffman_compression(array, src, length, header)) {
                syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_compression", __func__);
                return -1;
        }
        // Decompression 
        print_debug("start to huffman_decompression\n");
        if (huffman_decompression()) {
                syslog(LOG_USER | LOG_ERR , "%s : Fail to huffman_decompression", __func__);
                return -1;
        }

        print_debug("All goes well...\n");
        return 0;
}
