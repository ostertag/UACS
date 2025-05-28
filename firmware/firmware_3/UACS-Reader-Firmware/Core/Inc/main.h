/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RFID_IRQ_Pin GPIO_PIN_3
#define RFID_IRQ_GPIO_Port GPIOA
#define RFID_SDA_Pin GPIO_PIN_4
#define RFID_SDA_GPIO_Port GPIOA
#define RFID_SCK_Pin GPIO_PIN_5
#define RFID_SCK_GPIO_Port GPIOA
#define RFID_MISO_Pin GPIO_PIN_6
#define RFID_MISO_GPIO_Port GPIOA
#define RFID_MOSI_Pin GPIO_PIN_7
#define RFID_MOSI_GPIO_Port GPIOA
#define RFID_RST_Pin GPIO_PIN_0
#define RFID_RST_GPIO_Port GPIOB
#define LED_R2_Pin GPIO_PIN_8
#define LED_R2_GPIO_Port GPIOA
#define LED_G2_Pin GPIO_PIN_9
#define LED_G2_GPIO_Port GPIOA
#define LED_R1_Pin GPIO_PIN_6
#define LED_R1_GPIO_Port GPIOB
#define LED_G1_Pin GPIO_PIN_7
#define LED_G1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
void USB_CDC_RxHandler(uint8_t*, uint32_t);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
