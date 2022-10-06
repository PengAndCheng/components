
#include "loopQueue.h"



static void defaultAddOneDataFunc(lq_t* lq, void* newData){
    defaultQueueDataType* arrQueue = (defaultQueueDataType*)lq->queue;
    defaultQueueDataType* value_p = (defaultQueueDataType*)newData;
    arrQueue[lq->end]=*value_p;
}
static void defaultAeduceOneDataFunc(lq_t* lq,void* recvData, lq_size_t recvIndex){
    defaultQueueDataType* arrQueue = (defaultQueueDataType*)lq->queue;
    defaultQueueDataType* arrRecv = (defaultQueueDataType*)recvData;
    if (recvData) {
        arrRecv[0]=arrQueue[lq->head];
    }
}


void loopQueueInit(lq_t* lq, lq_size_t max, void* queue, addOneDataFunc_t ptrNewAddOneDaAddOneDataFunctaFunc, getOneDataFunc_t ptrNewGetOneDataFunc, void* userData){
    lq->head = 0;
    lq->end = 0;
    lq->max = max;

    lq->queue = queue;
    if (ptrNewAddOneDaAddOneDataFunctaFunc) {
        lq->addOneDataFunc = ptrNewAddOneDaAddOneDataFunctaFunc;
    }else {
        lq->addOneDataFunc = defaultAddOneDataFunc;
    }
    if (ptrNewGetOneDataFunc) {
        lq->getOneDataFunc = ptrNewGetOneDataFunc;
    }else {
        lq->getOneDataFunc = defaultAeduceOneDataFunc;
    }
    if (userData) {
        lq->userData = userData;
    }
}

lq_size_t loopQueueAddOne(lq_t* lq, void* newData){
    if (lq_FULL != loopQueueIsFull(lq)) {
        if (lq->addOneDataFunc) lq->addOneDataFunc(lq,newData);
        loopQueueNonFullFastAddOne(lq);
        return 1;
    }else {
        return 0;
    }
}

lq_size_t loopQueueForceAddOne(lq_t* lq, void* newData){
    if (lq->addOneDataFunc) lq->addOneDataFunc(lq,newData);
    loopQueueUnknownFullFastAddOne(lq);
    return 1;
}

lq_size_t loopQueueAddMulti(lq_t* lq, void* newData, lq_size_t multi){
    lq_size_t idle = loopQueueGetIdleLength(lq);
    if (multi < idle) {
        idle = multi;
    }
    lq_size_t i;
    for (i = 0; i < idle; ++i) {
        if (lq->addOneDataFunc) lq->addOneDataFunc(lq,newData);
        loopQueueNonFullFastAddOne(lq);
    }
    return idle;
}

lq_size_t loopQueueForceAddMulti(lq_t* lq, void* newData, lq_size_t multi){
    lq_size_t i;
    for (i = 0; i < multi; ++i) {
        if (lq->addOneDataFunc) lq->addOneDataFunc(lq,newData);
        loopQueueUnknownFullFastAddOne(lq);
    }
    return multi;
}

lq_size_t loopQueueGetOne(lq_t* lq,void* recvData){
    if (loopQueueIsEmpty(lq) == lq_EMPTY) {
        return 0;
    }else {
        loopQueueNonEmptyFastGetOne(lq);
        return 1;
    }
}

lq_size_t loopQueueGetMulti(lq_t* lq,void* recvData, lq_size_t recvSize){
    lq_size_t use = loopQueueGetUseLength(lq);
    if (recvSize < use) {
        use = recvSize;
    }
    lq_size_t i;
    for (i = 0; i < use; ++i) {
        if (lq->getOneDataFunc) lq->getOneDataFunc(lq,recvData,i);
        loopQueueNonEmptyFastGetOne(lq);
    }
    return use;
}

lq_size_t loopQueueInspectMulti(lq_t* lq,void* recvData, lq_size_t recvSize){
    lq_size_t use = loopQueueGetUseLength(lq);
    if (recvSize < use) {
        use = recvSize;
    }
    lq_size_t i;
    for (i = 0; i < use; ++i) {
        if (lq->getOneDataFunc) lq->getOneDataFunc(lq,recvData,i);
    }
    return use;
}

lq_size_t loopQueueInspectDelete(lq_t* lq, lq_size_t inspectMulti){
    lq_size_t i;
    for (i = 0; i < inspectMulti; ++i) {
        loopQueueNonEmptyFastGetOne(lq);
    }
    return inspectMulti;
}

lq_size_t loopQueueDeleteMulti(lq_t* lq, lq_size_t multi){
    lq_size_t use = loopQueueGetUseLength(lq);
    if (multi < use) {
        use = multi;
    }
    lq_size_t i;
    for (i = 0; i < use; ++i) {
        loopQueueNonEmptyFastGetOne(lq);
    }
    return use;
}

void loopQueueDeleteAll(lq_t* lq){
    loopQueueFastDeleteAll(lq);
}


