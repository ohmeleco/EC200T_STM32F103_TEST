#ifndef __Machinestate_H
#define __Machinestate_H

#include "stm32f1xx_hal.h"
#include <stdbool.h>


#define MAX_VAlidate_Data 5
typedef enum Mashine_Substate{
	Substate_set_Timer,
	Substate_set_Counter,
	Substate_Refresh_Need,
	Substate_Send_CMD,
	Substate_Refresh,
	Substate_Flag_Check,
	Substate_Timeout_Check,
	Substate_Goal_Check,
	Substate_Repeat_Check,
	Substate_Repeat_Value_Check,
	Substate_Send_Error_Message,
	Substate_Reset_Parameter,
	Substate_Update_MachineState,
	Substate_Normal_Search,
	Substate_Find_Ok,
	Substate_Find_Error,
	Substate_Other_Msg,
}Mashine_Substate;

typedef enum EC200_Response_t 
{
	AT_Unknow,
	AT_Turn_On,
	AT_TURN_Off,
	AT_Disable_Echo,
	AT_CREG,
	AT_COPS_MANUAL,
	AT_COPS_AUTO,
	AT_READ_APN,
	AT_QICSGP,
	AT_QIACT,
	AT_QIDEACT,
	AT_CONTEXT_ID,
	AT_RESPONSE_HEADRE,
	AT_HTTP_URL_SZIE,
	AT_SEND_URL,
	AT_HTTP_GET,
	AT_HTTP_READ,
	AT_CFUN_Min,
	AT_CFUN_Full,
	AT_POST_PRO1,
	AT_POST_PRO2,
	AT_Turn_OFF_Delay,
}EC200_Response_t;

typedef struct {
	bool IS_Passed;
	uint32_t t_Start;
	uint32_t t_Now;
	uint32_t TDelay;
}Time_Delay;

typedef struct {
	bool  	Repeat_Active;
	uint8_t counter_value;
	uint8_t Repeat_Value;
}Counter;

typedef struct {
	EC200_Response_t State;
	EC200_Response_t Shadow_State;
	EC200_Response_t Next_State;
	EC200_Response_t Prev_State;
	Mashine_Substate Sub_State;
	Mashine_Substate Shadow_SubState;
	uint8_t Reapet;
	Counter counter;
	Time_Delay time;
	Time_Delay GTime;
	bool ISValid;
	bool Reach_Goal;
	bool RST_Flag;
	bool Reconfig;
	bool Refresh_Machine_state;
	signed char Normal_Search;
}MachineState;


typedef struct {
	uint16_t DATA_RX[MAX_VAlidate_Data];
	uint8_t  Count_Data;
	uint8_t  Count_temp;
	uint8_t  EQU_VALID;
	uint16_t temp_Value;
	uint16_t Pre_Validate_Data;
	uint16_t New_Validate_Data;
	Time_Delay time;
	bool Data_ISValie;
	bool Date_Recived;
}DATA_Validation;

//=============== Timer Function
void Setup_Time(Time_Delay *time,uint32_t Delay);
bool Timer_update (Time_Delay *time);
bool Time_Is_Passed(Time_Delay *Time);
void Timer_Set_Timer_Delay(Time_Delay *Time, uint16_t Delay);
void Timer_Reset(Time_Delay *Time);

//=============== Counter Function
void Counter_Reset(Counter *count);
void Counter_Active_Repeat(Counter *count,bool repeat_state);
void Counter_Set_Repeat_Value(Counter *count,uint8_t value);
uint8_t Counter_Get_Value(Counter *count);
bool Counter_repeat_OVF(Counter *count);


void UART_Flag_Init(bool *Flag);
bool Get_UART_Interrupt(void);	
void Clear_UART_Interrupt(void);

void Init_Machinestate(MachineState *ms);
void Slave_Machine_State(MachineState *MS,void (*AT_Fun)());
void MAchinestate_Update(MachineState *machine);
void Machinestate_Active_Slave(MachineState *machine);

void Slave_Set_Order(MachineState *MS, EC200_Response_t Order);
void Data_Validation_Init(DATA_Validation *Data,uint8_t EQU_Valid,uint32_t Time_Delay);
void Data_Validation_Assign_Data(DATA_Validation *Data,char *MSG,uint8_t Size);
bool Data_Validation_EQual(DATA_Validation *Data);
bool Data_Validation_Rich_MaxValue(DATA_Validation *Data);
void Data_Validation_Reset_Count(DATA_Validation *Data);
#endif
