//==================================================================================================
#ifndef _ST_BL_APP_H_
#define _ST_BL_APP_H_
//==================================================================================================
//Soc List
#define SOC_BNRG2			0x01
#define SOC_BNRG_LPS		0x02
#define SOC_WB09			0x04

//Select SoC
#define TARGET_SOC			SOC_BNRG_LPS

//flash address
#define CHIP_FLASH_BASE_ADDRESS 	0x10040000		//BlueNRG base flash address
#define FILE_SAVED_BASE_ADDRESS		0x08020000		//binary saved in L4 flash

#define AUTO_BAUD_CHAR				0x7F
#define CHUNK_FRAME_LEN			  	224

#if(TARGET_SOC == SOC_BNRG2)
	#define FILE_SIZE					(256*1024)		//for 256BK
#elif(TARGET_SOC == SOC_BNRG_LPS)
	#define FILE_SIZE					(192*1024)		//for 256BK
#elif(TARGET_SOC == SOC_WB09)
	#define FILE_SIZE					(192*1024)		//for 256BK
#endif

/* bootloader commands */
#define COMMAND_LIST_COMMANDS     (uint8_t)0x00
#define AUTO_BAUDRATE_CHAR        (uint8_t)0x7F
#define COMMAND_ERASE             (uint8_t)0x43
#define COMMAND_WRITE_MEMORY      (uint8_t)0x31
#define COMMAND_READ_MEMORY       (uint8_t)0x11
#define COMMAND_GO                (uint8_t)0x21
#define RESPONSE_ACK              (uint8_t)0x79
#define RESPONSE_NACK             (uint8_t)0x1F
#define MASS_ERASE                (uint8_t)0xFF

#define HOST_TO_BE_32(buff, val) ( 				\
    ((buff)[0] = (val>>24)), 					\
    ((buff)[1] = (uint8_t)((val>>16) &0x00FF)), \
    ((buff)[2] = (uint8_t)((val>>8) &0x00FF)), 	\
    ((buff)[3] = (uint8_t)(val &0x00FF)))

typedef enum
{
	STATE_BL_INIT,
	STATE_BL_IDLE,
	STATE_BL_MODE,
	STATE_BL_RECEIVED
}en_stateBootload;


typedef enum
{
	REQ_NONE,
	REQ_BOOT_MODE,
	REQ_COMMAND,
	REQ_ADDR_SENT,
	REQ_DATA_SENT
}en_requestType;

typedef enum
{
	NO_CRC,
	WITH_CRC
}en_crcType;

void st_bnrgBootLoader(void);
void st_bleUartReceive(uint8_t a_dataByte);
uint8_t st_getChecksum(uint8_t* a_buffer, uint16_t a_bufLen);
void st_sendToConsole(char* a_pBuffer);
uint8_t st_blSetRunMode(void);
//==================================================================================================
#endif //_ST_BL_APP_H_
//==================================================================================================

