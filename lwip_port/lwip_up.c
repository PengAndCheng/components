
#include "lwip_up.h"
#include <stdio.h>

#include "lwip/opt.h"
#include "lwip/tcpip.h"

#include "ping_cmd.h"
#include "ifconfig_cmd.h"

//void   tcpip_init(tcpip_init_done_fn tcpip_init_done, void *arg);

void* tcpip_init_done_cfn(void *arg){
    printf("tcpip_init_done_cfn;\n");
}

//有的把这个函数名起为lwip_tcpip_init
void lwip_up(void){
    tcpip_init(tcpip_init_done_cfn,NULL);

    mobile_network_module_register_up();//里面调用了ppp例子，串口接收处理为ppp例子的接收线程

    pingCmdRegister();
    ifconfigCmdRegister();
}

