
#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"

#include "lwip/opt.h"
#include "ping.h"


static BaseType_t pingCmd( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
    const char *pcParameter;
    BaseType_t xParameterStringLength, xReturn;
    static UBaseType_t uxParameterNumber = 0;

	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );
    pcWriteBuffer[0]=0;

	if( uxParameterNumber == 0 )
	{
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
        printf("ping_init start:\r\n");
        ping_init(&addr);
        

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



