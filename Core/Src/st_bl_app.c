
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "main.h"
#include "st_bl_app.h"
#include "st_drv_flash.h"

uint8_t  g_flagBootload;
uint8_t  fg_ackChar;
uint16_t fg_pktCount;
uint32_t fg_fileAddress;
uint32_t fg_byteCount;
uint8_t fg_txBuffer[256];

extern UART_HandleTypeDef huart1, huart2;

static uint8_t st_waitForACK(void);
static uint8_t st_blSetBootMode(void);
static uint8_t st_blMassErase(void);
static uint8_t st_flashTheChip(void);
static void st_sendToChip(uint8_t* a_pBuffer, uint16_t a_dataLen, uint8_t a_withCRC);

//bootloader state machine
void st_bnrgBootLoader(void)
{
	uint8_t l_retVal = 0;

	st_sendToConsole("Bootloader Started\n");

	//actvating bootloader of chip with RESET and BOOT pin sequence
	l_retVal = st_blSetBootMode();
	if(!l_retVal)
	{
		st_sendToConsole("Boot loader activated\n");
	}
	else
	{
		st_sendToConsole("Boot loader activation FAILED\n");
		return;
	}

	//erasing the chip using mass erase command
	l_retVal = st_blMassErase();
	if(!l_retVal)
	{
		st_sendToConsole("Mass erase done.\n");
	}
	else
	{
		st_sendToConsole("Mass erase FAILED\n");
		return;
	}

	st_sendToConsole("Start flashing the chip.\n");

	//transferring new binary from L4 Flash --> Chip
	l_retVal = st_flashTheChip();
	if(!l_retVal)
	{
		st_sendToConsole("Chip flashed successfully.\n");
	}
	else
	{
		memset(fg_txBuffer, 0, sizeof(fg_txBuffer));
		sprintf((char*) fg_txBuffer, "Chip flashing FAILED [%d]\n", l_retVal);
		st_sendToConsole((char *) fg_txBuffer);
		return;
	}
}

/*
 * Flashing the chip. Reading BNRG/LPS binary from flash of L4 and
 * writes to BNRG/LPS. This binary is written to flash prio to the
 * process. Flashed to L4 @ FILE_SAVED_BASE_ADDRESS
 */
static uint8_t st_flashTheChip(void)
{
	uint8_t l_stars = 0;
	fg_fileAddress = 0;
	fg_pktCount = 0;

	for (fg_pktCount = 0; fg_pktCount < (FILE_SIZE / CHUNK_FRAME_LEN); fg_pktCount++)
	{
		memset(fg_txBuffer, 0, sizeof(fg_txBuffer));

		//writing the command
		fg_txBuffer[0] = COMMAND_WRITE_MEMORY;
		st_sendToChip(fg_txBuffer, 1, WITH_CRC);
		fg_ackChar = st_waitForACK();
		if (fg_ackChar != RESPONSE_ACK)
			return 1;

		//writing address
		fg_fileAddress = CHIP_FLASH_BASE_ADDRESS + 224 * fg_pktCount;
		HOST_TO_BE_32 (fg_txBuffer, fg_fileAddress);
		st_sendToChip(fg_txBuffer, 4, WITH_CRC);
		fg_ackChar = st_waitForACK();
		if (fg_ackChar != RESPONSE_ACK)
		  return 2;

		//writing binary chunk
		fg_txBuffer[0] = CHUNK_FRAME_LEN - 1;
		st_flashRead(FILE_SAVED_BASE_ADDRESS + fg_pktCount * CHUNK_FRAME_LEN, &fg_txBuffer[1], CHUNK_FRAME_LEN);
		st_sendToChip(fg_txBuffer, CHUNK_FRAME_LEN + 1, WITH_CRC);
		fg_ackChar = st_waitForACK();
		if (fg_ackChar != RESPONSE_ACK)
		  return 3;

		//writing "*" to console to indicate writing is in progress.
		st_sendToConsole("*"); l_stars++;
		if((l_stars) && (!(l_stars % 50)))
		{
			l_stars = 0;
			st_sendToConsole("\n");
		}

		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);		//toggling the LED
	}

	//if any residual of firmware remained
	if(FILE_SIZE % CHUNK_FRAME_LEN)
	{
		//jena try to implement inside abobe loop
	}


	memset(fg_txBuffer, 0, sizeof(fg_txBuffer));
	sprintf((char*) fg_txBuffer, "\nTotal packets sent = %d\n", fg_pktCount);
	st_sendToConsole((char*) fg_txBuffer);
	return(0);
}

/*
 * Function: Mass erase the chip. This is required to be done before flashing.
 */
static uint8_t st_blMassErase(void)
{
	//writing command
	fg_txBuffer[0] = COMMAND_ERASE;
	st_sendToChip(fg_txBuffer, 1, WITH_CRC);
	fg_ackChar = st_waitForACK();
	if (fg_ackChar != RESPONSE_ACK)
		return 0xFF;

	//writing mass erase flag
	fg_txBuffer[0] = 0xFF;
	st_sendToChip(fg_txBuffer, 1, WITH_CRC);
	fg_ackChar = st_waitForACK();
	if (fg_ackChar != RESPONSE_ACK)
		return 0xFF;

	return 0;		//on success
}

/*
 * Thius function brings the chip into normal mode (running mode).
 */
uint8_t st_blSetRunMode(void)
{
	HAL_GPIO_WritePin(BLE_RESET_GPIO_Port, BLE_RESET_Pin, 0);	//set low
	HAL_GPIO_WritePin(BLE_BOOT_GPIO_Port, BLE_BOOT_Pin, 0);		//set low
	HAL_Delay(250);
	HAL_GPIO_WritePin(BLE_RESET_GPIO_Port, BLE_RESET_Pin, 1);	//set high
	HAL_Delay(250);
	return(0);		//success
}

/*
 * This function brings the chip into bootloader mode.
 * Returns 0: on success and NON-ZERO on failure
 */
static uint8_t st_blSetBootMode(void)
{
	fg_ackChar = st_waitForACK();								//read to clear if any rx char pending
	HAL_GPIO_WritePin(BLE_RESET_GPIO_Port, BLE_RESET_Pin, 0);	//set low
	HAL_GPIO_WritePin(BLE_BOOT_GPIO_Port, BLE_BOOT_Pin, 1);		//set high
	HAL_Delay(100);
	HAL_GPIO_WritePin(BLE_RESET_GPIO_Port, BLE_RESET_Pin, 1);	//set high
	HAL_Delay(100);

	//auto baud character
	fg_txBuffer[0] = AUTO_BAUD_CHAR;
	st_sendToChip(fg_txBuffer, 1, 0);
	fg_ackChar = st_waitForACK();
	return(fg_ackChar == RESPONSE_ACK ? 0x00 : 0x01);		//00: on success, non-zero on failure
}

/*
 * This function will wait for ACK byte from UART (chip side).
 * Waiting Timeout is 1000ms
 * Function returns the character received, 0 if timeout occurred
 */
static uint8_t st_waitForACK(void)
{
	uint8_t l_rxChar = 0;

	HAL_UART_Receive(&huart1, &l_rxChar, 1, 1000);
	return(l_rxChar);
}

/* user-button press triggers bootloader process */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	g_flagBootload = 3;		//flag is set
}

/*
 * Calculation of CRC for bootloader protocol. It is single byte.
 * It is XOR byte of the payload, so that when XOR of whole received
 * buffer gets calculated, it results 0.
 */
uint8_t st_getChecksum(uint8_t* a_buffer, uint16_t a_bufLen)
{
    uint8_t l_checksum = 0;

    if(a_bufLen > 1)
    {
        for(uint16_t i = 0; i < a_bufLen; i++)
        {
        	l_checksum = l_checksum ^ a_buffer[i];
        }
    }
    else
    {
    	l_checksum = ~a_buffer[0];
    }

    return(l_checksum);
}

/*
 * This function will write text to console.
 * a_pBuffer: text to write. Max 80 bytes can be written
 */
void st_sendToConsole(char* a_pBuffer)
{
	uint8_t l_len = 0;
	uint8_t l_txData[80];

	memset(l_txData, 0, sizeof(l_txData));
	strcpy((char*) l_txData, a_pBuffer);
	l_len = strlen((char*) l_txData);
	if(l_len > 80) l_len = 80;
	HAL_UART_Transmit(&huart2, l_txData, l_len, 1000);
}

/*
 * Function sends data to chip. It may be command, address or data
 * a_pBuffer: buffer pointer
 * a_dataLen: number of bytes to be sent
 * a_withCRC: if set an additional CRC byte calculated and send
 */
static void st_sendToChip(uint8_t* a_pBuffer, uint16_t a_dataLen, uint8_t a_withCRC)
{
	uint8_t l_crcByte = 0;

	HAL_UART_Transmit(&huart1, a_pBuffer, a_dataLen, 1000);

	if(a_withCRC)
	{
		l_crcByte = st_getChecksum(a_pBuffer, a_dataLen);
		HAL_UART_Transmit(&huart1, &l_crcByte, 1, 1000);
	}
}
