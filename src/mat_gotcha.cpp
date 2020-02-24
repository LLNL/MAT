// Copyright 2019 Lawrence Livermore National Security, LLC
// See the top-level LICENSE file for details.
//
// SPDX-License-Identifier: Apache-2.0

#include <gotcha/gotcha.h>

#include "mat_util.h"
#include "mat_alloc_tree.h"

#define MAT_WRAPPER_(prefix, func) prefix ## _ ## func
#define MAT_WRAPPER(func) MAT_WRAPPER_(wrapper, func)

#define MAT_WRAPPEE_HANDLER_(prefix, func) prefix ## _ ## func ## _ ## handler
#define MAT_WRAPPEE_HANDLER(func) MAT_WRAPPEE_HANDLER_(wrappee, func)

#define MAT_WRAPPEE_(prefix, func) prefix ## _ ## func
#define MAT_WRAPPEE(func) MAT_WRAPPEE_(wrappee, func)

#define MAT_BINDING(name) { #name, (void*)(MAT_WRAPPER(name)), &MAT_WRAPPEE_HANDLER(name)}

#define MAT_USER_CODE_BEGIN	       \
  do {				       \
    if (mat_reentry++ > 0)  goto end;  \
  } while(0)

#define MAT_USER_CODE_END \
end:			  \
  do {			  \
    mat_reentry--;	  \
  } while(0)


/* YYY */
//static XXX   MAT_WRAPPER(YYY) (ZZZ);
//static XXX (*MAT_WRAPPEE(YYY))(ZZZ);
//static gotcha_wrappee_handle_t MAT_WRAPPEE_HANDLER(YYY) = NULL;
/* malloc */
static void*   MAT_WRAPPER(malloc) (size_t size);
static void* (*MAT_WRAPPEE(malloc))(size_t size);
static gotcha_wrappee_handle_t MAT_WRAPPEE_HANDLER(malloc) = NULL;
/* realloc */
static void*   MAT_WRAPPER(realloc) (void *ptr, size_t size);
static void* (*MAT_WRAPPEE(realloc))(void *ptr, size_t size);
static gotcha_wrappee_handle_t MAT_WRAPPEE_HANDLER(realloc) = NULL;
/* calloc */
static void*   MAT_WRAPPER(calloc) (size_t nmemb, size_t size);
static void* (*MAT_WRAPPEE(calloc))(size_t nmemb, size_t size);
static gotcha_wrappee_handle_t MAT_WRAPPEE_HANDLER(calloc) = NULL;
/* posix_memalign */
static int   MAT_WRAPPER(posix_memalign) (void **ptr, size_t alignment, size_t size);
static int (*MAT_WRAPPEE(posix_memalign))(void **ptr, size_t alignment, size_t size);
static gotcha_wrappee_handle_t MAT_WRAPPEE_HANDLER(posix_memalign) = NULL;
/* aligned_alloc */
static void*   MAT_WRAPPER(aligned_alloc) (size_t alignment, size_t size);
static void* (*MAT_WRAPPEE(aligned_alloc))(size_t alignment, size_t size);
static gotcha_wrappee_handle_t MAT_WRAPPEE_HANDLER(aligned_alloc) = NULL;
/* valloc */
static void*   MAT_WRAPPER(valloc) (size_t size);
static void* (*MAT_WRAPPEE(valloc))(size_t size);
static gotcha_wrappee_handle_t MAT_WRAPPEE_HANDLER(valloc) = NULL;
/* memalign */
static void*   MAT_WRAPPER(memalign) (size_t alignment, size_t size);
static void* (*MAT_WRAPPEE(memalign))(size_t alignment, size_t size);
static gotcha_wrappee_handle_t MAT_WRAPPEE_HANDLER(memalign) = NULL;
/* pvalloc */
static void*   MAT_WRAPPER(pvalloc) (size_t);
static void* (*MAT_WRAPPEE(pvalloc))(size_t);
static gotcha_wrappee_handle_t MAT_WRAPPEE_HANDLER(pvalloc) = NULL;
/* free */
static void   MAT_WRAPPER(free) (void *ptr);
static void (*MAT_WRAPPEE(free))(void *ptr);
static gotcha_wrappee_handle_t MAT_WRAPPEE_HANDLER(free) = NULL;
/* mmap */
static void*   MAT_WRAPPER(mmap) (void *addr, size_t length, int prot, int flags, int fd, off_t offset);
static void* (*MAT_WRAPPEE(mmap))(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
static gotcha_wrappee_handle_t MAT_WRAPPEE_HANDLER(mmap) = NULL;
/* munmap */
static int   MAT_WRAPPER(munmap) (void *addr, size_t length);
static int (*MAT_WRAPPEE(munmap))(void *addr, size_t length);
static gotcha_wrappee_handle_t MAT_WRAPPEE_HANDLER(munmap) = NULL;
/* sbrk */
static void*   MAT_WRAPPER(sbrk) (intptr_t increment);
static void* (*MAT_WRAPPEE(sbrk))(intptr_t increment);
static gotcha_wrappee_handle_t MAT_WRAPPEE_HANDLER(sbrk) = NULL;
/* brk */
static int   MAT_WRAPPER(brk) (void *addr);
static int (*MAT_WRAPPEE(brk))(void *addr);
static gotcha_wrappee_handle_t MAT_WRAPPEE_HANDLER(brk) = NULL;
/* --- */

// static void** wrappees[] =
// {
//   &(void*)MAT_WRAPPEE(malloc),
//   &(void*)MAT_WRAPPEE(realloc),
//   &(void*)MAT_WRAPPEE(calloc),
//   &(void*)MAT_WRAPPEE(free),
//   &(void*)MAT_WRAPPEE(mmap),
//   &(void*)MAT_WRAPPEE(munmap),
//   &(void*)MAT_WRAPPEE(sbrk),
//   &(void*)MAT_WRAPPEE(brk),
// };

static struct gotcha_binding_t wrap_actions [] = {
  MAT_BINDING(free),
  MAT_BINDING(malloc),
  MAT_BINDING(realloc),
  MAT_BINDING(calloc),
  MAT_BINDING(posix_memalign),
  MAT_BINDING(aligned_alloc),
  MAT_BINDING(valloc),
  MAT_BINDING(memalign),
  MAT_BINDING(pvalloc),
  MAT_BINDING(mmap),
  MAT_BINDING(munmap),
  MAT_BINDING(sbrk),
  MAT_BINDING(brk),
};

static int mat_reentry = 0;

void mat_gotcha_init()
{
  mat_alloc_tree_init();
  //  MAT_DBG("%s called: %p", __func__, MAT_WRAPPEE(malloc));
  //  MAT_WRAPPEE(XXX)  = (YYY (*)(ZZZ))gotcha_get_wrappee(MAT_WRAPPEE_HANDLER(XXX));
  gotcha_wrap(wrap_actions, sizeof(wrap_actions)/sizeof(struct gotcha_binding_t), "mat_gotcha_alloc");
  MAT_WRAPPEE(free)    = (void (*)(void *))gotcha_get_wrappee(MAT_WRAPPEE_HANDLER(free));
  MAT_WRAPPEE(malloc)  = (void *(*)(size_t))gotcha_get_wrappee(MAT_WRAPPEE_HANDLER(malloc));
  MAT_WRAPPEE(realloc) = (void *(*)(void *, size_t))gotcha_get_wrappee(MAT_WRAPPEE_HANDLER(realloc));
  MAT_WRAPPEE(calloc)  = (void *(*)(size_t, size_t))gotcha_get_wrappee(MAT_WRAPPEE_HANDLER(calloc));
  MAT_WRAPPEE(posix_memalign)  = (int (*)(void**, size_t, size_t))gotcha_get_wrappee(MAT_WRAPPEE_HANDLER(posix_memalign));
  MAT_WRAPPEE(aligned_alloc)  = (void* (*)(size_t, size_t))gotcha_get_wrappee(MAT_WRAPPEE_HANDLER(aligned_alloc));
  MAT_WRAPPEE(valloc)  = (void* (*)(size_t))gotcha_get_wrappee(MAT_WRAPPEE_HANDLER(valloc));
  MAT_WRAPPEE(memalign)  = (void* (*)(size_t, size_t))gotcha_get_wrappee(MAT_WRAPPEE_HANDLER(memalign));
  MAT_WRAPPEE(pvalloc)  = (void* (*)(size_t))gotcha_get_wrappee(MAT_WRAPPEE_HANDLER(pvalloc));
  MAT_WRAPPEE(mmap)    = (void *(*)(void *, size_t, int, int, int, off_t))gotcha_get_wrappee(MAT_WRAPPEE_HANDLER(mmap));
  MAT_WRAPPEE(munmap)  = (int (*)(void *, size_t))gotcha_get_wrappee(MAT_WRAPPEE_HANDLER(munmap));
  MAT_WRAPPEE(sbrk)    = (void *(*)(intptr_t))gotcha_get_wrappee(MAT_WRAPPEE_HANDLER(sbrk));
  MAT_WRAPPEE(brk)     = (int (*)(void *))gotcha_get_wrappee(MAT_WRAPPEE_HANDLER(brk));
  //wrappee_malloc = (void *(*)(size_t))gotcha_get_wrappee(wrappee_malloc_handler);
  return;
}

// /* XXX */
// static void*   MAT_WRAPPER(XXX) (size_t size)
// {
//   void *ptr;
//   ptr = MAT_WRAPPEE(XXX)(size);
//   MAT_USER_CODE_BEGIN;
//  /* Write your tracer */
//   mat_alloc_tree_insert(ptr, size);
//   MAT_USER_CODE_END;
//   return ptr;
// }
/* malloc */
static void*   MAT_WRAPPER(malloc) (size_t size)
{
  void *ptr;
  ptr = MAT_WRAPPEE(malloc)(size);
  MAT_USER_CODE_BEGIN;
  mat_alloc_tree_insert(ptr, size);
  MAT_USER_CODE_END;
  return ptr;
}
/* realloc */
static void*   MAT_WRAPPER(realloc) (void *ptr, size_t size)
{
  void *new_ptr;
  new_ptr = MAT_WRAPPEE(realloc)(ptr, size);
  MAT_USER_CODE_BEGIN;
  /* Write your tracer */
  mat_alloc_tree_insert(ptr, size);
  MAT_USER_CODE_END;  
  return new_ptr;
}
/* calloc */
static void*   MAT_WRAPPER(calloc) (size_t nmemb, size_t size)
{
  void *ptr;
  //  MAT_DBG("%s called", __func__);
  ptr = MAT_WRAPPEE(calloc)(nmemb, size);
  MAT_USER_CODE_BEGIN;
  /* Write your tracer */
  mat_alloc_tree_insert(ptr, size);
  MAT_USER_CODE_END;  
  return  ptr;
}
/* posix_memalign */
static int   MAT_WRAPPER(posix_memalign) (void **ptr, size_t alignment, size_t size)
{
  int ret;
  ret = MAT_WRAPPEE(posix_memalign)(ptr, alignment, size);
  MAT_USER_CODE_BEGIN;
  mat_alloc_tree_insert(*ptr, size);
  MAT_USER_CODE_END;
  return ret;
}
/* aligned_alloc */
static void*   MAT_WRAPPER(aligned_alloc) (size_t alignment, size_t size)
{
  void *ptr;
  ptr = MAT_WRAPPEE(aligned_alloc)(alignment, size);
  MAT_USER_CODE_BEGIN;
  mat_alloc_tree_insert(ptr, size);
  MAT_USER_CODE_END;
  return ptr;
}
/* valloc */
static void*   MAT_WRAPPER(valloc) (size_t size)
{
  void *ptr;
  ptr = MAT_WRAPPEE(valloc)(size);
  MAT_USER_CODE_BEGIN;
  mat_alloc_tree_insert(ptr, size);
  MAT_USER_CODE_END;
  return ptr;
}
/* memalign */
static void*   MAT_WRAPPER(memalign) (size_t alignment, size_t size)
{
  void *ptr;
  ptr = MAT_WRAPPEE(memalign)(alignment, size);
  MAT_USER_CODE_BEGIN;
  mat_alloc_tree_insert(ptr, size);
  MAT_USER_CODE_END;
  return ptr;
}
/* pvalloc */ 
static void*   MAT_WRAPPER(pvalloc) (size_t size)
{
  void *ptr;
  ptr = MAT_WRAPPEE(pvalloc)(size);
  MAT_USER_CODE_BEGIN;
  mat_alloc_tree_insert(ptr, size);
  MAT_USER_CODE_END;
  return ptr;
}
/* free */
static void   MAT_WRAPPER(free) (void *ptr)
{
  //  MAT_DBG("%s called", __func__);
  if (MAT_WRAPPEE(free) == NULL) {
    MAT_WRAPPEE(free)    = (void (*)(void *))gotcha_get_wrappee(MAT_WRAPPEE_HANDLER(free));
  }
  MAT_WRAPPEE(free)(ptr);
  MAT_USER_CODE_BEGIN;
  mat_alloc_tree_remove(ptr);
  MAT_USER_CODE_END;  
  return;
}
/* mmap */
static void*   MAT_WRAPPER(mmap) (void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
  void *ptr;
  //  MAT_DBG("%s called", __func__);
  ptr = MAT_WRAPPEE(mmap)(addr, length, prot, flags, fd, offset);
  MAT_USER_CODE_BEGIN;
  /* Write your tracer */
  mat_alloc_tree_insert(ptr, length);
  MAT_USER_CODE_END;  
  return ptr; 
}
/* munmap */
static int   MAT_WRAPPER(munmap) (void *addr, size_t length)
{
  int ret;
  //MAT_DBG("%s called", __func__);
  ret = MAT_WRAPPEE(munmap)(addr, length);
  MAT_USER_CODE_BEGIN;
  /* Write your tracer */
  mat_alloc_tree_remove(addr);
  MAT_USER_CODE_END;  
  return ret;
}
/* sbrk */
static void*   MAT_WRAPPER(sbrk) (intptr_t increment)
{
  void* ptr;
  //  MAT_DBG("%s called", __func__);
  ptr = MAT_WRAPPEE(sbrk)(increment);
  MAT_USER_CODE_BEGIN;
  /* Write your tracer */
  mat_alloc_tree_insert(ptr, increment);
  MAT_USER_CODE_END;  
  return ptr; 
}
/* brk */
static int   MAT_WRAPPER(brk) (void *addr)
{
  int ret;
  //MAT_DBG("%s called", __func__);
  ret = MAT_WRAPPEE(brk)(addr);
  MAT_USER_CODE_BEGIN;
  /* Write your tracer */
  mat_alloc_tree_remove(addr);
  MAT_USER_CODE_END;  
  return ret;
}




