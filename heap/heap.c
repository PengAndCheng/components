
#include <stdlib.h>

#include <stddef.h>//size_t默认使用操作系统就包含这个 不是就自定义

#include <stdint.h>//依赖了这个uint8_t

#include <stdio.h>

#include <assert.h>

#include "heap_private.h"

//32位系统 32位最大无符号值 vPortGetHeapStats函数依赖这个宏 有点奇怪 测试发现为了未初始化时xSizeOfSmallestFreeBlockInBytes=-1
#define portMAX_DELAY 0xffffffffUL
//字节对齐
#define portBYTE_ALIGNMENT 4
//字节对齐掩码 和字节对齐的关系是发现的 8->7  4->3    2->1    1->0
#define portBYTE_ALIGNMENT_MASK ( portBYTE_ALIGNMENT - 1 )

//一个新空闲块最小尺寸 这里是xHeapStructSize的两倍
#define heapMINIMUM_BLOCK_SIZE    ( ( size_t ) ( xHeapStructSize << 1 ) )

//xBlockSize的高位掩码使用了这个值进行初始化，其大小决定了最小申请尺寸的大小 如果是8X4最大申请尺寸还是很大的，自己解读源码理解的
#define heapBITS_PER_BYTE         ( ( size_t ) 8 )

#ifdef MUL_NON
//堆内存空间数组
#define configTOTAL_HEAP_SIZE 256
//为堆分配8位内存数组
static uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
#endif /* #define MUL_NON */

//声明在链表中插入新的节点内存块 有整理碎片功能   PRIVILEGED_FUNCTION这是干啥的？freerots中有PRIVILEGED_FUNCTION修饰函数好像是指定函数放在哪个段？下面函数均删除掉
static void prvInsertBlockIntoFreeList( Heap_t* heap, BlockLink_t * pxBlockToInsert ) /*PRIVILEGED_FUNCTION*/;

//初始化堆内存 主要理解 xStart pxFirstFreeBlock pxEnd
static void HeapInit( Heap_t* heap );

//位于每个分配内存开头的结构的大小 块必须正确对齐字节。
static const size_t xHeapStructSize = ( sizeof( BlockLink_t ) + ( ( size_t ) ( portBYTE_ALIGNMENT - 1 ) ) ) & ~( ( size_t ) portBYTE_ALIGNMENT_MASK );

#ifdef MUL_NON //在多堆应用中没有以下变量
//链表的全局节点 还有个比较在初始化函数中较为需要理解的节点pxFirstFreeBlock
static BlockLink_t xStart, * pxEnd = NULL;

//堆中状态全局函数
static size_t xFreeBytesRemaining = 0U;             //表示内存堆剩余大小
static size_t xMinimumEverFreeBytesRemaining = 0U;  //记录最小的那个空闲内存块大小
static size_t xNumberOfSuccessfulAllocations = 0;   //成功的申请一次内存统计
static size_t xNumberOfSuccessfulFrees = 0;         //表示成功释放一次统计
#endif /* #ifdef MUL_NON */

//xBlockSize的高位掩码，在初始化函数中使用heapBITS_PER_BYTE进行初始化
static size_t xBlockAllocatedBit = 0;   //第heapBITS_PER_BYTE * 4（从0开始）位（高位）设为了1 和xBlockSize heapBITS_PER_BYTE关系很大












//外调宏或hanshu
//禁止系统调度
#define vTaskSuspendAll()
//允许系统调度
#define xTaskResumeAll()
//关中断函数
#define taskENTER_CRITICAL()
//开中断函数
#define taskEXIT_CRITICAL()

//申请释放时出现异常调用的函数
#define mtCOVERAGE_TEST_MARKER()
//断言函数
#define configASSERT assert
//申请内存成功扩展调用函数
#define traceMALLOC( pvReturn, xWantedSize )
//成功释放调用函数
#define traceFREE( pv, Size )

/*-----------------------------------------------------------*/

static
void * PortMalloc( Heap_t* heap, size_t xWantedSize )
{
    BlockLink_t * pxBlock, * pxPreviousBlock, * pxNewBlockLink;
    void * pvReturn = NULL;

    //修改该算法来自行使用 尽量保持源码结构和原版一致
    vTaskSuspendAll();//挂起所有活动的实时内核，同时允许中断（包括内核滴答） 理解为停止调度 和xTaskResumeAll ()是一对
    {
        /* If this is the first call to malloc then the heap will require
         * initialisation to setup the list of free blocks. 如果这是第一次调用malloc，那么堆将需要
                 * 初始化以设置空闲块列表*/
        if( heap->pxEnd == NULL )
        {
            HeapInit(heap);//pxEnd数据区在尾端，所以可以使用null来判断初始化
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();//覆盖测试标记 这样可以预留后期实现变更，编译的时候也相当于没调用 理解为函数的切向编程
        }

        /* Check the requested block size is not so large that the top bit is
         * set.  The top bit of the block size member of the BlockLink_t structure
         * is used to determine who owns the block - the application or the
         * kernel, so it must be free. */
        if( ( xWantedSize & xBlockAllocatedBit ) == 0 )//xWantedSize的高位不能为1 如果8位时最大申请是127
        {
            /* The wanted size must be increased so it can contain a BlockLink_t
             * structure in addition to the requested amount of bytes. */
            if( ( xWantedSize > 0 ) && 
                ( ( xWantedSize + xHeapStructSize ) >  xWantedSize ) ) /* Overflow check */
            {
                xWantedSize += xHeapStructSize;                             //在想要的内存块中增加一块xHeapStructSize信息

                /* Ensure that blocks are always aligned. 确保块始终对齐*/
                if( ( xWantedSize & portBYTE_ALIGNMENT_MASK ) != 0x00 )
                {
                    /* Byte alignment required. Check for overflow. */
                    if( ( xWantedSize + ( portBYTE_ALIGNMENT - ( xWantedSize & portBYTE_ALIGNMENT_MASK ) ) ) 
                            > xWantedSize )
                    {
                        xWantedSize += ( portBYTE_ALIGNMENT - ( xWantedSize & portBYTE_ALIGNMENT_MASK ) );
                        configASSERT( ( xWantedSize & portBYTE_ALIGNMENT_MASK ) == 0 );
                    }
                    else
                    {
                        xWantedSize = 0;
                    }  
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            } 
            else 
            {
                xWantedSize = 0;
            }

            if( ( xWantedSize > 0 ) && ( xWantedSize <= heap->xFreeBytesRemaining ) )//xWantedSize是一个对齐的数字 且xWantedSize在最小可分配空间内 判断是小于xFreeBytesRemaining 所以出现了开始初始化时xStart.xBlockSize = ( size_t ) 0;
            {
                /* Traverse the list from the start	(lowest address) block until
                 * one of adequate size is found. 从开始（最低地址）块遍历列表，直到找到一个合适的尺寸*/
                pxPreviousBlock = &heap->xStart;
                pxBlock = heap->xStart.pxNextFreeBlock;

                while( ( pxBlock->xBlockSize < xWantedSize ) && ( pxBlock->pxNextFreeBlock != NULL ) )//找到块尺寸合适且不是pxEnd 初始化中已经对pxFirstFreeBlock进行初始化了 所以必有pxBlock->pxNextFreeBlock != NULL
                {
                    pxPreviousBlock = pxBlock;
                    pxBlock = pxBlock->pxNextFreeBlock;
                }

                /* If the end marker was reached then a block of adequate size
                 * was not found. */
                if( pxBlock != heap->pxEnd )
                {
                    /* Return the memory space pointed to - jumping over the
                     * BlockLink_t structure at its start. */
                    pvReturn = ( void * ) ( ( ( uint8_t * ) pxPreviousBlock->pxNextFreeBlock ) + xHeapStructSize );//得到返回申请的内存地址 以pxPreviousBlock = &xStart;为例好理解

                    /* This block is being returned for use so must be taken out
                     * of the list of free blocks. */
                    pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;//抠掉数据后当前节点指向下一个节点 后面会执行新块插入函数

                    /* If the block is larger than required it can be split into
                     * two. */
                    if( ( pxBlock->xBlockSize - xWantedSize ) > heapMINIMUM_BLOCK_SIZE )//如果抠掉数据后还有富余 有点难理解新节点信息块放在哪里
                    {
                        //有富余节点就新产生一个节点 将其插入链表中
                        /* This block is to be split into two.  Create a new
                         * block following the number of bytes requested. The void
                         * cast is used to prevent byte alignment warnings from the
                         * compiler. */
                        pxNewBlockLink = ( void * ) ( ( ( uint8_t * ) pxBlock ) + xWantedSize );//xWantedSize包含了一个xHeapStructSize
                        configASSERT( ( ( ( size_t ) pxNewBlockLink ) & portBYTE_ALIGNMENT_MASK ) == 0 );

                        /* Calculate the sizes of two blocks split from the
                         * single block. */
                        pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;//少了被扣走的尺寸 这个尺寸包含了一个xHeapStructSize
                        pxBlock->xBlockSize = xWantedSize;//被扣走的节点尺寸变短

                        /* Insert the new block into the list of free blocks. 将新块插入空闲块列表中*/
                        prvInsertBlockIntoFreeList( heap, pxNewBlockLink );//产生碎片的时候就进行了碎片整理
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }

                    heap->xFreeBytesRemaining -= pxBlock->xBlockSize;//表示内存堆剩余大小减少

                    if( heap->xFreeBytesRemaining < heap->xMinimumEverFreeBytesRemaining )
                    {
                        heap->xMinimumEverFreeBytesRemaining = heap->xFreeBytesRemaining;//记录最小的那个空闲内存块大小 会变小
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }

                    /* The block is being returned - it is allocated and owned
                     * by the application and has no "next" block. */
                    pxBlock->xBlockSize |= xBlockAllocatedBit;//标记高位为1 表示该块数据已经被使用
                    pxBlock->pxNextFreeBlock = NULL;
                    heap->xNumberOfSuccessfulAllocations++;//成功的申请一次内存统计
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }

        traceMALLOC( pvReturn, xWantedSize );
    }
    /*( void )*/ xTaskResumeAll();//和vTaskSuspendAll();成对使用 ROTS可以进行调度

    #if ( configUSE_MALLOC_FAILED_HOOK == 1 )//启用内存申请失败执行钩子
        {
            if( pvReturn == NULL )
            {
                extern void vApplicationMallocFailedHook( void );
                vApplicationMallocFailedHook();
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
    #endif /* if ( configUSE_MALLOC_FAILED_HOOK == 1 ) */

    configASSERT( ( ( ( size_t ) pvReturn ) & ( size_t ) portBYTE_ALIGNMENT_MASK ) == 0 );//申请到的地址应该是字节对齐的 断言判断
    return pvReturn;
}
/*-----------------------------------------------------------*/

static
void PortFree( Heap_t* heap, void * pv )
{
    uint8_t * puc = ( uint8_t * ) pv;
    BlockLink_t * pxLink;

    if( pv != NULL )
    {
        /* The memory being freed will have an BlockLink_t structure immediately
         * before it. */
        puc -= xHeapStructSize;//指针回退到节点信息处

        /* This casting is to keep the compiler from issuing warnings. 这种强制转换是为了防止编译器发出警告。*/
        pxLink = ( void * ) puc;

        /* Check the block is actually allocated. 检查块是否已实际分配*/
        configASSERT( ( pxLink->xBlockSize & xBlockAllocatedBit ) != 0 );//这个掩码坑还是很大的 直接影响申请的内存不能超过多少
        configASSERT( pxLink->pxNextFreeBlock == NULL );

        if( ( pxLink->xBlockSize & xBlockAllocatedBit ) != 0 )//和这个功能一样的configASSERT( ( pxLink->xBlockSize & xBlockAllocatedBit ) != 0 );
        {
            if( pxLink->pxNextFreeBlock == NULL )//申请时已经改为NULL了，不为NULL前面的溢出
            {
                /* The block is being returned to the heap - it is no longer
                 * allocated. */
                pxLink->xBlockSize &= ~xBlockAllocatedBit;//反掩码表示未使用内存

                vTaskSuspendAll();//禁止调度与xTaskResumeAll()成对使用
                {
                    /* Add this block to the list of free blocks. */
                    heap->xFreeBytesRemaining += pxLink->xBlockSize;                      //剩余内存增加
                    traceFREE( pv, pxLink->xBlockSize );
                    prvInsertBlockIntoFreeList( heap, ( ( BlockLink_t * ) pxLink ) );     //插入节点
                    heap->xNumberOfSuccessfulFrees++;                                     //表示成功释放一次统计
                }
                /*( void )*/ xTaskResumeAll();//允许调度与vTaskSuspendAll()成对使用
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }
    }
}

#ifdef USE_XXX
/*-----------------------------------------------------------*/
static
size_t xPortGetFreeHeapSize( Heap_t* heap )
{
    return heap->xFreeBytesRemaining;//返回内存堆剩余大小
}
/*-----------------------------------------------------------*/
static
size_t xPortGetMinimumEverFreeHeapSize( Heap_t* heap )
{
    return heap->xMinimumEverFreeBytesRemaining;//记录最小的那个空闲内存块大小 没看到碎片整理后增加？ 这个也称作 未分配的内存堆历史最小值 只会减少 至于想查看链表中最大空闲块和最小空闲块在void vPortGetHeapStats( HeapStats_t * pxHeapStats )中有实现
}
/*-----------------------------------------------------------*/
static
void vPortInitialiseBlocks( void )
{
    /* This just exists to keep the linker quiet. */
}
/*-----------------------------------------------------------*/
#endif /* #ifdef USE_XXX */

static void HeapInit( Heap_t* heap ) /* PRIVILEGED_FUNCTION */
{
    BlockLink_t * pxFirstFreeBlock;
    uint8_t * pucAlignedHeap;                       //对齐后的堆内存地址变量
    size_t uxAddress;                               //中间变量 被多次使用
    size_t xTotalHeapSize = heap->configTOTAL_HEAP_SIZE;  //内存池长度 进行字节对齐后会改变

    /* Ensure the heap starts on a correctly aligned boundary. 确保堆在正确对齐的边界上开始 一般定义的数组都在字节对齐的边界上*/
    uxAddress = ( size_t ) heap->ucHeap;

    if( ( uxAddress & portBYTE_ALIGNMENT_MASK ) != 0 )
    {
        uxAddress += ( portBYTE_ALIGNMENT - 1 );                //加上一个对齐减一必然落在下一个空闲对齐区域
        uxAddress &= ~( ( size_t ) portBYTE_ALIGNMENT_MASK );   //结果等于除以对齐数的整数 =》 (int)uxAddress/portBYTE_ALIGNMENT
        xTotalHeapSize -= uxAddress - ( size_t ) heap->ucHeap;        //内存池实际可用长度减减非对齐字节
    }

    pucAlignedHeap = ( uint8_t * ) uxAddress;                   //对齐后的堆内存地址变量

    /* xStart is used to hold a pointer to the first item in the list of free
     * blocks.  The void cast is used to prevent compiler warnings.  xStart用于保存指向空闲列表中第一项的指针
     * 阻碍。void强制转换用于防止编译器警告。 */
    heap->xStart.pxNextFreeBlock = ( void * ) pucAlignedHeap; //xStart是全局变量 应该是链表头句柄
    heap->xStart.xBlockSize = ( size_t ) 0;                   //为啥是0呢？链表头本来就没有空间 由xMinimumEverFreeBytesRemaining和xFreeBytesRemaining指示未使用内存

    /* pxEnd is used to mark the end of the list of free blocks and is inserted
     * at the end of the heap space.    pxEnd用于标记空闲块列表的末尾并插入
     * 在堆空间的末尾。 */
    uxAddress = ( ( size_t ) pucAlignedHeap ) + xTotalHeapSize;//指向等于对齐后的地址的末尾
    uxAddress -= xHeapStructSize;                              //然后回退一个BlockLink_t给pxEnd已对齐的数据区
    uxAddress &= ~( ( size_t ) portBYTE_ALIGNMENT_MASK );      //重新对齐pxEnd的地址
    heap->pxEnd = ( void * ) uxAddress;       //结束区域地址
    heap->pxEnd->xBlockSize = 0;              //尾部标记没有内存
    heap->pxEnd->pxNextFreeBlock = NULL;      //尾部标记没有内存块

    /* To start with there is a single free block that is sized to take up the
     * entire heap space, minus the space taken by pxEnd. */
    pxFirstFreeBlock = ( void * ) pucAlignedHeap;                           //局部变量 等同xStart.pxNextFreeBlock
    pxFirstFreeBlock->xBlockSize = uxAddress - ( size_t ) pxFirstFreeBlock; //但是这里却取了长度 pxEnd的地址-pucAlignedHeap的地址得长度 说明新块长度包含节点信息的长度
    pxFirstFreeBlock->pxNextFreeBlock = heap->pxEnd;                              //指向下一块

    /* Only one block exists - and it covers the entire usable heap space. */
    heap->xMinimumEverFreeBytesRemaining = pxFirstFreeBlock->xBlockSize;          //记录最小的那个空闲内存块大小
    heap->xFreeBytesRemaining = pxFirstFreeBlock->xBlockSize;                     //表示内存堆剩余大小

    /* Work out the position of the top bit in a size_t variable. */
    xBlockAllocatedBit = ( ( size_t ) 1 ) << ( ( sizeof( size_t ) * heapBITS_PER_BYTE ) - 1 );//第4X8=32位为1 意义何为？
}
/*-----------------------------------------------------------*/

static void prvInsertBlockIntoFreeList( Heap_t* heap, BlockLink_t * pxBlockToInsert ) /* PRIVILEGED_FUNCTION */
{
    BlockLink_t * pxIterator;
    uint8_t * puc;

    /* Iterate through the list until a block is found that has a higher address
     * than the block being inserted.
            遍历列表，直到找到具有较高地址的块
            而不是被插入的块。 意图找到的插入位置是空闲地址是增序排列的*/
    for( pxIterator = &heap->xStart; pxIterator->pxNextFreeBlock < pxBlockToInsert; pxIterator = pxIterator->pxNextFreeBlock )
    {
        /* Nothing to do here, just iterate to the right position. */
    }

    /* Do the block being inserted, and the block it is being inserted after
     * make a contiguous block of memory? 执行正在插入的块，以及在其之后插入的块
              创建一个连续的内存块？*/
    puc = ( uint8_t * ) pxIterator;

    if( ( puc + pxIterator->xBlockSize ) == ( uint8_t * ) pxBlockToInsert )
    {
        pxIterator->xBlockSize += pxBlockToInsert->xBlockSize;//如果和上一块内存连续则合并内存
        pxBlockToInsert = pxIterator;
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }

    /* Do the block being inserted, and the block it is being inserted before
     * make a contiguous block of memory? 执行正在插入的块和之前插入的块
               创建一个连续的内存块？*/
    puc = ( uint8_t * ) pxBlockToInsert;

    if( ( puc + pxBlockToInsert->xBlockSize ) == ( uint8_t * ) pxIterator->pxNextFreeBlock )
    {
        if( pxIterator->pxNextFreeBlock != heap->pxEnd )
        {
            /* Form one big block from the two blocks. */
            pxBlockToInsert->xBlockSize += pxIterator->pxNextFreeBlock->xBlockSize;//如果和下一块内存连续则合并内存
            pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock->pxNextFreeBlock;
        }
        else
        {
            pxBlockToInsert->pxNextFreeBlock = heap->pxEnd;
        }
    }
    else
    {
        pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;
    }

    /* If the block being inserted plugged a gab, so was merged with the block
     * before and the block after, then it's pxNextFreeBlock pointer will have
     * already been set, and should not be set here as that would make it point
     * to itself. */
    if( pxIterator != pxBlockToInsert )
    {
        pxIterator->pxNextFreeBlock = pxBlockToInsert;
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }
}
/*-----------------------------------------------------------*/





//获取堆内存状态
static
void vPortGetHeapStats(Heap_t* heap, HeapStats_t * pxHeapStats )
{
    BlockLink_t * pxBlock;
    size_t xBlocks = 0, xMaxSize = 0, xMinSize = portMAX_DELAY; /* portMAX_DELAY used as a portable way of getting the maximum value. */

    vTaskSuspendAll();//禁止调度
    {
        pxBlock = heap->xStart.pxNextFreeBlock;

        /* pxBlock will be NULL if the heap has not been initialised.  The heap
         * is initialised automatically when the first allocation is made. */
        if( pxBlock != NULL )
        {

//            do
//            {
//                /* Increment the number of blocks and record the largest block seen
//                 * so far. */
//                xBlocks++;
//
//                if( pxBlock->xBlockSize > xMaxSize )
//                {
//                    xMaxSize = pxBlock->xBlockSize;
//                }
//
//                if( pxBlock->xBlockSize < xMinSize )
//                {
//                    xMinSize = pxBlock->xBlockSize;
//                }
//
//                /* Move to the next block in the chain until the last block is
//                 * reached. */
//                pxBlock = pxBlock->pxNextFreeBlock;//当链表没有空闲数据块时存在 pxBlock = xStart.pxNextFreeBlock = pxEnd，若果时do-while时pxBlock = pxBlock->pxNextFreeBlock = NULL 会进入死循环 改成以下方式：
//            } while( pxBlock != pxEnd );

            while(pxBlock != heap->pxEnd){
                /* Increment the number of blocks and record the largest block seen
                 * so far. */
                xBlocks++;

                if( pxBlock->xBlockSize > xMaxSize )
                {
                    xMaxSize = pxBlock->xBlockSize;
                }

                if( pxBlock->xBlockSize < xMinSize )
                {
                    xMinSize = pxBlock->xBlockSize;
                }

                /* Move to the next block in the chain until the last block is
                 * reached. */
                pxBlock = pxBlock->pxNextFreeBlock;
            }

        }
    }
    /*( void )*/ xTaskResumeAll();//允许调度

    pxHeapStats->xSizeOfLargestFreeBlockInBytes = xMaxSize;
    pxHeapStats->xSizeOfSmallestFreeBlockInBytes = xMinSize;
    pxHeapStats->xNumberOfFreeBlocks = xBlocks;

    taskENTER_CRITICAL();//开中断
    {
        pxHeapStats->xAvailableHeapSpaceInBytes = heap->xFreeBytesRemaining;
        pxHeapStats->xNumberOfSuccessfulAllocations = heap->xNumberOfSuccessfulAllocations;
        pxHeapStats->xNumberOfSuccessfulFrees = heap->xNumberOfSuccessfulFrees;
        pxHeapStats->xMinimumEverFreeBytesRemaining = heap->xMinimumEverFreeBytesRemaining;
    }
    taskEXIT_CRITICAL();//关中断
}






//定义堆内存数组
#define HEAP_CFG(namestr,   name,   size)   uint8_t heap_memory_##name##_base[size]={0};
#include "heap_std.h"
#undef HEAP_CFG

//定义结构体
struct heap heap_arr[HEAP_MAX];

//需要初始化时调用
void heap_cfg(void){
//初始化结构体的configTOTAL_HEAP_SIZE 和数组指针ucHeap 和MEAP枚举
#define HEAP_CFG(namestr,   name,   size)               \
        heap_arr[HEAP_##name].configTOTAL_HEAP_SIZE = size;     \
        heap_arr[HEAP_##name].ucHeap = heap_memory_##name##_base;   \
        heap_arr[HEAP_##name].HEAP = HEAP_##name;   \
        heap_arr[HEAP_##name].namep = namestr;
#include "heap_std.h"
#undef HEAP_CFG


    /* 知道添加debug
    printf("MEAP_name0:%d;\n",HEAP_name0);    //输出枚举
    printf("MEAP_name1:%d;\n",HEAP_name1);    //输出枚举
    printf("heap_memory_name_base:%d; size:%d;\n",heap_memory_name0_base,sizeof(heap_memory_name0_base));
    printf("heap_memory_name_base:%d; size:%d;\n",heap_memory_name1_base,sizeof(heap_memory_name1_base));
    */

    //简单输出堆信息
    printf("\n");
    for (int i = 0; i < HEAP_MAX; ++i) {
        printf("HEAP:%d \t namep=%s \t ucHeap:%d \t size:%d;\n",(int)heap_arr[i].HEAP, heap_arr[i].namep, (int)heap_arr[i].ucHeap, (int)heap_arr[i].configTOTAL_HEAP_SIZE );
    }
    printf("\n");
}


#define HEAP_CFG(namestr,   name,   size) void * name##_reinit(void){  \
        Heap_t* heap = &heap_arr[HEAP_##name];  \
        heap->xStart.pxNextFreeBlock=0;         \
        heap->xStart.xBlockSize=0;              \
        heap->pxEnd = 0;                        \
        heap->xFreeBytesRemaining = 0;                                   \
        heap->xMinimumEverFreeBytesRemaining = 0;                        \
        heap->xNumberOfSuccessfulAllocations = 0;                        \
        heap->xNumberOfSuccessfulFrees = 0;                              \
        HeapInit(heap);                      \
        };
#include "heap_std.h"
#undef HEAP_CFG

#define HEAP_CFG(namestr,   name,   size) void * name##_malloc(unsigned int xWantedSize){  \
        Heap_t* heap = &heap_arr[HEAP_##name];  \
        return PortMalloc(heap,xWantedSize);  \
        };
#include "heap_std.h"
#undef HEAP_CFG

#define HEAP_CFG(namestr,   name,   size) void name##_free(void * pv){  \
        Heap_t* heap = &heap_arr[HEAP_##name];  \
        PortFree(heap,pv);  \
        };
#include "heap_std.h"
#undef HEAP_CFG







void heap_info( Heap_t* heap ){
    printf("\n");

    printf("HEAP:%d; namep=%s; ucHeap:%d; size:%d;\n",heap->HEAP,heap->namep,(int)heap->ucHeap,heap->configTOTAL_HEAP_SIZE );

    printf("xHeapStructSize=%d;\n",xHeapStructSize);
    printf("ucHeap=%d; configTOTAL_HEAP_SIZE=%d; +=%d;\n",(int)heap->ucHeap,heap->configTOTAL_HEAP_SIZE,(int)heap->ucHeap+heap->configTOTAL_HEAP_SIZE);

    int* ip = (int*)&heap->xStart;
    printf("&xStart=%d, xStart[0]=%d; xStart[1]=%d;\n",(int)&heap->xStart,ip[0],ip[1]);
    printf("xStart.xBlockSize=%d;\n",heap->xStart.xBlockSize);
    printf("xStart.pxNextFreeBlock=%d;\n",(int)heap->xStart.pxNextFreeBlock);

    printf("pxFirstFreeBlock=%d;\n",(int)heap->xStart.pxNextFreeBlock);
    printf("pxFirstFreeBlock->xBlockSize=%d;\n",heap->xStart.pxNextFreeBlock->xBlockSize);
    printf("pxFirstFreeBlock->pxNextFreeBlock=%d;\n",(int)heap->xStart.pxNextFreeBlock->pxNextFreeBlock);

    printf("pxEnd=%d;\n",(int)heap->pxEnd);
    printf("pxEnd->xBlockSize=%d;\n",heap->pxEnd->xBlockSize);
    printf("pxEnd->pxNextFreeBlock=%d;\n",(int)heap->pxEnd->pxNextFreeBlock);

    HeapStats_t state;
    vPortGetHeapStats(heap, &state);
    printf("xAvailableHeapSpaceInBytes=%d;\n",state.xAvailableHeapSpaceInBytes);
    printf("xSizeOfLargestFreeBlockInBytes=%d;\n",state.xSizeOfLargestFreeBlockInBytes);
    printf("xSizeOfSmallestFreeBlockInBytes=%d;\n",state.xSizeOfSmallestFreeBlockInBytes);
    printf("xNumberOfFreeBlocks=%d;\n",state.xNumberOfFreeBlocks);
    printf("xMinimumEverFreeBytesRemaining=%d;\n",state.xMinimumEverFreeBytesRemaining);
    printf("xNumberOfSuccessfulAllocations=%d;\n",state.xNumberOfSuccessfulAllocations);
    printf("xNumberOfSuccessfulFrees=%d;\n",state.xNumberOfSuccessfulFrees);
}

#define HEAP_CFG(namestr,   name,   size) void name##_heap_info(void){heap_info(&heap_arr[HEAP_##name]);}
#include "heap_std.h"
#undef HEAP_CFG



