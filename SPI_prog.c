#include "main.h"

const uint8_t num = 3;
uint8_t key = 4, i = 0, j = 0, delay = 0;
uint8_t bytes[] = {0xAA, 0xF0, 0xB2};

extern void SPI_prog_Reset(void);
extern void SPI_prog_Send_Byte(uint8_t *Buf);
void SPI_prog_GPIO_Configuration(void);
void SPI_prog_TIM_Config(void);

extern void SPI_prog_Reset(void) 
{
  SPI_prog_GPIO_Configuration();
  GPIO_ResetBits(GPIO_SPI1_MISO, SPI1_MISO);
  GPIO_ResetBits(GPIO_SPI1_SCK, SPI1_SCK);
  GPIO_ResetBits(GPIO_SPI1_NSS, SPI1_NSS);
}

extern void SPI_prog_Send_Data(void) 
{
  SPI_prog_GPIO_Configuration();
  GPIO_ResetBits(GPIO_SPI1_MISO, SPI1_MISO);
  GPIO_ResetBits(GPIO_SPI1_SCK, SPI1_SCK);
  GPIO_SetBits(GPIO_SPI1_NSS, SPI1_NSS);
  
  SPI_prog_TIM_Config();
}

void SPI_prog_GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = SPI1_SCK | SPI1_MISO;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = SPI1_NSS;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = SPI1_MOSI;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
}

void SPI_prog_TIM_Config(void)
{
  TIM_TimeBaseInitTypeDef timer;
  uint32_t Period = 1000; //us
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  
  TIM_TimeBaseStructInit(&timer);
  timer.TIM_Prescaler = 21-1;
  timer.TIM_Period = Period-1;
  TIM_TimeBaseInit(TIM4, &timer);
  
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM4, ENABLE);
  NVIC_EnableIRQ(TIM4_IRQn);
}

void TIM4_IRQHandler()
{
  TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
  
  switch (key)
  {
  case 0:
    key++;
    if(((bytes[j]<<i)&0x80) == 0x80)
    {
      GPIO_SetBits(GPIO_SPI1_MISO, SPI1_MISO);
    }
    else
    {
      GPIO_ResetBits(GPIO_SPI1_MISO, SPI1_MISO);
    }
    i++;
    break;
  case 1:
    GPIO_SetBits(GPIO_SPI1_SCK, SPI1_SCK);
    key++;
    break;
  case 2:
    key++;
    break;
  case 3:
    GPIO_ResetBits(GPIO_SPI1_SCK, SPI1_SCK);
    if (i < 8)
    {
      key = 0;
    }
    else
    {
      if (j < num-1)
      {
        j++;
        key = 0;
        i = 0;
      }
      else
      {
        j = 0;
        i = 0;
        //GPIO_SPI1_NSS->ODR ^= SPI1_NSS;
        
        key = 4;
        GPIO_SetBits(GPIO_SPI1_NSS, SPI1_NSS);
        /*
        TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);
        TIM_Cmd(TIM4, DISABLE);
        NVIC_DisableIRQ(TIM4_IRQn);
        */
      } 
    }
    case 4:
      if (delay < 5)
      {
        
        GPIO_ResetBits(GPIO_SPI1_NSS, SPI1_NSS);
        delay++;
      }
      else
      {
        delay=0;
        key=0;
      }
      break;
  }
}