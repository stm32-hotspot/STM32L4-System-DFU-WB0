
/*
 * st_drv_flash.c
 *
 *  Created on		: 01-03-2024
 *      Author		: PK Jena
 */

#include "stm32l4xx_hal.h"
#include <string.h>
#include "st_drv_flash.h"


un_4Byte fg_flashData;		//4B union data
un_8Byte fg_flash8bData;
//================================================================================================
//write to flash memory
uint8_t st_flashWrite(uint32_t a_flashAdrs, uint8_t* a_pdataBuf, uint16_t a_dataLen)
{
	uint32_t l_adrs = 0;
	HAL_StatusTypeDef l_retVal = HAL_ERROR;

	HAL_FLASH_Unlock();		//unload flash
	l_adrs = a_flashAdrs;	//assigned the start address

	for(uint32_t i = 0; i < a_dataLen;)
	{
		memcpy(fg_flash8bData.m_Byte, a_pdataBuf, 8);
		l_retVal = HAL_FLASH_Program((uint32_t) FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t) l_adrs, (uint64_t) fg_flash8bData.m_Value);

		if(l_retVal != HAL_OK)
		{
			a_dataLen = 0;				//failed, so data length written set to 0
			break;						//exit from loop
		}

		i += 8;
		l_adrs += 8;					//increment to next address by 4B
		a_pdataBuf += 8;				//increment to next address by 4B
	}

	HAL_FLASH_Lock();					//lock back the flash
	if(a_dataLen) a_dataLen = 0x01;		//to make return value in uint8_t
	return((uint8_t) a_dataLen);		//return number of bytes written
}

//====================================================
//read magic number from flash for authentication
uint8_t st_flashRead(uint32_t a_flashAdrs, uint8_t* a_pdataBuf, uint16_t a_dataLen)
{
	for(uint32_t i = 0; i < a_dataLen; i++)
	{
		*((uint8_t *) a_pdataBuf + i) = *(uint8_t *) a_flashAdrs;
		a_flashAdrs++;
	}

	if(a_dataLen) a_dataLen = 0x01;	//to make return value in uint8_t
	return((uint8_t) a_dataLen);
}

//================================================================================================
//objective   : erase pages
//a_pageStart : page number (like 0, 1, 2, 3 etc)
//a_pageCount : number of pages to be erased
uint8_t st_flashErase(uint32_t a_pageStart, uint32_t a_pageCount)
{
	uint8_t l_retVal = 0;
	uint32_t l_PageError = 0;
	uint32_t l_bankNo = 0;	//l_firstPage = 0, l_pageCount = 0,

	FLASH_EraseInitTypeDef l_eraseInitStruct;

	HAL_FLASH_Unlock();		//unlock the flash
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);	/* Clear OPTVERR bit set on virgin samples */

//	l_firstPage = st_flashGetPage(a_pageStart);					/* Get 1st Page */
//	l_pageCount = st_flashGetPage(FLASH_USER_END_ADDR) - FirstPage + 1;	/* Get the number of pages to erase from 1st page */
	l_bankNo    = st_flashGetBank(a_pageStart);					/* Get the bank */

	/* Fill EraseInit structure*/
	l_eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	l_eraseInitStruct.Banks = l_bankNo;
	l_eraseInitStruct.Page = a_pageStart;
	l_eraseInitStruct.NbPages = a_pageCount;

	if(HAL_FLASHEx_Erase(&l_eraseInitStruct, &l_PageError) != HAL_OK)
	{
		l_retVal = 0;					//failure - for return
	}
	else
	{
		if(l_PageError == 0xFFFFFFFF)	//erase succeeded
		{
			l_retVal = 1;				//success - for return
		}
		else							//erase failed
		{
			l_retVal = 0;				//failure - for return
		}
	}

	HAL_FLASH_Lock();		//lock the flash
	return(l_retVal);		//return pass/fail value
}

//=================================================================================================
/**
  * @brief  Gets the page of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The page of a given address
  */
uint32_t st_flashGetPage(uint32_t Addr)
{
	uint32_t page = 0;

	if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
	{
		/* Bank 1 */
		page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
	}
	else
	{
		/* Bank 2 */
		page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
	}

	return page;
}


