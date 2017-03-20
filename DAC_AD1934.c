#include "DAC_AD1934.h"

GPIO_InitTypeDef  GPIO_DAC_InitStructure;
TIM_TimeBaseInitTypeDef TIMER_DAC_InitStructure;

const uint32_t TIM_PER = 1000; //TIM_PER = Период SCK [us]
uint8_t SPI_DAC_KEY, SPI_DAC_i, SPI_DAC_j, TIM_KEY; //Переменные для TIM

uint8_t SPI_SEND_BYTES[] = {0x00, 0x00, 0x00}; //Массив байтов на отправку
uint8_t SPI_READ_BYTE = 0x00; //Байт на приём

void DAC_AD1934_SPI_GPIO_CONF(void);
void DAC_AD1934_TIM_Config(void);

void DAC_AD1934_POWER_CONF(uint8_t powerkey) 
{
  GPIO_DAC_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_DAC_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_DAC_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_DAC_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

  GPIO_DAC_InitStructure.GPIO_Pin = POWER_CONTROL_3;
  GPIO_Init(GPIO_POWER_CONTROL_3, &GPIO_DAC_InitStructure);
  
  switch (powerkey)
  {
  case 0:
    GPIO_ResetBits(GPIO_POWER_CONTROL_3, POWER_CONTROL_3);
    break;
  default:
    GPIO_SetBits(GPIO_POWER_CONTROL_3, POWER_CONTROL_3);
    break;
  }
}

void DAC_AD1934_SPI_GPIO_CONF(void)
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  
  GPIO_DAC_InitStructure.GPIO_Pin = SPI1_SCK | SPI1_MISO;
  GPIO_DAC_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_DAC_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_DAC_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_DAC_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOF, &GPIO_DAC_InitStructure);
  
  GPIO_DAC_InitStructure.GPIO_Pin = SPI1_NSS;
  GPIO_DAC_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_DAC_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_DAC_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_DAC_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIO_SPI1_NSS, &GPIO_DAC_InitStructure);
  
  GPIO_DAC_InitStructure.GPIO_Pin = SPI1_MOSI;
  GPIO_DAC_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_DAC_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_DAC_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIO_SPI1_MISO, &GPIO_DAC_InitStructure);
}

void DAC_AD1934_SPI_STANDALONE(void) 
{
  DAC_AD1934_SPI_GPIO_CONF();
  GPIO_ResetBits(GPIO_SPI1_MISO, SPI1_MISO);
  GPIO_ResetBits(GPIO_SPI1_SCK, SPI1_SCK);
  GPIO_ResetBits(GPIO_SPI1_NSS, SPI1_NSS);
}

void DAC_AD1934_SPI_SEND(uint8_t reg_adress, uint8_t data) 
{
  SPI_SEND_BYTES[0] = 0x08; /*The first byte is a global address with a 
  read/write bit. For the AD1934, the address is 0x04, shifted left 
  1 bit due to the R/W bit.*/
  SPI_SEND_BYTES[1] = reg_adress; /*The second byte is the AD1934 register address*/  
  SPI_SEND_BYTES[2] = data; /*The third byte is the data.*/
  
  DAC_AD1934_SPI_GPIO_CONF();
  GPIO_ResetBits(GPIO_SPI1_MISO, SPI1_MISO);
  GPIO_ResetBits(GPIO_SPI1_SCK, SPI1_SCK);
  GPIO_SetBits(GPIO_SPI1_NSS, SPI1_NSS);
  
  DAC_AD1934_TIM_Config();
  while(TIM_KEY);
}

uint8_t DAC_AD1934_SPI_READ(uint8_t reg_adress) 
{
  SPI_SEND_BYTES[0] = 0x09; /*The first byte is a global address with a 
  read/write bit. For the AD1934, the address is 0x04, shifted left 
  1 bit due to the R/W bit.*/
  SPI_SEND_BYTES[1] = reg_adress; /*The second byte is the AD1934 register address*/  
  SPI_SEND_BYTES[2] = 0x00; /*The third byte is the data.*/
  SPI_READ_BYTE = 0x00;
  
  DAC_AD1934_SPI_GPIO_CONF();
  GPIO_ResetBits(GPIO_SPI1_MISO, SPI1_MISO);
  GPIO_ResetBits(GPIO_SPI1_SCK, SPI1_SCK);
  GPIO_SetBits(GPIO_SPI1_NSS, SPI1_NSS);
  
  DAC_AD1934_TIM_Config();
  while(TIM_KEY);
  return SPI_READ_BYTE;
}

void DAC_AD1934_TIM_Config(void)
{
  
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  
  TIM_TimeBaseStructInit(&TIMER_DAC_InitStructure);
  TIMER_DAC_InitStructure.TIM_Prescaler = 21-1;
  TIMER_DAC_InitStructure.TIM_Period = TIM_PER-1;
  TIM_TimeBaseInit(TIM4, &TIMER_DAC_InitStructure);
  
  SPI_DAC_KEY = 0;
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM4, ENABLE);
  NVIC_EnableIRQ(TIM4_IRQn);
  TIM_KEY = 1;
}

void TIM4_IRQHandler()
{
  TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
  
  switch (SPI_DAC_KEY)
  {
  case 0:
    GPIO_SetBits(GPIO_SPI1_NSS, SPI1_NSS);
    SPI_READ_BYTE=0x00;
    SPI_DAC_j = 0; SPI_DAC_i = 0;
    SPI_DAC_KEY++;
    break;
  case 1:
    GPIO_ResetBits(GPIO_SPI1_NSS, SPI1_NSS);
    SPI_DAC_KEY++;
    break;  
  case 2:
    if(((SPI_SEND_BYTES[SPI_DAC_j]<<SPI_DAC_i)&0x80) == 0x80)
    {
      GPIO_SetBits(GPIO_SPI1_MISO, SPI1_MISO);
    }
    else
    {
      GPIO_ResetBits(GPIO_SPI1_MISO, SPI1_MISO);
    }
    if (SPI_DAC_j == 2)
    {
      SPI_READ_BYTE = SPI_READ_BYTE | ((GPIO_ReadInputDataBit(GPIO_SPI1_MOSI, SPI1_MOSI)) << (7-SPI_DAC_i));
    }
    SPI_DAC_KEY++;
    break;
  case 3:
    GPIO_SetBits(GPIO_SPI1_SCK, SPI1_SCK);
    SPI_DAC_KEY++;
    break;
  case 4:
    SPI_DAC_i++;
    SPI_DAC_KEY++;
    break;
  case 5:
    GPIO_ResetBits(GPIO_SPI1_SCK, SPI1_SCK);
    if (SPI_DAC_i < 8)
    {
      SPI_DAC_KEY = 2;
    }
    else
    {
      if (SPI_DAC_j < 3-1) //3 - количество байтов в массиве
      {
        SPI_DAC_j++;
        SPI_DAC_KEY = 2;
        SPI_DAC_i = 0;
      }
      else
      {
        SPI_DAC_KEY++;
      } 
    }
    break;
  case 6:
    GPIO_SetBits(GPIO_SPI1_NSS, SPI1_NSS);
    TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);
    TIM_Cmd(TIM4, DISABLE);
    NVIC_DisableIRQ(TIM4_IRQn);
    TIM_KEY = 0;
    SPI_DAC_KEY=0;
    break;  
  }
}