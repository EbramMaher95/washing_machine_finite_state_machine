/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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

// Include standard integer definitions
#include <stdint.h>

#include <stdio.h>
// Include header for HD44780 LCD driver
#include "../../Drivers/Device_Drivers/HD44780_LCD/HD44780_LCD.h"
#include "../../Drivers/Device_Drivers/Matrix_Keypad/Matrix_Keypad.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
// Initialize the LCD struct with GPIO pins
Alcd_t lcd =
		{ .RS_GPIO = GPIOA, .RS_GPIO_Pin = GPIO_PIN_4, .EN_GPIO = GPIOA,
				.EN_GPIO_Pin = GPIO_PIN_5, .Data_GPIO = GPIOA,
				.Data_GPIO_Start_Pin = 0 };

///
/// defining the matrix keypad pins and port
/// it is on port B
Keypad_Matrix key = { .Row_Port = GPIOB, .Row_Start_Pin = 12, .Col_Port = GPIOB,
		.Col_Start_Pin = 6, .Col = 4, .Row = 4,

};
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define filling_state 10
#define rinsing_state 20
#define spinning_state 30
#define finished_state 500
#define error_state -500

typedef enum {

	no_error,
	error1, //water level is low
	error2, //
}errors_list;

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

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
	/* USER CODE BEGIN 2 */
	// Initialize the LCD with 2 lines and 16 columns
	Alcd_Init(&lcd, 2, 16);

	// Clear the LCD
	Alcd_Clear(&lcd);

	//initializing the Keypad
	Keypad_Matrix_Init(&key);

	// Display initial messages on the LCD
	Alcd_PutAt(&lcd, 0, 0, "program start");

	int16_t state;
	uint32_t current_tick;
	uint32_t filling_timeout;
	uint32_t rinsing_time;
	uint32_t spinning_time;
	int16_t error_number;
	state = filling_state;

	filling_timeout = HAL_GetTick() + 10000;
	rinsing_time = HAL_GetTick() + 50000;
	spinning_time = HAL_GetTick() + 60000;

	uint16_t message;

	/*
	error_number =
	*/

	char str[16];
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

		//refresh the keypad
		Keypad_Matrix_Refresh(&key);

		current_tick = HAL_GetTick();

		switch (state) {

		case filling_state: {

			//the water valve in on PIN13 on port C -> water valve is open (filling)
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

			Alcd_Clear(&lcd);
			Alcd_PutAt(&lcd, 0, 0, "filling state");

			message = sprintf(str, "state = %d", state);
			Alcd_PutAt_n(&lcd, 1, 0, str, message);

			//in case timeout and the filling not complete
			if ((current_tick >= filling_timeout)
					& !(Keypad_Matrix_Read_Key(&key, 4))) {
				state = error_state;
			}
			//in case of filling complete
			if (Keypad_Matrix_Read_Key(&key, 4)) {
				//stop the filling valve
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

				//move to the rinsing state
				state = rinsing_state;

			}

			break;
		}

		case rinsing_state: {
			Alcd_Clear(&lcd);
			Alcd_PutAt(&lcd, 0, 0, "rinsing state");
			message = sprintf(str, "state = %d", state);
			Alcd_PutAt_n(&lcd, 1, 0, str, message);

			//disable filling valve -> already done but to ensure the closure
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

			//enable the motor -> on PIN 14 PORT C
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);

//check if water level is low
			if (!(Keypad_Matrix_Read_Key(&key, 4))) {
				//stop the filling valve
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

				//move to the error state
				state = error_state;

			}

			//check for the rinsing time is over
			if (current_tick >= rinsing_time) {
				//move to the next state
				state = spinning_state;
			}

			break;
		}

		case spinning_state: {

			Alcd_Clear(&lcd);
						Alcd_PutAt(&lcd, 0, 0, "spinning state");
						message = sprintf(str, "state = %d", state);
						Alcd_PutAt_n(&lcd, 1, 0, str, message);

			//for spinning -> open drain valve -> PIN 15 PORT C
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);

			//in case spining time is over and water level is high
			if ((current_tick >= spinning_time)
					& ((Keypad_Matrix_Read_Key(&key, 4)))) {
				//move to the next state
				state = error_state;
			}

			//check for the spinning time is over and water level is low
			if ((current_tick >= spinning_time)
					& (!(Keypad_Matrix_Read_Key(&key, 4)))) {
				//move to the next state
				state = finished_state;
			}
			break;
		}

		case error_state: {

			Alcd_Clear(&lcd);
			Alcd_PutAt(&lcd, 0, 0, "ERROR!!!");
			message = sprintf(str, "state = %d", state);
			Alcd_PutAt_n(&lcd, 1, 0, str, message);

			break;

		}

		}

	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

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
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
			GPIO_PIN_RESET);

	/*Configure GPIO pins : PC13 PC14 PC15 */
	GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/* USER CODE BEGIN MX_GPIO_Init_2 */
	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
