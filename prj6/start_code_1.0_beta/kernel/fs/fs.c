/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *            Copyright (C) 2019 University of Chinese Academic of Science, UCAS
 *               Author : Zhang Lei (Email : zhanglei171@mails.ucas.ac.cn)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *                       File System .C files
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * */

#include "type.h"
#include "fs.h"

uint32_t global_inode_id = 0;

void init_superblock(void)
{
    superblock.fs_size          = FS_SIZE;
    superblock.start_addr       = FS_START_ADDR;

    superblock.blockmap_addr    = FS_START_ADDR + BLOCK_SIZE;
    superblock.blockmap_numb    = BLOCKMAP_NUM;

    superblock.inodemap_addr    = FS_START_ADDR + BLOCKMAP_NUM * BLOCK_SIZE;
    superblock.inode_numb       = INODEMAP_NUM;  

    superblock.inode_addr       = FS_START_ADDR + (BLOCKMAP_NUM + INODEMAP_NUM) * BLOCK_SIZE;
    superblock.inode_numb       = INODE_NUM;

    superblock.datablock_addr   = FS_START_ADDR + (BLOCKMAP_NUM + INODEMAP_NUM + INODE_NUM) * BLOCK_SIZE;    
    superblock.datablock_numb   = DATABLOCK_NUM;

    superblock.magic_number     = FS_MAGIC_NUMBER;

    sdwrite(&superblock, FS_START_ADDR, sizeof(superblock_t));

    uint32_t zero_start;
    zero_start = superblock.start_addr + sizeof(superblock_t);
    uint32_t i;
    uint8_t zero;
    zero = 0;
    for(i = 0; i < BLOCKMAP_NUM * BLOCK_SIZE - sizeof(superblock_t); i++)
        sdwrite(&zero, zero_start + i, 1);
}

void init_blockmap(void)
{
    uint32_t i;
    uint32_t zero_start = superblock.blockmap_addr;
    uint8_t zero;
    zero = 0;
    for(i = 0; i < BLOCKMAP_NUM * BLOCK_SIZE; i++)
        sdwrite(&zero, zero_start + i, 1);
}

void init_inodemap(void)
{
    uint32_t i;
    uint32_t zero_start = superblock.inodemap_addr;
    uint8_t zero;
    zero = 0;
    for(i = 0; i < INODEMAP_NUM * BLOCK_SIZE; i++)
        sdwrite(&zero, zero_start + i, 1);
}

void init_inode(void)
{
    uint32_t i;
    inode.inode_id = global_inode_id++;
    inode.mode = O_NONE;
    inode.size = BLOCK_SIZE;
    for(i = 0; i < MAX_DIR_BLOCK; i++)
        inode.direct[i] = 0;
    inode.Indirect = 0;
    inode.Double_Indirect = 0;

    sdwrite((char *)(&inode), superblock.inode_addr, sizeof(inode_t));
    
    uint32_t zero_start = superblock.inode_addr + sizeof(inode_t);
    uint8_t zero;
    zero = 0;
    for(i = 0; i < INODE_NUM * BLOCK_SIZE - sizeof(inode_t); i++)
        sdwrite(&zero, zero_start + i, 1);
}

void print_superblock_info(void)
{
    my_printf("     magic : 0x%x                                 \n", superblock.magic_number);
    my_printf("     num sector : %d, start sector : %d           \n", BLOCK_SIZE/512, superblock.start_addr/512);
    my_printf("     inode map offset : %d (%d)                   \n", 1, superblock.inodemap_numb);
    my_printf("     block map offset : %d (%d)                   \n", 2, superblock.blockmap_numb);
    my_printf("     inode offset : %d (%d)                       \n", 3, superblock.inode_numb);
    my_printf("     data offset : %d (%d)                        \n", 4, superblock.datablock_numb);
    my_printf("     inode entry size : %dB, dir entry size : %dB \n",32, 32);
    screen_reflush();
}

// Direction Operations
int mkfs(int fs_size)
{
    // Create FS

    my_printf("[FS] Setting File System!                  \n");
    screen_reflush();

    my_printf("[FS] Setting SuperBlock...                 \n");
    screen_reflush();
    init_superblock();          // 1. Init SuperBlock
    print_superblock_info();

    my_printf("[FS] Setting Block_Map...                  \n");
    screen_reflush();
    init_blockmap();            // 2. Init Block Map

    my_printf("[FS] Setting Inode_Map...                  \n");
    screen_reflush();          	 
    init_inodemap();            // 3. Init Inode Map


    my_printf("[FS] Setting Inode...                      \n");
    screen_reflush();      
    init_inode();               // 4. Init Inode

    my_printf("[FS] File System Initialize Succeeded...   \n");
    screen_reflush();    
}

int mkdir(char *dir_name)
{
    // Create Dir
    vt100_move_cursor(1, 1);
    printk("mkdir, dir_name = %s                 ",dir_name);
}

int rmdir(char *dir_name)
{
    // Remove file
    vt100_move_cursor(1, 1);
    printk("rmdir, dir_name = %s                 ",dir_name);
}

int read_dir(void)
{
    // ls
    vt100_move_cursor(1, 1);
    printk("ls                                  ");
}

int fs_info(void)
{
    // statfs
    vt100_move_cursor(1, 1);
    printk("statfs                               ");
}

int enter_fs(char *path)
{
    // cd
    vt100_move_cursor(1, 1);
    printk("cd, path = %s                        ",path);
}



// File System Operations
int mknod(char *fname)
{
    // touch
    vt100_move_cursor(1, 1);
    printk("touch, fname = %s                    ",fname);
}

int fcat(char *fname)
{
    // cat
    vt100_move_cursor(1, 1);
    printk("cat, fname = %s                      ",fname);
}

int fopen(char *fname, int access)
{
    // open file
    vt100_move_cursor(1, 1);
    printk("fopen, fname = %s, access = %d       ",fname,access);
}

int fread(int fd, char *buff, int size)
{
    // read file
    vt100_move_cursor(1, 1);
    printk("fread                                ");
}

int fwrite(int fd, char *buff, int size)
{
    // write file
    vt100_move_cursor(1, 1);
    printk("fwrite                                ");
}

int fclose(int fd)
{
    // close file
        vt100_move_cursor(1, 1);
    printk("fclose                                ");
}

int frename(char *old_fname, char *new_fname)
{
    // rename file
    vt100_move_cursor(1, 1);
    printk("frename                                ");
}

int ffind(char *path, char *fname)
{
    // find file
    vt100_move_cursor(1, 1);
    printk("ffind                                ");
}

int flink(char *target, char *linkname)
{
    // link file
    vt100_move_cursor(1, 1);
    printk("flink                                ");
}

int fseek(int fd, uint32_t offset)
{
    // seek file
    vt100_move_cursor(1, 1);
    printk("fseek                                ");
}