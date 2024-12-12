#include "filesys.h"


void writePasswordsToFile(struct pwd* new_pwd) {
    FILE* file = fopen(PWD_FILE, "wb");
    if (file == NULL) {
        perror("Failed to open file for writing");
        return;
    }

    if (global_pwd_count > PWDNUM) {
        fprintf(stderr, "Password file contains more entries than expected.\n");
        global_pwd_count--;
        fclose(file);
        return;
    }

    // 写入密码数量
    fwrite(&global_pwd_count, sizeof(int), 1, file);

    // 写入密码数据
    fwrite(pwd, sizeof(struct pwd), global_pwd_count, file);

    fclose(file);

    printf("user %d:group %d, pwd %s have been write in file.\n", new_pwd->p_uid, new_pwd->p_gid, new_pwd->password);
}


void addUser(char* tstr, char* tstr1, char* token) {
	struct pwd* new_pwd = &pwd[global_pwd_count];
	new_pwd->p_uid = (unsigned short)atoi(tstr);
	new_pwd->p_gid = (unsigned short)atoi(tstr1);
	strncpy(new_pwd->password, token, PWDSIZ);
    new_pwd->password[PWDSIZ] = '\0';
	global_pwd_count++;
    writePasswordsToFile(new_pwd);
}