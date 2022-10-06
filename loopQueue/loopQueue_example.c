
#include <rtthread.h>
#include <stdio.h>

#include "loopQueue.h"

#ifndef NULL
#define NULL 0
#endif

void printf_lq(lq_t* lq){
    printf("lq:\t head:%d\t end:%d\t max:%d\t use:%d\t idle:%d\t Empty:%d\t Full:%d\t \r\n",lq->head,lq->end,lq->max,loopQueueGetUseLength(lq),loopQueueGetIdleLength(lq),loopQueueIsEmpty(lq),loopQueueIsFull(lq));
}

lq_t lq01;
defaultQueueDataType lqueue01[6];
void printf_lqueue01(void){
    printf("lqueue01:");
    for (int i = 0; i < sizeof(lqueue01)/sizeof(lqueue01[0]); ++i) {
        printf("\t%d",lqueue01[i]);
    }
    printf("\r\n");
}
void lq01_test(void){
    lq_t* lq = &lq01;

    loopQueueInit(lq, sizeof(lqueue01)/sizeof(lqueue01[0]), (void*)lqueue01, NULL, NULL, NULL);
    printf_lq(lq);
    //lq:      head:0  end:0   max:6   use:0   idle:5  Empty:2         Full:0
    printf_lqueue01();
    //lqueue01:       0       0       0       0       0       0
    for (int i = 0; i < 10; ++i) {
        loopQueueAddOne(lq,(void*)&i);
    }
    printf_lq(lq);
    //lq:      head:0  end:5   max:6   use:5   idle:0  Empty:0         Full:1
    printf_lqueue01();
    //lqueue01:       0       1       2       3       4       0
    loopQueueDeleteAll(lq);
    printf_lq(lq);
    //lq:      head:5  end:5   max:6   use:0   idle:5  Empty:2         Full:0
    printf_lqueue01();
    //lqueue01:       0       1       2       3       4       0
}
MSH_CMD_EXPORT(lq01_test,lq01_test)



















