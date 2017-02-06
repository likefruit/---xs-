#ifdef __cplusplus
extern "C"{
#endif
#include "xs.h"

int xs_file_length(const char* path)
{
#ifdef WIN32
    HANDLE hFile;
    DWORD dwFileSize = 0;


    hFile = CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,NULL,
        OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if(hFile != INVALID_HANDLE_VALUE)
    {
        dwFileSize = GetFileSize(hFile,NULL);

        CloseHandle(hFile);
        return (int)dwFileSize;
    }

#else
    struct stat st;
    int ret = lstat(path, &st);
    if(ret < 0)
        return ret;

    if(S_ISREG(st.st_mode))
        return st.st_size;


#endif
    return -1;
}

int xs_file_exist(const char* path)
{
#ifdef WIN32
    DWORD dw;
    dw = GetFileAttributesA(path);
    if(dw != (DWORD)-1 && dw != FILE_ATTRIBUTE_DIRECTORY)
        return 1;
    return 0;

#else
    struct stat st;
    int ret = lstat(path, &st);
    if(ret < 0)
        return 0;

    if(S_ISREG(st.st_mode))
        return 1;

    return 0;
#endif
}

int xs_dir_exist(const char* path)
{
#ifdef WIN32
    DWORD dw;
    dw = GetFileAttributesA(path);
    if(dw != (DWORD)-1 && dw == FILE_ATTRIBUTE_DIRECTORY)
        return 1;
    return 0;
#else
    struct stat st;
    int ret = lstat(path, &st);
    if(ret < 0)
        return 0;

    if(S_ISDIR(st.st_mode))
        return 1;
    return 0;
#endif
}
#ifdef __cplusplus
}
#endif
