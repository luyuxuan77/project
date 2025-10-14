/*********************************************************************
 * File Name          : config.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/8/15
 * Description        : file for Config.
 *********************************************************************************
* Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CONFIG_H__
#define __CONFIG_H__

/* LCD Screen Definition */
#define USE_HORIZONTAL    2
#define LCD_W             (320u)
#define LCD_H             (172u)

/* Buffer Definition */
#define COLOR_BYTE        (2u)
#define FILL_BUFFER_COUNT (256u * 1u)
#define FILL_BUFFER_SIZE  (FILL_BUFFER_COUNT * COLOR_BYTE)

/* SPI Flash Speed Definition */
#define SPI_NORMAL_MODE   1
#define SPI_FAST_MODE     2
#define SPI_SPEED_MODE    SPI_NORMAL_MODE

/* SPI FLASH and LCD_SCREEN GPIO Definition */
#define FLASH_CS_PORT     GPIOA
#define FLASH_CS_RCC      RCC_APB2Periph_GPIOA
#define FLASH_CS_PIN      (GPIO_Pin_2)

#define LCD_LED_RCC       RCC_APB2Periph_GPIOC
#define LCD_LED_PORT      GPIOC
#define LCD_LED_PIN       (GPIO_Pin_4)

#define LCD_DC_RCC        RCC_APB2Periph_GPIOC
#define LCD_DC_PORT       GPIOC
#define LCD_DC_PIN        (GPIO_Pin_3)

#define LCD_RST_RCC       RCC_APB2Periph_GPIOC
#define LCD_RST_PORT      GPIOC
#define LCD_RST_PIN       (GPIO_Pin_2)

#define LCD_CS_RCC        RCC_APB2Periph_GPIOA
#define LCD_CS_PORT       GPIOA
#define LCD_CS_PIN        (GPIO_Pin_1)

#endif
