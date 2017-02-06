#ifdef __cplusplus
extern "C"{
#endif
#ifndef __XS_SORT_H__
#define __XS_SORT_H__

void xs_sort(char *base, int num, int size,
      int (*cmp_func)(const char *, const char *),
      void (*swap_func)(char *, char *, int size));

#endif

#ifdef __cplusplus
}
#endif
