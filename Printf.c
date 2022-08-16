#include "Debug.h"
#include <stdio.h>

static UART_HandleTypeDef  *debuger;


void debug_init(UART_HandleTypeDef *UART)
{
	debuger=UART;
}

void Debug(uint8_t *massege,uint16_t size)
{
	HAL_UART_Transmit(debuger,massege,size,1000);
}


