
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
  * COPYRIGHT(c) 2019 STMicroelectronics
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
#include "spi.h"
#include "usb.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "stm32f3_discovery_accelerometer.h"
#include "stm32f3_discovery_gyroscope.h"
#include "I2C_LCD.h"
#include "math.h"
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
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_SPI1_Init();
  MX_USB_PCD_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */
  LCD_Init(0x27<<1, hi2c2);
  if(BSP_ACCELERO_Init() != HAL_OK)
    {
      /* Initialization Error */
	  LCD_Print(0, "Error initializing H");
	  LCD_Print(1, "AL.");
      while(1){}
    }
  if(BSP_GYRO_Init() != HAL_OK)
      {
        /* Initialization Error */
	  LCD_Print(0, "Error initializing H");
	  LCD_Print(1, "AL.");
        while(1){}
      }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  int16_t buffer[3] = {0};
  float gyro_buffer[3] = {0.0};
  double m_l = 0.0;
  float m_l_g = 0.0;
  double m_x = 0.0;
  double m_y = 0.0;
  double m_z = 0.0;
  float m_x_g = 0.0;
  float m_y_g = 0.0;
  float m_z_g = 0.0;
  while (1)
  {
	  LCD_Clear();
	  BSP_ACCELERO_GetXYZ(buffer);
	  double x = (double)(buffer[0]/16)/10000.0;
	  double y = (double)(buffer[1]/16)/1000.0;
	  double z = (double)(buffer[2]/16)/1000.0;
	  double l = (double)sqrt(x*x+y*y+z*z);
	  BSP_GYRO_GetXYZ(gyro_buffer);
	  float x_g = (float)(gyro_buffer[0])/1000.0;
	  float y_g = (float)(gyro_buffer[1])/1000.0;
	  float z_g = (float)(gyro_buffer[2])/1000.0;
	  float l_g = (float)sqrt(x_g*x_g+y_g*y_g+z_g*z_g);

	  LCD_PPrint(0, 0, "ACCELERO.X:");
	  LCD_PPrintDouble(0, 11, x);
	  LCD_PPrint(1, 0,  "Y:");
	  LCD_PPrintDouble(1, 2, y);
	  LCD_PPrint(1, 9, "Z:");
	  LCD_PPrintDouble(1, 11, z);

	  LCD_PPrint(2, 0, "GYRO.X:");
	  LCD_PPrintDouble(2, 7, x_g);
	  LCD_PPrint(3, 0, "Y:");
	  LCD_PPrintFloat(3, 2, y_g);
	  LCD_PPrint(3, 9, "Z:");
	  LCD_PPrintFloat(3, 11, z_g);

	  HAL_Delay(1000);

	  LCD_Clear();
	  LCD_PPrint(0, 0, "ACCELERO LENGTH:");
	  LCD_PPrintDouble(1, 0, l);
	  LCD_PPrint(2, 0, "GYRO LENGTH:");
	  LCD_PPrintFloat(3, 0, l_g);

	  HAL_Delay(1000);

	  LCD_Clear();

	  if (m_l < l){
		  m_l = l;
		  m_x = x;
		  m_y = y;
		  m_z = z;
	  }

	  if (m_l_g < l_g){
		  m_l_g = l_g;
		  m_x_g = x_g;
		  m_y_g = y_g;
		  m_z_g = z_g;
	  }

	  LCD_PPrint(0, 0, "ACCELEROM.X:");
	  LCD_PPrintDouble(0, 12, m_x);
	  LCD_PPrint(1, 0,  "Y:");
	  LCD_PPrintDouble(1, 2, m_y);
	  LCD_PPrint(1, 9, "Z:");
	  LCD_PPrintDouble(1, 11, m_z);

	  LCD_PPrint(2, 0, "GYROM.X:");
	  LCD_PPrintDouble(2, 8, m_x_g);
	  LCD_PPrint(3, 0, "Y:");
	  LCD_PPrintFloat(3, 2, m_y_g);
	  LCD_PPrint(3, 9, "Z:");
	  LCD_PPrintFloat(3, 11, m_z_g);

	  HAL_Delay(1000);

	  LCD_Clear();
	  LCD_PPrint(0, 0, "ACCELERO MAX LENGTH:");
	  LCD_PPrintDouble(1, 0, m_l);
	  LCD_PPrint(2, 0, "GYRO MAX LENGTH:");
	  LCD_PPrintFloat(3, 0, m_l_g);
	  HAL_Delay(1000);

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_I2C2;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_HSI;
  PeriphClkInit.USBClockSelection = RCC_USBCLKSOURCE_PLL;
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
