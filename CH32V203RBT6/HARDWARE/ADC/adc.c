#include "adc.h"
#include "debug.h"

volatile uint16_t adc_data = 0;

void adc_init(void)
{
    /*ADCͨ��1 ���ų�ʼ��*/
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    /*ADCģ���ʼ��*/
    ADC_InitTypeDef ADC_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);//APB2 - 96MHz��6��Ƶ--> ADCCLK 16MHz

    /*��λADC1����*/
    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    /*ʹ��ADC1*/
    ADC_Cmd(ADC1, ENABLE);

    /*ADCУ׼*/
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));

    /*ʹ��ADC Buffer*/
    ADC_BufferCmd(ADC1, ENABLE);

    /*����ADC�ж�*/
    ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);
    NVIC_SetPriority(ADC_IRQn,0X00);
    NVIC_EnableIRQ(ADC_IRQn);
}

/*ADC�ж�*/
void ADC1_2_IRQHandler(void) __attribute__((interrupt));
void ADC1_2_IRQHandler(void)
{
    if(ADC_GetITStatus(ADC1, ADC_IT_EOC) == SET)
    {
        adc_data = ADC_GetConversionValue(ADC1);
        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
    }
}
