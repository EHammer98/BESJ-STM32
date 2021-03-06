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
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MUSHROOM_X_MIN_BOUND 0
#define MUSHROOM_X_MAX_BOUND 32
#define MUSHROOM_Y_MIN_BOUND 2
#define MUSHROOM_Y_MAX_BOUND 20
#define MUSHROOM_MAX 20
#define BULLETS_MAX 100
#define PLAYER_X_MIN_BOUND 0
#define PLAYER_X_MAX_BOUND 32
#define CENTIPEDE_MAXLENGTH 6
//GROEN IS LINKS, GEEL IS RECHTS (MET STICKER)
//GROEN IS BOVEN, GEEL IS ONDER (ZONDER STICKER

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 TIM_HandleTypeDef htim16;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

//GRID
uint8_t GRID[32][20];

//PLAYERS
int player1_x_position,		//PLAYER 1
	player1_left_state, 	player1_left_state_prev,
	player1_right_state, 	player1_right_state_prev,
	player1_select_state,	player1_select_state_prev,
	player1_shoot_state, 	player1_shoot_state_prev;
int player2_x_position,		//PLAYER 2
	player2_left_state, 	player2_left_state_prev,
	player2_right_state, 	player2_right_state_prev,
	player2_select_state,	player2_select_state_prev,
	player2_shoot_state, 	player2_shoot_state_prev;

//BULLETS
int player1_bullet_positions[BULLETS_MAX][2];
int player1_bullets_active;
int player2_bullet_positions[BULLETS_MAX][2];
int player2_bullets_active;

//Centipedes
int centipede1_Position[CENTIPEDE_MAXLENGTH][2];
int centipede2_Position[CENTIPEDE_MAXLENGTH][2];

//MUSHROOMS
int mushroomFarm[MUSHROOM_MAX][2];
int mushroom_active;
int mushroomTimer;
//test
char uart_buf[50];
int uart_buf_len;
uint16_t timer_val;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM16_Init(void);
/* USER CODE BEGIN PFP */
//PLAYER1
int Player1_move(void);
int Player1_shoot(void);
int Player1_select(void);
void Player1_position(int);
//PLAYER2
int Player2_move(void);
int Player2_shoot(void);
int Player2_select(void);
void Player2_position(int);
//Centipede1
void Centipede1_INIT(void);
void Centipede1_move(void);
void Centipede1_position(void);
//Centipede2
void Centipede2_INIT(void);
void Centipede2_move(void);
void Centipede2_position(void);
//MUSHROOM
void mushroomFarm_INIT(void);
void mushroomSpawn(void);

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
  MX_USART2_UART_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */
  //INIT VARIABLES
  player1_x_position = 9;
  player2_x_position = 17;
  mushroomTimer = 0;
  HAL_TIM_Base_Start(&htim16);
  //INIT FUCNTIONS
  mushroomFarmInit();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	if (__HAL_TIM_GET_COUNTER(&htim16) - timer_val >= 1000) //runt elke 1khz
	{
		Player1_position(Player1_move());
		uart_buf_len = sprintf(uart_buf, "%d us\r\n", player1_x_position);
		HAL_UART_Transmit(&huart2, (uint8_t *)uart_buf, uart_buf_len, 100);
		timer_val = __HAL_TIM_GET_COUNTER(&htim16);
		mushroomTimer++;
	}
	if(mushroomTimer == 10)
	{
		mushroomSpawn();
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 4799;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 65535;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */

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
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA7 PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB10 PB3 PB4 PB5
                           PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

//player1 position changes position of sprite READY
void Player1_position(int movement)
{
	int next_position = player1_x_position + movement;
	if(PLAYER_X_MIN_BOUND <= next_position && next_position <= PLAYER_X_MAX_BOUND)
	{
		player1_x_position = next_position;
	}
}
//Player 1 move returns either 0 for no movement, -1 for left and 1 for right; READY
int Player1_move(void)
{
	player1_left_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3);
	if(player1_left_state != player1_left_state_prev && player1_left_state)
	{
		player1_left_state_prev = player1_left_state;
		return -1;
	}
	player1_left_state_prev = 0;
	player1_right_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5);
	if(player1_right_state != player1_right_state_prev && player1_right_state)
	{
		player1_right_state_prev = player1_right_state;
		return 1;
	}
	player1_right_state_prev = 0;
	return 0;
}
//Player 1 shoot returns either 0 or 1; READY
int Player1_shoot(void)
{
	player1_shoot_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4);
	if(player1_shoot_state != player1_shoot_state_prev && player1_shoot_state)
	{
		player1_shoot_state_prev = player1_shoot_state;
		return 1;
	}
	else if(player1_shoot_state == 0)
	{
		player1_shoot_state_prev = 0;
	}
	return 0;
}
//Player 1 select returns either 0 or 1; READY
int Player1_select(void)
{
	player1_select_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10);
	if(player1_select_state != player1_select_state_prev && player1_select_state)
	{
		player1_select_state_prev = player1_select_state;
		return 1;
	}
	else if (player1_select_state == 0)
	{
		player1_select_state_prev = 0;
	}
	return 0;
}

//Player 2 move returns either 0 for no movement, -1 for left and 1 for right; TODO
int Player2_move(void)
{
	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9))
	{
		return -1;
	}
	else if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
//Player 2 shoot returns either 0 or 1; TODO
int Player2_shoot(void)
{
	if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
//Player 2 select returns either 0 or 1; TODO
int Player2_select(void)
{
	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7))
	{
		return 1;
	}
	else
	{
		return 0;
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
