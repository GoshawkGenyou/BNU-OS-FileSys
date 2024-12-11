#include <stdio.h>
#include "filesys.h"




void serialize_filsys(FILE* file, const struct filsys* fs) {
    /**
        Serializes filsys struct into a file for storage
    */
    fwrite(&fs->s_isize, sizeof(fs->s_isize), 1, file); // Field 1
    fwrite(&fs->s_fsize, sizeof(fs->s_fsize), 1, file); // Field 2
    fwrite(&fs->s_nfree, sizeof(fs->s_nfree), 1, file); // Field 3
    fwrite(&fs->s_pfree, sizeof(fs->s_pfree), 1, file); // Field 4
    for (int i = 0; i < NICFREE; i++) {
        fwrite(&fs->s_free[i], sizeof(fs->s_free[i]), 1, file); // Array
    }
    fwrite(&fs->s_ninode, sizeof(fs->s_ninode), 1, file);
    fwrite(&fs->s_pinode, sizeof(fs->s_pinode), 1, file);
    for (int i = 0; i < NICINOD; i++) {
        fwrite(&fs->s_inode[i], sizeof(fs->s_inode[i]), 1, file);
    }
    fwrite(&fs->s_rinode, sizeof(fs->s_rinode), 1, file);
    fwrite(&fs->s_fmod, sizeof(fs->s_fmod), 1, file); // Last field
}
