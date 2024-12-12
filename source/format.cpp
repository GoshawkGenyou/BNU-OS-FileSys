#include <stdio.h> 
#include <string.h>
#include "filesys.h" 

int global_pwd_count = 0;

void readPasswordsFromFile(struct pwd* pwds, int maxCount);
void writePasswordsToFile(struct pwd* pwds, int count);

void format(){
	struct inode *inode;
	struct direct dir_buf[BLOCKSIZ/(DIRSIZ+4)];
	struct pwd passwd[PWDNUM]; // modify to not be magic number
	unsigned int block_buf[BLOCKSIZ/sizeof(int)];
	int i,j;

	//初始化硬盘 
	memset(disk, 0x00, ((DINODEBLK+FILEBLK+2)*BLOCKSIZ));

	/* 0.initialize the passwd */
	/*passwd[0].p_uid = 2116;
	passwd[0].p_gid = 03;
	strncpy(passwd[0].password, "dddd", PWDSIZ);
	passwd[0].password[PWDSIZ] = '\0';

	passwd[1].p_uid = 2117;
	passwd[1].p_gid = 03;
	strncpy(passwd[1].password, "bbbb", PWDSIZ);
	passwd[1].password[PWDSIZ] = '\0';

	passwd[2].p_uid = 2118;
	passwd[2].p_gid = 04;
	strncpy(passwd[2].password, "abcd", PWDSIZ);
	passwd[2].password[PWDSIZ] = '\0';

	passwd[3].p_uid = 2119;
	passwd[3].p_gid = 04;
	strncpy(passwd[3].password, "cccc", PWDSIZ);
	passwd[3].password[PWDSIZ] = '\0';

	passwd[4].p_uid = 2120;
	passwd[4].p_gid = 05;
	strncpy(passwd[4].password, "eeee", PWDSIZ);
	passwd[4].password[PWDSIZ] = '\0';


	writePasswordsToFile(passwd, 5);*/

	readPasswordsFromFile(passwd, PWDNUM);
	//printf("%d", global_pwd_count);


	/* 1.creat the main directory and its sub dir etc and the file password */

	inode = iget(0);   /* 0 empty dinode id*/
	inode->di_number = 1;			//??空i节点有何用????
	inode->di_mode = DIEMPTY;
	iput(inode);

	inode = iget(1);   /* 1 main dir id*/
	inode->di_number = 1;
	inode->di_mode = DEFAULTMODE | DIDIR;
	inode->di_size = 3*(DIRSIZ + 4);
	inode->di_addr[0] = 0; /*block 0# is used by the main directory*/
	
	strncpy(dir_buf[0].d_name,"..", DIRSIZ - 1);
	dir_buf[0].d_ino = 1;
	strncpy(dir_buf[1].d_name,".", DIRSIZ - 1);
	dir_buf[1].d_ino = 1;
	strncpy(dir_buf[2].d_name,"etc", DIRSIZ - 1);
	dir_buf[2].d_ino = 2;

	memcpy(disk+DATASTART, &dir_buf, 3*(DIRSIZ+4));
	iput(inode);

	inode = iget(2);  /* 2  etc dir id */ 
	inode->di_number = 1;
	inode->di_mode = DEFAULTMODE | DIDIR;
	inode->di_size = 3*(DIRSIZ + 4);
	inode->di_addr[0] = 1; /*block 1# is used by the etc directory*/
	
	strcpy(dir_buf[0].d_name,"..");
	dir_buf[0].d_ino = 1;
	strcpy(dir_buf[1].d_name,".");
	dir_buf[1].d_ino = 2;
	strncpy(dir_buf[2].d_name,"password", DIRSIZ - 1);
	dir_buf[2].d_ino = 3;

	memcpy(disk+DATASTART+BLOCKSIZ*1, dir_buf, 3*(DIRSIZ+4));
	iput(inode);

	inode = iget(3);  /* 3  password id */
	inode->di_number = 1;
	inode->di_mode = DEFAULTMODE | DIFILE;
	inode->di_size = BLOCKSIZ;
	inode->di_addr[0] = 2; /*block 2# is used by the password file*/

	for (i=global_pwd_count; i<PWDNUM; i++){
		passwd[i].p_uid = 0;
		passwd[i].p_gid = 0;
		strncpy(passwd[i].password, "            ", PWDSIZ);  // PWDSIZ " "
	}
	
	memcpy(pwd, passwd, PWDNUM*sizeof(struct pwd)); // modify to use const values
	memcpy(disk+DATASTART+BLOCKSIZ*2, passwd, BLOCKSIZ);
	iput(inode);

	/*2. initialize the superblock */

	filsys.s_isize = DINODEBLK;
	filsys.s_fsize = FILEBLK;

	filsys.s_ninode = DINODEBLK * BLOCKSIZ/DINODESIZ - 4;
	filsys.s_nfree = FILEBLK - 3;

	for (i=0; i < NICINOD; i++){
		/* begin with 4,    0,1,2,3, is used by main,etc,password */ 
		filsys.s_inode[i] = 4+i;
	}

	filsys.s_pinode = 0;
	filsys.s_rinode = NICINOD + 4; 

	block_buf[NICFREE-1] = FILEBLK+1;  /*FILEBLK+1 is a flag of end*/
	for (i=0; i<NICFREE-1; i++)
		block_buf[NICFREE-2-i] = FILEBLK-i-1;			//从最后一个数据块开始分配??????

	memcpy(disk+DATASTART+BLOCKSIZ*(FILEBLK-NICFREE), block_buf, BLOCKSIZ);
	for (i=FILEBLK-2*NICFREE+1; i>2; i-=NICFREE){
		for (j=0; j<NICFREE;j++){
			block_buf[j] = i+j;
		}
		memcpy(disk+DATASTART+BLOCKSIZ*(i-1), block_buf, BLOCKSIZ);
	}
	i += NICFREE;
	j = 1;
	for (; i>3; i--)
	{
		filsys.s_free[NICFREE-j] = i-1; 
		j ++;
	}
	
	filsys.s_pfree = NICFREE - j+1; 
	memcpy(disk+BLOCKSIZ, &filsys, sizeof(struct filsys));
	return;
	
}


void readPasswordsFromFile(struct pwd* pwds, int maxCount) {
	FILE* file = fopen(PWD_FILE, "rb"); // 打开文件
	if (file == NULL) {
		perror("Failed to open file for reading");
		return;
	}

	// 读取密码数量
	fread(&global_pwd_count, sizeof(int), 1, file);

	if (global_pwd_count > maxCount) {
		fprintf(stderr, "Password file contains more entries than expected.\n");
		global_pwd_count = maxCount;
	}

	// 读取密码数据
	fread(pwds, sizeof(struct pwd), global_pwd_count, file);

	fclose(file);
}


void writePasswordsToFile(struct pwd* pwds, int count) {
	FILE* file = fopen(PWD_FILE, "wb"); // 以二进制写入模式打开文件
	if (file == NULL) {
		perror("Failed to open file for writing");
		return;
	}

	global_pwd_count = count;

	// 写入密码数量
	fwrite(&global_pwd_count, sizeof(int), 1, file);

	// 写入密码数据
	fwrite(pwds, sizeof(struct pwd), global_pwd_count, file);

	fclose(file);
}
