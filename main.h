/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define EN_ESP_Pin GPIO_PIN_14
#define EN_ESP_GPIO_Port GPIOC
#define RST_ESP_Pin GPIO_PIN_15
#define RST_ESP_GPIO_Port GPIOC
#define R13_Pin GPIO_PIN_4
#define R13_GPIO_Port GPIOA
#define R14_Pin GPIO_PIN_5
#define R14_GPIO_Port GPIOA
#define R1_Pin GPIO_PIN_6
#define R1_GPIO_Port GPIOA
#define R2_Pin GPIO_PIN_7
#define R2_GPIO_Port GPIOA
#define R3_Pin GPIO_PIN_4
#define R3_GPIO_Port GPIOC
#define R4_Pin GPIO_PIN_5
#define R4_GPIO_Port GPIOC
#define R5_Pin GPIO_PIN_0
#define R5_GPIO_Port GPIOB
#define R6_Pin GPIO_PIN_1
#define R6_GPIO_Port GPIOB
#define R7_Pin GPIO_PIN_2
#define R7_GPIO_Port GPIOB
#define R8_Pin GPIO_PIN_10
#define R8_GPIO_Port GPIOB
#define R9_Pin GPIO_PIN_11
#define R9_GPIO_Port GPIOB
#define R12_Pin GPIO_PIN_12
#define R12_GPIO_Port GPIOB
#define W_DISABLE_Pin GPIO_PIN_13
#define W_DISABLE_GPIO_Port GPIOB
#define WEAKUP_EC200T_Pin GPIO_PIN_14
#define WEAKUP_EC200T_GPIO_Port GPIOB
#define SLEEP_STATUS_EC200T_Pin GPIO_PIN_15
#define SLEEP_STATUS_EC200T_GPIO_Port GPIOB
#define RESET_EC200T_Pin GPIO_PIN_6
#define RESET_EC200T_GPIO_Port GPIOC
#define ON_OFF_EC200T_Pin GPIO_PIN_7
#define ON_OFF_EC200T_GPIO_Port GPIOC
#define RI_EC200T_Pin GPIO_PIN_8
#define RI_EC200T_GPIO_Port GPIOC
#define RI_EC200T_EXTI_IRQn EXTI9_5_IRQn
#define DCD_EC200T_Pin GPIO_PIN_9
#define DCD_EC200T_GPIO_Port GPIOC
#define DCD_EC200T_EXTI_IRQn EXTI9_5_IRQn
#define DTR_EC200T_Pin GPIO_PIN_8
#define DTR_EC200T_GPIO_Port GPIOA
#define R11_Pin GPIO_PIN_3
#define R11_GPIO_Port GPIOB
#define R10_Pin GPIO_PIN_4
#define R10_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
