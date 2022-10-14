#include "lwip/raw.h" //"raw" API
#include "lwip/icmp.h" //Get ICMP structure.
#include "lwip/ip.h" //Get Information of ICMP
#include "lwip/timeouts.h" // cycle check timeout events.
#include "lwip/inet_chksum.h"//checksum.
#include "lwip/ip_addr.h"
#include "lwip/pbuf.h"      //allocate memory
#include "lwip/arch.h"
#include "lwip/netif.h"
#include "lwip/dns.h"

#include "ifconfig_cmd.h"





void netif_info_printf(struct netif *netif){
    printf("name   : %s;\r\n",netif->name);
    printf("ip_addr: %s;\r\n",ip4addr_ntoa(&netif->ip_addr));
    printf("netmask: %s;\r\n",ip4addr_ntoa(&netif->netmask));
    printf("gw     : %s;\r\n",ip4addr_ntoa(&netif->gw));
    printf("\r\n");
}






#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
static BaseType_t ifconfigCmd( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
    const char *pcParameter;
    BaseType_t xParameterStringLength, xReturn;
    static UBaseType_t uxParameterNumber = 0;

	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( uxParameterNumber == 0 )
	{
        extern struct netif *netif_list;
        struct netif *netif;
        int num_netifs;
        num_netifs = 0;
        for (netif = netif_list; netif != NULL; netif = netif->next) {
            num_netifs++;
            netif_info_printf(netif);
        }

        extern struct netif *netif_default;
        if (netif_default != NULL)
        {
            printf("netif_default name: %s;\r\n",netif_default->name);
            printf("\r\n");
        }

        for (int i = 0; i < DNS_MAX_SERVERS; i++)
        {
            const ip_addr_t * dns;
            dns = dns_getserver(i);
            char* str= ip4addr_ntoa(dns);
            printf("dns%d  : %s;\n",i,str);
        }

        //这个命令没有参数实现 可以执行一次退出
		uxParameterNumber = 0;
		xReturn = pdFALSE;
        return xReturn;
	}else{

        //退出
        uxParameterNumber = 0;
        xReturn = pdFALSE;
        return xReturn;
    }
}

static const CLI_Command_Definition_t ifconfigCmd_t =
{
	"ifconfig",             //命令名称
	"\r\n lwip ifconfig \r\n",     //函数名称
	ifconfigCmd,            //命令调用的函数
	0                       //参数个数
};

//需要函数调用注册，宏注册需要更改链接脚本 不便于常规移植
void ifconfigCmdRegister( void )
{
	FreeRTOS_CLIRegisterCommand( &ifconfigCmd_t );	
}



