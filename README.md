# project

�˲ֿ�������ѧ��һ�β�Ʒ����

spi1:
PA5->SCK
PA6->MISO
PA7->MOSI
PA4->�������Ƭѡ�� loar
PA1->�������Ƭѡ�� OLED
spi2:
PB13->SCK
PB14->MISO
PB15->MOSI
PC0 ->�������Ƭѡ�� ADC&spi
PC1 ->�������Ƭѡ�� DAC&spi

/**********************************************\***********************************************/
/_����Ϊ����ģ���Ӧ�� io������ѡ�� 64pin �� CH32V203RBT6_/

OLED:
SCL -> PA5
SDA-> PA7
RES -> PC2
DC -> PC3
CS -> PA1
BLK -> PC4
Loar:
NSS -> PA4
MOSI->PA7
MISO->PA6
SCK -> PA5
BUSY->PC5
DIO1->PC6
RESET->PC7

ADC&SPI
SCK->PB13
MISO->PB14
MOSI->PB15 #����� MISO �� MOSI ָ���� MCU �ϵģ�����֮ǰ�˶�һ��
�������Ƭѡ�� ADC&spi->PC0

DAC&SPI
SCK->PB13
MISO->PB14
MOSI->PB15 #����� MISO �� MOSI ָ���� MCU �ϵģ�����֮ǰ�˶�һ��
�������Ƭѡ�� DAC&spi->PC01

�ⲿ������������ 1����
PA9
PA10 #����Ҫһ�����Դ��ڣ������ؽӿڣ������Լ���һ�¡�

GPS:
RXD->PA2(PA2 Ϊ MCU �� TX)
TXD->PA3
ON/OFF->PC8
1PPS->PC9
4G:
RXD->PB10(PB10 Ϊ MCU �� TX)
TXD->PB11
EN ->PC12
��Ƶ������
RXD->PC10(PC10 Ϊ MCU �� TX)
TXD->PC11
/********************\*\*********************��β**********************\*\*\***********************/
