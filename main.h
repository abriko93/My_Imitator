/**
  ******************************************************************************
  * @file    USART/USART_HyperTerminal/main.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    18-January-2013
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"


   
   
#define GPIO_PIN9 GPIOD
#define GPIO_PIN10 GPIOD
#define GPIO_PIN11 GPIOD
#define GPIO_DIOD1 GPIOD
#define GPIO_DIOD2 GPIOG
#define GPIO_POWER_CONTROL_1 GPIOE
#define GPIO_POWER_CONTROL_2 GPIOE
#define GPIO_POWER_CONTROL_3 GPIOC
#define GPIO_POWER_CONTROL_4 GPIOE
#define GPIO_POWER_CONTROL_5 GPIOE
#define GPIO_SPI1_NSS GPIOF
#define GPIO_SPI1_SCK GPIOF
#define GPIO_SPI1_MOSI GPIOF
#define GPIO_SPI1_MISO GPIOF
#define GPIO_I2S2_MCK GPIOC
#define GPIO_I2S2_SD GPIOC
#define GPIO_I2S2_WS GPIOB
#define GPIO_I2S2_CK GPIOB

#define PIN9 GPIO_Pin_14
#define PIN10 GPIO_Pin_13
#define PIN11 GPIO_Pin_12
#define DIOD1 GPIO_Pin_15
#define DIOD2 GPIO_Pin_2
#define POWER_CONTROL_1 GPIO_Pin_2
#define POWER_CONTROL_2 GPIO_Pin_3
#define POWER_CONTROL_3 GPIO_Pin_5
#define POWER_CONTROL_4 GPIO_Pin_4
#define POWER_CONTROL_5 GPIO_Pin_5
#define SPI1_NSS GPIO_Pin_6
#define SPI1_SCK GPIO_Pin_7
#define SPI1_MOSI GPIO_Pin_8
#define SPI1_MISO GPIO_Pin_9
   

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
