
#ifndef _LWIPOPTS_H
#define _LWIPOPTS_H


//本文件和arch.h 和cc.h 都有可能重复定义

/***************************************************************************************************************/
//                                      NO_SYS 操作系统相关
/***************************************************************************************************************/
//NO_SYS == 1: 无操作系统，无法使用线程安全相关API，只能使用callback-style raw API 须要注意不能一次从多个上下文访问lwIP函数/结构 
//NO_SYS == 0: 有操作系统
#define NO_SYS        0

/***************************************************************************************************************/
//                                      Heap and memory pools
/***************************************************************************************************************/
//MEM_LIBC_MALLOC==1: 使用C库提供的malloc / free / realloc而不是lwip内部分配器。一般情况，C库的分配策略碎片化比较严重，不适用于嵌入式    
#define 	MEM_LIBC_MALLOC   0
//MEMP_MEM_MALLOC == 1：使用mem_malloc/mem_free而不是lwip内存池分配器。（堆分配可能比池分配慢得多）与MEM_USE_POOLS只能选择其一
#define 	MEMP_MEM_MALLOC   0
//字节对齐 老重要了
#define 	MEM_ALIGNMENT     4
//MEM_SIZE：堆内存的大小。 如果应用程序将发送大量需要复制的数据，则应将其设置为高。
#define 	MEM_SIZE   1600

//内存池溢出检查 0： 不检查 1： 释放时检查 2：调用memp_malloc() or memp_free() 检查，有用，但很慢
#define 	MEMP_OVERFLOW_CHECK   0
//内存池健全性检查 MEMP_SANITY_CHECK == 1：在每个memp_free（）之后运行一次健全性检查，以确保链表中没有循环。
#define 	MEMP_SANITY_CHECK   0

//堆内存溢出检查 0: 不检查 1： 释放时检查2：调用mem_malloc() or mem_free() 检查，有用，但很慢
#define 	MEM_OVERFLOW_CHECK   0
//堆内存健全性检查
#define 	MEM_SANITY_CHECK    0
//内存堆分配策略（是否用内存池分配策略实现内存堆分配） MEM_USE_POOLS==1: 使用内存池方式，还必须启用MEMP_USE_CUSTOM_POOLS。与MEMP_MEM_MALLOC只能选择其一
#define 	MEM_USE_POOLS   0
//MEM_USE_POOLS_TRY_BIGGER_POOL== 1：如果一个malloc_pool为空，请尝试下一个更大的池 - 警告：这个可能的废物存储器，但它可以使系统更可靠。
#define 	MEM_USE_POOLS_TRY_BIGGER_POOL   0
//MEMP_USE_CUSTOM_POOLS== 1：是否包含用户文件lwippools.h，该文件定义了lwIP所需的“标准”之外的其他池。如果将其设置为1，则必须在包含路径中的某个位置使用lwippools.h。
#define 	MEMP_USE_CUSTOM_POOLS   0
/*这个宏定义设置为 1 可以从中断（或其它需要立即响应的信号中） 释放 PBUF_RAM 的pbufs
  设置为1 时， mem_malloc 会被 semaphore 与 SYS_ARCH_PROTECT保护                                                                                          mem_free只会使用SYS_ARCH_PROTECT
  如果不希望这样，在拥有操作系统的情况下，卡可以使用
  pbuf_free_callback(p);
  mem_free_callback(m);
*/
#define 	LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT   0

/***************************************************************************************************************/
//                                      PPP
/***************************************************************************************************************/
//PPP示例
#define PPPOS_SUPPORT        1
#define PPP_SUPPORT          1


//没有以太网 暂时关闭
#define LWIP_ETHERNET        0
//没有以太网就没有APR
#define LWIP_ARP             0

//LWIP_PROVIDE_ERRNO==1：让LWIP提供ERRNO值和“ERRNO”变量。这个__DOXYGEN__（编程辅助工具）是不可能定义1的 ，定义会使用所有默认
#define LWIP_PROVIDE_ERRNO   1
//提供 int errno 不在头文件中 在lwipopts.c中
/*
#if LWIP_PROVIDE_ERRNO
    int errno;
#endif
*/


//lwip中有些示例采用这个宏来建立线程大小和线程等级 如pppos_example，可在文件中自行更改
#define DEFAULT_THREAD_STACKSIZE 1024
#define DEFAULT_THREAD_PRIO      14
//在tcpip_init函数中用到的宏
#define TCPIP_MBOX_SIZE         32
#define TCPIP_THREAD_STACKSIZE  2048
#define TCPIP_THREAD_PRIO       12

//使用RAW API 写ping命令
#define LWIP_RAW    1
//需要域名解析 开启DNS
#define LWIP_DNS    1
//使用DNS需要随机数
#define LWIP_RAND() ((u32_t)rand())

//debug总开关
#define LWIP_DEBUG          1
//debug分开关PPP
#define PPP_DEBUG            LWIP_DBG_OFF

//这一堆邮箱默认是0 这里不配置就有难以寻找的断言 邮箱的创建都是跟随着线程的创建
#define DEFAULT_RAW_RECVMBOX_SIZE 3
#define DEFAULT_UDP_RECVMBOX_SIZE 3
#define DEFAULT_TCP_RECVMBOX_SIZE 3
#define DEFAULT_ACCEPTMBOX_SIZE 3

//lwip_setsockopt中操作的各种功能
#define LWIP_SO_RCVTIMEO 1

#ifdef USE_XXX

/***************************************************************************************************************/
//                                      Timers
/***************************************************************************************************************/
/*
    LWIP_TIMEERS == 0:    删除对sys_timeout和lwip内部循环定时器的支持。 
                         （仍提供lwip内部循环定时器数组）
    （检查NO_SYS_NO_TIMERS是否与旧版本兼容）
*/
#define 	LWIP_TIMERS     1
/*
    LWIP_TIMERS_CUSTOM == 1: 提供自定义的定时器实现，timeouts.h中函数原型等及以下
                             sys_timeouts_init(), sys_timeout(), 
                             sys_untimeout(), sys_timeouts_mbox_fetch() 
*/ 
#define 	LWIP_TIMERS_CUSTOM   0


/***************************************************************************************************************/
//                                      memcpy
/***************************************************************************************************************/
/* 若实施速度比C库中包含的更快，则覆盖此项 */
#define 	MEMCPY(dst, src, len)    memcpy(dst,src,len)
/* 谨慎覆盖！ 如果长度在编译时已知且很小，则某些编译器（例如gcc）可以内联对memcpy（）的调用 */
#define 	SMEMCPY(dst, src, len)   memcpy(dst,src,len)
/* 若实施速度比C库中包含的更快，则覆盖此项。 lwIP仅在启用IPv6分段支持时才使用MEMMOVE。 */
#define 	MEMMOVE(dst, src, len)   memmove(dst,src,len)



/***************************************************************************************************************/
//                                      Core locking and MPU
/***************************************************************************************************************/
/* 
   MPU（内存保护单元） 兼容：
   启用特殊的内存管理机制，通过不将堆栈指针传递给其他线程，使lwip能够在MPU系统上工作
  （这会降低性能，因为内存是从池中分配而不是将其保留在堆栈中）
 */
#define 	LWIP_MPU_COMPATIBLE   0
/*
    创建在TCPIP线程操作期间保留的全局互斥锁。
    可以通过客户端代码锁定以执行lwIP操作，而无需使用回调更改为TCPIP线程。
    请参阅LOCK_TCPIP_CORE（）和UNLOCK_TCPIP_CORE（）。
    您的系统应提供支持优先级倒置的互斥锁以使用它
*/ 
#define 	LWIP_TCPIP_CORE_LOCKING   1
/*
    LWIP_TCPIP_CORE_LOCKING == 1
    使得tcpip_input（）也可以获取输入数据包的互斥锁，而不是分配消息并将其传递给tcpip_thread
    注意：从中断上下文调用tcpip_input（）时，这不起作用！
*/ 
#define 	LWIP_TCPIP_CORE_LOCKING_INPUT   0
/* 
    SYS_LIGHTWEIGHT_PROT   == 1    
    在缓冲区分配，释放和内存分配以及解除分配期间，为某些关键区域启用任务间保护（和任务与中断保护）。 
    注意：从多个上下文中使用lwIP时需要这样做！ 如果你禁用它，你必须确定你在做什么！ */ 
#define 	SYS_LIGHTWEIGHT_PROT   1
/* 
    宏/函数，用于检查当前函数调用期间是否满足lwIP的线程/锁定要求。 
    此宏通常调用在依赖于OS的sys层中实现的函数，并执行以下检查：
    不在ISR中（这也应该检查NO_SYS == 1！）
     如果LWIP_TCPIP_CORE_LOCKING = 1：保持TCPIP核心锁定
     如果LWIP_TCPIP_CORE_LOCKING = 0：从TCPIP线程调用函数
*/ 
#define 	LWIP_ASSERT_CORE_LOCKED()
/* 
    在lwIP TCPIP线程中首次调用。 可以与LWIP_ASSERT_CORE_LOCKED一起使用来检查内核锁定。
    实际标识 TCPIP线程的句柄
*/ 
#define 	LWIP_MARK_TCPIP_THREAD()



/***************************************************************************************************************/
//                                      Heap and memory pools
/***************************************************************************************************************/
/*
    内存堆（heap）分配策略
    MEM_LIBC_MALLOC==1: 使用C库提供的malloc / free / realloc而不是lwip内部分配器。
    一般情况，C库的分配策略碎片化比较严重，不适用于嵌入式    
*/
#define 	MEM_LIBC_MALLOC   0
/*
    内存池分配策略（是否用内存堆分配策略实现内存池分配）
    MEMP_MEM_MALLOC == 1：使用mem_malloc/mem_free而不是lwip内存池分配器。
    （堆分配可能比池分配慢得多）
    与MEM_USE_POOLS只能选择其一
*/ 
#define 	MEMP_MEM_MALLOC   0
/*
    MEMP_MEM_INIT == 1：
    强制使用memset初始化池内存。 如果池在未初始化的内存部分中移动，则很有用。
    这将确保pcbs结构中的默认值在所有条件下都能很好地初始化。
*/ 
#define 	MEMP_MEM_INIT   0
/* 字节对齐*/ 
#define 	MEM_ALIGNMENT   4
/*
    MEM_SIZE：堆内存的大小。 如果应用程序将发送大量需要复制的数据，则应将其设置为高。
*/ 
#define 	MEM_SIZE   1600
/* 
    内存池溢出检查
    0： 不检查
    1： 释放时检查
    2：调用memp_malloc() or memp_free() 检查，有用，但很慢
*/ 
#define 	MEMP_OVERFLOW_CHECK   0
/*
    内存池健全性检查
    MEMP_SANITY_CHECK == 1：在每个memp_free（）之后运行一次健全性检查，以确保链表中没有循环。
*/ 
#define 	MEMP_SANITY_CHECK   0
/* 
    堆内存溢出检查
    0: 不检查
    1： 释放时检查
    2：调用mem_malloc() or mem_free() 检查，有用，但很慢
 */ 
#define 	MEM_OVERFLOW_CHECK   0
/* 堆内存健全性检查 */ 
#define 	MEM_SANITY_CHECK   0
/* 
    内存堆分配策略（是否用内存池分配策略实现内存堆分配） 
    MEM_USE_POOLS==1: 使用内存池方式，还必须启用MEMP_USE_CUSTOM_POOLS。
    与MEMP_MEM_MALLOC只能选择其一
*/ 
#define 	MEM_USE_POOLS   0
/* 
    MEM_USE_POOLS_TRY_BIGGER_POOL== 1：
    如果一个malloc_pool为空，请尝试下一个更大的池 - 
    警告：这个可能的废物存储器，但它可以使系统更可靠。
 */ 
#define 	MEM_USE_POOLS_TRY_BIGGER_POOL   0
/*
    MEMP_USE_CUSTOM_POOLS== 1：
    是否包含用户文件lwippools.h，该文件定义了lwIP所需的“标准”之外的其他池。
    如果将其设置为1，则必须在包含路径中的某个位置使用lwippools.h。
*/ 
#define 	MEMP_USE_CUSTOM_POOLS   0
/*
    其他上下文释放内存
*/ 
#define 	LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT   0



/***************************************************************************************************************/
//                                      Internal memory pools
/***************************************************************************************************************/
/*
    协议包缓存数量 PBUF(Packet buffers)
   （用于PBUF_ROM和PBUF_REF）。 如果应用程序从ROM（或其他静态存储器）发送大量数据，则应将其设置为高。
*/
#define 	MEMP_NUM_PBUF   16

/*
    Raw 协议控制块数量（需LWIP_RAW选项）
*/ 
#define 	MEMP_NUM_RAW_PCB   4
 
/* UDP协议控制块的数量。 每个活动UDP“连接”一个。 （需LWIP_UDP选项）*/ 
#define 	MEMP_NUM_UDP_PCB   4
 
/* 并发的TCP连接数。 （需要LWIP_TCP选项） */ 
#define 	MEMP_NUM_TCP_PCB   5
 
/* 侦听TCP连接的数量。 （需要LWIP_TCP选项） */ 
#define 	MEMP_NUM_TCP_PCB_LISTEN   8
 
/* 同时排队的TCP段的数量。 （需要LWIP_TCP选项） */ 
#define 	MEMP_NUM_TCP_SEG   16
 
/*
    同时活动的altcp层控制块的数量。
  （需要LWIP_ALTCP选项）具有多个层的连接需要多个altcp_pcb（例如，TCP上的TLS需要2个altcp_pcbs，                一个用于TLS，一个用于TCP）。
*/ 
#define 	MEMP_NUM_ALTCP_PCB   MEMP_NUM_TCP_PCB
 
/*   同时排队等待重组的IP数据包数量（整个数据包，而不是片段！） */ 
#define 	MEMP_NUM_REASSDATA   5
 
/*
    同时发送的IP分片数（片段，而不是整个数据包！）。
    这仅用于LWIP_NETIF_TX_SINGLE_PBUF == 0并且只有在启用DMA的MAC时必须> 1，其中当          netif->output返回时尚未发送数据包。
*/ 
#define 	MEMP_NUM_FRAG_PBUF   15
 
/*
    ARP 队列数（需要ARP_QUEUEING选项）
*/ 
#define 	MEMP_NUM_ARP_QUEUE   30
 
/*  组播网络接口数 */ 
#define 	MEMP_NUM_IGMP_GROUP   8
 
/* 
    协议栈内核使用超时数量（不包含APP）此处为所有已启用的模块计算默认的超时数。
*/ 
#define 	LWIP_NUM_SYS_TIMEOUT_INTERNAL   (LWIP_TCP + IP_REASSEMBLY + LWIP_ARP + (2*LWIP_DHCP) + LWIP_AUTOIP + LWIP_IGMP + LWIP_DNS + PPP_NUM_TIMEOUTS + (LWIP_IPV6 * (1 + LWIP_IPV6_REASS + LWIP_IPV6_MLD)))
 
/* 同时活动的超时数。 此处为所有已启用的模块计算默认的超时数。 公式要求设置为“0”或“1”。 */ 
#define 	MEMP_NUM_SYS_TIMEOUT   LWIP_NUM_SYS_TIMEOUT_INTERNAL
 
/* 
    netbuf 结构的数量。 （仅在使用sequential API时才需要，例如api_lib.c） 
*/ 
#define 	MEMP_NUM_NETBUF   2
 
/*
    netconns 结构的数量。 （仅在使用顺序API时才需要，例如api_lib.c）
*/ 
#define 	MEMP_NUM_NETCONN   4
 
/*
  lwip_select_cb 结构的数量。 
（仅当你有LWIP_MPU_COMPATIBLE == 1并使用套接字API时才需要。在这种情况下，每个线程调用lwip_select需要一个。）
*/ 
#define 	MEMP_NUM_SELECT_CB   4
 
/*
 struct tcpip_msg的数量，用于回调/超时API通信。 （仅在使用tcpip.c时才需要）
*/ 
#define 	MEMP_NUM_TCPIP_MSG_API   8
 
/* 
    struct tcpip_msg的数量，用于传入的数据包。 （仅在使用tcpip.c时才需要）
*/ 
#define 	MEMP_NUM_TCPIP_MSG_INPKT   8
 
/* 并发运行的lwip_addrinfo（）调用次数（在使用lwip_freeaddrinfo（）释放相应的内存之前）。 */ 
#define 	MEMP_NUM_NETDB   1
 
/* DNS_LOCAL_HOSTLIST_IS_DYNAMIC == 1时本地主机列表中的主机条目数。 */ 
#define 	MEMP_NUM_LOCALHOSTLIST   1
 
/* pbuf池中的缓冲区数。 */ 
#define 	PBUF_POOL_SIZE   16
 
/* 各种socket，netconn和tcpip函数的并发活动调用数 */ 
#define 	MEMP_NUM_API_MSG   MEMP_NUM_TCPIP_MSG_API
 
/* netconn_gethostbyname的并发活动调用次数 */ 
#define 	MEMP_NUM_DNS_API_MSG   MEMP_NUM_TCPIP_MSG_API
 
/*   getsockopt / setsockopt的并发活动调用数 */ 
#define 	MEMP_NUM_SOCKET_SETGETSOCKOPT_DATA   MEMP_NUM_TCPIP_MSG_API
 
/* netifapi函数同时调用的次数 */ 
#define 	MEMP_NUM_NETIFAPI_MSG   MEMP_NUM_TCPIP_MSG_API




/***************************************************************************************************************/
//                                      SNMP MIB2 callbacks
/***************************************************************************************************************/
/*
    LWIP_MIB2_CALLBACKS == 1：
    打开SNMP MIB2回调。 打开它以获得实现MIB2所需的回调。 通常也应该启用MIB2_STATS。
*/
#define 	LWIP_MIB2_CALLBACKS   0



/***************************************************************************************************************/
//                                      Multicast（组播）
/***************************************************************************************************************/
/* 
    启用多播TX支持，
    如套接字选项IP_MULTICAST_TTL / IP_MULTICAST_IF / IP_MULTICAST_LOOP，
    以及（当前仅支持）相应IPv6选项的核心支持。 
*/
#define 	LWIP_MULTICAST_TX_OPTIONS   ((LWIP_IGMP || LWIP_IPV6_MLD) && (LWIP_UDP || LWIP_RAW))



/***************************************************************************************************************/
//                                      Threading
/***************************************************************************************************************/
/* TCPIP 主线程名称 */
#define 	TCPIP_THREAD_NAME   "tcpip_thread"
 
/* TCPIP 主线程 栈大小 */ 
#define 	TCPIP_THREAD_STACKSIZE   512
 
/* TCPIP 主线程 优先级 */ 
#define 	TCPIP_THREAD_PRIO   1
 
/* TCPIP 主线程消息的邮箱大小 */ 
#define 	TCPIP_MBOX_SIZE   0
 
/* TCPIP 将其定义为触发监视程序的内容。 */ 
#define 	LWIP_TCPIP_THREAD_ALIVE()
 
/* SLIPIF 线程名称 */ 
#define 	SLIPIF_THREAD_NAME   "slipif_loop"
 
/* SLIPIF 线程 栈大小 */ 
#define 	SLIPIF_THREAD_STACKSIZE   0
 
/* SLIPIF 线程 优先级 */ 
#define 	SLIPIF_THREAD_PRIO   1
 
/* 默认 线程名称 */ 
#define 	DEFAULT_THREAD_NAME   "lwIP"
 
/* 默认 线程 栈大小 */ 
#define 	DEFAULT_THREAD_STACKSIZE   0
 
/* 默认 线程 优先级 */ 
#define 	DEFAULT_THREAD_PRIO   1
 
/* 默认 NETCONN_RAW传入数据包的邮箱大小。 */ 
#define 	DEFAULT_RAW_RECVMBOX_SIZE   0
 
/* 默认 NETCONN_UDP传入数据包的邮箱大小。 */ 
#define 	DEFAULT_UDP_RECVMBOX_SIZE   0
 
/* 默认 NETCONN_TCP传入数据包的邮箱大小。 */  
#define 	DEFAULT_TCP_RECVMBOX_SIZE   0
 
/* 默认 传入连接的邮箱大小 */ 
#define 	DEFAULT_ACCEPTMBOX_SIZE   0



/***************************************************************************************************************/
//                                      Checksum
/***************************************************************************************************************/
/* 
    LWIP_CHECKSUM_CTRL_PER_NETIF== 1：
    netif启用/禁用校验和生成/检查
    注意：如果启用，则必须启用CHECKSUM_GEN_ *和CHECKSUM_CHECK_ *定义！
 */
#define 	LWIP_CHECKSUM_CTRL_PER_NETIF   0
 
/* 为传出IP数据包生成校验和*/ 
#define 	CHECKSUM_GEN_IP   1
 
 
/* 为传出UDP数据包生成校验和*/  
#define 	CHECKSUM_GEN_UDP   1
 
/* 为传出TCP数据包生成校验和*/  
#define 	CHECKSUM_GEN_TCP   1
 
/* 为传出ICMP数据包生成校验和*/  
#define 	CHECKSUM_GEN_ICMP   1
 
/* 为传出ICMP6数据包生成校验和*/  
#define 	CHECKSUM_GEN_ICMP6   1
 
/* 为传入IP数据包校验和检查 */ 
#define 	CHECKSUM_CHECK_IP   1
 
/* 为传入UDP数据包校验和检查 */  
#define 	CHECKSUM_CHECK_UDP   1
 
/* 为传入TCP数据包校验和检查 */  
#define 	CHECKSUM_CHECK_TCP   1
 
/* 为传入ICMP数据包校验和检查 */  
#define 	CHECKSUM_CHECK_ICMP   1
 
/* 为传入ICMP6数据包校验和检查 */  
#define 	CHECKSUM_CHECK_ICMP6   1
 
/* 将数据从应用程序缓冲区复制到pbuf时计算校验和。 */  
#define 	LWIP_CHECKSUM_ON_COPY   0


/***************************************************************************************************************/
//                                      Hooks（钩子选项）
/***************************************************************************************************************/
/* 默认情况下钩子未定义，如果需要，可将它们定义为函数。 */
//#define 	LWIP_HOOK_FILENAME   "path/to/my/lwip_hooks.h"
 
#define 	LWIP_HOOK_TCP_ISN(local_ip, local_port, remote_ip, remote_port)
 
#define 	LWIP_HOOK_TCP_INPACKET_PCB(pcb, hdr, optlen, opt1len, opt2, p)
 
#define 	LWIP_HOOK_TCP_OUT_TCPOPT_LENGTH(pcb, internal_len)
 
#define 	LWIP_HOOK_TCP_OUT_ADD_TCPOPTS(p, hdr, pcb, opts)
 
#define 	LWIP_HOOK_IP4_INPUT(pbuf, input_netif)
 
#define 	LWIP_HOOK_IP4_ROUTE()
 
#define 	LWIP_HOOK_IP4_ROUTE_SRC(src, dest)
 
#define 	LWIP_HOOK_IP4_CANFORWARD(src, dest)
 
#define 	LWIP_HOOK_ETHARP_GET_GW(netif, dest)
 
#define 	LWIP_HOOK_IP6_INPUT(pbuf, input_netif)
 
#define 	LWIP_HOOK_IP6_ROUTE(src, dest)
 
#define 	LWIP_HOOK_ND6_GET_GW(netif, dest)
 
#define 	LWIP_HOOK_VLAN_CHECK(netif, eth_hdr, vlan_hdr)
 
#define 	LWIP_HOOK_VLAN_SET(netif, p, src, dst, eth_type)
 
#define 	LWIP_HOOK_MEMP_AVAILABLE(memp_t_type)
 
#define 	LWIP_HOOK_UNKNOWN_ETH_PROTOCOL(pbuf, netif)
 
#define 	LWIP_HOOK_DHCP_APPEND_OPTIONS(netif, dhcp, state, msg, msg_type, options_len_ptr)
 
#define 	LWIP_HOOK_DHCP_PARSE_OPTION(netif, dhcp, state, msg, msg_type, option, len, pbuf, offset)
 
#define 	LWIP_HOOK_DHCP6_APPEND_OPTIONS(netif, dhcp6, state, msg, msg_type, options_len_ptr, max_len)
 
#define 	LWIP_HOOK_SOCKETS_SETSOCKOPT(s, sock, level, optname, optval, optlen, err)
 
#define 	LWIP_HOOK_SOCKETS_GETSOCKOPT(s, sock, level, optname, optval, optlen, err)
 
#define 	LWIP_HOOK_NETCONN_EXTERNAL_RESOLVE(name, addr, addrtype, err)









/***************************************************************************************************************/
//                                      IPv4 ARP
/***************************************************************************************************************/
/*
    LWIP_ARP == 1 : 使能ARP功能
*/
#define 	LWIP_ARP   1
 
/* MAC-IP地址缓存对大小*/ 
#define 	ARP_TABLE_SIZE   10
 
/* ARP条目在上次更新后保持有效的时间 */ 
#define 	ARP_MAXAGE   300
 
/*
    ARP_QUEUEING== 1：
    在硬件地址解析期间，多个传出数据包排队。 默认情况下，每个IP地址仅排队最新的数据包。 
    这对大多数协议来说已足够，主要是减少TCP连接启动时间。
*/ 
 
#define 	ARP_QUEUEING   0
 
/* 
    其他网络层可能为每个未解析的地址排队的最大数据包数。
    默认为3,0表示禁用。 丢弃旧数据包，新数据包排队。
 */ 
#define 	ARP_QUEUE_LEN   3
 
/*
    ETHARP_SUPPORT_VLAN == 1：支持使用VLAN标头接收和发送以太网数据包
*/ 
#define 	ETHARP_SUPPORT_VLAN   0
 
/*
    LWIP_ETHERNET== 1：启用以太网支持
*/ 
#define 	LWIP_ETHERNET   LWIP_ARP
 
/* 在以太网标头之前添加的字节数，以确保在该标头之后对齐有效负载。 */ 
#define 	ETH_PAD_SIZE   0
 
/*
    ETHARP_SUPPORT_STATIC_ENTRIES== 1：
    启用以支持静态ARP表条目（使用etharp_add_static_entry / etharp_remove_static_entry）。
*/ 
#define 	ETHARP_SUPPORT_STATIC_ENTRIES   0
 
/*
    ETHARP_TABLE_MATCH_NETIF== 1：
    匹配netif的ARP表条目。 如果禁用，则不支持多个netif上的重复IP地址（但这仅适用于AutoIP）
*/ 
#define 	ETHARP_TABLE_MATCH_NETIF   !LWIP_SINGLE_NETIF



/***************************************************************************************************************/
//                                      IPv4 IP
/***************************************************************************************************************/
/* 使能IPv4 */
#define 	LWIP_IPV4   1
 
/*
    IP_FORWARD== 1：
    启用跨网络接口转发IP数据包的功能。 如果要在只有一个网络接口的设备上运行lwIP，请将其定义为0。
*/ 
#define 	IP_FORWARD   0
 
/*
    IP_REASSEMBLY== 1：重新组合传入的分段IP数据包。 
*/ 
#define 	IP_REASSEMBLY   1
 
/*
    IP_FRAG== 1：若传出的IP数据包大小超过MTU，则将其分段。
*/ 
#define 	IP_FRAG   1
 
/*
    定义IP选项的行为。
    IP_OPTIONS_ALLOWED == 0：丢弃所有带IP选项的数据包。 
    IP_OPTIONS_ALLOWED == 1：允许IP选项（但未解析）
*/ 
#define 	IP_OPTIONS_ALLOWED   1
 
/* 
    分段IP数据包等待所有片段到达的最长时间。 
    如果此时并非所有片段都到达，则丢弃整个数据包。 
    秒
*/ 
#define 	IP_REASS_MAXAGE   15
 
/*
    等待重新组装的pbuf的最大总量。
    由于接收到的pbuf已入队，因此请务必配置PBUF_POOL_SIZE> IP_REASS_MAX_PBUFS，
    以便即使最大数量的片段入队重组，堆栈仍能够接收数据包！ 
    启用IPv4和IPv6时，甚至会更改为（PBUF_POOL_SIZE> 2 * IP_REASS_MAX_PBUFS）！
*/ 
#define 	IP_REASS_MAX_PBUFS   10
 
/* 传输层使用的生存时间的默认值。 */ 
#define 	IP_DEFAULT_TTL   255
 
/*
    IP_SOF_BROADCAST= 1：
    使用SOF_BROADCAST字段在udp和raw 发送
*/ 
#define 	IP_SOF_BROADCAST   0
 
/*
    IP_SOF_BROADCAST_RECV==1:
    （需要IP_SOF_BROADCAST = 1）启用接收过滤。
*/ 
#define 	IP_SOF_BROADCAST_RECV   0
 
/*
    IP_FORWARD_ALLOW_TX_ON_RX_NETIF== 1：
    允许ip_forward（）在收到它的netif上发回数据包。 仅用于无线网络。
    注意：当为1时，请确保您的netif驱动程序使用相应的pbuf标记正确标记传入的链路层广播/多播数据包！
*/ 
#define 	IP_FORWARD_ALLOW_TX_ON_RX_NETIF   0



/***************************************************************************************************************/
//                                      IPv4 ICMP
/***************************************************************************************************************/
/* 协议栈使能ICMP模块*/
#define 	LWIP_ICMP   1
 
/* ICMP 数据包生存时间 */ 
#define 	ICMP_TTL   IP_DEFAULT_TTL
 
/*  LWIP_BROADCAST_PING== 1：响应广播ping（默认为仅单播）*/ 
#define 	LWIP_BROADCAST_PING   0
 
/* LWIP_MULTICAST_PING == 1：响应多播ping（默认为单播） */
#define 	LWIP_MULTICAST_PING   0



/***************************************************************************************************************/
//                                      IPv4 DHCP
/***************************************************************************************************************/
/* 使能 DHCP 功能 */
#define 	LWIP_DHCP   0
 
/* DHCP_DOES_ARP_CHECK== 1：对提供的地址进行ARP检查。*/ 
#define 	DHCP_DOES_ARP_CHECK   (LWIP_DHCP && LWIP_ARP)
 
/* LWIP_DHCP_BOOTP_FILE== 1：存储provided_si_addr和boot_file_name。*/ 
#define 	LWIP_DHCP_BOOTP_FILE   0
 
/*
    LWIP_DHCP_GETS_NTP== 1：
    通过发现/选择请求NTP服务器。 对于每个响应数据包，将调用一个回调，该回调必须由端口提供：
    void dhcp_set_ntp_servers（u8_t num_ntp_servers，ip_addr_t * ntp_server_addrs）;
*/ 
#define 	LWIP_DHCP_GET_NTP_SRV   0
 
/* NTP 服务最大请求数 */ 
#define 	LWIP_DHCP_MAX_NTP_SERVERS   1
 
#define 	LWIP_DHCP_MAX_DNS_SERVERS   DNS_MAX_SERVERS


/***************************************************************************************************************/
//                                      IPv4 AUTOIP
/***************************************************************************************************************/
/* 使能 AUTOIP 模块*/
#define 	LWIP_AUTOIP   0
 
/*
    LWIP_DHCP_AUTOIP_COOP == 1：允许同时在同一接口上同时启用DHCP和AUTOIP。
*/ 
#define 	LWIP_DHCP_AUTOIP_COOP   0
 
/* DHCP 发现次数 */ 
#define 	LWIP_DHCP_AUTOIP_COOP_TRIES   9





/***************************************************************************************************************/
//                                      IPv4 IGMP
/***************************************************************************************************************/
/* 使能 IGMP 模块 */
#define 	LWIP_IGMP   0


/***************************************************************************************************************/
//                                      Callback-style APIs RAW
/***************************************************************************************************************/
/* LWIP_RAW == 1：启用应用层挂钩到IP层本身。 */
#define 	LWIP_RAW   0
 
#define 	RAW_TTL   IP_DEFAULT_TTL

/***************************************************************************************************************/
//                                      Callback-style APIs RAW
/***************************************************************************************************************/
/* DNS 模块 */
#define 	LWIP_DNS   0
 
/* DNS本地维护的最大条目数。 */ 
#define 	DNS_TABLE_SIZE   4
 
/* 名称表中支持的DNS最大主机名长度。 */ 
#define 	DNS_MAX_NAME_LENGTH   256
 
/* DNS服务器的最大数量。 */ 
#define 	DNS_MAX_SERVERS   2
 
/* 在“超时”之前，在请求名称时DNS重试的最大次数。 */ 
#define 	DNS_MAX_RETRIES   4
 
/* DNS在查询和响应之间进行名称检查。 */ 
#define 	DNS_DOES_NAME_CHECK   1
 
/* 控制DNS实施的安全级别，默认情况下使用所有DNS安全功能。 */ 
#define 	LWIP_DNS_SECURE   (LWIP_DNS_SECURE_RAND_XID | LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING | LWIP_DNS_SECURE_RAND_SRC_PORT)
 
/* DNS 地址列表。 */ 
#define 	DNS_LOCAL_HOSTLIST   0
 
/* DNS 动态 地址列表 */ 
#define 	DNS_LOCAL_HOSTLIST_IS_DYNAMIC   0
 
/* MDNS 查询支持*/ 
#define 	LWIP_DNS_SUPPORT_MDNS_QUERIES   0


/***************************************************************************************************************/
//                                      Callback-style APIs UDP
/***************************************************************************************************************/
/* 使能UDP */
#define 	LWIP_UDP   1
 
#define 	LWIP_UDPLITE   0
 
/* 生存时间 */ 
#define 	UDP_TTL   IP_DEFAULT_TTL
 
/*
    LWIP_NETBUF_RECVINFO == 1：将目标地址和端口附加到每个netbuf。
*/ 
#define 	LWIP_NETBUF_RECVINFO   0


/***************************************************************************************************************/
//                                      Callback-style APIs TCP
/***************************************************************************************************************/
/* 使能TCP */
#define 	LWIP_TCP   1
 
#define 	TCP_TTL   IP_DEFAULT_TTL
 
/* TCP 窗口大小 ：必须至少（2 * TCP_MSS）*/ 
#define 	TCP_WND   (4 * TCP_MSS)
 
/* 数据段 最大重传数 */ 
#define 	TCP_MAXRTX   12
 
/* SYN段 最大重传数 */ 
#define 	TCP_SYNMAXRTX   6
 
/*
    TCP_QUEUE_OOSEQ==1: 
    TCP将对无序到达的段进行排队。 如果设备内存不足，请定义为0。
*/ 
#define 	TCP_QUEUE_OOSEQ   LWIP_TCP
 
/*
    LWIP_TCP_SACK_OUT== 1：
    TCP将支持发送选择性确认（SACK）
*/ 
#define 	LWIP_TCP_SACK_OUT   0
 
#define 	LWIP_TCP_MAX_SACK_NUM   4
 
/* TCP_MSS：TCP最大段大小 */ 
#define 	TCP_MSS   536
 
/* tcp预计发送的分段长度，为1则根据窗口大小分配 */ 
#define 	TCP_CALCULATE_EFF_SEND_MSS   1
 
/* 发送缓冲区(bytes)*/ 
#define 	TCP_SND_BUF   (2 * TCP_MSS)
 
/* TCP发送方缓冲区空间（pbufs），队列长度 */ 
#define 	TCP_SND_QUEUELEN   ((4 * (TCP_SND_BUF) + (TCP_MSS - 1))/(TCP_MSS))
 
/* TCP可写空间（bytes）。 这必须小于TCP_SND_BUF */ 
#define 	TCP_SNDLOWAT   LWIP_MIN(LWIP_MAX(((TCP_SND_BUF)/2), (2 * TCP_MSS) + 1), (TCP_SND_BUF) - 1)
 
/*
    TCP可写bufs（pbuf计数）。 这必须小于TCP_SND_QUEUELEN。 
    如果在pcb上排队的pbuf数量低于此数量，则select返回可写（与TCP_SNDLOWAT结合使用）
*/ 
#define 	TCP_SNDQUEUELOWAT   LWIP_MAX(((TCP_SND_QUEUELEN)/2), 5)
 
/* 在ooseq上排队的默认最大字节数 */ 
#define 	TCP_OOSEQ_MAX_BYTES   0
 
/* 在ooseq上排队的默认最大pbufs 数 */ 
#define 	TCP_OOSEQ_MAX_PBUFS   0
 
/* 启用backlog选项 */ 
#define 	TCP_LISTEN_BACKLOG   0
 
/* TCP侦听netconns允许的最大backlog */ 
#define 	TCP_DEFAULT_LISTEN_BACKLOG   0xff
 
/*
    tcp_write可以提前分配的最大字节数，以尝试创建更短的pbuf链进行传输。
    有意义的范围是0到TCP_MSS。
*/ 
#define 	TCP_OVERSIZE   TCP_MSS
 
/* 支持TCP时间戳选项 */ 
#define 	LWIP_TCP_TIMESTAMPS   0
 
/* 窗口中的差异触发显式窗口更新 */ 
#define 	TCP_WND_UPDATE_THRESHOLD   LWIP_MIN((TCP_WND / 4), (TCP_MSS * 4))
 
/* 事件或回调二选一，默认回调 */ 
#define 	LWIP_EVENT_API   0
#define          LWIP_CALLBACK_API 1
 
/* TCP Window Scale Option (WSopt):   Kind: 3 Length: 3 bytes  */ 
#define 	LWIP_WND_SCALE   0
#define         TCP_RCV_SCALE    0
 
/* 扩展条目数 */ 
#define 	LWIP_TCP_PCB_NUM_EXT_ARGS   0
 
/*
    Application layered TCP
    LWIP_ALTCP == 1：
        启用altcp API。 altcp是一个抽象层，可以防止应用程序链接到tcp.h函数，但提供相同的功能。
    LWIP_ALTCP == 0:
        仍可编译针对altcp API编写的应用程序，但是直接链接到tcp.h回调API，然后无法使用分层协议。
*/ 
#define 	LWIP_ALTCP   0
 
/* LWIP_ALTCP_TLS== 1：altcp API启用TLS(安全传输协议)支持 */ 
#define 	LWIP_ALTCP_TLS   0







/***************************************************************************************************************/
//                                      PBUF
/***************************************************************************************************************/
/* 链路层头分配的字节数。 默认值为14，即以太网的标准值。 */
#define 	PBUF_LINK_HLEN   (14 + ETH_PAD_SIZE)
 
/* 以太网头之前应为其他封装头分配的字节数 */ 
#define 	PBUF_LINK_ENCAPSULATION_HLEN   0
 
/*
    pbuf池中每个pbuf的大小。
    默认设计用于在一个pbuf中容纳单个完整大小的TCP帧，包括TCP_MSS，IP头和链接头。
*/ 
#define 	PBUF_POOL_BUFSIZE   LWIP_MEM_ALIGN_SIZE(TCP_MSS+40+PBUF_LINK_ENCAPSULATION_HLEN+PBUF_LINK_HLEN)
 
/* pbuf中的Refcount类型。 如果255 refs不够，可以增加u8_t的默认宽度。 */ 
#define 	LWIP_PBUF_REF_T   u8_t



/***************************************************************************************************************/
//                                      NETIF
/***************************************************************************************************************/
/* 
    LWIP_SINGLE_NETIF== 1：
    只使用一个netif。这是小型现实生活目标的常见情况。 可以省略一些像路由等代码。
*/
#define 	LWIP_SINGLE_NETIF   0
 
/*
    LWIP_NETIF_HOSTNAME== 1：
    将DHCP_OPTION_HOSTNAME与netif的主机名字段一起使用。
*/ 
#define 	LWIP_NETIF_HOSTNAME   0
 
/* 
    LWIP_NETIF_API == 1：支持netif api（在netifapi.c中） 
    (to be used from non-TCPIP threads)
*/ 
#define 	LWIP_NETIF_API   0
 
/* 
    LWIP_NETIF_STATUS_CALLBACK== 1：
    每当接口改变其上/下状态时（即，由于DHCP IP获取），支持回调功能 
*/ 
#define 	LWIP_NETIF_STATUS_CALLBACK   0
 
/*
    LWIP_NETIF_EXT_STATUS_CALLBACK== 1：
    支持多个支持多个订户的netif相关事件的扩展回调函数。
*/ 
#define 	LWIP_NETIF_EXT_STATUS_CALLBACK   0
 
/*
    LWIP_NETIF_LINK_CALLBACK== 1：
    只要链接发生变化（即链接断开），就从接口支持回调函数
*/ 
#define 	LWIP_NETIF_LINK_CALLBACK   0
 
/*
    LWIP_NETIF_REMOVE_CALLBACK== 1：
    支持在删除netif时调用的回调函数
*/ 
#define 	LWIP_NETIF_REMOVE_CALLBACK   0
 
/*
    LWIP_NETIF_HWADDRHINT== 1：在struct netif中缓存链接层地址提示（例如表索引）。
    TCP和UDP可以利用它来防止扫描每个发送的数据包的ARP表。 虽然这对于大型ARP表或许多并发连接来说速    度更快，但如果您拥有一个小型ARP表或者从不存在并发连接，则可能会适得其反。
*/ 
#define 	LWIP_NETIF_HWADDRHINT   0
 
/* 
    单一PBUF发送 
    LWIP_NETIF_TX_SINGLE_PBUF== 1
    lwIP会尝试将所有数据发送到一个pbuf中。 这是为了与不支持分散 - 聚集的支持DMA的MAC兼容。
*/ 
#define 	LWIP_NETIF_TX_SINGLE_PBUF   0
 
/* 可以在struct netif的client_data成员数组中存储数据的客户端数量（最多256个）。 */ 
#define 	LWIP_NUM_NETIF_CLIENT_DATA   0






/***************************************************************************************************************/
//                                      IPV6 不管
/***************************************************************************************************************/

/***************************************************************************************************************/
//                                      Thread-safe APIs Netconn
/***************************************************************************************************************/
/* LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)  */
#define 	LWIP_NETCONN   1
 
/*
    LWIP_TCPIP_TIMEOUT== 1：
    启用tcpip_timeout / tcpip_untimeout以从另一个线程创建在tcpip_thread中运行的计时器。
*/
#define 	LWIP_TCPIP_TIMEOUT   0
 
/*
    LWIP_NETCONN_SEM_PER_THREAD== 1：
    每个线程使用一个（本地线程）信号量调用socket / netconn函数，而不是每netconn分配一个信号量（和每个选择等）
*/ 
#define 	LWIP_NETCONN_SEM_PER_THREAD   0
 
/*
    LWIP_NETCONN_FULLDUPLEX == 1：
    启用允许从一个线程读取，从第二个线程写入并同时从第三个线程关闭的代码。
*/ 
#define 	LWIP_NETCONN_FULLDUPLEX   0






/***************************************************************************************************************/
//                                      Thread-safe APIs Sockets
/***************************************************************************************************************/
/* LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)  */
#define 	LWIP_SOCKET   1
 
/* 
    LWIP_COMPAT_SOCKETS== 1：启用BSD样式的套接字函数名称。
 */ 
#define 	LWIP_COMPAT_SOCKETS   1
 
/*
    LWIP_POSIX_SOCKETS_IO_NAMES== 1：启用POSIX样式的套接字函数名称
*/ 
#define 	LWIP_POSIX_SOCKETS_IO_NAMES   1
 
/* 增加LwIP创建的文件描述符编号 */ 
#define 	LWIP_SOCKET_OFFSET   0
 
#define 	LWIP_TCP_KEEPALIVE   0
 
/* 为套接字/ netconns和SO_SNDTIMEO处理启用发送超时。 */ 
#define 	LWIP_SO_SNDTIMEO   0
 
/* 为套接字/ netconns和SO_SNDTIMEO处理启用接收超时。 */ 
#define 	LWIP_SO_RCVTIMEO   0
 
#define 	LWIP_SO_SNDRCVTIMEO_NONSTANDARD   0
 
/* LWIP_SO_RCVBUF == 1：启用SO_RCVBUF处理。 */ 
#define 	LWIP_SO_RCVBUF   0
 
/* LWIP_SO_LINGER == 1：启用SO_LINGER处理。 */ 
#define 	LWIP_SO_LINGER   0
 
/* 如果使用LWIP_SO_RCVBUF，则这是recv_bufsize的默认值。 */ 
#define 	RECV_BUFSIZE_DEFAULT   INT_MAX
 
/* 关闭超时默认值 */ 
#define 	LWIP_TCP_CLOSE_TIMEOUT_MS_DEFAULT   20000
 
/* SO_REUSE==1: Enable SO_REUSEADDR option. */
#define 	SO_REUSE   0
 
#define 	SO_REUSE_RXTOALL   0
 
/*  数据模式选择：0 win 1:linux*/ 
#define 	LWIP_FIONREAD_LINUXMODE   0
 
/*
    LWIP_SOCKET_SELECT == 1（默认值）：为套接字启用select（）（使用netconn回调来跟踪事件）。         这样可以节省RAM（每个插槽的计数器）和代码（netconn事件回调），这可以提高性能。
*/ 
#define 	LWIP_SOCKET_SELECT   1
 
/*
    LWIP_SOCKET_POLL == 1（默认值）：
    为套接字启用poll（）（包括struct pollfd，nfds_t和constants）
*/ 
#define 	LWIP_SOCKET_POLL   1





/***************************************************************************************************************/
//                                      Debugging Assertion handling（断言检查）
/***************************************************************************************************************/
/* 禁用LWIP_ASSERT检查：要禁用断言，请在arch/cc.h中定义LWIP_NOASSERT。 */
#define 	LWIP_NOASSERT




/***************************************************************************************************************/
//                                      Debugging Statistics（状态信息）
/***************************************************************************************************************/
/* 
    LWIP_STATS== 1：
    在lwip_stats中启用统计信息收集。 
    调试有帮助
*/
#define 	LWIP_STATS   1
 
/*
    LWIP_STATS_DISPLAY== 1：
    编译统计输出函数。
*/ 
#define 	LWIP_STATS_DISPLAY   0
 
/* 连接状态 */ 
#define 	LINK_STATS   1
 
#define 	ETHARP_STATS   (LWIP_ARP)
 
#define 	IP_STATS   1
 
#define 	IPFRAG_STATS   (IP_REASSEMBLY || IP_FRAG)
 
#define 	ICMP_STATS   1
 
#define 	IGMP_STATS   (LWIP_IGMP)
 
#define 	UDP_STATS   (LWIP_UDP)
 
#define 	TCP_STATS   (LWIP_TCP)
 
#define 	MEM_STATS   ((MEM_LIBC_MALLOC == 0) && (MEM_USE_POOLS == 0))
 
#define 	MEMP_STATS   (MEMP_MEM_MALLOC == 0)
 
#define 	SYS_STATS   (NO_SYS == 0)
 
#define 	IP6_STATS   (LWIP_IPV6)
 
#define 	ICMP6_STATS   (LWIP_IPV6 && LWIP_ICMP6)
 
#define 	IP6_FRAG_STATS   (LWIP_IPV6 && (LWIP_IPV6_FRAG || LWIP_IPV6_REASS))
 
#define 	MLD6_STATS   (LWIP_IPV6 && LWIP_IPV6_MLD)
 
#define 	ND6_STATS   (LWIP_IPV6)
 
#define 	MIB2_STATS   0



/***************************************************************************************************************/
//                                      Debugging Debug messages（调试信息）
/***************************************************************************************************************/
/* 调试信息配置 */
#define 	LWIP_DBG_MIN_LEVEL   LWIP_DBG_LEVEL_ALL
 
#define 	LWIP_DBG_TYPES_ON   LWIP_DBG_ON
 
#define 	ETHARP_DEBUG   LWIP_DBG_OFF
 
#define 	NETIF_DEBUG   LWIP_DBG_OFF
 
#define 	PBUF_DEBUG   LWIP_DBG_OFF
 
#define 	API_LIB_DEBUG   LWIP_DBG_OFF
 
#define 	API_MSG_DEBUG   LWIP_DBG_OFF
 
#define 	SOCKETS_DEBUG   LWIP_DBG_OFF
 
#define 	ICMP_DEBUG   LWIP_DBG_OFF
 
#define 	IGMP_DEBUG   LWIP_DBG_OFF
 
#define 	INET_DEBUG   LWIP_DBG_OFF
 
#define 	IP_DEBUG   LWIP_DBG_OFF
 
#define 	IP_REASS_DEBUG   LWIP_DBG_OFF
 
#define 	RAW_DEBUG   LWIP_DBG_OFF
 
#define 	MEM_DEBUG   LWIP_DBG_OFF
 
#define 	MEMP_DEBUG   LWIP_DBG_OFF
 
#define 	SYS_DEBUG   LWIP_DBG_OFF
 
#define 	TIMERS_DEBUG   LWIP_DBG_OFF
 
#define 	TCP_DEBUG   LWIP_DBG_OFF
 
#define 	TCP_INPUT_DEBUG   LWIP_DBG_OFF
 
#define 	TCP_FR_DEBUG   LWIP_DBG_OFF
 
#define 	TCP_RTO_DEBUG   LWIP_DBG_OFF
 
#define 	TCP_CWND_DEBUG   LWIP_DBG_OFF
 
#define 	TCP_WND_DEBUG   LWIP_DBG_OFF
 
#define 	TCP_OUTPUT_DEBUG   LWIP_DBG_OFF
 
#define 	TCP_RST_DEBUG   LWIP_DBG_OFF
 
#define 	TCP_QLEN_DEBUG   LWIP_DBG_OFF
 
#define 	UDP_DEBUG   LWIP_DBG_OFF
 
#define 	TCPIP_DEBUG   LWIP_DBG_OFF
 
#define 	SLIP_DEBUG   LWIP_DBG_OFF
 
#define 	DHCP_DEBUG   LWIP_DBG_OFF
 
#define 	AUTOIP_DEBUG   LWIP_DBG_OFF
 
#define 	DNS_DEBUG   LWIP_DBG_OFF
 
#define 	IP6_DEBUG   LWIP_DBG_OFF
 
#define 	DHCP6_DEBUG   LWIP_DBG_OFF

/***************************************************************************************************************/
//                                      Debugging Performance（性能测试）
/***************************************************************************************************************/
/* LWIP_PERF: Enable performance testing for lwIP (if enabled, arch/perf.h is included)  */
#define 	LWIP_PERF   0



#endif /* #ifdef USE_XXX */













/***************************************************************************************************************/
//                                      其它
/***************************************************************************************************************/


#endif /* _LWIPOPTS_H */
