#ifndef LOOPQUEUE_H__
#define LOOPQUEUE_H__




/************** Platform related configuration start **************/
/**
 * Define the data type according to the platform, and ensure that the maximum positive integer is greater than the total number of elements in the queue.
 */
typedef int lq_size_t;
typedef char defaultQueueDataType;
/************** Platform related configuration end **************/

typedef struct _lq lq_t;
typedef void (*addOneDataFunc_t)(lq_t* lq, void* newData);
typedef void (*getOneDataFunc_t)(lq_t* lq,void* recvData, lq_size_t recvIndex);

struct _lq{
    lq_size_t head;
    lq_size_t end;
    lq_size_t max;

    void *queue;
    void (*addOneDataFunc)(lq_t* lq, void* newData);
    void (*getOneDataFunc)(lq_t* lq,void* recvData, lq_size_t recvIndex);

    void *userData;
};

typedef struct _lqs{
    lq_size_t head;
    lq_size_t end;
    lq_size_t max;

    void *userData;
}lqs_t;

typedef enum {
    lq_OK,
    lq_FULL,
    lq_EMPTY,
} lq_state;




#define loopQueueIsEmpty(lq)    (                           \
        (lq->head == lq->end)?                              \
        lq_EMPTY:                                           \
        lq_OK)

#define loopQueueIsFull(lq)     (                           \
        ((lq->end + 1) % lq->max == lq->head)?              \
        lq_FULL:                                            \
        lq_OK )

#define loopQueueGetUseLength(lq)   (                       \
        (lq->end >= lq->head)?                              \
        (lq->end - lq->head):                               \
        ((lq->max - lq->head) + lq->end))

#define loopQueueGetIdleLength(lq)  (                       \
        lq->max - 1 - loopQueueGetUseLength(lq))

//前提是已知道列队没满
#define loopQueueNonFullFastAddOne(lq)                      \
    lq->end = (lq->end + 1)%lq->max;

//不知道列队是否已满添加 会强制添加
#define loopQueueUnknownFullFastAddOne(lq)                  \
        loopQueueNonFullFastAddOne(lq);                     \
        if(lq->end == lq->head) lq->head = (lq->head + 1)%lq->max;

//前提是已只列队中有数据
#define loopQueueNonEmptyFastGetOne(lq)                     \
    lq->head=(lq->head+1)%lq->max;

//未知列队中是否有数据 有就减少一个
#define loopQueueUnknownEmptyFastGetOne(lq)                 \
        (loopQueueGetUseLength(lq) > 0)?loopQueueNonEmptyFastGetOne(lq):;

#define loopQueueFastDeleteAll(lq)                          \
        lq->head = lq->end;

void loopQueueInit(lq_t* lq, lq_size_t max, void* queue, addOneDataFunc_t ptrNewAddOneDaAddOneDataFunctaFunc, getOneDataFunc_t ptrNewGetOneDataFunc, void* userData);

lq_size_t loopQueueAddOne(lq_t* lq, void* newData);

lq_size_t loopQueueForceAddOne(lq_t* lq, void* newData);

lq_size_t loopQueueAddMulti(lq_t* lq, void* newData, lq_size_t multi);

lq_size_t loopQueueForceAddMulti(lq_t* lq, void* newData, lq_size_t multi);

lq_size_t loopQueueGetOne(lq_t* lq,void* recvData);

lq_size_t loopQueueGetMulti(lq_t* lq,void* recvData, lq_size_t recvSize);

lq_size_t loopQueueInspectMulti(lq_t* lq,void* recvData, lq_size_t recvSize);

lq_size_t loopQueueInspectDelete(lq_t* lq, lq_size_t inspectMulti);

lq_size_t loopQueueDeleteMulti(lq_t* lq, lq_size_t multi);

void loopQueueDeleteAll(lq_t* lq);


#endif /* #ifndef LOOPQUEUE_H__ */
