/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *USART Print debugging routine:
 *USART1_Tx(PA9).
 *This example demonstrates using USART1(PA9) as a print debug port output.
 *
 */

#include "debug.h"
#include "adc.h"
/* Global typedef */

/* Global define */
extern uint16_t adc_data ;
/* Global Variable */

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    printf("=== 系统启动 ===\r\n");
    printf("SystemClk:%d\r\n", SystemCoreClock);
    adc_init();
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5);
    printf("2 - ADC通道配置完成\r\n");
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    printf("3 - 软件触发配置完成\r\n");
    
    while(1)
    {
        Delay_Ms(50);
        printf("adc_data = %d\r\n", adc_data);
    }
}
