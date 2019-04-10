
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f3xx_hal.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "I2C_LCD.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
volatile uint32_t tim6_overflows = 0;
#define SystemCoreClockInMHz (SystemCoreClock/1000000)
typedef enum state_t {
	 IDLE_S,
	 TRIGGERING_S,
	 WAITING_FOR_ECHO_START_S,
	 WAITING_FOR_ECHO_STOP_S,
	 TRIG_NOT_WENT_LOW_S,
	 ECHO_TIMEOUT_S,
	 ECHO_NOT_WENT_LOW_S,
	 READING_DATA_S,
	 ERROR_S
} state_t;

volatile state_t state = IDLE_S;
volatile uint32_t echo_start;
volatile uint32_t echo_finish;
volatile uint32_t measured_time;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if( htim->Instance == TIM6 )
	{
		++tim6_overflows;
	}
}
#define USE_TIM6_TIMING 1

inline void TIM6_reinit()
{
	 HAL_TIM_Base_Stop(&htim6);
	 __HAL_TIM_SET_PRESCALER( &htim6, (SystemCoreClockInMHz-1) );
	 __HAL_TIM_SET_COUNTER( &htim6, 0 );
	 tim6_overflows = 0;
	 HAL_TIM_Base_Start_IT(&htim6);
}

inline uint32_t get_tim6_us()
{
	 __HAL_TIM_DISABLE_IT(&htim6, TIM_IT_UPDATE);
	 //__disable_irq();
	 uint32_t res = tim6_overflows * 10000 + __HAL_TIM_GET_COUNTER(&htim6);
	 //__enable_irq();
	 __HAL_TIM_ENABLE_IT(&htim6, TIM_IT_UPDATE);
	 return res;
}

inline void udelay_TIM6(uint32_t useconds) {
	 uint32_t before = get_tim6_us();
	 while( get_tim6_us() < before+useconds){}
}

void init_timing()
{
	#ifdef  USE_HAL_DELAY_AND_ASM
		return;
	#elif defined USE_DWT_TIMING
		DWT_Init();
	#elif defined USE_TIM6_TIMING
		TIM6_reinit();
	#else
		#error "Unknown timing method."
	#endif
}

inline uint32_t get_us()
{
	#ifdef  USE_HAL_DELAY_AND_ASM
		return 1000*HAL_GetTick();
	#elif defined USE_DWT_TIMING
		return get_DWT_us();
	#elif defined USE_TIM6_TIMING
		return get_tim6_us();
	#else
		#error "Unknown timing method."
	#endif
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	 if (GPIO_Pin == ECHOI_Pin )
	 {
		 switch (state) {
		 	 case WAITING_FOR_ECHO_START_S: {
		 		 echo_start =  get_us();
		 		 state = WAITING_FOR_ECHO_STOP_S;
		 		 break;
		 	 }
		 	 case WAITING_FOR_ECHO_STOP_S: {
		 		 echo_finish = get_us();
		 		 measured_time = echo_finish - echo_start;
		 		 state = READING_DATA_S;
		 		 break;
		 	 }
		 	 default:
		 		 state = ERROR_S;
		 	 }
	 }
}

inline void udelay(uint32_t useconds)
{
	#ifdef  USE_HAL_DELAY_AND_ASM
	 	 udelay_asm(useconds);
	#elif defined USE_DWT_TIMING
	 	 udelay_DWT(useconds);
	#elif defined USE_TIM6_TIMING
	 	 udelay_TIM6(useconds);
	#else
		#error "Unknown timing method."
	#endif
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  extern void initialise_monitor_handles(void);
  initialise_monitor_handles();

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  init_timing();
  /* USER CODE BEGIN WHILE */
  HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_RESET);
  LCD_Init(0x27<<1, hi2c1);
    while (1)
    {
     LCD_Clear();
     HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_SET);
     udelay(16);
     HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_RESET);

     state = WAITING_FOR_ECHO_START_S;
     while( state == WAITING_FOR_ECHO_START_S && state != ERROR_S )
     {}
     if ( state == ERROR_S )
     {
    	 LCD_SendString(0x27<<1, "Unexpected error");
    	 LCD_Print(1, "Waiting for ECHO Start");
		 printf("Unexpected error while waiting for ECHO to start.\n");
		 continue;
     }
     while( state == WAITING_FOR_ECHO_STOP_S && state != ERROR_S )
     {}
     if ( state == ERROR_S )
     {
    	 LCD_SendString(0x27<<1, "Unexpected error");
    	 LCD_Print(1, "Waiting for ECHO finish");
		 printf("Unexpected error while waiting for ECHO to finish.\n");
		 continue;
     }

     uint32_t distance = measured_time/58;
     if(distance >= 500 || measured_time <= 250){
    	 LCD_SendString(0x27<<1, "Distance is too");
    	 LCD_Print(1, "long or too short!");
    	 printf("Distance is too long or too short!\n");
     }else{
    	 LCD_SendString(0x27<<1, "Time: ");
    	 LCD_SendInt(measured_time, 10);
    	 LCD_PPrint(0, 12, "us");
    	 LCD_Print(1, "Distance:");
    	 LCD_PPrintInt(1, 10, distance, 10);
    	 LCD_PPrint(1, 14, "cm");
		 printf("Time: %lu us, distance: %lu cm\n",
				 measured_time,
				 distance);
     }
     udelay(100000);
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

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
