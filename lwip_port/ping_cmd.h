#ifndef __PING_CMD_H__
#define __PING_CMD_H__

//type
#define PING_REQUEST (0x08) //ping命令请求
#define PING_REPLY   (0X00) //ping命令回送
//code
#define PING_CODE    (0x00) //ping命令code
//extend
#define PING_ID      (0xAAFF)
#define PING_SEQ     (0xAA55)

//delay
#define PING_DELAY  (1000)   //ping命令请求间隔
//debug
#define FUNC_DBG_ON     1


void pingCmdRegister( void );

#endif //__PING_CMD_H__
