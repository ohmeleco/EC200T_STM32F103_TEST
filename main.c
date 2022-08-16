/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "Relay.h"
#include "EC200T.h"
#include "Machinestate.h"
#include "Debug.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

//============================== Interrupu and Hardware Variable =================================
volatile uint8_t RX_Buffer[EC200T_RX_Buffer_Size],RX_Buffer_Shadow[EC200T_RX_Buffer_Size];
char Message_Buffer[EC200T_MSG_Buffer_Size];
uint8_t TX_Buffer[EC200T_Tx_Buffer_Size];
volatile uint8_t RX_Byte;
volatile uint16_t RX_Counter=0,RX_Counter_Shadow=0;
volatile bool UART_Interrupt=false,EC200T_Interrupt=false;;
bool Interrupt_Recived=false,Enable_Recived_Data=true;
volatile bool Timer_State=false;
volatile bool Buffer_Overflow=false;
EC200_Search_t search;
MachineState MS1;
void (*AT_FUN)(void);
DATA_Validation EC200T_data_t;
//============================== Interrupu and Hardware Variable =================================
//============================== RS485 Interrupu and Hardware Variable ===========================
volatile bool RS485_UART_Interrupt=false,RS485_Interrupt=false,RS485_TX_CMD=false;
volatile bool RS485_Overflow=false;
volatile uint16_t RS485_Shadow_Byte_Counter=0,RS485_Byte_Counter=0;
volatile uint8_t RS485_buffer[EC200T_RX_Buffer_Size],Shadow_R485_buffer[EC200T_RX_Buffer_Size];
volatile uint8_t RX_Buffer_RS485=0;

//char s[7]="->025\r\n";
//volatile uint8_t CP=0,CP1=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	//----------------------------- Echo (RS485)
	HAL_UART_Receive_IT(&huart3,(uint8_t*)&RX_Buffer_RS485,1);
	debug_init(&huart3);
	//-----------------------------  Init Software/Hardware 
	EC200T_Init_Buffer(TX_Buffer,RX_Buffer_Shadow,RX_Buffer,Message_Buffer);
	EC200T_Init_UART(&huart1,&Enable_Recived_Data);
	EC200T_Recived_NUM_Byte((uint16_t*)&RX_Counter_Shadow);
	UART_Flag_Init(&Interrupt_Recived);
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_Delay(10);
	HAL_UART_Receive_IT(&huart1,(uint8_t*)&RX_Byte,1);
	EC200T_Init_Search_str(&search);
	HAL_Delay(10);
	Data_Validation_Init(&EC200T_data_t,4,3000);
	Data_Validation_Assign_Data(&EC200T_data_t,search.Massage_Buffer,search.Message_Lenght);
	//-----------------------------  Init Software/Hardware 
	//Init_Machinestate(&MS1);
	uint8_t count=0;	// just for display variable
	char s[11];
//	while(!__HAL_TIM_GET_FLAG(&htim1,TIM_FLAG_CC1))
//	{
//		__NOP();
//	}
		EC200T_TURN_ON_OFF_EXT(1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		//======================= EC200T TX Interrupt 
		if(RS485_TX_CMD)
		{
			//ECHO mode
			Debug((uint8_t*)TX_Buffer,EC200T_GET_TXDATA_Size());
			RS485_TX_CMD=false;
		}
		//======================= EC200T TX Interrupt 

		//======================= RS485 RX Interrupt 
		if(RS485_UART_Interrupt)
		{
			RS485_UART_Interrupt=false;
			if(RS485_Overflow)
			{
				RS485_Overflow=false;
				__nop();
				HAL_Delay(10);
			}
			memcpy((uint8_t*)Shadow_R485_buffer,(uint8_t*)RS485_buffer,RS485_Shadow_Byte_Counter);
			//echo
			Debug((uint8_t*)Shadow_R485_buffer,RS485_Shadow_Byte_Counter);
			//HAL_UART_Transmit(&huart3,(uint8_t*)Shadow_R485_buffer,RS485_Shadow_Byte_Counter,500);//echo
			HAL_UART_Transmit(&huart1,(uint8_t*)Shadow_R485_buffer,RS485_Shadow_Byte_Counter,500); // send to EC200T
		}
		//======================= RS485 RX Interrupt 
		
		//======================= EC200T RX Interrupt 
		if(UART_Interrupt)
		{
			RX_Counter_Shadow=RX_Counter;
			RX_Counter=0;
			if(Enable_Recived_Data)
			{
				memcpy((uint8_t*)RX_Buffer_Shadow,(uint8_t*)RX_Buffer,RX_Counter_Shadow);
				HAL_UART_Transmit(&huart3,(uint8_t*)RX_Buffer,RX_Counter_Shadow,1000); // send to RS485
				Interrupt_Recived=true;
			}
			
			UART_Interrupt=false;
//			sprintf(s,"->%03d",RX_Counter_Shadow);
			Debug((uint8_t*)"End-Massege\r\n",strlen("End-Massege\r\n"));
		}
		//======================= EC200T RX Interrupt 
		
		if(MS1.State !=MS1.Shadow_State)
		{
			
			switch(MS1.State)
			{
				case(AT_Unknow):// other mode (retry section)
				{
					MAchinestate_Update(&MS1);
					MS1.State=MS1.Prev_State;
					MS1.Prev_State=AT_Unknow;
					break;
				}
				case(AT_Turn_On):				// Turn on module
				{
					Timer_Set_Timer_Delay(&MS1.time,5000);
					Counter_Active_Repeat(&MS1.counter,true);
					Counter_Set_Repeat_Value(&MS1.counter,3);
					MAchinestate_Update(&MS1);
					Machinestate_Active_Slave(&MS1);
					AT_FUN=EC200T_Send_AT;
					Slave_Set_Order(&MS1, AT_Turn_On);
					__nop();
					break;
				}
				case(AT_Disable_Echo):	// Disable ECHO
				{
					if(Timer_update(&MS1.GTime))
					{
						Timer_Set_Timer_Delay(&MS1.time,4000);
						Timer_Set_Timer_Delay(&MS1.GTime,1000); // setup Gtimer
						Counter_Active_Repeat(&MS1.counter,true);
						Counter_Set_Repeat_Value(&MS1.counter,3);
						MAchinestate_Update(&MS1);
						Machinestate_Active_Slave(&MS1);
						AT_FUN=EC200T_Disable_ECHO;
						Slave_Set_Order(&MS1, AT_Disable_Echo);
						__nop();
					}
					break;
				}
				case(AT_CONTEXT_ID):		// Set ContexID
				{
					if(Timer_update(&MS1.GTime))
					{
						Timer_Set_Timer_Delay(&MS1.time,3000);
						Timer_Set_Timer_Delay(&MS1.GTime,2000); // setup Gtimer
						Counter_Active_Repeat(&MS1.counter,true);
						Counter_Set_Repeat_Value(&MS1.counter,2);
						MAchinestate_Update(&MS1);
						Machinestate_Active_Slave(&MS1);
						AT_FUN=EC200T_Set_Contex_ID;
						Slave_Set_Order(&MS1, AT_CONTEXT_ID);
						__nop();
					}
					break;
				}
				case(AT_RESPONSE_HEADRE):// Set Response Header
				{
					if(Timer_update(&MS1.GTime))
					{
						Timer_Set_Timer_Delay(&MS1.time,3500);
						Timer_Set_Timer_Delay(&MS1.GTime,2000); // setup Gtimer
						Counter_Active_Repeat(&MS1.counter,true);
						Counter_Set_Repeat_Value(&MS1.counter,3);
						MAchinestate_Update(&MS1);
						Machinestate_Active_Slave(&MS1);
						AT_FUN=EC200T_Set_Responseheader;
						Slave_Set_Order(&MS1, AT_RESPONSE_HEADRE);
						__nop();
					}
					break;
				}
				case(AT_READ_APN):			// Read APN config
				{
					if(Timer_update(&MS1.GTime))// read APN Status
					{
						Timer_Set_Timer_Delay(&MS1.time,50000);
						Timer_Set_Timer_Delay(&MS1.GTime,3000); // setup Gtimer
						Counter_Active_Repeat(&MS1.counter,false);// --> repeat is Disable
						Counter_Set_Repeat_Value(&MS1.counter,3);
						MAchinestate_Update(&MS1);
						Machinestate_Active_Slave(&MS1);
						AT_FUN=EC200T_Read_APN_Status;
						Slave_Set_Order(&MS1, AT_READ_APN);
						__nop();
					}
					break;
				}
				case(AT_QICSGP):				// Set APN Config
				{
					if(Timer_update(&MS1.GTime))
					{
						Timer_Set_Timer_Delay(&MS1.time,5000);
						Timer_Set_Timer_Delay(&MS1.GTime,3000); // setup Gtimer
						Counter_Active_Repeat(&MS1.counter,false);
						Counter_Set_Repeat_Value(&MS1.counter,3);
						MAchinestate_Update(&MS1);
						Machinestate_Active_Slave(&MS1);
						AT_FUN=EC200T_SET_APN_config;
						Slave_Set_Order(&MS1, AT_QICSGP);
						__nop();
					}
					break;
				}
				case(AT_QIACT):				// Active APN
				{
					if(Timer_update(&MS1.GTime))
					{
						Timer_Set_Timer_Delay(&MS1.time,50000);
						Timer_Set_Timer_Delay(&MS1.GTime,5000); // setup Gtimer
						Counter_Active_Repeat(&MS1.counter,false);
						Counter_Set_Repeat_Value(&MS1.counter,3);
						MAchinestate_Update(&MS1);
						Machinestate_Active_Slave(&MS1);
						AT_FUN=EC200T_Active_APN;
						Slave_Set_Order(&MS1, AT_QIACT);
						__nop(); // 0)--> return:OK  1)-->return:ERROR  
					}
					break;
				}
				case(AT_HTTP_URL_SZIE):// Set URL Size
				{
						if(Timer_update(&MS1.GTime))
						{
							Timer_Set_Timer_Delay(&MS1.time,2000);
							Timer_Set_Timer_Delay(&MS1.GTime,1000); // setup Gtimer
							Counter_Active_Repeat(&MS1.counter,false);
							Counter_Set_Repeat_Value(&MS1.counter,3);
							MAchinestate_Update(&MS1);
							Machinestate_Active_Slave(&MS1);
							AT_FUN=EC200T_SET_URL_Size;
							Slave_Set_Order(&MS1, AT_HTTP_URL_SZIE);
							__nop();
						}
					break;
				}
				case(AT_SEND_URL):		//Set URL
				{
						if(Timer_update(&MS1.GTime))
						{
							Timer_Set_Timer_Delay(&MS1.time,2000);
							Timer_Set_Timer_Delay(&MS1.GTime,2000); // setup Gtimer
							Counter_Active_Repeat(&MS1.counter,false);
							Counter_Set_Repeat_Value(&MS1.counter,3);
							MAchinestate_Update(&MS1);
							Machinestate_Active_Slave(&MS1);
							AT_FUN=EC200T_Send_URL;
							Slave_Set_Order(&MS1, AT_SEND_URL);
							__nop();
						}
					break;
				}
				case(AT_HTTP_GET):	//HTTP GET
				{
					if(Timer_update(&MS1.GTime))
					{
						Timer_Set_Timer_Delay(&MS1.time,20000);
						Timer_Set_Timer_Delay(&MS1.GTime,6000); // setup Gtimer
						Counter_Active_Repeat(&MS1.counter,true);
						Counter_Set_Repeat_Value(&MS1.counter,3);
						MAchinestate_Update(&MS1);
						Machinestate_Active_Slave(&MS1);
						AT_FUN=EC200T_HTTP_GET;
						Slave_Set_Order(&MS1, AT_HTTP_GET);
						__nop();
						}// 0) --> return:OK 1)-->return: QHTTPGET 2)-->return:ERROR
					break;
				}
				case(AT_HTTP_READ):	//HTTP Read
				{
						if(Timer_update(&MS1.GTime))
						{
							Timer_Set_Timer_Delay(&MS1.time,5000);
							Timer_Set_Timer_Delay(&MS1.GTime,4500); // setup Gtimer
							Counter_Active_Repeat(&MS1.counter,true);
							Counter_Set_Repeat_Value(&MS1.counter,3);
							MAchinestate_Update(&MS1);
							Machinestate_Active_Slave(&MS1);
							AT_FUN=EC200T_HTTP_READ;
							Slave_Set_Order(&MS1, AT_HTTP_READ);
							__nop();
						}
					break;
				}
				case(AT_QIDEACT):		//QIDEACT
				{
						if(Timer_update(&MS1.GTime))
						{
							Timer_Set_Timer_Delay(&MS1.time,5000);
							Timer_Set_Timer_Delay(&MS1.GTime,10000); // setup Gtimer
							Counter_Active_Repeat(&MS1.counter,false);
							Counter_Set_Repeat_Value(&MS1.counter,3);
							MAchinestate_Update(&MS1);
							Machinestate_Active_Slave(&MS1);
							AT_FUN=EC200T_DiActive_APN;
							Slave_Set_Order(&MS1, AT_QIDEACT);
							__nop();
						}
					break;
				}
				case(AT_CFUN_Min):	//AT_CFUN_Min
				{
						if(Timer_update(&MS1.GTime))
						{
							Timer_Set_Timer_Delay(&MS1.time,5000);
							Timer_Set_Timer_Delay(&MS1.GTime,4000); // setup Gtimer
							Counter_Active_Repeat(&MS1.counter,false);
							Counter_Set_Repeat_Value(&MS1.counter,3);
							MAchinestate_Update(&MS1);
							Machinestate_Active_Slave(&MS1);
							AT_FUN=EC200T_Set_CFUN_to_minimum;
							Slave_Set_Order(&MS1, AT_CFUN_Min);
							__nop();
						}
					break;
				}
				case(AT_CFUN_Full):	//AT_CFUN_Full
				{
						if(Timer_update(&MS1.GTime))
						{
							Timer_Set_Timer_Delay(&MS1.time,5000);
							Timer_Set_Timer_Delay(&MS1.GTime,15000); // setup Gtimer
							Counter_Active_Repeat(&MS1.counter,false);
							Counter_Set_Repeat_Value(&MS1.counter,3);
							MAchinestate_Update(&MS1);
							Machinestate_Active_Slave(&MS1);
							AT_FUN=EC200T_Set_CFUN_to_full;
							Slave_Set_Order(&MS1, AT_CFUN_Full);
							__nop();
						}
					break;
				}
				case(AT_POST_PRO1):	//AT_POST_PRO1
				{
					MS1.Prev_State=AT_POST_PRO1;
					MAchinestate_Update(&MS1);
					Slave_Set_Order(&MS1, AT_Unknow);
					//=========================== Find Data in Buffer
					if(EC200T_find_Message_between_2lable("bytes","OK",true))// we recived Data with Header Data
					{
						Data_Validation_Assign_Data(&EC200T_data_t,search.Massage_Buffer,search.Message_Lenght);
						//==debug
//						sprintf(ss,"0-%d",EC200T_data_t.Count_Data);
//						Debug((uint8_t*)ss,3);
//						Debug((uint8_t*)":",1);
//						Debug((uint8_t*)Message_Buffer,search.Message_Lenght);
//						Debug((uint8_t*)"\r\n",2);
						//==debug 
						EC200T_data_t.Count_Data++;
					}
					else
					{
						if(EC200T_find_Message_between_2lable("CONNECT","OK",true))
						{
							Data_Validation_Assign_Data(&EC200T_data_t,search.Massage_Buffer,search.Message_Lenght);
							//==debug
//							sprintf(ss,"1-%d",EC200T_data_t.Count_Data);
//							Debug((uint8_t*)ss,3);
							Debug((uint8_t*)Message_Buffer,search.Message_Lenght);
							EC200T_data_t.Count_Data++;
						}
					}
					//=========================== Find Data in Buffer
					//=========================== Temp Section
					if(EC200T_data_t.Count_temp>=EC200T_data_t.EQU_VALID)	
					{
						Debug((uint8_t*)"Recived Data reach max temp value\r\n",strlen("Recived Data reach max temp value\r\n"));
						EC200T_data_t.Count_Data=EC200T_data_t.Count_temp;
						EC200T_data_t.Count_temp=0;
						EC200T_data_t.Pre_Validate_Data=EC200T_data_t.temp_Value;
						SET_Relay_State(EC200T_data_t.temp_Value);
					}
					//=========================== Temp Section
					if(EC200T_data_t.Pre_Validate_Data != EC200T_data_t.DATA_RX[EC200T_data_t.Count_Data-1])
					{
						Debug((uint8_t*)"We recived new Data\r\n",strlen("We recived new Data\r\n"));
						Setup_Time(&MS1.GTime,6000); // setup timer
					}
					else
					{
						Debug((uint8_t*)"We recived same Data\r\n",strlen("We recived Same Data\r\n"));
						Setup_Time(&MS1.GTime,15000); // setup timer
					}
					//=========================== Temp Section
					
					//=========================== Display Vaiable in Debuger
					Debug((uint8_t*)"---------\r\n",11);
					for(count=0;count<MAX_VAlidate_Data;count++)
					{
						if(count==EC200T_data_t.Count_Data-1)
							sprintf(s,"->%1d",count);
						else
							sprintf(s,"--%1d",count);
						sprintf(&s[3],"-%06d",EC200T_data_t.DATA_RX[count]);
						Debug((uint8_t*)s,11);
						Debug((uint8_t*)"\r\n",2);
						
					}
					Debug((uint8_t*)"---------\r\n",11);
					//=========================== Display Vaiable in Debuger
					
					//=========================== Check to reach the max value
					if(Data_Validation_Rich_MaxValue(&EC200T_data_t))	//reach the muximum value of data
					{
						Debug((uint8_t*)"Recived Data reach max value\r\n",strlen("Recived Data reach max value\r\n"));
						if(Data_Validation_EQual(&EC200T_data_t))		// all recived Data meat condition 
						{
							
							SET_Relay_State(EC200T_data_t.New_Validate_Data);
							if(EC200T_data_t.Count_temp>=MAX_VAlidate_Data)
								EC200T_data_t.Count_temp=0;
						}
						Data_Validation_Reset_Count(&EC200T_data_t);
						
						MS1.State=AT_QIDEACT;
					}
					//=========================== Check to reach the max value
					else
						MS1.State=AT_HTTP_GET;
					
					break;
				}
				case(AT_TURN_Off):	// module Turn off 
				{
						if(Timer_update(&MS1.GTime))
						{
							Timer_Set_Timer_Delay(&MS1.time,5000);
							Timer_Set_Timer_Delay(&MS1.GTime,20000); // setup Gtimer
							Counter_Active_Repeat(&MS1.counter,false);
							Counter_Set_Repeat_Value(&MS1.counter,3);
							MAchinestate_Update(&MS1);
							Machinestate_Active_Slave(&MS1);
							AT_FUN=EC200T_TURN_POWER_OFF_software;
							Slave_Set_Order(&MS1, AT_TURN_Off);
							__nop();
						}
					__nop();
					break;
				}
				case(AT_Turn_OFF_Delay):	// software turn off | Turn off Delay
				{
					if(Timer_update(&MS1.GTime))
					{
						MAchinestate_Update(&MS1);
						Slave_Set_Order(&MS1, AT_Unknow);
						MS1.State=AT_Turn_On;
						__nop();
						Debug((uint8_t*)"EC200t forced to shout down\r\n",strlen("EC200t forced to shout down\r\n"));
					}
					break;
				}
				default:
				{
					break;
				}
			}
		}
		
		if(MS1.ISValid)
		{
			Slave_Machine_State(&MS1,AT_FUN);
			HAL_Delay(50);
		}
		
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

	//72->1us,720->10us,7200->100us
  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 7200-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 10000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OnePulse_Init(&htim1, TIM_OPMODE_SINGLE) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 72-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 99;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OnePulse_Init(&htim2, TIM_OPMODE_SINGLE) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, EN_ESP_Pin|RST_ESP_Pin|R3_Pin|R4_Pin
                          |RESET_EC200T_Pin|ON_OFF_EC200T_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, R13_Pin|R14_Pin|R1_Pin|R2_Pin
                          |DTR_EC200T_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, R5_Pin|R6_Pin|R7_Pin|R8_Pin
                          |R9_Pin|R12_Pin|W_DISABLE_Pin|WEAKUP_EC200T_Pin
                          |SLEEP_STATUS_EC200T_Pin|R11_Pin|R10_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : EN_ESP_Pin RST_ESP_Pin R3_Pin R4_Pin
                           RESET_EC200T_Pin ON_OFF_EC200T_Pin */
  GPIO_InitStruct.Pin = EN_ESP_Pin|RST_ESP_Pin|R3_Pin|R4_Pin
                          |RESET_EC200T_Pin|ON_OFF_EC200T_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : R13_Pin R14_Pin R1_Pin R2_Pin
                           DTR_EC200T_Pin */
  GPIO_InitStruct.Pin = R13_Pin|R14_Pin|R1_Pin|R2_Pin
                          |DTR_EC200T_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : R5_Pin R6_Pin R7_Pin R8_Pin
                           R9_Pin R12_Pin W_DISABLE_Pin WEAKUP_EC200T_Pin
                           SLEEP_STATUS_EC200T_Pin R11_Pin R10_Pin */
  GPIO_InitStruct.Pin = R5_Pin|R6_Pin|R7_Pin|R8_Pin
                          |R9_Pin|R12_Pin|W_DISABLE_Pin|WEAKUP_EC200T_Pin
                          |SLEEP_STATUS_EC200T_Pin|R11_Pin|R10_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : RI_EC200T_Pin DCD_EC200T_Pin */
  GPIO_InitStruct.Pin = RI_EC200T_Pin|DCD_EC200T_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)
	{
		HAL_UART_Receive_IT(&huart1,(uint8_t*)&RX_Byte,1);
		RX_Buffer[RX_Counter]=RX_Byte;
		RX_Counter++;
		EC200T_Interrupt=true;
		if(RX_Counter>EC200T_RX_Buffer_Size) // overflow error
		{
			RX_Counter=0;
			Buffer_Overflow=true;
		}
		if(Timer_State)
		{
			//__HAL_TIM_SET_COUNTER(&htim1,0); //reset counter
			__HAL_TIM_SetCounter(&htim1,0); //reset counter
		}
		else
		{
			
			//HAL_TIM_Base_Start_IT(&htim1);
			__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
			__HAL_TIM_ENABLE(&htim1);
			Timer_State=true;
		}
	}
	if(huart->Instance==USART3)// RS485
	{
		HAL_UART_Receive_IT(&huart3,(uint8_t*)&RX_Buffer_RS485,1);		
		RS485_buffer[RS485_Byte_Counter]=RX_Buffer_RS485;
		RS485_Byte_Counter++;
		RS485_Interrupt=true;
		if(RS485_Byte_Counter>EC200T_RX_Buffer_Size) 
		{
			RS485_Overflow=true;
			RS485_Byte_Counter=0;
		}
		if(Timer_State)
		{
			//__HAL_TIM_SET_COUNTER(&htim1,0); //reset counter
			__HAL_TIM_SetCounter(&htim1,0); //reset counter
		}
		else
		{
			//HAL_TIM_Base_Start_IT(&htim1);
			__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
			__HAL_TIM_ENABLE(&htim1);
			Timer_State=true;
		}
	}
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)
	{
		RS485_TX_CMD=true;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM1)
	{
		HAL_TIM_Base_Stop_IT(&htim1);
		Timer_State=false;
		// update Register
		//======================= EC200T RX Interrupt 
		if(EC200T_Interrupt )
		{
			EC200T_Interrupt=false;
			UART_Interrupt=true;
		}
		//======================= EC200T RX Interrupt 
		if(RS485_Interrupt)
		{
			RS485_Interrupt=false;
			RS485_UART_Interrupt=true;
			RS485_Shadow_Byte_Counter=RS485_Byte_Counter;
			RS485_Byte_Counter=0;
			RX_Buffer_RS485=0;
		}
	}
	
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

