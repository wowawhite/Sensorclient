/*
 * helperfunctions.c
 *
 *  Created on: 05.02.2023
 *      Author: wowa
 */

#include "helperfunctions.h"
/**
  * @brief  Fills buffer with user predefined data.
  * @param  pBuffer: pointer on the buffer to fill
  * @param  uwBufferLenght: size of the buffer to fill
  * @param  uwOffset: first value to fill on the buffer
  * @retval None
  */

static void Fill_Buffer(uint32_t *pBuffer, uint32_t uwBufferLength, uint16_t uwOffset)
{
	uint16_t tmpIndex = 0;
	for (tmpIndex = 0; tmpIndex < uwBufferLength; tmpIndex++ )
	{
	pBuffer[tmpIndex] = tmpIndex + uwOffset;
	}
}

