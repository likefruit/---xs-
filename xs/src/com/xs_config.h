#ifdef __cplusplus
extern "C"{
#endif
#ifndef __XS_CONFIG_H__
#define __XS_CONFIG_H__

typedef xs_dict_t xs_config_t;

xs_config_t* xs_config_load(const char* file);
int xs_config_save(const char* file, xs_config_t* config);
#define xs_config_destroy xs_dict_destroy

#endif
#ifdef __cplusplus
}
#endif
