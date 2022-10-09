
//在RT-THREAD系统下利用FinSH组件进行测试
#include <rtthread.h>
#include <board.h>
#include <rtdevice.h>

#include <stdio.h>

#include "heap.h"


char *r[120]={0};
int rnb=0;

void main_heap_test(void){
    static int heap_cfg_is_init = 0;
    if (heap_cfg_is_init == 0) {
        heap_cfg();heap_cfg_is_init=1;
    }

    name0_heap_info();
    printf("\n\n\n");


    while(1){
        for (int i = 0; i < 120; ++i) {
            r[i]=name0_malloc(1);
            if (r[i]) {
                rnb++;
            }
        }
        printf("rnb=%d;\n",rnb);
        name0_heap_info();

        for (int i = 0; i < 120; ++i) {
            if (r[i]) {
                name0_free(r[i]);
                r[i]=0;
                rnb--;
            }
        }
        printf("rnb=%d;\n",rnb);
        name0_heap_info();

        break;
    }

}
MSH_CMD_EXPORT(main_heap_test,main_heap_test)
