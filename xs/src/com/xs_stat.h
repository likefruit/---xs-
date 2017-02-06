#ifdef __cplusplus
extern "C"{
#endif
#ifndef __XS_FS_H__
#define __XS_FS_H__

int xs_file_length(const char* path);
int xs_file_exist(const char* path);
int xs_dir_exist(const char* path);
#define xs_get_home_dir() getenv("HOME")

#endif
#ifdef __cplusplus
}
#endif
