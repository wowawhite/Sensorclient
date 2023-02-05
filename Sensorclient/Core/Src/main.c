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
#include "crc.h"
#include "dma2d.h"
#include "ltdc.h"
#include "lwip.h"
#include "pdm2pcm.h"
#include "rng.h"
#include "rtc.h"
#include "sai.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32746g_discovery.h"  // get LED and BUTTONS stuff
#include "stm32746g_discovery_sdram.h"
#include "helperfunctions.h"
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
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

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CRC_Init();
  MX_FMC_Init();
  MX_RTC_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_RNG_Init();
  MX_LWIP_Init();
  MX_PDM2PCM_Init();
  MX_SAI1_Init();
  MX_DMA2D_Init();
  MX_LTDC_Init();
  /* USER CODE BEGIN 2 */
  // Start SDRAM Config
	if (BSP_SDRAM_Init() != SDRAM_OK)
	{
		Error_Handler();
	}
  /**
   * array to test SDram
   * BSP_SDRAM_WriteData_DMA();
   * BSP_SDRAM_ReadData_DMA();
   * from driver .h file: DMA_CHANNEL_0, DMA2_Stream0
   *
   */

	//linker script definition
	//https://stackoverflow.com/questions/48561217/how-to-get-value-of-variable-defined-in-ld-linker-script-from-c/54728097#54728097
/*	extern uint8_t _sdram_data_begin[];
	extern uint8_t _sdram_data_end[];
	uint32_t start_of_sdram = (uint32_t)_sdram_data_begin;
	uint32_t end_of_sdram = (uint32_t)_sdram_data_end;
	char sdram_buffer[SDRAM_BUFFER_SIZE] __attribute__((section (".sdram_data")));
	*/

// SDRAM manual test
	const uint32_t testdatasize = 10;
	//uint32_t sourcearray[testdatasize];
	uint32_t targetarray[testdatasize];

	uint32_t SDRAM_absolute_start = SDRAM_DEVICE_ADDR;
	uint32_t SDRAM_absolute_end = SDRAM_DEVICE_ADDR+SDRAM_DEVICE_SIZE;
	volatile uint32_t SDRAM_datapointer = SDRAM_absolute_start;
	volatile uint32_t SDRAM_dummydata = 0;
	// fill array start

	for(uint32_t i=0; i< SDRAM_absolute_end; i+4)
	{
		SDRAM_datapointer = SDRAM_datapointer + i;  // TODO: bug with SDRAM_datapointer
		SDRAM_dummydata = (SDRAM_dummydata+i) % 0xFFFFFFF;  // modulo for max uint32_t

//		if( BSP_SDRAM_WriteData(SDRAM_datapointer, &SDRAM_dummydata, 1 ) != SDRAM_OK ){
//			  Error_Handler();
//		}

	}

  /* startadress from driver .h file
   * SDRAM_DEVICE_ADDR  ((uint32_t)0xC0000000)
   * #define SDRAM_DEVICE_SIZE  ((uint32_t)0x800000)  // SDRAM device size in MBytes
   * 64 Mbit = 8 MByte, 0x800000 = 8 * 1024 * 1024
   */

/*
  if( BSP_SDRAM_WriteData(RAM_relative_addr, sourcearray, testdatasize ) != SDRAM_OK ){
	  Error_Handler();
  }*/

  if(BSP_SDRAM_ReadData(SDRAM_absolute_start-testdatasize, targetarray, testdatasize ) != SDRAM_OK ){
	  Error_Handler();
  }
/*  for(int i=0; i<testdatasize; i++){
	  targetarray[i] = sourcearray[i];

  }*/

  //uint32_t newline[5] = {0x41,0x2d,0x42, 0x0d, 0x0a};  // W-G CR LF
  uint8_t printline[6] = {0xAF,0x00,0x00,0xFE,0x0d,0x0a};

  size_t printline_size = ARRAY_LENGTH(printline);
  size_t sendonce = 0;

/*
  if (HAL_UART_Transmit(&huart1, start_of_sdram, 4, 1000) != HAL_OK)  // sending to Linux /dev/ttyACM0, can only transmit 16 bis max
  {
	  Error_Handler();
  }
*/
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

/*	  //USERBUTTON_Pin
	  if (HAL_UART_Transmit(&huart1, newline, newline_size, 1000) != HAL_OK)  // sending to Linux /dev/ttyACM0
	  {
		  Error_Handler();
	  }
	  HAL_Delay(1000);
	  if (HAL_UART_Transmit(&huart1, testdatasize, 1, 1000) != HAL_OK)  // sending to Linux /dev/ttyACM0
	  {
		  Error_Handler();
	  }
	  HAL_Delay(1000);

*/

	if(sendonce == 0)
	{
		if(HAL_GPIO_ReadPin(USERBUTTON_GPIO_Port, USERBUTTON_Pin))
		{
			for(int i=0; i<testdatasize; i++)
			{
				printline[0] = (uint8_t)targetarray[i]>>4;
				printline[1] = (uint8_t)targetarray[i]>>8;
				printline[2] = (uint8_t)targetarray[i]>>12;
				printline[3] = (uint8_t)targetarray[i]>>16;

				if (HAL_UART_Transmit(&huart1, printline, printline_size, 1000) != HAL_OK)  // sending to Linux /dev/ttyACM0
				{
				  Error_Handler();
				}
			}
			sendonce = 1;
		}
	}

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 400;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC|RCC_PERIPHCLK_SAI1
                              |RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 384;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 5;
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV8;
  PeriphClkInitStruct.PLLSAIDivQ = 1;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_8;
  PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLSAI;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLLSAIP;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
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
	//TODO: Error assertion
	// https://stackoverflow.com/questions/53123403/how-to-find-which-function-is-calling-another-function-in-c
	#ifdef DEBUG
	  asm("BKPT #0");
	#else
	  while(1) {
		HAL_Delay(250);
		HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);// user LEDs are used by SD-RAM
	  }
	#endif


	// print errorcode line here
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
