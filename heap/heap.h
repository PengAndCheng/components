#ifndef __HEAP_H__
#define __HEAP_H__

#ifdef __cplusplus
extern "C" {
#endif


//声明堆枚举
typedef enum {
#define HEAP_CFG(namestr,   name,   size) HEAP_##name,
#include "heap_std.h"
#undef HEAP_CFG //C语言对#ifndef的预处理，只检查关键字，后面的"(namestr,   name,   size)"，认为是多余的字符
  HEAP_MAX
} HEAP_t;

void heap_cfg(void);

#define HEAP_CFG(namestr,   name,   size) void * name##_reinit(void);
#include "heap_std.h"
#undef HEAP_CFG

#define HEAP_CFG(namestr,   name,   size) void * name##_malloc(unsigned int xWantedSize);
#include "heap_std.h"
#undef HEAP_CFG

#define HEAP_CFG(namestr,   name,   size) void name##_free(void * pv);
#include "heap_std.h"
#undef HEAP_CFG

#define HEAP_CFG(namestr,   name,   size) void name##_heap_info(void);
#include "heap_std.h"
#undef HEAP_CFG

#ifdef __cplusplus
}
#endif

#endif /* __HEAP_H__ */
