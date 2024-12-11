#include <stdio.h>
#include "filesys.h" 
 
void delete(char *filename){
	unsigned int dirid;
	struct inode *inode;

	dirid = namei(filename);
	if (dirid == -1){
		printf("�ļ������ڣ�����!\n"); 
		return;
	}
	inode = iget(dir.direct[dirid].d_ino);
	if(!(inode->di_mode &DIFILE)){
		printf("�������ļ������飡\n");
		iput(inode);
		return;
	}

	int blocks = (inode->di_size + BLOCKSIZ - 1) / BLOCKSIZ; // �����ļ�ռ�õ��ܴ��̿���
	for (int i = 0; i < blocks; i++) {
		if (inode->di_addr[i] != 0) {
			bfree(inode->di_addr[i]); // �ͷŴ��̿�
			inode->di_addr[i] = 0; // �����̿������Ϊ0
		}
	}

	inode->di_size = 0; // �����ļ���СΪ0
	
	dir.direct[dirid].d_ino = DIEMPTY;
	dir.size --;
	inode->di_number--;
	iput(inode);
	return;
}











