/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "rng.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"
#include "stdint.h"

// include got error ../Core/Src/main.c:31:10: fatal error: ILI9341_Touchscreen.h: No such file or directory
#include "ILI9341_Touchscreen.h"
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "5x5_font.h"
#include "spi.h"
#include "snow_tiger.h"
#include "student_photo.h"
#include "bitmaps.h"
#include <stdint.h>
#include "poker.h"
#include "draw_poker.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
void ft_printf(char *buffer);
int display_start_page(void);

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LIGHT_RED    0xFC90    // A lighter red shade
#define LIGHT_GREEN  0xBFE0    // A lighter green shade
#define LIGHT_BLUE   0x8DFF    // A lighter blue shade
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
typedef enum{
	JOY_MID,
	JOY_UP,
	JOY_DOWN,
	JOY_LEFT,
	JOY_RIGHT,
} joy_state;
// Define states for modifying players and bots
typedef enum {
    STATE_MODIFY_PLAYERS,
    STATE_MODIFY_BOTS,
    STATE_NONE
} State;

State current_state = STATE_NONE;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint32_t adc_val = 0;
int n_players = 1;
int n_bots = 1;
/* Private variables ---------------------------------------------------------*/
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
uint16_t CRC_16_2(uint8_t*, uint8_t);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t previous_x_val = 0; // Initialize a variable to store the previous x_val
uint32_t previous_y_val = 0; // Initialize a variable to store the previous y_val
uint8_t pwm;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

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
  MX_USART3_UART_Init();
  MX_SPI5_Init();
  MX_TIM1_Init();
  MX_RNG_Init();
  MX_TIM2_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_ADC2_Init();
  MX_ADC3_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
	HAL_ADC_Start(&hadc1);

	ILI9341_Init();

	HAL_TIM_Base_Start_IT(&htim2);

	ILI9341_Fill_Screen(WHITE);
	ILI9341_Set_Rotation(SCREEN_HORIZONTAL_2);

	uint32_t random_number;

	// Enable RNG clock
	__HAL_RCC_RNG_CLK_ENABLE();

	// Initialize RNG
	RNG_HandleTypeDef hrng;
	hrng.Instance = RNG;
	HAL_RNG_Init(&hrng);

	// Get random number
	HAL_RNG_GenerateRandomNumber(&hrng, &random_number);


	// init joy stick;
	HAL_ADC_Start(&hadc1);
	HAL_ADC_Start(&hadc2);
	srand(random_number);
	while (display_start_page() != 1) {}
	resetScreen(5);
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 1);
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 1);
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 1);
	playPokerGame(n_players, n_bots);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {

    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */

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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

int display_start_page(void){
	// Poll and get values for ADC1 (y_val)
		    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		    uint32_t y_val = HAL_ADC_GetValue(&hadc1);

		    // Poll and get values for ADC2 (x_val)
		    HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
		    uint32_t x_val = HAL_ADC_GetValue(&hadc2);

		    // Define thresholds for increasing and decreasing player/bot count
		    const int threshold_increase = 3000;  // Value to increase by 1
		    const int threshold_decrease = 1000;   // Value to decrease by 1

		    // Define thresholds for switching modes
		    const int mode_switch_high = 3000;     // Threshold to switch to player mode
		    const int mode_switch_low = 1000;      // Threshold to switch to bot mode

		    char buffer[50];

		    // Check for switching modes based on y_val
		    if (y_val > mode_switch_high) {
		        // Switch to player mode
		        current_state = STATE_MODIFY_PLAYERS; // Set state to modify players
		        sprintf(buffer, "Switched to Player Mode\n");
		        ft_printf(buffer);
		        ILI9341_Draw_Text(buffer, 30, 100, BLACK, 3, WHITE);
		    }
		    else if (y_val < mode_switch_low) {
		        // Switch to bot mode
		        current_state = STATE_MODIFY_BOTS; // Set state to modify bots
		        sprintf(buffer, "Switched to Bot Mode\n");
		        ILI9341_Draw_Text(buffer, 30, 100, BLACK, 3, WHITE);
		        ft_printf(buffer);
		    }

		    // Modify players or bots based on the current state
		    if (current_state == STATE_MODIFY_PLAYERS) {
		        // Check for increasing players
		        if (x_val > threshold_increase && (n_players + n_bots < 4)) {
		            n_players++;  // Increase the number of players by 1
		            sprintf(buffer, "Players increased: %d\n", n_players);
		            ft_printf(buffer);
		        }
		        // Check for decreasing players
		        else if (x_val < threshold_decrease) {
		            if (n_players > 0) {
		                n_players--;  // Decrease the number of players by 1
		                sprintf(buffer, "Players decreased: %d\n", n_players);
		                ft_printf(buffer);
		            }
		        }
		    }
		    else if (current_state == STATE_MODIFY_BOTS) {
		        // Check for increasing bots
		        if (x_val > threshold_increase && (n_players + n_bots < 4)) {
		            n_bots++;  // Increase the number of bots by 1
		            sprintf(buffer, "Bots increased: %d\n", n_bots);
		            ft_printf(buffer);
		        }
		        // Check for decreasing bots
		        else if (x_val < threshold_decrease) {
		            if (n_bots > 0) {
		                n_bots--;  // Decrease the number of bots by 1
		                sprintf(buffer, "Bots decreased: %d\n", n_bots);
		                ft_printf(buffer);
		            }
		        }
		    }
		    else {
		        // No significant change in player/bot count
		        sprintf(buffer, "No significant change: x_val = %d, y_val = %d\n", x_val, y_val);
		        ft_printf(buffer);
		    }

		    // Update previous values for the next loop
		    previous_x_val = x_val;
		    previous_y_val = y_val;

		    HAL_Delay(300); // Delay for 300 milliseconds

		    // Update the display with the current number of players and bots
		    char bf[20];
		    sprintf(bf, "PLAYERS: %d", n_players);
		    ILI9341_Draw_Text(bf, 30, 30, BLACK, 3, WHITE);
		    sprintf(bf, "BOTS: %d", n_bots);
		    ILI9341_Draw_Text(bf, 30, 50, BLACK, 3, WHITE);
		    HAL_Delay(5000);
		    return (1);
}

void ft_printf(char *buffer)
{
	while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){}
	HAL_UART_Transmit(&huart3, (uint8_t*) buffer, strlen(buffer),1000);
}

uint16_t CRC_16_2(uint8_t *ptr, uint8_t length) {
	uint16_t crc = 0xFFFF;
	uint8_t s = 0x00;
	while (length--) {
		crc ^= *ptr++;
		for (s = 0; s < 8; s++) {
			if ((crc & 0x01) != 0) {
				crc >>= 1;
				crc ^= 0xA001;
			} else {
				crc >>= 1;
			}
		}
	}
	return crc;
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
