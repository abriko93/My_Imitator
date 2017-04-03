#include "DAC_AD1934.h"

const uint32_t TIM_PER = 1000; //TIM_PER = Период SCK [us]
uint8_t SPI_DAC_KEY, SPI_DAC_i, SPI_DAC_j, TIM_KEY; //Переменные для TIM

uint8_t SPI_SEND_BYTES[] = {0x00, 0x00, 0x00}; //Массив байтов на отправку
uint8_t SPI_READ_BYTE = 0x00; //Байт на приём




//I2S
const uint16_t aTxMasterBuffer[] =
  {
    0x1234, 0x5678, 0x1ABC, 0xDEF0, 0x5555,
    0x1748, 0x9027, 0x4424, 0x9623, 0x0932,
    0x9744, 0x7845, 0x0912, 0x1111, 0x1772,
    0x9736, 0x0933, 0x0187, 0x9567, 0x9977
  };

__IO uint8_t  ubBufferCounter = 0;



void DAC_AD1934_SPI_GPIO_CONF(void);
void DAC_AD1934_TIM_Config(void);

void DAC_AD1934_I2S_TRIGGER_Config(void);

void DAC_AD1934_POWER_CONF(uint8_t powerkey) 
{
  GPIO_InitTypeDef  GPIO_DAC_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_POWER_CONTROL_3, ENABLE);
  
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
  GPIO_InitTypeDef  GPIO_DAC_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_SPI1_NSS, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_SPI1_SCK, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_SPI1_MOSI, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_SPI1_MISO, ENABLE);
  
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
  TIM_TimeBaseInitTypeDef TIMER_DAC_InitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_DAC_SPI_TIM, ENABLE);
  
  TIM_TimeBaseStructInit(&TIMER_DAC_InitStructure);
  TIMER_DAC_InitStructure.TIM_Prescaler = 21-1;
  TIMER_DAC_InitStructure.TIM_Period = TIM_PER-1;
  TIM_TimeBaseInit(TIM4, &TIMER_DAC_InitStructure);
  
  SPI_DAC_KEY = 0;
  TIM_ITConfig(DAC_SPI_TIM, TIM_IT_Update, ENABLE);
  TIM_Cmd(DAC_SPI_TIM, ENABLE);
  NVIC_EnableIRQ(DAC_SPI_TIM_IRQ);
  TIM_KEY = 1;
}

void TIM4_IRQHandler()
{
  TIM_ClearITPendingBit(DAC_SPI_TIM, TIM_IT_Update);
  
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
    TIM_ITConfig(DAC_SPI_TIM, TIM_IT_Update, DISABLE);
    TIM_Cmd(DAC_SPI_TIM, DISABLE);
    NVIC_DisableIRQ(DAC_SPI_TIM_IRQ);
    TIM_KEY = 0;
    SPI_DAC_KEY=0;
    break;  
  }
}

void DAC_AD1934_I2S_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  I2S_InitTypeDef I2S_InitStructure;

  /* Peripheral Clock Enable -------------------------------------------------*/
  /* Enable the DAC_I2S_CLK clock */
  DAC_I2S_CLK_INIT(DAC_I2S_CLK, ENABLE);
  
  /* Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(DAC_I2S_WS_GPIO_CLK | DAC_I2S_CK_GPIO_CLK | 
                         DAC_I2S_SD_GPIO_CLK, ENABLE);

  /* I2S GPIO Configuration --------------------------------------------------*/
  /* Connect I2S pins to Alternate functions */  
  GPIO_PinAFConfig(DAC_I2S_WS_GPIO_PORT, DAC_I2S_WS_SOURCE, DAC_I2S_WS_AF);
  GPIO_PinAFConfig(DAC_I2S_CK_GPIO_PORT, DAC_I2S_CK_SOURCE, DAC_I2S_CK_AF);
  GPIO_PinAFConfig(DAC_I2S_SD_GPIO_PORT, DAC_I2S_SD_SOURCE, DAC_I2S_SD_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

  /* DAC_I2S_WS pin configuration */
  GPIO_InitStructure.GPIO_Pin = DAC_I2S_WS_PIN;
  GPIO_Init(DAC_I2S_WS_GPIO_PORT, &GPIO_InitStructure);

  /* DAC_I2S_CK pin configuration */
  GPIO_InitStructure.GPIO_Pin =  DAC_I2S_CK_PIN;
  GPIO_Init(DAC_I2S_CK_GPIO_PORT, &GPIO_InitStructure);
  
  /* DAC_I2S_SD pin configuration */
  GPIO_InitStructure.GPIO_Pin = DAC_I2S_SD_PIN;
  GPIO_Init(DAC_I2S_SD_GPIO_PORT, &GPIO_InitStructure);
 
  /* I2S configuration -------------------------------------------------------*/
  /* Initialize  DAC_I2S*/
  SPI_I2S_DeInit(DAC_I2S);
  
  /* Configure the Audio Frequency, Standard and the data format */
  I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_192k;
  I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
  I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
  I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
  I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_32b;

  /* Configure DAC_I2S in Master Transmitter Mode */
  I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
  I2S_Init(DAC_I2S, &I2S_InitStructure);

  //Trigger configuration
  DAC_AD1934_I2S_TRIGGER_Config();
  
  /* Enable the I2Sx peripheral */
  I2S_Cmd(DAC_I2S, ENABLE);
  
  
}

void DAC_AD1934_I2S_TRIGGER_Config(void)
{
  GPIO_InitTypeDef  GPIO_TRIG_InitStructure;
  
  RCC_AHB1PeriphClockCmd(DAC_I2S_PRE1_GPIO_CLK | DAC_I2S_CLR1_GPIO_CLK | 
                         DAC_I2S_PRE2_GPIO_CLK | DAC_I2S_CLR2_GPIO_CLK, ENABLE);
  
  GPIO_TRIG_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_TRIG_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_TRIG_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_TRIG_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

  GPIO_TRIG_InitStructure.GPIO_Pin = DAC_I2S_PRE1_PIN;
  GPIO_Init(DAC_I2S_PRE1_GPIO_PORT, &GPIO_TRIG_InitStructure);
  
  GPIO_TRIG_InitStructure.GPIO_Pin = DAC_I2S_CLR1_PIN;
  GPIO_Init(DAC_I2S_CLR1_GPIO_PORT, &GPIO_TRIG_InitStructure);
  
  GPIO_TRIG_InitStructure.GPIO_Pin = DAC_I2S_PRE2_PIN;
  GPIO_Init(DAC_I2S_PRE2_GPIO_PORT, &GPIO_TRIG_InitStructure);
  
  GPIO_TRIG_InitStructure.GPIO_Pin = DAC_I2S_CLR2_PIN;
  GPIO_Init(DAC_I2S_CLR2_GPIO_PORT, &GPIO_TRIG_InitStructure);
  
  GPIO_SetBits(DAC_I2S_CLR1_GPIO_PORT, DAC_I2S_CLR1_PIN);
  GPIO_ResetBits(DAC_I2S_PRE1_GPIO_PORT, DAC_I2S_PRE1_PIN);
  
  GPIO_SetBits(DAC_I2S_PRE2_GPIO_PORT, DAC_I2S_PRE2_PIN);
  GPIO_ResetBits(DAC_I2S_CLR2_GPIO_PORT, DAC_I2S_CLR2_PIN);
  
  GPIO_SetBits(DAC_I2S_PRE1_GPIO_PORT, DAC_I2S_PRE1_PIN);
  GPIO_SetBits(DAC_I2S_CLR2_GPIO_PORT, DAC_I2S_CLR2_PIN);
}


void DAC_AD1934_I2S_SEND(void) 
{
        /* Master full Duplex Communication ----------------------------------------*/
      /* Communication Full Duplex Started */
      
      ubBufferCounter = 0;
      while ((ubBufferCounter != TX_MASTER_BUFFERSIZE))
      {
      
        /* Data to transmitted through I2Sx SD pin */
        while (SPI_I2S_GetFlagStatus(DAC_I2S, SPI_I2S_FLAG_TXE ) != SET);
        SPI_I2S_SendData(DAC_I2S, aTxMasterBuffer[ubBufferCounter]);
        ubBufferCounter++;
      }	
}
