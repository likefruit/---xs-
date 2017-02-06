/*
 * Copyright 2012-2015 Xueguoliang(xueguoliang@gmail.com)
 * */
#ifdef __cplusplus
extern "C"{
#endif
#ifndef __XS_ATOMIC_H__
#define __XS_ATOMIC_H__

/*
 * type __sync_fetch_and_add(type* ptr, type value)
 * type __sync_fetch_and_sub(type* ptr, type value)
 * type __sync_fetch_and_or (type* ptr, type value)
 * type __sync_fetch_and_and(type* ptr, type value)
 * type __sync_fetch_and_xor(type* ptr, type value)
 * type __sync_fetch_add_nand(type* ptr, type value)
 *
 * */


#define xs_atomic_get(ptr) __sync_fetch_and_add(ptr, 0)
#define xs_atomic_inc(ptr) __sync_fetch_and_add(ptr, 1)
#define xs_atomic_dec(ptr) __sync_fetch_and_sub(ptr, 1)
#define xs_atomic_zero(ptr) __sync_fetch_and_and(ptr, 0)

#endif
#ifdef __cplusplus
}
#endif
