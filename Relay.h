#ifndef __RELAY_H
#define __RELAY_H
#include "stm32f1xx_hal.h"
void Set_Relay_On(unsigned int relay_Num);
void Set_Relay_Off(unsigned int relay_Num);
void Test_Relay(void);
void SET_Relay_State(uint16_t value);
#endif 
