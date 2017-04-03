#include "main.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_dac.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_tim.h"

#include "DAC_AD1934.h"

/*******************************************************************/
// Объявляем переменные
GPIO_InitTypeDef  GPIO_InitStructure;





uint8_t state = 0x00;
uint8_t res = 0x00;
uint8_t error = 0x00;



static void I2S_Config(void);
static void SysTickConfig(void);
void ParamDACConf();

// Проверка диодов 
void initDIOD()
{
GPIO_InitTypeDef  GPIO_InitStructure;
  

RCC_AHB1PeriphClockCmd(RCC_PIN9, ENABLE);
RCC_AHB1PeriphClockCmd(RCC_PIN10, ENABLE);
RCC_AHB1PeriphClockCmd(RCC_PIN11, ENABLE);
RCC_AHB1PeriphClockCmd(RCC_DIOD1, ENABLE);
RCC_AHB1PeriphClockCmd(RCC_DIOD2, ENABLE);

GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  // направление - выход
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  // Двухтактный выход
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  // Без подтяжки
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  // Скорость низкая

GPIO_InitStructure.GPIO_Pin = PIN9 | PIN10 | PIN11 | DIOD1;

GPIO_Init(GPIOD, &GPIO_InitStructure);  // Функция, выполняющая настройку портов

GPIO_InitStructure.GPIO_Pin = DIOD2;

    GPIO_Init(GPIO_DIOD2, &GPIO_InitStructure);  // Функция, выполняющая настройку портов
    GPIO_SetBits(GPIO_PIN9, PIN9);
    GPIO_SetBits(GPIO_PIN10, PIN10);
    GPIO_SetBits(GPIO_PIN11, PIN11);
    GPIO_SetBits(GPIO_DIOD1, DIOD1);
    GPIO_ResetBits(GPIO_DIOD2, DIOD2);
}

// Проверка питания
void initPOWER()
{
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  // направление - выход
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  // Двухтактный выход
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  // Без подтяжки
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  // Скорость низкая

GPIO_InitStructure.GPIO_Pin = POWER_CONTROL_1;
GPIO_Init(GPIO_POWER_CONTROL_1, &GPIO_InitStructure);
GPIO_ResetBits(GPIO_POWER_CONTROL_1, POWER_CONTROL_1);

GPIO_InitStructure.GPIO_Pin = POWER_CONTROL_2;
GPIO_Init(GPIO_POWER_CONTROL_2, &GPIO_InitStructure);
GPIO_SetBits(GPIO_POWER_CONTROL_2, POWER_CONTROL_2);

GPIO_InitStructure.GPIO_Pin = POWER_CONTROL_4;
GPIO_Init(GPIO_POWER_CONTROL_4, &GPIO_InitStructure);
GPIO_ResetBits(GPIO_POWER_CONTROL_4, POWER_CONTROL_4);

/* Перемычка
GPIO_InitStructure.GPIO_Pin = POWER_CONTROL_5;
GPIO_Init(GPIO_POWER_CONTROL_5, &GPIO_InitStructure);
GPIO_ResetBits(GPIO_POWER_CONTROL_5, POWER_CONTROL_5);
GPIO_SetBits(GPIO_POWER_CONTROL_5, POWER_CONTROL_5);
*/
}

void initTIMER()
{
  TIM_TimeBaseInitTypeDef timer;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
 
    TIM_TimeBaseStructInit(&timer);
    timer.TIM_Prescaler = 84-1;
    timer.TIM_Period = 1000-1;
    TIM_TimeBaseInit(TIM2, &timer);
    
    // настраиваем прерывание по переполнению таймера
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
    NVIC_EnableIRQ(TIM2_IRQn);
}

int main()
{
    // Разрешаем прерывания
    __enable_irq();
    
   /*
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    */
 
    // Вызываем функцию инициализации
    initDIOD();
    initPOWER();
    initTIMER();
    
    DAC_AD1934_POWER_CONF(ENABLE);
    DAC_AD1934_SPI_STANDALONE();
    

    

    
    
    
    DAC_AD1934_I2S_Config();
    
        
    /* SysTick configuration ---------------------------------------------------*/
    //SysTickConfig();



    DAC_AD1934_I2S_Config();
    while(1)
    {    

      DAC_AD1934_I2S_SEND();
      
    } 
}

void TIM2_IRQHandler()
{
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    GPIO_PIN9->ODR ^= PIN9;
}

void ParamDACConf()
{
    /*
    Address 0 - PLL and Clock Control 0
      0         0 - PLL power-down: Normal operation
?    2:1         00 - MCLK pin functionality (PLL active): INPUT 256
    4:3         11 - MCLKO pin: OFF
    6:5         01 - PLL input: DLRCLK
      7         1 - Internal MCLK enable: Enable: DAC active
    */
    DAC_AD1934_SPI_SEND(0x00, 0xB8);
    if (DAC_AD1934_SPI_READ(0x00) != 0xB8)
    {
      error++;
    }
    
    /*
    Address 1 - PLL and Clock Control 1
      0         0 - DAC clock source select: PLL clock
      1         0 - Clock source select: PLL clock
      2         0 - On-chip voltage reference
?      3         0 - PLL lock indicator (read-only): Not locked
    7:4         0000 - Reserved
    */
    DAC_AD1934_SPI_SEND(0x01, 0x00);
    if (DAC_AD1934_SPI_READ(0x01) != 0x00)
    {
      error++;
    }
    
    /*
    Address 2 - DAC Control 0
      0         0 - Power-down: Normal
?    2:1         00 - Sample rate: 48kHz
?    5:3         000 - SDATA delay (BCLK periods): 1
    7:6         01 - Serial format: TDM (daisy chain)
    */
    DAC_AD1934_SPI_SEND(0x02, 0x40);
    if (DAC_AD1934_SPI_READ(0x02) != 0x40)
    {
      error++;
    }
    
    /*
    Address 3 - DAC Control 1
?      0         0 - BCLK active edge (TDM in): Latch in midcycle (normal)
    2:1         10 - BCLKs per frame: 256 (8 channels)
      3         0 - LRCLK polarity: Left low
      4         0 - LRCLK master/slave: Slave
      5         0 - BCLK master/slave: Slave
      6         0 - BCLK source: DBCLK pin
?      7         0 - BCLK polarity: Normal
    */
    DAC_AD1934_SPI_SEND(0x04, 0x04);
    if (DAC_AD1934_SPI_READ(0x04) != 0x04)
    {
      error++;
    }

}


static void SysTickConfig(void)
{
  /* Setup SysTick Timer for 10ms interrupts  */
  if (SysTick_Config(SystemCoreClock / 100))
  {
    /* Capture error */
    while (1);
  }
  /* Configure the SysTick handler priority */
  NVIC_SetPriority(SysTick_IRQn, 0x0);
}


