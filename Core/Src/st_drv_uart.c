//
///*
// * st_drv_uart.c
// *
// *  Created on: 29-02-2024
// *     Author: PK Jena
// *     Objective : UART Driver
// */
//
//#include <stdint.h>
//#include <string.h>
//#include <stdarg.h>
//#include <stdio.h>
//#include "main.h"
//#include "st_drv_uart.h"
//
//
//static uint8_t fg_rxBleByte;
//UART_HandleTypeDef huartBle;
//
////void st_bleUartReceive(uint8_t a_dataByte);				//receiving function from BLE chip
////static pUART_CallbackTypeDef st_uartBleRxByte(void);
////===================================================================================
////static pUART_CallbackTypeDef st_uartBleRxByte(void)
////{
////	st_bleUartReceive(fg_rxBleByte);
////	HAL_UART_Receive_IT(&huartBle, &fg_rxBleByte, 1);
////	return(0);
////}
//
//////===================================================================================
////void st_uartBleRxEnable(void)
////{
////	HAL_UART_Receive_IT(&huartBle, &fg_rxBleByte, 1);
////}
////
////extern uint8_t g_Buffer1[20];
////===================================================================================
////return: 0-Success, nonzero: failure
//uint8_t st_uartBleTransmit(uint8_t* a_pBuffer, uint16_t a_bufLen)
//{
//	HAL_StatusTypeDef l_retVal;
//
//	if(!a_bufLen) return(1);	//discard on zero length transmit
//
//	l_retVal = HAL_UART_Transmit(&huartBle, a_pBuffer, a_bufLen, 1000);
//
//	if(l_retVal != HAL_OK)
//	{
//		Error_Handler();
//	}
//
//	return(0);	//success
//}
//
////===================================================================================
//void st_uartBleInit(uint32_t a_baudRate)
//{
//	huartBle.Instance = USART1;
//	huartBle.Init.BaudRate = 115200;
//	huartBle.Init.WordLength = UART_WORDLENGTH_8B;
//	huartBle.Init.StopBits = UART_STOPBITS_1;
//	huartBle.Init.Parity = UART_PARITY_NONE;
//	huartBle.Init.Mode = UART_MODE_TX_RX;
//	huartBle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//	huartBle.Init.OverSampling = UART_OVERSAMPLING_16;
//	huartBle.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
//	huartBle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
//	if (HAL_UART_Init(&huartBle) != HAL_OK)
//	{
//		Error_Handler();
//	}
//
////	/* USER CODE BEGIN uart_Init 2 */
////	// register receiving call back
////	HAL_UART_RegisterCallback(&huartBle, HAL_UART_RX_COMPLETE_CB_ID, (pUART_CallbackTypeDef) st_uartBleRxByte);
////
////	// receiving enable
////	HAL_UART_Receive_IT(&huartBle, &fg_rxBleByte, 1);
//	HAL_UART_Receive(&huartBle, &fg_rxBleByte, 1, 1000);
////	/* USER CODE END uart_Init 2 */
//}
//
