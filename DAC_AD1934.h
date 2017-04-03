/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DAC_AD1934_H
#define __DAC_AD1934_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx.h"

   
/******************************************************************************/
/*            DAC POWER                                                       */
/******************************************************************************/
#define RCC_POWER_CONTROL_3  RCC_AHB1Periph_GPIOC
#define GPIO_POWER_CONTROL_3 GPIOC
#define POWER_CONTROL_3 GPIO_Pin_5

void DAC_AD1934_POWER_CONF(uint8_t);
  

/******************************************************************************/
/*            DAC SPI                                                         */
/******************************************************************************/ 
#define RCC_SPI1_NSS RCC_AHB1Periph_GPIOF 
#define GPIO_SPI1_NSS GPIOF
#define SPI1_NSS GPIO_Pin_6

#define RCC_SPI1_SCK RCC_AHB1Periph_GPIOF 
#define GPIO_SPI1_SCK GPIOF
#define SPI1_SCK GPIO_Pin_7

#define RCC_SPI1_MOSI RCC_AHB1Periph_GPIOF 
#define GPIO_SPI1_MOSI GPIOF
#define SPI1_MOSI GPIO_Pin_8
 
#define RCC_SPI1_MISO RCC_AHB1Periph_GPIOF 
#define GPIO_SPI1_MISO GPIOF
#define SPI1_MISO GPIO_Pin_9
   
#define RCC_DAC_SPI_TIM RCC_APB1Periph_TIM4 
#define DAC_SPI_TIM TIM4 
#define DAC_SPI_TIM_IRQ TIM4_IRQn
   
void DAC_AD1934_SPI_STANDALONE(void);
void DAC_AD1934_SPI_SEND(uint8_t, uint8_t);
uint8_t DAC_AD1934_SPI_READ(uint8_t);


/******************************************************************************/
/*            DAC I2S                                                         */
/******************************************************************************/ 
#define DAC_I2S_CLK_INIT                  RCC_APB1PeriphClockCmd
#define DAC_I2S_CLK                       RCC_APB1Periph_SPI2
#define DAC_I2S                           SPI2

//DAC_I2S_WS -> CPU I2S2_WS1 <-> DAC DLRCLK
#define DAC_I2S_WS_PIN                    GPIO_Pin_12
#define DAC_I2S_WS_GPIO_PORT              GPIOB
#define DAC_I2S_WS_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define DAC_I2S_WS_SOURCE                 GPIO_PinSource12
#define DAC_I2S_WS_AF                     GPIO_AF_SPI2

//DAC_I2S_CK -> CPU I2S2_SCK1 <-> DAC SCK1
#define DAC_I2S_CK_PIN                    GPIO_Pin_10
#define DAC_I2S_CK_GPIO_PORT              GPIOB
#define DAC_I2S_CK_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define DAC_I2S_CK_SOURCE                 GPIO_PinSource10
#define DAC_I2S_CK_AF                     GPIO_AF_SPI2

//DAC_I2S_SD -> CPU I2S2_SD1 <-> DAC SD1
#define DAC_I2S_SD_PIN                    GPIO_Pin_3
#define DAC_I2S_SD_GPIO_PORT              GPIOC
#define DAC_I2S_SD_GPIO_CLK               RCC_AHB1Periph_GPIOC
#define DAC_I2S_SD_SOURCE                 GPIO_PinSource3
#define DAC_I2S_SD_AF                     GPIO_AF_SPI2

//DAC Triggers
//PRE1
#define DAC_I2S_PRE1_PIN                    GPIO_Pin_12
#define DAC_I2S_PRE1_GPIO_PORT              GPIOF
#define DAC_I2S_PRE1_GPIO_CLK               RCC_AHB1Periph_GPIOF

//CLR1
#define DAC_I2S_CLR1_PIN                    GPIO_Pin_14
#define DAC_I2S_CLR1_GPIO_PORT              GPIOF
#define DAC_I2S_CLR1_GPIO_CLK               RCC_AHB1Periph_GPIOF

//PRE2
#define DAC_I2S_PRE2_PIN                    GPIO_Pin_11
#define DAC_I2S_PRE2_GPIO_PORT              GPIOF
#define DAC_I2S_PRE2_GPIO_CLK               RCC_AHB1Periph_GPIOF

//CLR2
#define DAC_I2S_CLR2_PIN                    GPIO_Pin_13
#define DAC_I2S_CLR2_GPIO_PORT              GPIOF
#define DAC_I2S_CLR2_GPIO_CLK               RCC_AHB1Periph_GPIOF


void DAC_AD1934_I2S_Config(void);
void DAC_AD1934_I2S_SEND(void);

#define countof(a)   (sizeof(a) / sizeof(*(a)))





#define TX_MASTER_BUFFERSIZE           (countof(aTxMasterBuffer) - 1)
#define RX_MASTER_BUFFERSIZE           TX_MASTER_BUFFERSIZE

#define TX_SLAVE_BUFFERSIZE           (countof(aTxSlaveBuffer) - 1)
#define RX_SLAVE_BUFFERSIZE           TX_SLAVE_BUFFERSIZE



#ifdef __cplusplus
}
#endif

#endif 