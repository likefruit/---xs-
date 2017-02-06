#ifdef __cplusplus
extern "C"{
#endif
#include "xs.h"

xs_config_t* xs_config_load(const char* config_file)
{
    char line[1024];
    char* key;
    char* value;
    char* saveptr;
    FILE* f = fopen(config_file, "r");
    xs_config_t* config = NULL;
    if(f == NULL)
        return NULL;

    while(fgets(line, sizeof(line), f))
    {
        saveptr = NULL;
        key = strtok_r(line, " =\n\t", &saveptr);
        if(key == NULL)
            continue;
        if(*key == '#')
            continue;

        value = strtok_r(NULL, "# \n\t", &saveptr);
        if(value == NULL)
            continue;

        config = xs_dict_add_str(config, key, value);
    }

    fclose(f);

    return config;
}

int xs_config_save(const char* file, xs_config_t* config)
{
    int ret;
    char* buf = xs_dict_to_buf_s(config, NULL, '=', '\n');
    FILE* f = fopen(file, "w");
    if(f == NULL)
        return -1;
    ret = fprintf(f, "%s", buf);

    fclose(f);
    xs_free(buf);
    return ret;
}

#ifdef __cplusplus
}
#endif
