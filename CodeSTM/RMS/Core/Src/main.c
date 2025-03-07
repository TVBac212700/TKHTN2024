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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "liquidcrystal_i2c.h"
#include "stdio.h"
#include "MPR121.h"
#include "bmp180_for_stm32_hal.h"
#include "string.h"
#include "stdlib.h"
//#include "task.h"
//#include "FreeRTOSConfig.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define MASK_RESPONSE 127
#define RESPONSES(Code)   (Code)
#define UPGRADE(Code) (Code+MASK_RESPONSE)
#define CODE_TEMP  1
#define CODE_PRES  2
#define CODE_KEY   3
#define CODE_COUNT 4
#define CODE_RESP  5
#define MY_ADDRESS 111
#include "funtion.h"
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

osThreadId RXUARTTaskHandle;
osThreadId LCDHandle;
osThreadId ReadTempTaskHandle;
osThreadId ReadPressTaskHandle;
osThreadId KeyTaskHandle;
osThreadId TXUARTTaskHandle;
osMessageQId StringPrintHandle;
osMessageQId StringRS485Handle;
osMutexId I2C_BusHandle;
/* USER CODE BEGIN PV */

#define C_INDEX 0
#define D_INDEX 1
#define T_INDEX 2

#define RX_PM_C 100
#define RX_PM_D 150
#define RX_PM_T 150

#define TX_PM_C 10
#define TX_PM_D 100
#define TX_PM_T 1000

#define TEMP_PM_C 7
#define TEMP_PM_D 5000
#define TEMP_PM_T 5000

#define PRES_PM_C 36
#define PRES_PM_D 5000
#define PRES_PM_T 5000

#define KEYB_PM_C 1
#define KEYB_PM_D 100
#define KEYB_PM_T 200

#define LCD_PM_C 50
#define LCD_PM_D 100
#define LCD_PM_T 1000

int RX_PM[] = {RX_PM_C, RX_PM_D, RX_PM_T};
int TX_PM[] = {TX_PM_C, TX_PM_D, TX_PM_T};
int Temp_PM[] = {TEMP_PM_C, TEMP_PM_D, TEMP_PM_T};
int Pres_PM[] = {PRES_PM_C, PRES_PM_D, PRES_PM_T};
int KeyB_PM[] = {KEYB_PM_C, KEYB_PM_D, KEYB_PM_T};
int LCD_PM[] = {LCD_PM_C, LCD_PM_D, LCD_PM_T};

int *ALL_TASK[] = {RX_PM, TX_PM, Temp_PM, Pres_PM, KeyB_PM, LCD_PM};

int DF_RX_PM[] = {RX_PM_C, RX_PM_D, RX_PM_T};
int DF_TX_PM[] = {TX_PM_C, TX_PM_D, TX_PM_T};
int DF_Temp_PM[] = {TEMP_PM_C, TEMP_PM_D, TEMP_PM_T};
int DF_Pres_PM[] = {PRES_PM_C, PRES_PM_D, PRES_PM_T};
int DF_KeyB_PM[] = {KEYB_PM_C, KEYB_PM_D, KEYB_PM_T};
int DF_LCD_PM[] = {LCD_PM_C, LCD_PM_D, LCD_PM_T};

int *DF_ALL_TASK[] = {DF_RX_PM, DF_TX_PM, DF_Temp_PM, DF_Pres_PM, DF_KeyB_PM, DF_LCD_PM};

char indicate_1[] = "Send To RS485";
char indicate_T[] = "Send Temperature";
char indicate_P[] = "Send Pressure";
char indicate_C[] = "Send Count";
char indicate_M[] = "Send Status";
char detect_touch[] = "Detect Touch %d";
char indicate_E[] = "CRC Error";
char indicate_E_CDT[] = "Invalid C D T";
char indicate_C_D_T[] = "Set C D T";
char indicate_R_C_D_T[] = "Reset C D T";
//char key_board[20];
float temperature;
int32_t pressure;
//uint8_t len_key_board = 0, last_send = 0;
uint16_t count_touch = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
void RX_Task(void const * argument);
void LCD_Task(void const * argument);
void Temp_Task(void const * argument);
void Press_Task(void const * argument);
void Key_Task(void const * argument);
void TX_Task(void const * argument);

/* USER CODE BEGIN PFP */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void UserDelayUntil(uint32_t *lastTick, uint32_t periodMs)
{
    uint32_t currentTick = xTaskGetTickCount();  // Lấy tick hiện tại
    uint32_t nextTick = *lastTick + pdMS_TO_TICKS(periodMs);  // Tính tick tiếp theo
    if ((int32_t)(nextTick - currentTick) > 0)
    {
        vTaskDelay(nextTick - currentTick);  // Delay nếu chưa tới thời gian tiếp theo
    }
    *lastTick = nextTick;  // Cập nhật tick cho lần tiếp theo
}

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
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HD44780_Init(2);
  MPR121_Init();
  BMP180_Init(&hi2c1);
  MPR121_Init();
  BMP180_SetOversampling(BMP180_ULTRA);
  BMP180_UpdateCalibrationData();
  HD44780_SetCursor(0,0);
  HD44780_PrintStr("INIT LCD 1602");
//  count_touch = 0;

  /* USER CODE END 2 */

  /* Create the mutex(es) */
  /* definition and creation of I2C_Bus */
  osMutexDef(I2C_Bus);
  I2C_BusHandle = osMutexCreate(osMutex(I2C_Bus));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of StringPrint */
  osMessageQDef(StringPrint, 10, 20);
  StringPrintHandle = osMessageCreate(osMessageQ(StringPrint), NULL);

  /* definition and creation of StringRS485 */
  osMessageQDef(StringRS485, 50, 50);
  StringRS485Handle = osMessageCreate(osMessageQ(StringRS485), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of RXUARTTask */
  osThreadDef(RXUARTTask, RX_Task, osPriorityRealtime, 0, 512);
  RXUARTTaskHandle = osThreadCreate(osThread(RXUARTTask), NULL);

  /* definition and creation of LCD */
  osThreadDef(LCD, LCD_Task, osPriorityAboveNormal, 0, 512);
  LCDHandle = osThreadCreate(osThread(LCD), NULL);

  /* definition and creation of ReadTempTask */
  osThreadDef(ReadTempTask, Temp_Task, osPriorityBelowNormal, 0, 256);
  ReadTempTaskHandle = osThreadCreate(osThread(ReadTempTask), NULL);

  /* definition and creation of ReadPressTask */
  osThreadDef(ReadPressTask, Press_Task, osPriorityBelowNormal, 0, 256);
  ReadPressTaskHandle = osThreadCreate(osThread(ReadPressTask), NULL);

  /* definition and creation of KeyTask */
  osThreadDef(KeyTask, Key_Task, osPriorityHigh, 0, 512);
  KeyTaskHandle = osThreadCreate(osThread(KeyTask), NULL);

  /* definition and creation of TXUARTTask */
  osThreadDef(TXUARTTask, TX_Task, osPriorityNormal, 0, 512);
  TXUARTTaskHandle = osThreadCreate(osThread(TXUARTTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : INT_Pin */
  GPIO_InitStruct.Pin = INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(INT_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == INT_Pin) {
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		count_touch++;
	} else {
		__NOP();
	}
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_RX_Task */
/**
  * @brief  Function implementing the RXUARTTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_RX_Task */
void RX_Task(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
	char read_to_rs485[50];
	char send_to_rs485[50];
	uint32_t GetTick = 0;
  for(;;)
  {
		memset((void*)read_to_rs485,0,sizeof(read_to_rs485));
		HAL_UART_Receive(&huart1, (uint8_t *)&read_to_rs485, sizeof (read_to_rs485) , RX_PM[C_INDEX]);
		  if(strlen(read_to_rs485) > 1)
		  {
			  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
			  DataStruct result;
			  parse_data(read_to_rs485,&result);
			  if(result.Addr == MY_ADDRESS && check_crc(&result) == result.Crc)
			  {
				  if(result.Code == 1)
				  {
					  if(result.Data[0] == '1')
					  {
						  memset((void*)send_to_rs485,0,sizeof(send_to_rs485));
						  sprintf(send_to_rs485, "%.1fC", temperature);
						  send_to_rs485[strlen(send_to_rs485)+1] = RESPONSES(CODE_TEMP);
						  osMessagePut(StringRS485Handle, (uint32_t)send_to_rs485, osWaitForever);
						  osMessagePut(StringPrintHandle, (uint32_t)indicate_T, osWaitForever);
					  }
					  else if (result.Data[0] == '2')
					  {
						  memset((void*)send_to_rs485,0,sizeof(send_to_rs485));
						  sprintf(send_to_rs485, "%dPa", (int) pressure);
						  send_to_rs485[strlen(send_to_rs485)+1] = RESPONSES(CODE_PRES);
						  osMessagePut(StringRS485Handle, (uint32_t)send_to_rs485, osWaitForever);
						  osMessagePut(StringPrintHandle, (uint32_t)indicate_P, osWaitForever);
					  }
					  else if (result.Data[0] == '4')
					  {
						  memset((void*)send_to_rs485,0,sizeof(send_to_rs485));
						  sprintf(send_to_rs485, "%dN", (int) count_touch);
						  send_to_rs485[strlen(send_to_rs485)+1] = RESPONSES(CODE_COUNT);
						  osMessagePut(StringRS485Handle, (uint32_t)send_to_rs485, osWaitForever);
						  osMessagePut(StringPrintHandle, (uint32_t)indicate_C, osWaitForever);
					  }
					  else
					  {
						  memset((void*)send_to_rs485,0,sizeof(send_to_rs485));
						  sprintf(send_to_rs485, "Invalid Data");
						  send_to_rs485[strlen(send_to_rs485)+1] = RESPONSES(CODE_RESP);
						  osMessagePut(StringRS485Handle, (uint32_t)send_to_rs485, osWaitForever);
						  osMessagePut(StringPrintHandle, (uint32_t)send_to_rs485, osWaitForever);
					  }
				  }
				  else if(result.Code == 2)
				  {
					  result.Data[result.Len] = '\0';
					  memset((void*)send_to_rs485,0,sizeof(send_to_rs485));
					  sprintf(send_to_rs485, "Okee");
					  send_to_rs485[strlen(send_to_rs485)+1] = RESPONSES(CODE_RESP);
					  osMessagePut(StringRS485Handle, (uint32_t)send_to_rs485, osWaitForever);
					  osMessagePut(StringPrintHandle, (uint32_t)result.Data, osWaitForever);
				  }
				  else if(result.Code == 3)
				  {
					  result.Data[result.Len] = '\0';
					  memset((void*)send_to_rs485,0,sizeof(send_to_rs485));
					  sprintf(send_to_rs485, "Okee");
					  send_to_rs485[strlen(send_to_rs485)+1] = RESPONSES(CODE_RESP);
					  osMessagePut(StringRS485Handle, (uint32_t)send_to_rs485, osWaitForever);
					  osMessagePut(StringPrintHandle, (uint32_t)indicate_M, osWaitForever);
				  }
				  else if(result.Code == 4)
				  {
					  int Task = -1, NewC = 0, NewD = 0, NewT = 0;
					  char *Res = indicate_E_CDT;
					  result.Data[result.Len] = '\0';
					  if(sscanf((char*)result.Data, "%d%d%d%d",&Task, &NewC, &NewD, &NewT) == 4)
					  {
						  if(Task >= 0 && Task < 6 && NewC < NewD && NewD <= NewT)
						  {
							  ALL_TASK[Task][C_INDEX] = NewC;
							  ALL_TASK[Task][D_INDEX] = NewD;
							  ALL_TASK[Task][T_INDEX] = NewT;
							  Res = indicate_C_D_T;
						  }
					  }
					  memset((void*)send_to_rs485,0,sizeof(send_to_rs485));
					  sprintf(send_to_rs485, Res);
					  send_to_rs485[strlen(send_to_rs485)+1] = RESPONSES(CODE_RESP);
					  osMessagePut(StringRS485Handle, (uint32_t)send_to_rs485, osWaitForever);
					  osMessagePut(StringPrintHandle, (uint32_t)Res, osWaitForever);
				  }
				  else if(result.Code == 5)
				  {
					  for(int i = 0; i < 5; i++)
					  {
						  for (int j = 0; j < 3; j++)
						  {
							  ALL_TASK[i][j] = DF_ALL_TASK[i][j];
						  }
					  }
					  memset((void*)send_to_rs485,0,sizeof(send_to_rs485));
					  sprintf(send_to_rs485, indicate_R_C_D_T);
					  send_to_rs485[strlen(send_to_rs485)+1] = RESPONSES(CODE_RESP);
					  osMessagePut(StringRS485Handle, (uint32_t)send_to_rs485, osWaitForever);
					  osMessagePut(StringPrintHandle, (uint32_t)indicate_R_C_D_T, osWaitForever);
				  }
				  else
				  {
					  memset((void*)send_to_rs485,0,sizeof(send_to_rs485));
					  sprintf(send_to_rs485, "Invalid Code");
					  send_to_rs485[strlen(send_to_rs485)+1] = RESPONSES(CODE_RESP);
					  osMessagePut(StringRS485Handle, (uint32_t)send_to_rs485, osWaitForever);
					  osMessagePut(StringPrintHandle, (uint32_t)send_to_rs485, osWaitForever);
				  }
			  }
			  else if(check_crc(&result) != result.Crc)
			  {
				  result.Data[result.Len] = '\0';
				  memset((void*)send_to_rs485,0,sizeof(send_to_rs485));
				  sprintf(send_to_rs485, "Error Crc");
				  osMessagePut(StringRS485Handle, (uint32_t)send_to_rs485, osWaitForever);
				  osMessagePut(StringPrintHandle, (uint32_t)indicate_E, osWaitForever);
			  }
		  }
		  osDelay(20);
		  UserDelayUntil(&GetTick,RX_PM[T_INDEX]);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_LCD_Task */
/**
* @brief Function implementing the LCD thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LCD_Task */
void LCD_Task(void const * argument)
{
  /* USER CODE BEGIN LCD_Task */
  /* Infinite loop */
	  osEvent event;
	  char LastMessage[20] = "Init LCD";
	  char *NewMessage = NULL;
	  uint32_t GetTick = 0;
  for(;;)
  {
	  event = osMessageGet(StringPrintHandle, osWaitForever);
	  if (event.status == osEventMessage)
	  {
		  NewMessage = (char*)event.value.v;
		  if (osMutexWait(I2C_BusHandle, osWaitForever) == osOK)
		  {
			HD44780_Clear();
	        HD44780_SetCursor(0,0);
			HD44780_PrintStr(LastMessage);
			HD44780_SetCursor(0,1);
			HD44780_PrintStr(NewMessage);
	        osMutexRelease(I2C_BusHandle);
		  }
		  if(NewMessage != NULL)
		  {
			  memcpy(LastMessage, NewMessage, strlen(NewMessage));
			  LastMessage[strlen(NewMessage)] = '\0';
		  }
	  }
	  UserDelayUntil(&GetTick,LCD_PM[T_INDEX]);
//	  osDelay(1000);
  }
  /* USER CODE END LCD_Task */
}

/* USER CODE BEGIN Header_Temp_Task */
/**
* @brief Function implementing the ReadTempTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Temp_Task */
void Temp_Task(void const * argument)
{
  /* USER CODE BEGIN Temp_Task */
  /* Infinite loop */
//	float temperature;
	uint32_t GetTick = 0;
  for(;;)
  {
	    if (osMutexWait(I2C_BusHandle, osWaitForever) == osOK)
	    {
	    	temperature = BMP180_GetTemperature();
	        osMutexRelease(I2C_BusHandle);
	    }
	    char sen_to_lcd[20] = "";
	    sprintf(sen_to_lcd,"%0.1fC", temperature);
	    osMessagePut(StringPrintHandle, (uint32_t)sen_to_lcd, osWaitForever);
	    sen_to_lcd[strlen(sen_to_lcd)+1] = UPGRADE(CODE_TEMP);
	    osMessagePut(StringRS485Handle, (uint32_t)sen_to_lcd, osWaitForever);
//	    osDelay(5000);
	    UserDelayUntil(&GetTick,Temp_PM[T_INDEX]);
  }
  /* USER CODE END Temp_Task */
}

/* USER CODE BEGIN Header_Press_Task */
/**
* @brief Function implementing the ReadPressTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Press_Task */
void Press_Task(void const * argument)
{
  /* USER CODE BEGIN Press_Task */
  /* Infinite loop */
//	int32_t pressure;
	osDelay(2500);
	uint32_t GetTick = 0;
  for(;;)
  {
	    if (osMutexWait(I2C_BusHandle, osWaitForever) == osOK)
	    {
	    	pressure = BMP180_GetPressure();
	        osMutexRelease(I2C_BusHandle);
	    }
	    osDelay(Pres_PM[T_INDEX]/2);
	    char sen_to_lcd[20];
	    sprintf(sen_to_lcd,"%dPa", (int)pressure);
	    osMessagePut(StringPrintHandle, (uint32_t)sen_to_lcd, osWaitForever);
	    sen_to_lcd[strlen(sen_to_lcd)+1] = UPGRADE(CODE_PRES);
	    osMessagePut(StringRS485Handle, (uint32_t)sen_to_lcd, osWaitForever);
	    UserDelayUntil(&GetTick,Pres_PM[T_INDEX]);
  }
  /* USER CODE END Press_Task */
}

/* USER CODE BEGIN Header_Key_Task */
/**
* @brief Function implementing the KeyTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Key_Task */
void Key_Task(void const * argument)
{
  /* USER CODE BEGIN Key_Task */
  /* Infinite loop */
	char key_board[20];
	uint8_t len_key_board = 0;
	uint16_t last_touch = 0, new_touch = 0;
	uint32_t GetTick = 0;
  for(;;)
  {
		if (osMutexWait(I2C_BusHandle, osWaitForever) == osOK)
		{
			new_touch = touched();
			osMutexRelease(I2C_BusHandle);
		}
	  if(new_touch != last_touch)
	  {
		  if(new_touch == 0)
		  {
			  char C = convert_to_char(last_touch);
			  if(C != '\0' && C != '*' && C != '#')
			  {
				  if(len_key_board < 16)
				  {
					  key_board[len_key_board++] = C;
					  key_board[len_key_board] = '\0';
				  }
				  else
				  {
					  memmove(key_board,&key_board[1],sizeof(key_board) - 1);
					  key_board[len_key_board - 1] = C;
					  key_board[len_key_board] = '\0';
				  }
				  char sen_to_lcd[20];
				  sprintf(sen_to_lcd, "Detect Touch %d",count_touch);
				  osMessagePut(StringPrintHandle, (uint32_t)sen_to_lcd, osWaitForever);
			  }
			  else if(C == '#' && len_key_board > 0)
			  {
//				  osMessagePut(StringPrintHandle, (uint32_t)indicate_1, osWaitForever);
				  osMessagePut(StringPrintHandle, (uint32_t)key_board, osWaitForever);
				  key_board[strlen(key_board)+1] = UPGRADE(CODE_KEY);
				  osMessagePut(StringRS485Handle, (uint32_t)key_board, osWaitForever);
				  len_key_board = 0;
			  }
			  else if(C == '*' && len_key_board > 0)
			  {
				  key_board[--len_key_board] = '\0';
				  osMessagePut(StringPrintHandle, (uint32_t)key_board, osWaitForever);
			  }
		  }
	  }
	  last_touch = new_touch;
	  osDelay(20);
	  UserDelayUntil(&GetTick,KeyB_PM[T_INDEX]);
  }
  /* USER CODE END Key_Task */
}

/* USER CODE BEGIN Header_TX_Task */
/**
* @brief Function implementing the TXUARTTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TX_Task */
void TX_Task(void const * argument)
{
  /* USER CODE BEGIN TX_Task */
  /* Infinite loop */
	char *receivedMessageTX = NULL;
	char send_to_bus_rs485[50];
	osEvent event;
	uint32_t GetTick = 0;
  for(;;)
  {
	  event = osMessageGet(StringRS485Handle, osWaitForever);
	  if (event.status == osEventMessage) {
		  receivedMessageTX = (char*)event.value.v;
		  memset(send_to_bus_rs485,0,sizeof(send_to_bus_rs485));
		  uint8_t Code = receivedMessageTX[strlen(receivedMessageTX)+1];
		  memcpy(send_to_bus_rs485, receivedMessageTX, strlen(receivedMessageTX));
		  gen_data_send_rs485(Code,send_to_bus_rs485);
		  if(HAL_UART_Transmit(&huart1, (uint8_t *)send_to_bus_rs485, strlen(send_to_bus_rs485) , 1000) == HAL_OK)
		  {
			  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		  }
	  }
	  osDelay(20);
	  UserDelayUntil(&GetTick,TX_PM[T_INDEX]);
  }
  /* USER CODE END TX_Task */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
