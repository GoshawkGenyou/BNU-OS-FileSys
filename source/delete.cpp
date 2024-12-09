#include <stdio.h>
#include "filesys.h" 
 
void delete(char *filename){
	unsigned int dirid;
	struct inode *inode;

	dirid = namei(filename);
	if (dirid == -1){
		printf("文件不存在，请检查!\n"); 
		return;
	}
	inode = iget(dir.direct[dirid].d_ino);
	if(!(inode->di_mode &DIFILE)){
		printf("对象不是文件，请检查！\n");
		iput(inode);
		return;
	}

	int blocks = (inode->di_size + BLOCKSIZ - 1) / BLOCKSIZ; // 计算文件占用的总磁盘块数
	for (int i = 0; i < blocks; i++) {
		if (inode->di_addr[i] != 0) {
			bfree(inode->di_addr[i]); // 释放磁盘块
			inode->di_addr[i] = 0; // 将磁盘块号设置为0
		}
	}

	inode->di_size = 0; // 重置文件大小为0
	
	dir.direct[dirid].d_ino = DIEMPTY;
	dir.size --;
	inode->di_number--;
	iput(inode);
	return;
}











