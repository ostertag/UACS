/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include "rc522.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} LedConfig_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SAMPLES 64
#define PI 3.14159265358979323846f
#define LED_COUNT 4
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_tx;
DMA_HandleTypeDef hdma_usart3_rx;

/* USER CODE BEGIN PV */
uint16_t sine_table[SAMPLES];
uint8_t sample_no = 0;
volatile uint8_t uart_running = 0;
volatile uint8_t rc522_running = 0;

uint8_t state = 0;
uint8_t current_led = 0;
uint32_t blink_duration = 200;
uint32_t last_change_time = 0;
uint8_t total_cycles = 0;
uint8_t completed_cycles = 0;
uint8_t sequence_active = 0;

uint8_t status;
uint8_t sNum[8];
uint8_t card_id[8];
uint8_t tmp[8];
char out[24];

uint8_t uart_txbuf[64];
uint8_t uart_rxbuf[64];

char rx_buffer[1024];
uint8_t rx_index = 0;

char led_status[] = "grgr";

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
LedConfig_t led_sequence[LED_COUNT] = {
    {GPIOB, GPIO_PIN_6},
    {GPIOB, GPIO_PIN_7},
    {GPIOA, GPIO_PIN_8},
    {GPIOA, GPIO_PIN_9}
};
typedef enum {
    MENU_MAIN,
    MENU_LED,
	MENU_BLICK,
    MENU_UART,
    MENU_TIMER,
	MENU_RC522,
} MenuState;

MenuState currentMenu = MENU_MAIN;
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
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_USART3_UART_Init();
  MX_USB_DEVICE_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();

  /* USER CODE BEGIN 2 */

  MFRC522_Init();
  Generate_Pulse_Lenght();
  Period_Led_Blick(2);
  Print_Menu();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  Led_Sequence_Update();
	  status = MFRC522_Request(PICC_REQIDL, card_id);
	  status = MFRC522_Anticoll(sNum);
	  memcpy(tmp, sNum, 5);

	  if(rc522_running && (tmp[0] != 147 && tmp[1] != 32)){
		  sprintf(out, "%x%x%x%x%x\r\n", tmp[0], tmp[1], tmp[2], tmp[3], tmp[4]);
		  CDC_Transmit_FS(out, sizeof(out));
	  }else if (uart_running){
		  HAL_UART_Transmit(&huart3, uart_txbuf, sizeof(uart_txbuf), 100);
	  }
	  HAL_UART_Receive_IT(&huart3, uart_rxbuf, sizeof(uart_rxbuf));
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1124;
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
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1124;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

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
  huart3.Init.BaudRate = 9600;
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
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RFID_SDA_Pin|LED_R2_Pin|LED_G2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, RFID_RST_Pin|LED_R1_Pin|LED_G1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : RFID_IRQ_Pin */
  GPIO_InitStruct.Pin = RFID_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RFID_IRQ_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RFID_SDA_Pin LED_R2_Pin LED_G2_Pin */
  GPIO_InitStruct.Pin = RFID_SDA_Pin|LED_R2_Pin|LED_G2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : RFID_RST_Pin LED_R1_Pin LED_G1_Pin */
  GPIO_InitStruct.Pin = RFID_RST_Pin|LED_R1_Pin|LED_G1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
int _write(int file, char *ptr, int len) {
	CDC_Transmit_FS((uint8_t*) ptr, len);
	return len;
}

void Led_Sequence_Update(void) {
    if (!sequence_active) return;

    uint32_t now = HAL_GetTick();

    if (state == 0) {
        HAL_GPIO_WritePin(led_sequence[current_led].port, led_sequence[current_led].pin, GPIO_PIN_SET);
        last_change_time = now;
        state = 1;
    } else {
        if (now - last_change_time >= blink_duration) {
            HAL_GPIO_WritePin(led_sequence[current_led].port, led_sequence[current_led].pin, GPIO_PIN_RESET);
            last_change_time = now;
            state = 0;
            current_led++;

            if (current_led >= LED_COUNT) {
                current_led = 0;
                completed_cycles++;

                if (completed_cycles >= total_cycles) {
                    sequence_active = 0;
                }
            }
        }
    }
}

void Generate_Pulse_Lenght() {
    for (uint8_t i = 0; i < SAMPLES; i++) {
    	float value = ((sinf(2.0f*PI*((float)i/(float)SAMPLES)) + 1.0f)/2.0f)*1124.0f;
        sine_table[i] = value;
    }
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	TIM3->CCR1 = sine_table[sample_no];
	sample_no++;
	if (sample_no > SAMPLES){
		sample_no = 0;
	}
}
void USB_CDC_RxHandler(uint8_t* Buf, uint32_t Len){
	if (Len == 0 && strcmp((char*)Buf, "\n")==0){
		Serial_Command_Handler((char*)Buf);
	}
	for (uint32_t i = 0; i < Len; i++) {
	        char ch = Buf[i];

	        if (ch == '\r' || ch == '\n') {
	            rx_buffer[rx_index] = '\0';
	            Serial_Command_Handler(rx_buffer);
	            rx_index = 0;
	        }
	        else {
	            if (rx_index < 1024 - 1) {
	                rx_buffer[rx_index] = ch;
	                rx_index++;
	            }
	        }
	    }
}
void Serial_Command_Handler(char * Buf){
	uint8_t invalid[] = "Invalid command\r\n";
	if(strcmp(Buf, "q")==0 || strcmp(Buf, "Q" )== 0){
		Print_Menu();
		switch(currentMenu){
		case MENU_TIMER:
			Timer_Stoper();
		case MENU_UART:
			UART_Stop_Tr();
		case MENU_RC522:
			RC522_Stop_Tr();
		}

		currentMenu = MENU_MAIN;
		return;
	}
	if (strcmp(Buf, "\n") == 0){
		Print_Menu();
		currentMenu = MENU_MAIN;
		return;
	}
	switch (currentMenu){
	case MENU_MAIN:
		if (strcmp(Buf, "L") == 0) {
			Led_Menu();
			currentMenu = MENU_LED;
		}else if (strcmp(Buf, "B") == 0){
			Blick_Menu();
			currentMenu = MENU_BLICK;
		}else if (strcmp(Buf, "U") == 0)  {
			UART_Menu();
			currentMenu = MENU_UART;
		}else if (strcmp(Buf, "P") == 0){
			Timer_Menu();
			currentMenu = MENU_TIMER;
		}else if (strcmp(Buf, "R") == 0){
			RC522_Menu();
			RC522_Start_Tr();
			currentMenu = MENU_RC522;
		}else {
			CDC_Transmit_FS(invalid, sizeof(invalid));
		}
		break;
	case MENU_LED:
		if (strcmp(Buf, "1") == 0) {
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
			Swap_Char(0);
		}else if (strcmp(Buf, "2") == 0){
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);
			Swap_Char(1);
		}else if (strcmp(Buf, "3") == 0)  {
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
			Swap_Char(2);
		}else if (strcmp(Buf, "4") == 0){
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
			Swap_Char(3);
		}else {
			CDC_Transmit_FS(invalid, sizeof(invalid));
		}

		uint8_t led[16];
		sprintf((char *)led, "%s | ledtest> ", led_status);
		CDC_Transmit_FS(led, sizeof(led));
		break;
	case MENU_BLICK:
		char* t_c = strtok(Buf, ",");
		char* t_d = strtok(NULL, ",");
		if (strlen(t_d) != 0){
			blink_duration = atoi(t_d);
		}
		uint8_t cycles = atoi(t_c);
		Period_Led_Blick(cycles);
		uint8_t blickled[] = "blinkled> ";
		CDC_Transmit_FS(blickled, sizeof(blickled));
		break;
	case MENU_TIMER:
		Timer_Starter(atoi(Buf));
		uint8_t pwm[] = "pwmtest> ";
		CDC_Transmit_FS(pwm, sizeof(pwm));
		break;
	case MENU_UART:
		UART_Start_Tr(Buf);
		break;
	}
}
void Swap_Char(uint8_t i){
	if(led_status[i] == 71 || led_status[i] == 82){
		char c =  led_status[i] + 32;
		led_status[i] = c;
	}else if(led_status[i] == 103 || led_status[i] == 114){
		char c =  led_status[i] - 32;
		led_status[i] = c;
	}
}



void Print_Menu(){
	uint8_t buf[] = "q return/end action\r\nMenu:\r\nT) Toggle led\r\nB) Blink led\r\nU) UART test\r\nP) PWM test\r\nR) RC522 test\r\nmenu> ";
	CDC_Transmit_FS(buf, sizeof(buf));
}
void Led_Menu(){
	uint8_t buf[76];
	sprintf((char *)buf, "Toggle led: \r\n1) 1 Green\r\n2) 1 Red\r\n3) 2 Green \r\n4) 2 Red\r\n%s | ledtest> ", led_status);
	CDC_Transmit_FS(buf, sizeof(buf));
	}
void Blick_Menu(){
	uint8_t buf[] = "Blink led | Number of cycles,delay (ms):\r\nblinkled> ";
	CDC_Transmit_FS(buf, sizeof(buf));
}
void UART_Menu(){
	uint8_t buf[] = "UART | Write message:\r\nuarttest> ";
	CDC_Transmit_FS(buf, sizeof(buf));
}
void RC522_Menu(){
	uint8_t buf[] = "RC522 | Output:\r\n";
	CDC_Transmit_FS(buf, sizeof(buf));
}
void Timer_Menu(){
	uint8_t buf[] = "PWM | Frequency of sinus (Hz):\r\npwmtest> ";
	CDC_Transmit_FS(buf, sizeof(buf));
}


void Period_Led_Blick(uint8_t cycles){
	    if (cycles == 0) return;
	    total_cycles = cycles;
	    completed_cycles = 0;
	    current_led = 0;
	    state = 0;
	    sequence_active = 1;
}

void UART_Start_Tr(char * Buf){
	uart_running = 1;
	memcpy(uart_txbuf, Buf, strlen(Buf));
}

void UART_Stop_Tr(){
	uart_running = 0;
}

void RC522_Start_Tr(){
	rc522_running = 1;
}

void RC522_Stop_Tr(){
	rc522_running = 0;
}
void Timer_Starter(uint16_t freq){
	Modify_Timer_ARR(freq);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_Base_Start_IT(&htim2);
}

void Timer_Stoper(){
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
	HAL_TIM_Base_Stop_IT(&htim2);

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	CDC_Transmit_FS(uart_rxbuf, sizeof(uart_rxbuf));
}

void Modify_Timer_ARR(uint16_t freq){
	float time = 1.0f/(float)freq;
	float arr = ((time/(float)SAMPLES)*72000000.0f)/1.0f - 1.0f;
	TIM2->ARR = (uint32_t)arr;
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
