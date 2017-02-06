#include "main.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_dac.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_tim.h"

/*******************************************************************/
// Объявляем переменные
GPIO_InitTypeDef  GPIO_InitStructure;
I2S_InitTypeDef I2S_InitStructure;

#define SPI1_NSS GPIO_Pin_6
#define SPI1_SCK GPIO_Pin_7
#define SPI1_MOSI GPIO_Pin_8
#define SPI1_MISO GPIO_Pin_9
//#define I2S2_MCK GPIO_Pin_6
//#define I2S2_SD GPIO_Pin_3
//#define I2S2_WS GPIO_Pin_12
//#define I2S2_CK GPIO_Pin_10

#define GPIO_Source_I2S2_MCK GPIO_PinSource6
#define GPIO_Source_I2S2_SD GPIO_PinSource3
#define GPIO_Source_I2S2_WS GPIO_PinSource12
#define GPIO_Source_I2S2_CK GPIO_PinSource10

#define countof(a)   (sizeof(a) / sizeof(*(a)))
/* I2Sx Communication boards Interface */
#define I2Sx                           SPI2
#define I2Sxext                        I2S2ext
#define I2Sx_CLK                       RCC_APB1Periph_SPI2
#define I2Sx_CLK_INIT                  RCC_APB1PeriphClockCmd

#define I2Sx_WS_PIN                    GPIO_Pin_12
#define I2Sx_WS_GPIO_PORT              GPIOB
#define I2Sx_WS_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define I2Sx_WS_SOURCE                 GPIO_PinSource12
#define I2Sx_WS_AF                     GPIO_AF_SPI2

#define I2Sx_CK_PIN                    GPIO_Pin_10
#define I2Sx_CK_GPIO_PORT              GPIOB
#define I2Sx_CK_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define I2Sx_CK_SOURCE                 GPIO_PinSource10
#define I2Sx_CK_AF                     GPIO_AF_SPI2

#define I2Sx_SD_PIN                    GPIO_Pin_3
#define I2Sx_SD_GPIO_PORT              GPIOC
#define I2Sx_SD_GPIO_CLK               RCC_AHB1Periph_GPIOC
#define I2Sx_SD_SOURCE                 GPIO_PinSource3
#define I2Sx_SD_AF                     GPIO_AF_SPI2

#define I2Sxext_SD_PIN                 GPIO_Pin_2
#define I2Sxext_SD_GPIO_PORT           GPIOI
#define I2Sxext_SD_GPIO_CLK            RCC_AHB1Periph_GPIOI
#define I2Sxext_SD_SOURCE              GPIO_PinSource2
#define I2Sxext_SD_AF                  GPIO_AF_SPI3

#define TX_MASTER_BUFFERSIZE           (countof(aTxMasterBuffer) - 1)
#define RX_MASTER_BUFFERSIZE           TX_MASTER_BUFFERSIZE

#define TX_SLAVE_BUFFERSIZE           (countof(aTxSlaveBuffer) - 1)
#define RX_SLAVE_BUFFERSIZE           TX_SLAVE_BUFFERSIZE


uint8_t state = 0x00;

const uint16_t aTxMasterBuffer[] =
  {
    0x1234, 0x5678, 0x1ABC, 0xDEF0, 0x5555,
    0x1748, 0x9027, 0x4424, 0x9623, 0x0932,
    0x9744, 0x7845, 0x0912, 0x1111, 0x1772,
    0x9736, 0x0933, 0x0187, 0x9567, 0x9977
  };

__IO uint16_t aRxMasterBuffer [RX_MASTER_BUFFERSIZE];
__IO uint8_t  ubBufferCounter = 0;

static void I2S_Config(void);
static void SysTickConfig(void);
static void Fill_Buffer(__IO uint8_t *pBuffer, uint16_t BufferLength);

// Проверка диодов 
void initDIOD()
{
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

// Проверка ЦАП
void initDAC()
{
//Standalone Mode
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  // направление - выход
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  // Двухтактный выход
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  // Без подтяжки
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  // Скорость низкая

GPIO_InitStructure.GPIO_Pin = SPI1_MISO;
GPIO_Init(GPIO_SPI1_MISO, &GPIO_InitStructure);
GPIO_ResetBits(GPIO_SPI1_MISO, SPI1_MISO);

GPIO_InitStructure.GPIO_Pin = SPI1_SCK;
GPIO_Init(GPIO_SPI1_SCK, &GPIO_InitStructure);
GPIO_ResetBits(GPIO_SPI1_SCK, SPI1_SCK);

GPIO_InitStructure.GPIO_Pin = SPI1_NSS;
GPIO_Init(GPIO_SPI1_NSS, &GPIO_InitStructure);
GPIO_ResetBits(GPIO_SPI1_NSS, SPI1_NSS);


//Reset DAC
GPIO_InitStructure.GPIO_Pin = POWER_CONTROL_3;
GPIO_Init(GPIO_POWER_CONTROL_3, &GPIO_InitStructure);
GPIO_ResetBits(GPIO_POWER_CONTROL_3, POWER_CONTROL_3);

/* I2S configuration -------------------------------------------------------*/
I2S_Config();
}

void initTIMER()
{
  TIM_TimeBaseInitTypeDef timer;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
 
    TIM_TimeBaseStructInit(&timer);
    timer.TIM_Prescaler = 1000;
    timer.TIM_Period = 10000;
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
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    
 
    // Вызываем функцию инициализации
    initDIOD();
    initPOWER();
    initTIMER();
    initDAC();
        
    /* SysTick configuration ---------------------------------------------------*/
    SysTickConfig();




    while(1)
    {    
      /* Master full Duplex Communication ----------------------------------------*/
      /* Communication Full Duplex Started */
      ubBufferCounter = 0;
      while ((ubBufferCounter != TX_MASTER_BUFFERSIZE))
      {
        /* Data to transmitted through I2Sx SD pin */
        while (SPI_I2S_GetFlagStatus(I2Sx, SPI_I2S_FLAG_TXE ) != SET);
        SPI_I2S_SendData(I2Sx, aTxMasterBuffer[ubBufferCounter]);
        ubBufferCounter++;
      }			
    } 
}

void TIM2_IRQHandler()
{
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    GPIO_PIN9->ODR ^= PIN9;
}


static void I2S_Config(void)
{
  /* Peripheral Clock Enable -------------------------------------------------*/
  /* Enable the I2Sx/I2Sx_ext clock */
  I2Sx_CLK_INIT(I2Sx_CLK, ENABLE);
  
  /* Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(I2Sx_WS_GPIO_CLK | I2Sx_CK_GPIO_CLK | 
                         I2Sx_SD_GPIO_CLK | I2Sxext_SD_GPIO_CLK, ENABLE);

  /* I2S GPIO Configuration --------------------------------------------------*/
  /* Connect I2S pins to Alternate functions */  
  GPIO_PinAFConfig(I2Sx_WS_GPIO_PORT, I2Sx_WS_SOURCE, I2Sx_WS_AF);
  GPIO_PinAFConfig(I2Sx_CK_GPIO_PORT, I2Sx_CK_SOURCE, I2Sx_CK_AF);
  GPIO_PinAFConfig(I2Sx_SD_GPIO_PORT, I2Sx_SD_SOURCE, I2Sx_SD_AF);
  GPIO_PinAFConfig(I2Sxext_SD_GPIO_PORT, I2Sxext_SD_SOURCE, I2Sxext_SD_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

  /* I2S WS pin configuration */
  GPIO_InitStructure.GPIO_Pin = I2Sx_WS_PIN;
  GPIO_Init(I2Sx_WS_GPIO_PORT, &GPIO_InitStructure);

  /* I2S CK pin configuration */
  GPIO_InitStructure.GPIO_Pin =  I2Sx_CK_PIN;
  GPIO_Init(I2Sx_CK_GPIO_PORT, &GPIO_InitStructure);
  
  /* I2S SD pin configuration */
  GPIO_InitStructure.GPIO_Pin = I2Sx_SD_PIN;
  GPIO_Init(I2Sx_SD_GPIO_PORT, &GPIO_InitStructure);

  /* I2S Extended SD pin configuration */
  GPIO_InitStructure.GPIO_Pin =  I2Sxext_SD_PIN;
  GPIO_Init(I2Sxext_SD_GPIO_PORT, &GPIO_InitStructure);
 
  /* I2S configuration -------------------------------------------------------*/
  /* Initialize  I2Sx and I2Sxext peripherals */
  SPI_I2S_DeInit(I2Sx);
  /* Configure the Audio Frequency, Standard and the data format */
  I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_8k;
  I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
  I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
  I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
  I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
  
  /* Master full Duplex configuration ----------------------------------------*/
  /* Clear the Rx Master Buffer */
  Fill_Buffer((uint8_t*)aRxMasterBuffer, (TX_MASTER_BUFFERSIZE*2));

  /* Configure I2Sx in Master Transmitter Mode */
  I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
  I2S_Init(I2Sx, &I2S_InitStructure);
  
  /* Configure the I2Sx_ext (the second instance) in Slave Receiver Mode */
  I2S_FullDuplexConfig(I2Sxext, &I2S_InitStructure);
  
  /* Enable the I2Sx peripheral */
  I2S_Cmd(I2Sx, ENABLE);
  /* Enable the I2Sx_ext peripheral for Full Duplex mode */ 
  I2S_Cmd(I2Sxext, ENABLE);
  /* Master full Duplex Communication ----------------------------------------*/
  /* Communication Full Duplex Started */
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


static void Fill_Buffer(__IO uint8_t *pBuffer, uint16_t BufferLength)
{
  uint32_t index = 0;

  /* Put in global buffer same values */
  for (index = 0; index < BufferLength; index++ )
  {
    pBuffer[index] = 0x00;
  }
}