
/*
 * st_drv_flash.h
 *
 *  Created on		: 01-03-2024
 *      Author		: PK Jena
 */
//=================================================================================================
#ifndef _ST_DRV_FLASH_H_
#define _ST_DRV_FLASH_H_
//=================================================================================================

/* General type 4 bytes union */
typedef union
{
    uint32_t Value;
    uint8_t Byte[4];
}un_4Byte;

typedef union
{
	uint64_t m_Value;
	uint8_t  m_Byte[8];
}un_8Byte;

//void FLASH_PageErase(uint32_t PageAddress);
uint8_t st_flashErase(uint32_t a_pageStart, uint32_t a_pageCount);
uint8_t st_flashRead(uint32_t a_flashAdrs, uint8_t* a_pdataBuf, uint16_t a_dataLen);
uint8_t st_flashWrite(uint32_t a_flashAdrs, uint8_t* a_pdataBuf, uint16_t a_dataLen);

uint32_t st_flashGetPage(uint32_t Addr);
uint32_t st_flashGetBank(uint32_t Addr);
//=================================================================================================
#endif //_ST_DRV_FLASH_H_
//=================================================================================================

