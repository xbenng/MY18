
#include "main.h"

#include <stdbool.h>
#include "stdio.h"

#ifdef __GNUC__
  /* With GCC Compilers, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

USART_HandleTypeDef  USARTHandle;
CAN_HandleTypeDef    CanHandle;

// static void CAN_Config(void);
static void SystemClock_Config(void);
static void Error_Handler(void);

int main(void)
{
  HAL_Init();
  
  SystemClock_Config();

  Can_Init(500000);

  // Setup an LED for debugging
  GPIO_InitTypeDef gpioinit;
  LED_CLK_ENABLE();
  gpioinit.Pin = LED_PIN;
  gpioinit.Mode = GPIO_MODE_OUTPUT_PP;
  gpioinit.Pull = GPIO_PULLUP;
  gpioinit.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(LED_PORT, &gpioinit);

  // SETUP THE CONTACTOR GPIOS
  initLowSideContactor();
  initHighSideContactor();

  // Setup USART for debugging
  USARTHandle.Instance = USARTx_INSTANCE;
  USARTHandle.Init.BaudRate = 115200;
  USARTHandle.Init.WordLength = USART_WORDLENGTH_8B;
  USARTHandle.Init.StopBits = USART_STOPBITS_1;
  USARTHandle.Init.Parity = USART_PARITY_NONE;
  USARTHandle.Init.Mode = USART_MODE_TX_RX;
  USARTHandle.Init.CLKPolarity = USART_POLARITY_LOW;
  USARTHandle.Init.CLKPhase = USART_PHASE_1EDGE;
  USARTHandle.Init.CLKLastBit = USART_LASTBIT_DISABLE;

  if(HAL_USART_Init(&USARTHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  // Toggle the LED after this regular setup
  // HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
  // HAL_Delay(1000);
  // HAL_GPIO_TogglePin(LED_PORT, LED_PIN);

  printf("\r\nMEGALV PERIPHERALS ONLINE\r\n");

  setupVCU();

  while(1)
  {
    loopVCU(&USARTHandle);
  } 
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  HAL_USART_Transmit(&USARTHandle, (uint8_t *)&ch, 1, 0xFFFF); 

  return ch;
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 120000000
  *            HCLK(Hz)                       = 120000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 240
  *            PLL_P                          = 2
  *            PLL_Q                          = 5
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 3
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 24;
  RCC_OscInitStruct.PLL.PLLN = 240;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

static void Error_Handler(void)
{
  while(1)
  {
  }
}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* CanHandle)
{
  handleCanVCU(CanHandle);

  /* Receive */
  if(HAL_CAN_Receive_IT(CanHandle, CAN_FIFO0) != HAL_OK)
  {
    /* Reception Error */
    Error_Handler();
  }
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

