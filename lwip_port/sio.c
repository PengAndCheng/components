#include "gd32f30x.h"
#include "lwip/sio.h"

#include "board_uart.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <string.h>

#include "lwip/netif.h"

#define SIO_READ_DEBUG      0
#define SIO_WRITE_DEBUG     0

//#define EC200S_REST GET_PIN(B, 1)
static int module_4G_is_init = 0;
static void module_4G_init(void){
    /* enable the led clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* configure led GPIO port */ 
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
}
void module_4G_OFF(void){
    if (module_4G_is_init==0)
    {
        module_4G_is_init=1;
        module_4G_init();
    }
    
    gpio_bit_set(GPIOB,GPIO_PIN_1);
    //rt_kprintf("PB8=%d;\n", gpio_output_bit_get(GPIOB,GPIO_PIN_1));
}

void module_4G_ON(void){
    if (module_4G_is_init==0)
    {
        module_4G_is_init=1;
        module_4G_init();
    }
    gpio_bit_reset(GPIOB,GPIO_PIN_1);
    //rt_kprintf("PB8=%d;\n", gpio_output_bit_get(GPIOB,GPIO_PIN_1));
}

void module_OFF(void){
    module_4G_OFF();
}

void module_ON(void){
    module_4G_ON();
}

static void module_reboot(void){
    module_OFF();
    vTaskDelay(1000);
    module_ON();
}



static SemaphoreHandle_t enter_ppp;
static QueueHandle_t recvQueue;
static void mobile_network_module_receive_callback_function(uint8_t* data, int datalen){
    for(int i=0; i < datalen; i++){
        xQueueSendFromISR( recvQueue, &data[i], NULL );
    }
}



typedef enum {
    at_reset,
    at_ok,
    at_cgreg,
    at_atd,
    at_enter_ppp,

    at_reboot,
    register_network_status_MAXNB,
}register_network_status_m;
static char* prtrnsm[register_network_status_MAXNB]={"at_reset","at_ok","at_cgreg","at_atd","at_enter_ppp","at_reboot"};

typedef struct{
    register_network_status_m rnsm;
    unsigned int m_reboot_count;
} module_t;
static module_t m;
void m_state_change(register_network_status_m s){
    printf("m_state_change --> %s;\n",prtrnsm[s]);
    if (s == at_reboot)
    {
        m.m_reboot_count++;
        printf("m.m_reboot_count=%d;\n",m.m_reboot_count);
    }

    if (s == at_enter_ppp)
    {
        xSemaphoreGive(enter_ppp);
        pppos_example_connect();
        printf("m_state_change give enter_ppp, pppos_example_connect;\n");
    }
    m.rnsm = s;
}

#define mobile_network_task_PRIO     5
#define mobile_network_task_SIZE      256
TaskHandle_t mobile_network_task_Handler;
void mobile_network_task(void *pvParameters)
{
    printf("mobile_network_task;\n");

    static unsigned int count=0;

    //?????????????????????
    /*
    1) ??????+++?????????????????????????????????????????????
    2) 1 ????????????+++????????????????????????????????????????????????
    3) ??????+++??? 1 ?????????????????????????????????
    */
   /*
    vTaskDelay(1100);
    char cmd[]="+++";
    uart1_send(cmd,3);
    vTaskDelay(1100);
    */
    while (1)
    {

        count=0;
        while (m.rnsm == at_reboot)
        {
            module_reboot();
            m_state_change(at_reset);
            break;
        }


        count=0;
        while (m.rnsm == at_reset)
        {
            //????????????100ms????????????
            char cmd[]="AT\r\n";
            uart1_send(cmd,sizeof(cmd)-1);
            count++;
            unsigned int delay = 100;
            int timeout_count = 0;
            vTaskDelay(100);
            if (m.m_reboot_count==0)
            {
                //?????????????????????5???????????????ok ???????????????ppp???????????????????????? ????????????
                timeout_count=1000/delay;
            }else{
                timeout_count=60000/delay;
            }
            
            if (count > timeout_count)
            {
                printf("AT timeout;\n");
                if (m.m_reboot_count==0)
                {
                    //????????????????????????PPP?????? ??????????????????
                    
                    //??????PPP????????????
                    /*
                    vTaskDelay(1100);
                    char cmd[]="+++";
                    uart1_send(cmd,3);
                    vTaskDelay(1100);
                    //????????????PPP??????
                    char cmd2[]="AT&D2\r\n";
                    uart1_send(cmd2,sizeof(cmd2)-1);
                    */
                    

                    //?????????????????????PPP??????????????????
                    //m_state_change(at_enter_ppp);

                    //????????????
                    m_state_change(at_reboot);

                    break;
                }
                
                //???????????????
                m_state_change(at_reboot);
                break;
            }
        }

        count=0;
        while (m.rnsm == at_ok)
        {
            char cmd[]="AT+CGREG?\r\n";
            uart1_send(cmd,sizeof(cmd)-1);
            count++;

            vTaskDelay(1000);
            if (count > 60)
            {
                //???????????????
                break;
            }
        }

        count=0;
        while (m.rnsm == at_cgreg)
        {
            //?????????????????????CMNET
            char cmd[]="AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n";
            uart1_send(cmd,sizeof(cmd)-1);
            count++;

            vTaskDelay(1000);
            if (count > 60)
            {
                //???????????????
                break;
            }
        }
        
        count=0;
        while (m.rnsm == at_atd)
        {
            while (1)
            {
                break;
                vTaskDelay(1000);//??????????????????PPP
                //printf("sys_now=%d;\n",sys_now());
            }
            
            //?????????????????????CMNET
            char cmd[]="ATD*99#\r\n";
            uart1_send(cmd,sizeof(cmd)-1);
            count++;

            vTaskDelay(1000);
            if (count > 60)
            {
                //???????????????
                break;
            }
        }

        if (m.rnsm == at_enter_ppp)
        {
            //??????????????????
            vTaskDelay(1000);
            
            extern struct netif *netif_default;
            struct netif* ep = netif_default;
            if(ep){
                //printf("%s    ",ep->name);
                //???????????????
                //printf("%d->ip: %d.%d.%d.%d ;\n",ep->ip_addr.addr, ep->ip_addr.addr>>24 & 0xff,ep->ip_addr.addr>>16 & 0xff,ep->ip_addr.addr>>8 & 0xff,ep->ip_addr.addr>>0 & 0xff);
            }


            //?????????????????????????????????????????? ????????????????????? ??????????????????????????????
            if (netif_default != NULL && netif_default->ip_addr.addr != 0)
            {
                extern void jmqtt_up(void);
                jmqtt_up();
            }
            

        }
        
    }
}




uint32_t mobile_network_rx(uint8_t *data, uint32_t len)
{
    while (1)
    {
        char cRxedChar;
        char lastRxChar;
        char newRxChar;
        char arrRxChar[1500];
        static int arrRxCharNb=0;

        if( xQueueReceive( recvQueue, &cRxedChar, 0xffffffff ) == pdPASS ){
            /*
            if (cRxedChar=='\r')
            {
                printf("\\r",cRxedChar);
            }else if (cRxedChar=='\n')
            {
                printf("\\n\n",cRxedChar);
            }else{
                printf("%c",cRxedChar);
            }*/
            if (m.rnsm != at_enter_ppp)
            {
                lastRxChar = newRxChar;
                newRxChar = cRxedChar;
                if (arrRxCharNb < sizeof(arrRxChar))
                {
                    arrRxChar[arrRxCharNb] = newRxChar;
                    arrRxCharNb++;
                }else{
                    //???????????? ??????????????????????????????
                    printf("arrRxCharNb >= sizof(arrRxChar);");
                    arrRxCharNb=0;
                }
                
                if (lastRxChar=='\r' && newRxChar == '\n')
                {
                    for (int i = 0; i < arrRxCharNb-2; i++)
                    {
                        printf("%c",arrRxChar[i]);
                    }
                    printf("\\r\\n\n");

                    if (m.rnsm == at_reset)
                    {
                        char features[]="OK";//??????????????????????????????????????????len=sizeof(features)-1
                        if (strncmp(arrRxChar,features,sizeof(features)-1)==0)
                        {
                            m_state_change(at_ok);
                        }
                    }else if (m.rnsm == at_ok)
                    {
                        char features1[]="+CGREG: 0,1";
                        if (strncmp(arrRxChar,features1,sizeof(features1)-1)==0)
                        {
                            //?????????????????????
                            m_state_change(at_cgreg);
                        }

                        char features2[]="+CGREG: 0,5";
                        if (strncmp(arrRxChar,features2,sizeof(features2)-1)==0)
                        {
                            //??????????????????
                            m_state_change(at_cgreg);
                        }
                    }else if (m.rnsm == at_cgreg)
                    {
                        char features[]="OK";
                        if (strncmp(arrRxChar,features,sizeof(features)-1)==0)
                        {
                            m_state_change(at_atd);
                        }
                    }else if (m.rnsm == at_atd)
                    {
                        char features[]="CONNECT";
                        if (strncmp(arrRxChar,features,sizeof(features)-1)==0)
                        {
                            m_state_change(at_enter_ppp);
                        }
                    }

                    //???????????????
                    arrRxCharNb=0;
                }
            }else if (m.rnsm == at_enter_ppp)
            {
                //???????????? ????????????ppp??????api?????????????????????
                /* ???????????????????????????????????? ????????????
                data[0]=cRxedChar;
                printf(" %02x ",cRxedChar);
                return 1;
                */

               arrRxChar[arrRxCharNb]=cRxedChar;
               arrRxCharNb++;
               if ( (arrRxCharNb != 1 && cRxedChar == 0x7e) || arrRxCharNb==len)
               {
                    #if SIO_READ_DEBUG
                    //???????????????????????????????????????
                    printf("sio_read:");
                    #endif /*#if SIO_READ_DEBUG*/
                    for (int i = 0; i < arrRxCharNb; i++)
                    {
                        data[i]=arrRxChar[i];
                        #if SIO_READ_DEBUG
                        printf(" %02x",data[i]);
                        #endif /*#if SIO_READ_DEBUG*/
                    }
                    #if SIO_READ_DEBUG
                    printf(" ;\n");
                    #endif /*#if SIO_READ_DEBUG*/

                    int ret = arrRxCharNb;
                    arrRxCharNb = 0;
                    return ret;
               }
            }
        }else{
            continue;
        }
    }
    
}

void mobile_network_module_register_up(void){
    recvQueue = xQueueCreate( 260, 1 );
    enter_ppp = xSemaphoreCreateBinary();
    
    uart1_init();
    uart1_set_receive_callback_function(mobile_network_module_receive_callback_function);
    m.rnsm = at_reset;

    xTaskCreate((TaskFunction_t )mobile_network_task,
                    (const char*    )"mnt",
                    (uint16_t       )mobile_network_task_SIZE,
                    (void*          )NULL,
                    (UBaseType_t    )mobile_network_task_PRIO,
                    (TaskHandle_t*  )&mobile_network_task_Handler);

    pppos_example_init();
}



sio_fd_t sio_open(u8_t devnum){
    printf("sio_open;\n");
    return (sio_fd_t)1;
}


u32_t sio_read(sio_fd_t fd, u8_t *data, u32_t len){
    return mobile_network_rx(data, len);
}


u32_t sio_write(sio_fd_t fd, u8_t *data, u32_t len){
    if (m.rnsm != at_enter_ppp)
    {
        //????????????????????????????????? ????????????
        return 0;
    }
    
    #if SIO_WRITE_DEBUG
    printf("sio_write:");
    for (int i = 0; i < len; i++)
    {
        printf(" %02x",data[i]);
    }
    printf(" ;\n");
    #endif /*SIO_WRITE_DEBUG*/
    uart1_send(data,len);
    return len;
}








