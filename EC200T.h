#ifndef __EC200T_H
#define __EC200T_H
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "stm32f1xx_hal.h"

#define EC200T_RX_Buffer_Size 512
#define EC200T_Tx_Buffer_Size 128
#define EC200T_MSG_Buffer_Size 128

typedef struct 
{
	bool Answare;
	uint16_t MSG_start_index;
	uint16_t MSG_end_index;
	uint8_t label1_size;
	uint8_t label2_size;
	char *label1;
	char *label2;
	char *Massage_Buffer;
	uint16_t Message_Lenght;
	uint8_t error;
	uint16_t label2_start_index;
	uint16_t label2_end_index;
	uint16_t label1_start_index;
	uint16_t label1_end_index;
	bool IGNORE_CRLF;
}EC200_Search_t;


typedef struct {
	uint16_t start_MSG;
	uint16_t end_MSG;
	uint16_t end_Of_keywords;
	uint16_t MSG_Lenght;
}Message_Elemnts;


void EC200T_Init_Buffer(uint8_t *Tx_Buffer, volatile uint8_t *Rx_buffer,volatile uint8_t *ref_RX_BUFFER,char *Massage_Buffer);
void EC200T_Init_UART(UART_HandleTypeDef *UART,bool *Enable_Flag);
void EC200T_Recived_NUM_Byte(uint16_t *Size_recived_byte);
void UART_Flag_Init(bool *Flag);

void EC200T_Clear_Buffer_Size(uint8_t size);
void EC200T_Clear_Buffer(void);
void EC200T_Clear_Recived_Data(void);
void EC200T_Stop_Reciving_Data(void);
void EC200T_Start_Reciving_Data(void);
void EC200T_Init_Search_str(EC200_Search_t *search_obj);
uint16_t EC200T_GET_TXDATA_Size(void);
bool EC200T_UPDATA_SHADOW_Buffer(void);

void EC200T_TURN_ON_OFF_EXT(bool state);
void EC200T_TURN_POWER_OFF_software(void);
void EC200T_Send_AT(void);
void EC200T_Disable_ECHO(void);
void EC200T_Set_Contex_ID(void);
void EC200T_Set_Responseheader(void);
bool EC200T_Search_OK(void);
void EC200T_ERROR_type_Numerical(void);
void EC200T_SET_APN_config(void);
void EC200T_Active_APN(void);
void EC200T_PING_Google(void);
bool EC200T_find_MEssage_in_Buffer(char msg[],uint16_t size,uint16_t Offset);
void EC200T_Set_CFUN_to_full(void);
void EC200T_Set_CFUN_to_minimum(void);
void EC200T_DiActive_APN(void);
void EC200T_SET_URL_Size(void);
void EC200T_Send_URL(void);
void EC200T_HTTP_GET(void);
void EC200T_HTTP_READ(void);
void EC200T_SET_CREG(void);
void EC200T_Read_APN_Status(void);
void EC200T_SET_COPS_Automatic(void);
void EC200T_SET_COPS_Manually(void);

uint16_t GET_POS_MSG(void);
uint16_t extract_Data(uint16_t start_pos,uint16_t end_pos);
uint16_t search_Byte(void);
bool EC200T_find_Message_between_2lable(char *Label1,char *Label2,bool Remove_CRLF);
int EC200T_Normal_Search(void);
uint16_t EC200T_Remove_Special_Charachter(char *buffer,uint16_t size, char charachter);
#endif
