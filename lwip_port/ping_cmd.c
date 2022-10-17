#include "lwip/raw.h" //"raw" API
#include "lwip/icmp.h" //Get ICMP structure.
#include "lwip/ip.h" //Get Information of ICMP
#include "lwip/timeouts.h" // cycle check timeout events.
#include "lwip/inet_chksum.h"//checksum.
#include "lwip/ip_addr.h"
#include "lwip/pbuf.h"      //allocate memory
#include "ping_cmd.h"
#include "lwip/arch.h"


char message[] = "This a ping test,only for REQUSET message.";
u32_t rtt_cnt;
//u8_t ping_cnt = 4 - 1;//do four times. 

ip_addr_t local_ip;
ip_addr_t dest_ip;
struct raw_pcb *ping_pcb = NULL;

extern u32_t sys_now(void);
static void PingCmd_start(struct raw_pcb *pcb);
static void timeoutHandle(void *arg);
u8_t  recv_callback(void *arg, struct raw_pcb *pcb, struct pbuf *p, ip_addr_t *addr);
void Ping_Init(ip_addr_t* destip)
{
#if FUNC_DBG_ON        
    printf("run in %s .\r\n",__FUNCTION__);
#endif     
    //IP4_ADDR(&dest_ip,192,168,1,31);
    //IP4_ADDR(&local_ip,192,168,1,10);
    extern struct netif *netif_default;
    struct netif* nd = netif_default;
    local_ip.addr = nd->ip_addr.addr;
    dest_ip.addr = destip->addr;

    /*bind local IP.*/
    printf("local IP address:%4d.%4d.%4d.%4d\r\n",ip4_addr1(&local_ip),\
                                                    ip4_addr2(&local_ip),\
                                                    ip4_addr3(&local_ip),\
                                                    ip4_addr4(&local_ip));
    /*bind dest IP.*/
    printf("dest IP address:%4d.%4d.%4d.%4d\r\n", ip4_addr1(&dest_ip),\
                                                    ip4_addr2(&dest_ip),\
                                                    ip4_addr3(&dest_ip),\
                                                    ip4_addr4(&dest_ip));    
    /*allocate a new pcb*/
    ping_pcb = raw_new(IP_PROTO_ICMP);//set IP protocol type to ICMP
    if(NULL == ping_pcb) {
        printf("get n new raw pcb failed.\r\n");
        return;
    }
    
    PingCmd_start(ping_pcb);
}


u8_t recv_callback(void *arg, struct raw_pcb *pcb, struct pbuf *p, ip_addr_t *addr)
{                   //  p->payload point the IP header/Ethernet frame data field.

    struct icmp_echo_hdr *icmp_hdr = NULL;
    struct ip_hdr *ip_hdr = NULL;
    u8_t ip_hlen = 0;
    u8_t *data_ptr = NULL;
#if FUNC_DBG_ON        
    printf("run in %s .\r\n",__FUNCTION__);
#endif     		
		(void)arg;
    if(ip_addr_cmp(&dest_ip,addr) &&\
         p->tot_len > (IP_HLEN + sizeof(struct icmp_echo_hdr)))//process packet
    {
        ip_hdr = (struct ip_hdr *)p->payload; //point to IP header
        ip_hlen = IPH_HL(ip_hdr)*4;           //1 len means 4 bytes.

        //adjust p->payload point ICMP header.
        if(pbuf_header(p,-ip_hlen))
            printf("Adjust header failed.\r\n");
        icmp_hdr = (struct icmp_echo_hdr *)p->payload;
        if(0x00 == icmp_hdr->code)
        {
            data_ptr = (u8_t*)p->payload + sizeof(struct icmp_echo_hdr);
            printf("icmp_hdr->type %d .\r\n",icmp_hdr->type);            
            switch (icmp_hdr->type)
            {
            case PING_REPLY:    //receive a reply message.
                printf("[Receive message]:%s.\r\n",data_ptr);
                printf("[ID]:%x.\r\n",icmp_hdr->id);
                printf("[SEQ Number]:%x.\r\n",icmp_hdr->seqno);
                static int count = 0;   
                count++;
                if (count < 3)
                {
                    sys_timeout(PING_DELAY,timeoutHandle,pcb);//continue to send a ping REQUEST packet.     
                }else{
                    count=0;
                }
                break;
            case PING_REQUEST:  //need ack
                printf("Receive a REQUEST message.\r\n");
                printf("[Receive message]:%s.\r\n",data_ptr);
                printf("[ID]:%x.\r\n",icmp_hdr->id);
                printf("[SEQ Number]:%x.\r\n",icmp_hdr->seqno); 

                /*change type,reset checksum*/
                ICMPH_TYPE_SET(icmp_hdr,PING_REPLY);//set type.
                /*checksum*/
                if(icmp_hdr->chksum >= htons(0xffffU - (PING_REQUEST<<8)))
                    icmp_hdr->chksum +=htons(PING_REQUEST<<8 + 1);
                else
                    icmp_hdr->chksum +=htons(PING_REQUEST<<8);
                raw_sendto(pcb,p,addr);

                break;
            
            default:
                break;
            }  
        }
        else
            printf("[ERROR]:It's not a ping format packet.\r\n");
    }
    pbuf_free(p);
    return 1;//delete pbuf.
}


static void timeoutHandle(void *arg)
{
    struct pbuf *icmp_pbuf = NULL;
    struct raw_pcb  *pcb = NULL;
    struct icmp_echo_hdr *icmp_hdr = NULL;
    u8_t *data_ptr = NULL;
    u8_t i =0;

    pcb = (struct raw_pcb*)arg;
#if FUNC_DBG_ON    
    printf("%s pcb->protocol= %d",__FUNCTION__,pcb->protocol);
#endif 
#if FUNC_DBG_ON        
    printf("run in %s .\r\n",__FUNCTION__);
#endif    
                                /* IP header(pre allocated) + payload(ICMP header + data)    */
    icmp_pbuf = pbuf_alloc(PBUF_IP,sizeof(struct icmp_echo_hdr) + sizeof(message),PBUF_POOL);//allocate memory;
    if(NULL == icmp_pbuf)
    {
        printf("pbuf_alloc() failed.\r\n");
        return;
    }
    icmp_hdr = (struct icmp_echo_hdr*)icmp_pbuf->payload; 

    //set ICMP info.
    icmp_hdr->type = PING_REQUEST;
    icmp_hdr->code = PING_CODE;
    icmp_hdr->chksum = 0x0000; //set zero.
    icmp_hdr->id = PING_ID;
    icmp_hdr->seqno = PING_SEQ;
    //add data.
    data_ptr = (u8_t*)icmp_pbuf->payload + sizeof(struct icmp_echo_hdr);
    for(i =0;i < sizeof(message);i++)
        *data_ptr++ = message[i]; //copy end of '\0'.

    icmp_hdr->chksum = inet_chksum(icmp_pbuf->payload,sizeof(struct icmp_echo_hdr) \
                                                                + sizeof(message));
    //send packet   
                                                        
    raw_sendto(pcb,icmp_pbuf,&dest_ip);
    rtt_cnt = sys_now(); //calculate time of RTT
    pbuf_free(icmp_pbuf);
}


void PingCmd_start(struct raw_pcb *pcb)
{
#if FUNC_DBG_ON        
    printf("run in %s .\r\n",__FUNCTION__);
#endif   
    raw_bind(pcb,&local_ip);//bind local IP to pcb.
  //  raw_connect(pcb,&dest_ip);
    raw_recv(pcb,recv_callback,NULL);
   // ping_cnt--; 
  sys_timeout(PING_DELAY,timeoutHandle,pcb);//set a oneshot time event for periodicly.
}





#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"

static BaseType_t pingCmd( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
    const char *pcParameter;
    BaseType_t xParameterStringLength, xReturn;
    static UBaseType_t uxParameterNumber = 0;

	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( uxParameterNumber == 0 )
	{
		sprintf( pcWriteBuffer, "enter ping cmd:\r\n" );

		uxParameterNumber = 1U;
		xReturn = pdPASS;
        return xReturn;
	}else if (uxParameterNumber == 1)
    {
		pcParameter = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							uxParameterNumber,		/* Return the next parameter. */
							&xParameterStringLength	/* Store the parameter string length. */
						);

        ip4_addr_t addr;
        ip4addr_aton(pcParameter,&addr);
        Ping_Init(&addr);

        uxParameterNumber = 0;
        xReturn = pdFALSE;
        return xReturn;
    }else{

        uxParameterNumber = 0;
        xReturn = pdFALSE;
        return xReturn;
    }
}

static const CLI_Command_Definition_t pingCmd_t =
{
	"ping",
	"\r\nping <ip>\r\n",
	pingCmd, /* The function to run. */
	1 /* Three parameters are expected, which can take any value. */
};

void pingCmdRegister( void )
{
	FreeRTOS_CLIRegisterCommand( &pingCmd_t );	
}



