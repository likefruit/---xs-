#ifdef __cplusplus
extern "C"{
#endif
#ifndef __XS_SORT_H__
#define __XS_SORT_H__

void xs_sort(void *base, int num, int size,
      int (*cmp_func)(const void *, const void *),
      void (*swap_func)(void *, void *, int size));

#endif

#ifdef __cplusplus
}
#endif
