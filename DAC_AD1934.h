/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DAC_AD1934_H
#define __DAC_AD1934_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
   
#define GPIO_POWER_CONTROL_3 GPIOC
#define POWER_CONTROL_3 GPIO_Pin_5
  
#define GPIO_SPI1_NSS GPIOF
#define SPI1_NSS GPIO_Pin_6
   
#define GPIO_SPI1_SCK GPIOF
#define SPI1_SCK GPIO_Pin_7
   
#define GPIO_SPI1_MOSI GPIOF
#define SPI1_MOSI GPIO_Pin_8
   
#define GPIO_SPI1_MISO GPIOF
#define SPI1_MISO GPIO_Pin_9
   

void DAC_AD1934_POWER_CONF(uint8_t);
void DAC_AD1934_SPI_STANDALONE(void);
void DAC_AD1934_SPI_SEND(uint8_t, uint8_t);
uint8_t DAC_AD1934_SPI_READ(uint8_t);


#ifdef __cplusplus
}
#endif

#endif 