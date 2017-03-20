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
#define GPIO_POWER_CONTROL_4 GPIOE
#define GPIO_POWER_CONTROL_5 GPIOE

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
#define POWER_CONTROL_4 GPIO_Pin_4
#define POWER_CONTROL_5 GPIO_Pin_5

   

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
