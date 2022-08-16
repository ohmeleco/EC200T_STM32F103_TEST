#include "Machinestate.h"
#include "Relay.h"
#include "EC200T.h"
#include "Math.h"
#include "Debug.h"
//================= private Variable ===========
uint16_t Local_Var1=0,Local_Var2=0;
Mashine_Substate LocalTracked[75];
EC200_Response_t State_Tracked[127];
static bool *UART_Flag;
//================= private Variable ===========

//================= private function ===========

void Tracker_store(Mashine_Substate sub_state)
{
	LocalTracked[Local_Var2]=sub_state;
	Local_Var2++;
	if(Local_Var2>74) Local_Var2=0;
}

void Clear_Tracker(void)
{
	for(uint8_t i=0;i<75;i++)
	{
		LocalTracked[i]=0;
	}
}
void Setup_Time(Time_Delay *time,uint32_t Delay);
bool Timer_update (Time_Delay *time);
bool Time_Is_Passed(Time_Delay *Time);

void Counter_Reset(Counter *count);
void Counter_Active_Repeat(Counter *count,bool repeat_state);
uint8_t Counter_Get_Value(Counter *count);
bool Counter_repeat_OVF(Counter *count);


void UART_Flag_Init(bool *Flag);
bool Get_UART_Interrupt(void);	
void Clear_UART_Interrupt(void);

void UART_Flag_Init(bool *Flag)
{
	UART_Flag=Flag;
}

bool Get_UART_Interrupt(void)
{
	return *UART_Flag;
}
void Clear_UART_Interrupt(void)
{
	*UART_Flag=false;
}


//================== Timer =====================
void Setup_Time(Time_Delay *time,uint32_t Delay)
{
	time->IS_Passed=false;
	time->t_Start=HAL_GetTick();
	time->TDelay=Delay;
}
bool Timer_update (Time_Delay *time)
{
	time->t_Now=HAL_GetTick();
	if(time->t_Now - time->t_Start < time->TDelay)
		return false;
	else
		time->IS_Passed=true;
	return true;
}
bool Time_Is_Passed(Time_Delay *Time)
{
	return Time->IS_Passed;
}
void Timer_Set_Timer_Delay(Time_Delay *Time, uint16_t Delay)
{
	Time->TDelay=Delay;
}

void Timer_Reset(Time_Delay *Time)
{
	Time->IS_Passed=false;
	Time->t_Now=0;
}

//================== Timer =====================


//================== Counter ===================
void Counter_Reset(Counter *count)
{
	count->counter_value=0;
	count->Repeat_Value=0;
}

void Counter_Active_Repeat(Counter *count,bool repeat_state)
{
	count->Repeat_Active=repeat_state;
}


uint8_t Counter_Get_Value(Counter *count)
{
	return count->counter_value;
}

bool Counter_repeat_OVF(Counter *count)
{
	if(count->Repeat_Active)
	{
		if(count->counter_value>count->Repeat_Value) 
		{
			__nop();
			return true;
		}
		else
		{
			__nop();
			return false;
		}
	}
	else
		return false;
}
void Counter_Set_Repeat_Value(Counter *count,uint8_t value)
{
	count->Repeat_Value=value;
}
//================== Counter ===================



//================== Low levels Function =======
void SET_Reconfig_NEED(MachineState *MS)
{
	MS->Reconfig=true;
}
void RST_Reconfig_NEED(MachineState *MS)
{
	MS->Reconfig=false;
}
void Machinestate_Active_Slave(MachineState *machine)
{
	machine->ISValid=true;
	machine->Sub_State=Substate_set_Timer;
}
void MAchinestate_Update(MachineState *machine)
{
	machine->Shadow_State=machine->State;
}

void Slave_Set_Order(MachineState *MS, EC200_Response_t State)
{
	MS->State=State;
}

void Indicatore_Status(uint8_t state,uint8_t substate,bool ON)
{
	if(ON)
	{
		switch(state)
		{
			case(0):Set_Relay_On(1); break;
			case(1):Set_Relay_On(2); break;
			case(2):Set_Relay_On(3); break;
			case(3):Set_Relay_On(4); break;
			case(4):Set_Relay_On(5); break;
			case(5):Set_Relay_On(6); break;
			case(6):Set_Relay_On(7); break;
			case(7):Set_Relay_On(8); break;
			case(8):Set_Relay_On(9); break;
			case(9):Set_Relay_On(10); break;
		}
	}
	else
	{
		switch(state)
		{
			case(0):Set_Relay_Off(1); break;
			case(1):Set_Relay_Off(2); break;
			case(2):Set_Relay_Off(3); break;
			case(3):Set_Relay_Off(4); break;
			case(4):Set_Relay_Off(5); break;
			case(5):Set_Relay_Off(6); break;
			case(6):Set_Relay_Off(7); break;
			case(7):Set_Relay_Off(8); break;
			case(8):Set_Relay_Off(9); break;
			case(9):Set_Relay_Off(10); break;
		}
	}
}

void Init_Machinestate(MachineState *ms)
{
	
	ms->State=AT_Turn_On;
	ms->Shadow_State=AT_Unknow;
	ms->Sub_State=Substate_set_Timer;
	ms->ISValid=false;
	ms->Refresh_Machine_state=false;
}


//================== Low levels Function =======


//================== MID levels Function =======
void Slave_Machine_SubRutine_Display_error(MachineState *MS1)
{
	MS1->Reach_Goal=false;
	MS1->Sub_State=Substate_Reset_Parameter;	// OverWrite in some case
	switch(MS1->State)
	{
		case(AT_Turn_On)://state:0 Turn:ON/Off Module
		{
			
			EC200T_TURN_ON_OFF_EXT(1);
			//retry
			MS1->Next_State=AT_Unknow;
			//MS1->Sub_State=Substate_set_Timer;
			break;
		}
		case(AT_Disable_Echo):
		{
			__nop();
			break;
		}
		case(AT_CONTEXT_ID):
		{
			Debug((uint8_t*)"ContextID substate8-(error)\r\n",strlen("ContextID substate8-(error)\r\n"));
			MS1->Next_State=AT_QIDEACT;
			break;
		}
		case(AT_RESPONSE_HEADRE):
		{
			Debug((uint8_t*)"ContextID substate8-(error)\r\n",strlen("ContextID substate8-(error)\r\n"));
			MS1->Next_State=AT_QIDEACT;
			break;
		}
		case(AT_READ_APN)://Send: AT_QIACT?
		{
			Debug((uint8_t*)"AT_READ_APN substate8-(error)\r\n",strlen("AT_READ_APN substate8-(error)\r\n"));
			MS1->Next_State=AT_CFUN_Min;
			__nop();
			break;
		}
		case(AT_QICSGP):// Send: APN Config
		{
			Debug((uint8_t*)"AT_QICSGP substate8-(error)\r\n",strlen("AT_QICSGP substate8-(error)\r\n"));
			MS1->Next_State=AT_CFUN_Min;
			__nop();
			break;
		}
		case(AT_QIACT)://Send: AT_QIACT=1
		{
			Debug((uint8_t*)"AT_QIACT=1 substate8-(error)\r\n",strlen("AT_QIACT=1 substate8-(error)\r\n"));
			MS1->Next_State=AT_QIDEACT;	//AT_QIDEACT
			__nop();
			break;
		}
		case(AT_HTTP_URL_SZIE):
		{
			__nop();
			break;
		}
		case(AT_SEND_URL):
		{
			__nop();
			break;
		}
		case(AT_HTTP_GET):
		{
			Debug((uint8_t*)"AT_HTTP_GET substate8-(error)\r\n",strlen("AT_HTTP_GET substate8-(error)\r\n"));
			MS1->Next_State=AT_CFUN_Min;//AT_CFUN=0
			break;
		}
		case(AT_HTTP_READ):
		{
			MS1->Sub_State=Substate_Reset_Parameter;
			MS1->Next_State=AT_CFUN_Min;//AT_CFUN=0
			break;
		}
		case(AT_CREG)://Send: AT_CREG
		{
			__nop();
			break;
		}
		case(AT_QIDEACT)://Send: AT_QIDEACT=1
		{
			Debug((uint8_t*)"AT_QIDEACT substate8-(error)\r\n",strlen("AT_QIDEACT substate8-(error)\r\n"));
			MS1->Next_State=AT_CFUN_Min;	//AT_CFUN=0
			__nop();
			break;
		}
		case(AT_COPS_AUTO):
		{
			break;
		}
		case(AT_COPS_MANUAL):
		{
			break;
		}
		case(AT_CFUN_Min):
		{
			break;
		}
		case(AT_CFUN_Full):
		{
			break;
		}
		case(AT_Unknow):
		{
			break;
		}
		case(AT_TURN_Off):
		{
			
			break;
		}
		default:
		{
			break;
		}
	}// end switch
}

void Slave_Machine_SubRutine_State13_error(MachineState *MS1)
{
	MS1->Sub_State=Substate_Goal_Check;
	MS1->Reach_Goal=false;
	switch(MS1->State)
	{
		case(AT_Turn_On)://state:0 Turn:ON/Off Module
		{
			__nop();
			break;
		}
		case(AT_Disable_Echo):
		{
			__nop();
			break;
		}
		case(AT_CONTEXT_ID):
		{
			__nop();
			HAL_Delay(2000);
			Debug((uint8_t*)"ContextID substate13-(error)\r\n",strlen("ContextID substate13-(error)\r\n"));
			break;
		}
		case(AT_RESPONSE_HEADRE):
		{
			HAL_Delay(2000);
			Debug((uint8_t*)"Response Header substate13-(error)\r\n",strlen("Response Header substate13-(error)\r\n"));
			__nop();
			break;
		}
		case(AT_READ_APN)://Send: AT_QIACT?
		{
			Debug((uint8_t*)"AT_READ_APN substate13-(error)\r\n",strlen("AT_READ_APN substate13-(error)\r\n"));
			__nop();
			break;
		}
		case(AT_QICSGP):// Send: APN Config
		{
			Debug((uint8_t*)"AT_QICSGP substate13-(error)\r\n",strlen("AT_QICSGP substate13-(error)\r\n"));
			__nop();
			break;
		}
		case(AT_QIACT)://Send: AT_QIACT=1
		{
			Debug((uint8_t*)"AT_QIACT substate13-(error)\r\n",strlen("AT_QIACT substate13-(error)\r\n"));
			__nop();
			break;
		}
		case(AT_CREG)://Send: AT_CREG
		{
			__nop();
			break;
		}
		case(AT_QIDEACT)://Send: AT_QIDEACT=1
		{
			Debug((uint8_t*)"AT_QIDEACT substate13-(error)\r\n",strlen("AT_QIDEACT substate13-(error)\r\n"));
			__nop();
			break;
		}
		case(AT_HTTP_URL_SZIE):
		{
			Debug((uint8_t*)"AT_HTTP_URL_SZIE substate13-(error)\r\n",strlen("AT_HTTP_URL_SZIE substate13-(error)\r\n"));
			__nop();
			break;
		}
		case(AT_SEND_URL):
		{
			Debug((uint8_t*)"AT_SEND_URL substate13-(error)\r\n",strlen("AT_SEND_URL substate13-(error)\r\n"));
			__nop();
			break;
		}
		case(AT_HTTP_GET):
		{	
			Debug((uint8_t*)"AT_HTTP_GET substate13-(error)\r\n",strlen("AT_HTTP_GET substate13-(error)\r\n"));
			break;
		}
		case(AT_HTTP_READ):
		{
			Debug((uint8_t*)"AT_HTTP_READ substate13-(error)\r\n",strlen("AT_HTTP_READ substate13-(error)\r\n"));
			HAL_Delay(2000);
			break;
		}
		case(AT_COPS_AUTO):
		{
			break;
		}
		case(AT_COPS_MANUAL):
		{
			break;
		}
		case(AT_CFUN_Min):
		{
			Debug((uint8_t*)"AT_CFUN_Min substate13-(error)\r\n",strlen("AT_CFUN_Min substate13-(error)\r\n"));
			__nop();
			break;
		}
		case(AT_CFUN_Full):
		{
			Debug((uint8_t*)"AT_CFUN_Full substate13-(error)\r\n",strlen("AT_CFUN_Full substate13-(error)\r\n"));
			__nop();
			break;
		}
		case(AT_Unknow):
		{
			__nop();
			break;
		}
		case(AT_TURN_Off):
		{
			__nop();
			break;
		}
		default:
		{
			break;
		}
	}// end switch
}



void Slave_Machine_SubRutine_State12_OK(MachineState *MS1)
{
	switch(MS1->State)
	{
		case(AT_Turn_On)://state:0 Turn:ON/Off Module
		{
			
			MS1->Sub_State=Substate_Goal_Check;
			MS1->Reach_Goal=true;
			MS1->Next_State=AT_Disable_Echo;
			__nop();
			break;
		}
		case(AT_Disable_Echo):
		{
			MS1->Sub_State=Substate_Goal_Check;
			MS1->Reach_Goal=true;
			MS1->Next_State=AT_CONTEXT_ID;
			__nop();
			break;
		}
		case(AT_CONTEXT_ID):
		{
			MS1->Sub_State=Substate_Goal_Check;
			MS1->Reach_Goal=true;
			MS1->Next_State=AT_RESPONSE_HEADRE;
			__nop();
			break;
		}
		case(AT_RESPONSE_HEADRE):
		{
			MS1->Sub_State=Substate_Goal_Check;
			MS1->Reach_Goal=true;
			MS1->Next_State=AT_READ_APN;
			__nop();
			break;
		}
		case(AT_CREG)://Send: AT_CREG
		{
//			MS1->Sub_State=5;
//			MS1->Reach_Goal=true;
//			MS1->Next_State=3;
			__nop();
			break;
		}
		case(AT_READ_APN)://Send: AT_QIACT?
		{
			MS1->Sub_State=Substate_Goal_Check;
			MS1->Reach_Goal=true;
			MS1->Next_State=AT_QICSGP;
			if(MS1->Prev_State == AT_RESPONSE_HEADRE)
  		{
				if(EC200T_find_MEssage_in_Buffer("+QIACT:",sizeof("QIACT:"),0)) //MCU is reset but Module work pretty well and APN is Active
					MS1->Next_State=AT_HTTP_URL_SZIE;
			}
//			if(MS1->State == 6)
//				MS1->Next_State=7;
//			__nop();
			break;
		}
		case(AT_QICSGP):// Send: APN Config
		{
			__nop();
			MS1->Sub_State=Substate_Goal_Check;
			MS1->Reach_Goal=true;
			MS1->Next_State=AT_QIACT;	//AT_QIACT=1
			break;
		}
		case(AT_QIACT)://Send: AT_QIACT=1
		{
			MS1->Sub_State=Substate_Goal_Check;
			MS1->Reach_Goal=true;
			MS1->Next_State=AT_HTTP_URL_SZIE;
			__nop();
			break;
		}
		case(AT_QIDEACT)://Send: AT_QIDEACT=1
		{
			MS1->Sub_State=Substate_Goal_Check;
			MS1->Reach_Goal=true;
			MS1->Next_State=AT_CFUN_Min;//CFUN=0
			Debug((uint8_t*)"AT_QIDEACT-->OK\r\n",strlen("AT_QIDEACT-->OK\r\n"));
			if(MS1->Prev_State==AT_POST_PRO1)
			{
				Debug((uint8_t*)"AT_POST_PRO1-->AT_QIDEACT\r\n",strlen("AT_POST_PRO1-->AT_QIDEACT\r\n"));
				MS1->Next_State=AT_READ_APN;
			}
			__nop();
			break;
		}
		case(AT_HTTP_URL_SZIE):
		{
			MS1->Sub_State=Substate_Goal_Check;
			MS1->Reach_Goal=true;
			MS1->Next_State=AT_SEND_URL;
			__nop();
			break;
		}
		case(AT_SEND_URL):
		{
			MS1->Sub_State=Substate_Goal_Check;
			MS1->Reach_Goal=true;
			MS1->Next_State=AT_HTTP_GET;
			__nop();
			break;
		}
		case(AT_HTTP_GET):
		{
			Debug((uint8_t*)"AT_HTTP_GET-->OK\r\n",strlen("AT_HTTP_GET-->OK\r\n"));
			MS1->Sub_State=Substate_Goal_Check;
			if(EC200T_find_MEssage_in_Buffer("+QHTTPGET",sizeof("+QHTTPGET"),0)) // we recived successfully 
			{
				Debug((uint8_t*)"AT_HTTP_GET-->QHTTPGET\r\n",strlen("AT_HTTP_GET-->QHTTPGET\r\n"));
				MS1->Next_State=AT_HTTP_READ;
				MS1->Reach_Goal=true;
			}
			else
			{
				MS1->Reach_Goal=false;
				MS1->Refresh_Machine_state=true;
			}
			//MS1->Next_State=12;
			break;
		}
		case(AT_HTTP_READ):
		{
			Debug((uint8_t*)"AT_HTTP_READ-->OK\r\n",strlen("AT_HTTP_READ-->OK\r\n"));
			MS1->Sub_State=Substate_Goal_Check;
			if(EC200T_find_MEssage_in_Buffer("+QHTTPREAD",sizeof("+QHTTPREAD"),0)) // we READ successfully 
			{
				Debug((uint8_t*)"AT_HTTP_READ-->QHTTPREAD\r\n",strlen("AT_HTTP_READ-->QHTTPREAD\r\n"));
				MS1->Next_State=AT_POST_PRO1;
				MS1->Reach_Goal=true;
			}
			else
			{
				MS1->Refresh_Machine_state=true;
				MS1->Reach_Goal=false;
			}
			
			//MS1->Next_State=13;
			break;
		}
		case(AT_COPS_AUTO):
		{
			break;
		}
		case(AT_COPS_MANUAL):
		{
			break;
		}
		case(AT_CFUN_Min)://Send: AT_CFUN=0
		{
			MS1->Sub_State=Substate_Goal_Check;
			MS1->Reach_Goal=true;
			MS1->Next_State=AT_CFUN_Full;
			__nop();
			break;
		}
		case(AT_CFUN_Full)://Send AT_CFUN=1
		{
			MS1->Sub_State=Substate_Goal_Check;
			MS1->Reach_Goal=true;
			MS1->Next_State=AT_READ_APN;
			__nop();
			break;
		}
		case(AT_Unknow):
		{
			break;
		}
		case(AT_TURN_Off):
		{
			__nop();
			break;
		}
		default:
		{
			break;
		}
	}// end switch
}


void Slave_Machine_SubRutine_State14_OTher(MachineState *MS1)
{
	MS1->Sub_State=Substate_Goal_Check;
	switch(MS1->State)
	{
		case(AT_Turn_On)://state:0 Turn:ON/Off Module
		{
			if(EC200T_find_MEssage_in_Buffer("PB DONE",sizeof("PB DONE"),0)) // Module is turned on
			{
				Debug((uint8_t*)"AT_TURN_ON_EXT we recived PB Done\r\n",strlen("AT_TURN_ON_EXT we recived PB Done\r\n"));
				MS1->Next_State=AT_Disable_Echo;
				MS1->Reach_Goal=true;
			}
			else
			{
				MS1->Sub_State=Substate_set_Timer;
				MS1->Refresh_Machine_state=true;
			}
			break;
		}
		case(AT_Disable_Echo):
		{
			if(EC200T_find_MEssage_in_Buffer("E0",sizeof("E0"),0)) //Disable ECHO
			{
				MS1->Next_State=AT_CONTEXT_ID;
				MS1->Reach_Goal=true;
			}
			__nop();
			break;
		}
		case(AT_CONTEXT_ID):
		{
			__nop();
			break;
		}
		case(AT_RESPONSE_HEADRE):
		{
			__nop();
			break;
		}
		case(AT_CREG)://Send: AT_CREG
		{
			__nop();
			break;
		}
		case(AT_READ_APN)://Send: AT_QIACT?
		{
			if(EC200T_find_MEssage_in_Buffer("+QIACT:",sizeof("QIACT:"),0)) // APN is Active
			{
				MS1->Reach_Goal=true;
				MS1->Next_State=AT_HTTP_URL_SZIE;
			}
			__nop();
			break;
		}
		case(AT_QICSGP):// Send: APN Config
		{
			__nop();
			break;
		}
		case(AT_QIACT)://Send: AT_QIACT=1
		{
			__nop();
			break;
		}
		case(AT_QIDEACT)://Send: AT_QIDEACT=1
		{
			__nop();
			break;
		}
		case(AT_HTTP_URL_SZIE):
		{
			if(EC200T_find_MEssage_in_Buffer("CONNECT",sizeof("CONNECT"),0)) // URL Size set successfully
			{
				MS1->Reach_Goal=true;
				MS1->Next_State=AT_SEND_URL;
			}
			break;
		}
		case(AT_SEND_URL):
		{
			__nop();
			break;
		}
		case(AT_HTTP_GET):
		{
			__nop();
			if(EC200T_find_MEssage_in_Buffer("+QHTTPGET",sizeof("+QHTTPGET"),0)) // we recived successfully 
			{
				Debug((uint8_t*)"AT_HTTP_GET-->QHTTPGET",strlen("AT_HTTP_GET-->QHTTPGET"));
				MS1->Reach_Goal=true;
				MS1->Next_State=AT_HTTP_READ;
			}
			break;
		}
		case(AT_HTTP_READ):
		{
			__nop();
			if(EC200T_find_MEssage_in_Buffer("+QHTTPREAD",sizeof("+QHTTPREAD"),0)) // we READ successfully 
			{
				Debug((uint8_t*)"AT_HTTP_READ-->QHTTPREAD\r\n",strlen("AT_HTTP_READ-->QHTTPREAD\r\n"));
				MS1->Reach_Goal=true;
				MS1->Next_State=AT_POST_PRO1;
			}
			break;
		}
		case(AT_COPS_AUTO):
		{
			__nop();
			break;
		}
		case(AT_COPS_MANUAL):
		{
			__nop();
			break;
		}
		case(AT_CFUN_Min):
		{
			__nop();
			if(EC200T_find_MEssage_in_Buffer("+QIURC:",sizeof("+QIURC:"),0)) // Set Module to Minimum Function
			{
				MS1->Reach_Goal=true;
				MS1->Next_State=AT_TURN_Off;
			}
			break;
		}
		case(AT_CFUN_Full):
		{
			__nop();
			if(EC200T_find_MEssage_in_Buffer("PB DONE",sizeof("PB DONE"),0)) // Set Module to Minimum Function
			{
				MS1->Reach_Goal=true;
				MS1->Next_State=AT_READ_APN;
			}
			break;
		}
		case(AT_Unknow):
		{
			break;
		}
		case(AT_TURN_Off):
		{
			Debug((uint8_t*)"Software-->Off substate14-(Other)\r\n",strlen("Software-->Off substate14-(Other)\r\n"));
			__nop();
			if(EC200T_find_MEssage_in_Buffer("POWERED DOWN",strlen("+POWERED DOWN"),0)) // we recived successfully 
			{
				Debug((uint8_t*)"state18-->acivate\r\n",strlen("state18-->acivate\r\n"));
				MS1->Reach_Goal=true;
				MS1->Next_State=AT_Turn_OFF_Delay;
				HAL_Delay(2000);
			}
			break;
		}
		default:
		{
			break;
		}
	}// end switch
}


void Slave_Machine_State(MachineState *MS,void (*AT_Fun)())
{
	uint8_t i;
	switch(MS->Sub_State)
	{
		case(Substate_set_Timer)://set timer
		{
			Tracker_store(MS->Sub_State);
			Setup_Time(&MS->time,MS->time.TDelay);
			MS->Sub_State=Substate_set_Counter;
			break;
		}
		case(Substate_set_Counter)://set counter
		{
			Tracker_store(MS->Sub_State);
			MS->counter.counter_value++;
			MS->Sub_State=Substate_Refresh_Need;
			break;
		}
		case(Substate_Refresh_Need):
		{
			Tracker_store(MS->Sub_State);
			if(MS->Refresh_Machine_state)
				MS->Sub_State=Substate_Refresh;
			else
				MS->Sub_State=Substate_Send_CMD;
			break;
		}
		case(Substate_Send_CMD)://send AT Command
		{
			Tracker_store(MS->Sub_State);
			
			MS->Sub_State=Substate_Flag_Check;
			EC200T_Clear_Buffer();	// clear buffer 
			Clear_UART_Interrupt();	// clear UART Interrupt
			EC200T_Start_Reciving_Data();
			AT_Fun();
			break;
		}
		case(Substate_Refresh):
		{
			Debug((uint8_t*)"Substate_Refresh\r\n",strlen("Substate_Refresh\r\n"));
			MS->Sub_State=Substate_Flag_Check;
			//EC200T_Clear_Buffer();	// clear buffer 
			Clear_UART_Interrupt();	// clear UART Interrupt
			EC200T_Start_Reciving_Data();
			*UART_Flag=EC200T_UPDATA_SHADOW_Buffer();// update UART FLAG if buffer updated
			break;
		}
		case(Substate_Flag_Check)://check Interrupt Flag
		{
			
			if(Get_UART_Interrupt())
			{
				Debug((uint8_t*)"We recived Interrupt\r\n",strlen("We recived Interrupt\r\n"));
				EC200T_Stop_Reciving_Data();
				Clear_UART_Interrupt();
				Tracker_store(MS->Sub_State);
				MS->Sub_State=Substate_Normal_Search;
				// data process
			}
			else
			{
				MS->Sub_State=Substate_Timeout_Check;
			}
			break;
		}
		case(Substate_Timeout_Check):// check TimeOut
		{
			if(Timer_update(&MS->time)) // Time out Happent
			{
				__NOP();
				Tracker_store(MS->Sub_State);
				MS->Sub_State=Substate_Goal_Check;
				MS->Refresh_Machine_state=false;
			}
			else
			{
				MS->Sub_State=Substate_Flag_Check;
			}
			break;
		}
		case(Substate_Goal_Check):// cheack :Reach Goal
		{
			Tracker_store(MS->Sub_State);
			if(MS->Reach_Goal) // Reach Goal
			{
				// update MS
				MS->Sub_State=Substate_Send_Error_Message;
				
				__nop();
			}
			else
				MS->Sub_State=Substate_Repeat_Check;
			break;
		}
		case(Substate_Repeat_Check):// check Repeat Active
		{
			Tracker_store(MS->Sub_State);
			if(MS->counter.Repeat_Active) // repeat mode is Active
				MS->Sub_State=Substate_Repeat_Value_Check;
			else
				MS->Sub_State=Substate_Send_Error_Message;
			break;
		}
		case(Substate_Repeat_Value_Check):// check Repeat Value
		{
			Tracker_store(MS->Sub_State);
			if(Counter_repeat_OVF(&MS->counter)) // counter value > Reapeat Value
				MS->Sub_State=Substate_Send_Error_Message;
			else
				MS->Sub_State=Substate_set_Timer;
			break;
		}
		case(Substate_Send_Error_Message):// Display Message/Error(8)
		{
			Tracker_store(MS->Sub_State);
			if(MS->Reach_Goal)// we riched to our Goal --> display successful massege
			{
				//Indicatore_Status(MS->State,MS->Sub_State,true);
				__NOP();
				MS->Sub_State=Substate_Reset_Parameter;
			}
			else // we have error do something and --> display Erro massage
			{
				Slave_Machine_SubRutine_Display_error(MS);
			}
			break;
		}
		case(Substate_Reset_Parameter):// reset parameter
		{
			Tracker_store(MS->Sub_State);
			Counter_Reset(&MS->counter);
			Timer_Reset(&MS->time);
			EC200T_Clear_Recived_Data();
			MS->Sub_State=Substate_Update_MachineState;
			Clear_Tracker();
			MS->Refresh_Machine_state=false;
			MS->Reach_Goal=false;
			
			break;
		}
		case(Substate_Update_MachineState):// exit the substate
		{
			Tracker_store(MS->Sub_State);
			EC200T_Start_Reciving_Data();
			Local_Var2=0;
			State_Tracked[Local_Var1]=MS->State;
			Local_Var1++;
			if(Local_Var1>= sizeof(State_Tracked)) Local_Var1=0; // reset counter
			
			MS->Prev_State=MS->State;	//update previwe state
			MS->State=MS->Next_State; // update_state
			Setup_Time(&MS->GTime,MS->GTime.TDelay);
			MS->ISValid=false;
			break;
		}
		case(Substate_Normal_Search):// Normal Search
		{
			Tracker_store(MS->Sub_State);
			MS->Normal_Search=EC200T_Normal_Search();
			switch(MS->Normal_Search)
			{
				case(2): MS->Sub_State=Substate_Other_Msg;    	break;
				case(1): MS->Sub_State=Substate_Find_Ok;    		break;
				case(-1):MS->Sub_State=Substate_Find_Error; 		break;
				default:
					 MS->Sub_State=Substate_Other_Msg;
				break;
			}
			break;
		}
		case(Substate_Find_Ok):// we find recieved OK 
		{
			Tracker_store(MS->Sub_State);
			Slave_Machine_SubRutine_State12_OK(MS);
			break;
		}
		case(Substate_Find_Error)://we recived Error MSG from Module
		{
			Slave_Machine_SubRutine_State13_error(MS);
			Tracker_store(MS->Sub_State);
			MS->Reach_Goal=false;
			EC200T_Clear_Recived_Data();
			break;
		}
		case(Substate_Other_Msg)://Message Other Mode
		{
			Tracker_store(MS->Sub_State);
			Slave_Machine_SubRutine_State14_OTher(MS);
			break;
		}
	}
}







//================== MID levels Function =======


//================== Data Validation Function ===
void Data_Validation_Init(DATA_Validation *Data,uint8_t EQU_Valid,uint32_t Time_Delay)
{
	Data->Count_Data=0;
	Data->Data_ISValie=false;
	if(EQU_Valid>MAX_VAlidate_Data)	
		Data->EQU_VALID=MAX_VAlidate_Data;
	else
		Data->EQU_VALID=EQU_Valid;
	
	Data->Pre_Validate_Data=0;
	Data->New_Validate_Data=0;
	Data->temp_Value=0;
	Data->Count_temp=0;
	Setup_Time(&Data->time,Time_Delay);
}

void Data_Validation_Reset_Count(DATA_Validation *Data)
{
	Data->Count_Data=0;
	Data->Data_ISValie=false;
}
bool Data_Validation_Rich_MaxValue(DATA_Validation *Data)
{
	if(Data->Count_Data >= MAX_VAlidate_Data)
		return true;
	else
		return false;
}
void Data_Validation_Assign_Data(DATA_Validation *Data,char *MSG,uint8_t Size)
{
	uint8_t i;
	uint16_t data=0;
	Data->Data_ISValie=true;
	for(i=0;i<Size;i++)
	{
		if(!(MSG[i]>='0' && MSG[i]<='9'))	// check data --> if data is'nt number than break the loop;  
		{
				Data->Data_ISValie=false;
				break;
		}
		else
		{
			data+=(MSG[i]-'0')*pow(10,Size-i-1);
		}
	}
	if(Data->Data_ISValie)
	{
		Data->DATA_RX[Data->Count_Data]=data;
		//================= new section=======
		if(Data->temp_Value == data) Data->Count_temp++;
		//================= new section=======
		//
		//Data->Count_Data++;
	}
	
}



bool Data_Validation_EQual(DATA_Validation *Data)
{
	uint8_t index[MAX_VAlidate_Data];//stored repeat sorted data
	uint8_t i,j,counter;
	uint16_t dummy1;
	//=================================
	for(i=0;i<MAX_VAlidate_Data;i++)
	{
		dummy1=Data->DATA_RX[i];
		counter=0;
		for(j=0;j<MAX_VAlidate_Data;j++)
		{
			if(dummy1==Data->DATA_RX[j])
				counter++;
		}
		index[i]=counter;
	}
	//================================ sort section
	for(i=0;i<MAX_VAlidate_Data;i++)
	{
		for(j=0;j<MAX_VAlidate_Data-1;j++)
		{
			if(index[j+1] > index[j])
			{
				dummy1=index[j];
				index[j]=index[j+1];
				index[j+1]=dummy1;
				
				dummy1=Data->DATA_RX[j];
				Data->DATA_RX[j]=Data->DATA_RX[j+1];
				Data->DATA_RX[j+1]=dummy1;
			}
		}
	}
	//================================ sort section
	if(index[0]>=Data->EQU_VALID)// if max repeat value > EQU_Valid 
	{
		Data->New_Validate_Data=Data->DATA_RX[0];
		if(Data->New_Validate_Data!=Data->Pre_Validate_Data)
		{
			Data->Date_Recived=true;
			Data->Pre_Validate_Data=Data->New_Validate_Data;
		}
		else
			Data->Date_Recived=false;
	}
	//================= new section=======
	for(i=0;i<MAX_VAlidate_Data;i++)
	{
		if(Data->DATA_RX[i]!=Data->New_Validate_Data)
		{
			Data->Count_temp=index[i];
			Data->temp_Value=Data->DATA_RX[i];
			break;
		}
	}
	//================= new section=======
	return Data->Date_Recived;
}

//================== Data Validation Function ===

