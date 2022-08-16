#include "EC200T.h"
#include "main.h"
#include <math.h>
static uint8_t *tx_buffer;
static volatile uint8_t *rx_buffer;// shadow RX_buffer
static volatile uint8_t *ref_rx_buffer; // ref rx_buffer
static uint16_t tx_Data_Size=0;
static uint16_t *Recived_byte;
static uint16_t address_Ok=0;
static bool *Enable_UART_Flag;
UART_HandleTypeDef *uart;
static EC200_Search_t *Search_Obj;
static char *MSG_buffer;
uint16_t find_start,find_end;
uint8_t URL_GET_ADDRESS[64]="https://www.digix.ir/api/devices/zones/state/1";

void EC200T_Init_Buffer(uint8_t *Tx_Buffer, volatile uint8_t *Rx_buffer,volatile uint8_t *ref_RX_BUFFER,char *Massage_Buffer)
{
	tx_buffer=Tx_Buffer;
	rx_buffer=Rx_buffer;
	ref_rx_buffer=ref_RX_BUFFER;
	MSG_buffer=Massage_Buffer;
}

void EC200T_Init_UART(UART_HandleTypeDef *UART,bool *Enable_Flag)
{
	uart=UART;
	Enable_UART_Flag=Enable_Flag;
}

void EC200T_Stop_Reciving_Data(void)
{
	*Enable_UART_Flag=false;
}

void EC200T_Start_Reciving_Data(void)
{
	*Enable_UART_Flag=true;
}

bool EC200T_UPDATA_SHADOW_Buffer(void)
{
	if(memcmp((uint8_t*)rx_buffer,(uint8_t*)ref_rx_buffer,*Recived_byte))
	{
		memcpy((uint8_t*)rx_buffer,(uint8_t*)ref_rx_buffer,*Recived_byte);
		return true;
	}
	else
		return false;
	
}
void EC200T_Recived_NUM_Byte(uint16_t *Size_recived_byte)
{
	Recived_byte=Size_recived_byte;
}
uint16_t EC200T_GET_TXDATA_Size(void)
{
	return tx_Data_Size;
}
void EC200T_TURN_ON_OFF_EXT(bool state)
{
	if(state)
	{
		HAL_GPIO_WritePin(ON_OFF_EC200T_GPIO_Port,ON_OFF_EC200T_Pin,GPIO_PIN_SET);
		HAL_Delay(1000);
		HAL_GPIO_WritePin(ON_OFF_EC200T_GPIO_Port,ON_OFF_EC200T_Pin,GPIO_PIN_RESET);
		HAL_Delay(1000);
		//HAL_GPIO_WritePin(ON_OFF_EC200T_GPIO_Port,ON_OFF_EC200T_Pin,GPIO_PIN_SET);
		HAL_Delay(20000);
	}
	else
	{
		HAL_GPIO_WritePin(ON_OFF_EC200T_GPIO_Port,ON_OFF_EC200T_Pin,GPIO_PIN_RESET);
		HAL_Delay(1000);
		HAL_GPIO_WritePin(ON_OFF_EC200T_GPIO_Port,ON_OFF_EC200T_Pin,GPIO_PIN_SET);
		HAL_Delay(4000);
	}
}

void EC200T_Clear_Buffer_Size(uint8_t size)
{
	for(uint16_t i=0;i<size;i++)
	{
		rx_buffer[i]=0;	
	}
}
void EC200T_Init_Search_str(EC200_Search_t *search_obj)
{
	Search_Obj=search_obj;
	Search_Obj->Massage_Buffer=MSG_buffer;
}

void EC200T_Clear_Buffer(void)
{
	for(uint16_t i=0;i<EC200T_RX_Buffer_Size;i++)
	rx_buffer[i]=0;		
}

void EC200T_Clear_Recived_Data(void)
{
	EC200T_Clear_Buffer_Size(*Recived_byte);
}

void EC200T_Send_AT(void)
{
	tx_Data_Size=sizeof("AT\r\n")-1;
	memcpy(tx_buffer,"AT\r\n",tx_Data_Size);
	HAL_UART_Transmit_IT(uart,tx_buffer,tx_Data_Size);
}

void EC200T_Disable_ECHO(void)
{
	char cmd[]="ATE0\r\n";
	tx_Data_Size=sizeof(cmd)-1;
	strcpy((char*)tx_buffer,cmd);
	HAL_UART_Transmit_IT(uart,tx_buffer,tx_Data_Size);
}

void EC200T_Set_Contex_ID(void)
{
	char cmd[]="AT+QHTTPCFG=\"contextid\",1\r\n";
	tx_Data_Size=sizeof(cmd)-1;
	strcpy((char*)tx_buffer,cmd);
	HAL_UART_Transmit_IT(uart,tx_buffer,tx_Data_Size);
}

void EC200T_Set_Responseheader(void)
{
	char cmd[]="AT+QHTTPCFG=\"responseheader\",1\r\n";
	tx_Data_Size=sizeof(cmd)-1;
	strcpy((char*)tx_buffer,cmd);
	HAL_UART_Transmit_IT(uart,tx_buffer,tx_Data_Size);
}



void EC200T_ERROR_type_Numerical(void)
{
	char cmd[]="AT+CMEE=1\r\n";
	tx_Data_Size=sizeof(cmd)-1;
	strcpy((char*)tx_buffer,cmd);
	HAL_UART_Transmit_IT(uart,tx_buffer,tx_Data_Size);
}
void EC200T_SET_APN_config(void)
{
	char cmd[]="AT+QICSGP=1,1,\"P012E01\",\"\",\"\"1\r\n";
	tx_Data_Size=sizeof(cmd)-1;
	strcpy((char*)tx_buffer,cmd);
	HAL_UART_Transmit_IT(uart,tx_buffer,tx_Data_Size);
}

void EC200T_Active_APN(void)
{
	char cmd[]="AT+QIACT=1\r\n";
	tx_Data_Size=sizeof(cmd)-1;
	strcpy((char*)tx_buffer,cmd);
	HAL_UART_Transmit_IT(uart,tx_buffer,tx_Data_Size);
}

void EC200T_DiActive_APN(void)
{
	char cmd[]="AT+QIDEACT=1\r\n";
	tx_Data_Size=sizeof(cmd)-1;
	strcpy((char*)tx_buffer,cmd);
	HAL_UART_Transmit_IT(uart,tx_buffer,tx_Data_Size);
}
void EC200T_Set_CFUN_to_minimum(void)
{
	char cmd[]="AT+CFUN=0\r\n";
	tx_Data_Size=sizeof(cmd)-1;
	strcpy((char*)tx_buffer,cmd);
	HAL_UART_Transmit_IT(uart,tx_buffer,tx_Data_Size);	
}
void EC200T_Set_CFUN_to_full(void)
{
	char cmd[]="AT+CFUN=1\r\n";
	tx_Data_Size=sizeof(cmd)-1;
	strcpy((char*)tx_buffer,cmd);
	HAL_UART_Transmit_IT(uart,tx_buffer,tx_Data_Size);	
}

void EC200T_PING_Google(void)
{
	char cmd[]="AT+QPING=1,WWW.GOOGLE.COM\r\n";
	tx_Data_Size=sizeof(cmd)-1;
	strcpy((char*)tx_buffer,cmd);
	HAL_UART_Transmit_IT(uart,tx_buffer,tx_Data_Size);

}
void EC200T_SET_COPS(void)
{
	char cmd[]="AT+COPS=0\r\n";
	tx_Data_Size=sizeof(cmd)-1;
	strcpy((char*)tx_buffer,cmd);
	HAL_UART_Transmit_IT(uart,tx_buffer,tx_Data_Size);
}
void EC200T_SET_URL_Size(void)
{
	char cmd[]="AT+QHTTPURL=46,80\r\n";
	tx_Data_Size=sizeof(cmd)-1;
	strcpy((char*)tx_buffer,cmd);
	HAL_UART_Transmit_IT(uart,tx_buffer,tx_Data_Size);
}

void EC200T_TURN_POWER_OFF_software(void)
{
	char cmd[]="AT+QPOWD=1\r\n";
	tx_Data_Size=sizeof(cmd)-1;
	strcpy((char*)tx_buffer,cmd);
	HAL_UART_Transmit_IT(uart,tx_buffer,tx_Data_Size);	
}
void EC200T_Send_URL(void)
{
	tx_Data_Size=sizeof(URL_GET_ADDRESS)-1;
	strcpy((char*)tx_buffer,(char*)URL_GET_ADDRESS);
	HAL_UART_Transmit_IT(uart,tx_buffer,tx_Data_Size);
}
void EC200T_HTTP_GET(void)
{
	char cmd[]="AT+QHTTPGET=80\r\n";
	tx_Data_Size=sizeof(cmd)-1;
	strcpy((char*)tx_buffer,cmd);
	HAL_UART_Transmit_IT(uart,tx_buffer,tx_Data_Size);
}
void EC200T_HTTP_READ(void)
{
	char cmd[]="AT+QHTTPREAD=80\r\n";
	tx_Data_Size=sizeof(cmd)-1;
	strcpy((char*)tx_buffer,cmd);
	HAL_UART_Transmit_IT(uart,tx_buffer,tx_Data_Size);
}

void EC200T_SET_CREG(void)
{
	char cmd[]="AT+CREG=0\r\n";
	tx_Data_Size=sizeof(cmd)-1;
	strcpy((char*)tx_buffer,cmd);
	HAL_UART_Transmit_IT(uart,tx_buffer,tx_Data_Size);
}
void EC200T_Read_APN_Status(void)
{
	char cmd[]="AT+QIACT?\r\n";
	tx_Data_Size=sizeof(cmd)-1;
	strcpy((char*)tx_buffer,cmd);
	HAL_UART_Transmit_IT(uart,tx_buffer,tx_Data_Size);
}


void EC200T_SET_COPS_Manually(void)
{
	char cmd[]="AT+COPS=2\r\n";
	tx_Data_Size=sizeof(cmd)-1;
	strcpy((char*)tx_buffer,cmd);
	HAL_UART_Transmit_IT(uart,tx_buffer,tx_Data_Size);
}
void EC200T_SET_COPS_Automatic(void)
{
	char cmd[]="AT+COPS=0\r\n";
	tx_Data_Size=sizeof(cmd)-1;
	strcpy((char*)tx_buffer,cmd);
	HAL_UART_Transmit_IT(uart,tx_buffer,tx_Data_Size);
}
uint16_t GET_Pos_END_find_MEssage(void)
{
	uint16_t i;
	for(i=0;i<EC200T_RX_Buffer_Size-1;i++)
	{
		if(rx_buffer[i]=='O' && rx_buffer[i+1]=='K')
			break;
	}
	address_Ok=i;
	return i;
}



bool EC200T_Search_OK(void)
{
	uint16_t i;
	bool ans=false;
	for(i=0;i<EC200T_RX_Buffer_Size-1;i++)
	{
		if(rx_buffer[i]=='O' && rx_buffer[i+1]=='K')
		{
			ans=true;
			break;
		}
	}
	return ans;
}

bool EC200T_find_MEssage_in_Buffer(char msg[],uint16_t size,uint16_t Offset)
{
uint16_t i=0;
uint16_t j,count=0;
size--;
	if(Offset>=EC200T_RX_Buffer_Size) 
		return false;
	for(i=Offset;i<EC200T_RX_Buffer_Size-size;i++)
	{
		if(msg[0]==rx_buffer[i])
		{
			for(j=0;j<size;j++)
				{
					if(msg[j]==rx_buffer[i+j])
						count++;
					else
					{
						count=0;
						break;
					}
				}
				if(count==size)
				{
					__nop();
					break;
				}
		}
	}
	if(count==size)
	{
		Search_Obj->MSG_start_index=i;
		Search_Obj->MSG_end_index=i+size;
		Search_Obj->Answare=true;
		return true;
	}
	else
	{
		Search_Obj->Answare=false;
		return false;
	}
}
bool EC200T_find_Message_between_2lable(char *Label1,char *Label2,bool Remove_CRLF)
{
	uint16_t i=0,j=0;
	Search_Obj->label1=Label1;
	Search_Obj->label2=Label2;
	Search_Obj->label1_size=strlen(Label1);
	Search_Obj->label2_size=strlen(Label2);
	Search_Obj->Answare=EC200T_find_MEssage_in_Buffer(Label1,Search_Obj->label1_size,0);
	if(Search_Obj->Answare)
	{
		Search_Obj->label1_start_index=Search_Obj->MSG_start_index;
		Search_Obj->label1_end_index=Search_Obj->MSG_end_index;
		Search_Obj->Answare=EC200T_find_MEssage_in_Buffer(Label2,Search_Obj->label2_size,Search_Obj->MSG_end_index);
		if(Search_Obj->Answare)
		{
			Search_Obj->Answare=true;
			Search_Obj->label2_start_index=Search_Obj->MSG_start_index;
			Search_Obj->label2_end_index=Search_Obj->MSG_end_index;
			Search_Obj->Message_Lenght=(Search_Obj->label2_start_index) - Search_Obj->label1_end_index-1;
			if(Remove_CRLF)
			{
				Search_Obj->IGNORE_CRLF=Remove_CRLF;
				for(i=Search_Obj->label1_end_index+1;i<Search_Obj->label2_start_index;i++)
				{
					if(rx_buffer[i]!=13 && rx_buffer[i+1]!=10)
						{
							MSG_buffer[j]=rx_buffer[i];
							j++;
						}
						else i++;
				}
				Search_Obj->Message_Lenght=j;
			}
			else
				memcpy(MSG_buffer,(uint8_t*)&rx_buffer[Search_Obj->label1_end_index+1],Search_Obj->Message_Lenght);
			return true;
		}
	}
	return false;
}

int EC200T_Normal_Search(void)
{
	uint16_t i;
	char ok[]="OK";
	char error[]="ERROR";
	for(i=0;i<EC200T_RX_Buffer_Size-sizeof(error)-1;i++)
	{
		if(ok[0]==rx_buffer[i] && ok[1]==rx_buffer[i+1])
			return 1;
		if(error[0]==rx_buffer[i] && error[1]==rx_buffer[i+1]&& error[2]==rx_buffer[i+2]&& error[3]==rx_buffer[i+3])
			return -1;
	}
	return 2;
}



uint16_t EC200T_Remove_Special_Charachter(char *buffer,uint16_t size, char charachter)
{
	uint8_t dummy[size];
	uint16_t j=0;
	for(uint16_t i=0;i<size;i++)
	{
		if(buffer[i]!=charachter) 
		{
			dummy[j]=buffer[i];
			j++;
		}
	}
	memcpy(buffer,dummy,size);
	return j;
}

uint16_t GET_POS_MSG(void)
{
	return find_start; 
}



uint16_t search_Byte(void)
{
	uint16_t i;
	for(i=0;i<EC200T_RX_Buffer_Size-4;i++)
	{
		if(rx_buffer[i]=='b' && rx_buffer[i+1]=='y'&& rx_buffer[i+2]=='t'&& rx_buffer[i+3]=='e'&& rx_buffer[i+4]=='s')
			break;
	}
	return i+4+2;
}

uint16_t extract_Data(uint16_t start_pos,uint16_t end_pos)
{
	uint8_t data[end_pos-start_pos];
	int i=0,j=0,k=0;
	uint16_t state=0;
	for(i=start_pos;i<end_pos;i++)
	{
		if((rx_buffer[i]!=13 && rx_buffer[i]!=10 )&& (rx_buffer[i]>=48 && rx_buffer[i]<=57))
		{
			data[j]=rx_buffer[i]-'0';
			j++;
		}
	}
	for(i=j-1;i>=0;i--)
	{
		state+=data[k]*pow(10,i);
		k++;
	}
	return state;
}
