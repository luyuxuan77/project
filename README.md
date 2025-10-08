# project

此仓库用作大学第一次产品尝试

spi1:
PA5->SCK
PA6->MISO
PA7->MOSI
PA4->软件控制片选至 loar
PA1->软件控制片选至 OLED
spi2:
PB13->SCK
PB14->MISO
PB15->MOSI
PC0 ->软件控制片选至 ADC&spi
PC1 ->软件控制片选至 DAC&spi

/**********************************************\***********************************************/
/_以下为各个模块对应的 io，主控选择 64pin 的 CH32V203RBT6_/

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
MOSI->PB15 #这里的 MISO 和 MOSI 指的是 MCU 上的，画板之前核对一下
软件控制片选至 ADC&spi->PC0

DAC&SPI
SCK->PB13
MISO->PB14
MOSI->PB15 #这里的 MISO 和 MOSI 指的是 MCU 上的，画板之前核对一下
软件控制片选至 DAC&spi->PC01

外部调试器（串口 1）：
PA9
PA10 #我需要一个调试串口，和下载接口，最后可以集成一下。

GPS:
RXD->PA2(PA2 为 MCU 的 TX)
TXD->PA3
ON/OFF->PC8
1PPS->PC9
4G:
RXD->PB10(PB10 为 MCU 的 TX)
TXD->PB11
EN ->PC12
音频蓝牙：
RXD->PC10(PC10 为 MCU 的 TX)
TXD->PC11
/********************\*\*********************结尾**********************\*\*\***********************/
