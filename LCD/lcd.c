/*********************************************************************
 * File Name          : lcd.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/8/15
 * Description        : file for lcd screen.
 *********************************************************************************
* Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "lcd.h"
#include "debug.h"
#include "math.h"

static uint16_t width=200;
static uint16_t height=320;

#define LCD_SOFT_RESET          (0x01)
#define LCD_READ_ID             (0x04)
#define LCD_READ_DISPLAY_STATUS (0x09)
#define LCD_ON                  (0x28)
#define LCD_OFF                 (0x29)

#define LCD_SET_X               (0x2a)
#define LCD_SET_Y               (0x2B)
#define LCD_MEM_WRITE           (0x2C)
#define LCD_MEM_READ            (0x2e)

static uint16_t color_buffer[2][FILL_BUFFER_COUNT] = {0};

/*********************************************************************
 * @fn      LCD_WR_REG
 *
 * @brief   Write an 8-bit command to the LCD screen
 * 
 * @param   data - Command value to be written
 *
 * @return  none
 */
static void LCD_WR_REG(uint8_t data)
{
    LCD_CS_CLR;
    LCD_DC_CLR;
    SPI1_Write(data);
    LCD_CS_SET;
}

/*********************************************************************
 * @fn      LCD_WR_DATA
 *
 * @brief   Write an 8-bit data to the LCD screen
 * 
 * @param   data - the data to write
 * 
 */
static void LCD_WR_DATA(uint8_t data)
{
    LCD_CS_CLR;
    LCD_DC_SET;
    SPI1_Write(data);
    LCD_CS_SET;
}

/*********************************************************************
 * @fn      LCD_WriteReg
 * 
 * @brief   write a data to the register
 * 
 * @param   LCD_Reg - register address
 *          LCD_RegValue - the data to write
 * 
 * @return  none
 */
static void LCD_WriteReg(uint8_t LCD_Reg, uint8_t LCD_RegValue)
{
    LCD_WR_REG(LCD_Reg);
    LCD_WR_DATA(LCD_RegValue);
}

/*********************************************************************
 * @fn      Lcd_WriteData_16Bit
 * 
 * @brief   write two bytes to the lcd screen
 * 
 * @param   Data - the data to write
 * 
 * @return  none
 */
void Lcd_WriteData_16Bit(uint16_t Data)
{
    LCD_CS_CLR;
    LCD_DC_SET;
    SPI1_Write(Data >> 8);
    Delay_Ms(1);
    SPI1_Write(Data);
    LCD_CS_SET;
}

/*********************************************************************
 * @fn      LCD_GPIOInit
 * 
 * @brief   Configuring the control gpio for the lcd screen
 * 
 * @return  none
 */
void LCD_GPIOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(LCD_LED_RCC, ENABLE);
    RCC_APB2PeriphClockCmd(LCD_DC_RCC, ENABLE);
    RCC_APB2PeriphClockCmd(LCD_RST_RCC, ENABLE);
    RCC_APB2PeriphClockCmd(LCD_CS_RCC, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = LCD_LED_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(LCD_LED_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = LCD_DC_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(LCD_DC_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = LCD_RST_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(LCD_RST_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = LCD_CS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(LCD_CS_PORT, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      LCD_RESET
 * 
 * @brief   reset the lcd screen by the rst pin
 * 
 * @return  none
 */
void LCD_RESET(void)
{
    LCD_RST_CLR;
    Delay_Ms(100);
    LCD_RST_SET;
    Delay_Ms(50);
}


void Write(uint8_t CD,uint8_t hex)
{
    if(CD==Command)
    {
        LCD_WR_REG(hex);
    }
    else
    {
        LCD_WR_DATA(hex);
    }

}

void Write_Command(uint8_t cmd) {
    LCD_WR_REG(cmd);
}
void Write_Data(uint8_t dat) {
    LCD_WR_DATA(dat);
}
void Write_Color(uint16_t color) {
    Write_Data(color >> 8);   // ������ɫ���ݵĸ�8λ
    Write_Data(color & 0xFF); // ������ɫ���ݵĵ�8λ
}
// ���ý�Ҫ���л�ͼ�Ĵ������� (x_start, y_start) �� (x_end, y_end)
void Set_Draw_Window(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end) {
    Write_Command(0x2A); // �е�ַ��������
    Write_Data(x_start >> 8);   Write_Data(x_start & 0xFF); // ��ʼ�и�8λ�͵�8λ
    Write_Data(x_end >> 8);     Write_Data(x_end & 0xFF);   // �����и�8λ�͵�8λ

    Write_Command(0x2B); // �е�ַ��������
    Write_Data(y_start >> 8);   Write_Data(y_start & 0xFF); // ��ʼ�и�8λ�͵�8λ
    Write_Data(y_end >> 8);     Write_Data(y_end & 0xFF);   // �����и�8λ�͵�8λ

    Write_Command(0x2C); // ��ʼд��GRAM���Դ棩����
    // ִ�д�����󣬺�������д������ݽ�ֱ����䵽�趨�Ĵ�����
}

double my_pow_basic(double base, int exponent) {
    if (exponent == 0) return 1.0; // �κ�����0���ݶ���1[1](@ref)
    
    // ����ָ����a^(-b) = 1/(a^b)[1](@ref)
    if (exponent < 0) {
        base = 1 / base;
        exponent = -exponent;
    }
    
    double result = 1.0;
    for (int i = 0; i < exponent; i++) {
        result *= base; // ѭ�����[2](@ref)
    }
    return result;
}

/*********************************************************************
 * @fn      LCD_Init
 * 
 * @brief   Initialization LCD screen
 * 
 * @return  none
 */
void LCD_Init(void)
{
    SPI1_Init();
    //SPI2_DMA_Init();
    LCD_GPIOInit();
    LCD_RESET();
    LCD_LED_SET;

    Write(Command , 0x36);     
    Write(Parameter , 0x00);   

    Write(Command , 0x3A);     
    Write(Parameter , 0x05);   //0x05 65k,0x06 262k

    Write(Command , 0xB2);     
    Write(Parameter , 0x0C);   
    Write(Parameter , 0x0C);   
    Write(Parameter , 0x00);   
    Write(Parameter , 0x33);   
    Write(Parameter , 0x33);   

    Write(Command , 0xB7);     
    Write(Parameter , 0x17);   

    Write(Command , 0xBB);     
    Write(Parameter , 0x35);   

    Write(Command , 0xC0);     
    Write(Parameter , 0x2C);   

    Write(Command , 0xC2);     
    Write(Parameter , 0x01);   

    Write(Command , 0xC3);     
    Write(Parameter , 0x13);   

    Write(Command , 0xC4);     
    Write(Parameter , 0x20);   //VDV, 0x20:0v

    Write(Command , 0xC6);     
    Write(Parameter , 0x18);  

    Write(Command , 0xD0);     
    Write(Parameter , 0xA7);   
    Write(Parameter , 0xA1);   

    Write(Command , 0xD0);     
    Write(Parameter , 0xA4);   
    Write(Parameter , 0xA1);   

    Write(Command , 0xD6);     
    Write(Parameter , 0xA1);   //sleep in��gate���ΪGND

    Write(Command , 0xE0);     
    Write(Parameter , 0xF0);   
    Write(Parameter , 0x00);   
    Write(Parameter , 0x04);   
    Write(Parameter , 0x04);   
    Write(Parameter , 0x04);   
    Write(Parameter , 0x05);   
    Write(Parameter , 0x29);   
    Write(Parameter , 0x33);   
    Write(Parameter , 0x3E);   
    Write(Parameter , 0x38);   
    Write(Parameter , 0x12);   
    Write(Parameter , 0x12);   
    Write(Parameter , 0x28);   
    Write(Parameter , 0x30);   

    Write(Command , 0xE1);     
    Write(Parameter , 0xF0);   
    Write(Parameter , 0x07);   
    Write(Parameter , 0x0A);   
    Write(Parameter , 0x0D);   
    Write(Parameter , 0x0B);   
    Write(Parameter , 0x07);   
    Write(Parameter , 0x28);   
    Write(Parameter , 0x33);   
    Write(Parameter , 0x3E);   
    Write(Parameter , 0x36);   
    Write(Parameter , 0x14);   
    Write(Parameter , 0x14);   
    Write(Parameter , 0x29);   
    Write(Parameter , 0x32);  

    Write(Command , 0x35);   //��TE    
    Write(Parameter , 0x00);   

    Write(Command , 0x44);     //�ƶ�˺����
    Write(Parameter , 0x00);   //0x01&0x28
    Write(Parameter , 0x10);  

    Write(Command , 0x21);     

    Write(Command , 0x11);     

    Delay_Ms(120);   

    Write(Command , 0x29);    

    LCD_direction(0);  
}

/*********************************************************************
 * @fn      LCD_SetWindows
 * 
 * @brief   Setting LCD display window
 * 
 * @param   xStar -  the bebinning x coordinate of the LCD display window
 *          yStar -  the bebinning y coordinate of the LCD display window
 *          xEnd -  the endning x coordinate of the LCD display window
 *          yEnd -  the endning y coordinate of the LCD display window
 * 
 * @return  none
 */
void LCD_SetWindows(uint16_t xStar, uint16_t yStar, uint16_t xEnd, uint16_t yEnd)
{
    LCD_WR_REG(LCD_SET_X);
    LCD_WR_DATA(xStar >> 8);
    LCD_WR_DATA(0x00FF & xStar);
    LCD_WR_DATA(xEnd >> 8);
    LCD_WR_DATA(0x00FF & xEnd);

    LCD_WR_REG(LCD_SET_Y);
    LCD_WR_DATA(yStar >> 8);
    LCD_WR_DATA(0x00FF & yStar);
    LCD_WR_DATA(yEnd >> 8);
    LCD_WR_DATA(0x00FF & yEnd);
    LCD_WR_REG(LCD_MEM_WRITE);  
}

/*********************************************************************
 * @fn      LCD_direction
 * 
 * @brief   Setting the display direction of LCD screen
 * 
 * @param   direction -   direction:0-0 degree  1-90 degree  2-180 degree  3-270 degree
 * 
 * @return  none
 */
void LCD_direction(uint8_t direction)
{
    switch (direction)
    {
        case 0:
            width  = LCD_W;
            height = LCD_H;
            LCD_WriteReg(0x36, (1 << 3) | (0 << 6) | (0 << 7));  //BGR==1,MY==0,MX==0,MV==0
            break;
        case 1:
            width  = LCD_H;
            height = LCD_W;
            LCD_WriteReg(0x36, (1 << 3) | (0 << 7) | (1 << 6) | (1 << 5));  //BGR==1,MY==1,MX==0,MV==1
            break;
        case 2:
            width  = LCD_W;
            height = LCD_H;
            LCD_WriteReg(0x36, (1 << 3) | (1 << 6) | (1 << 7));  //BGR==1,MY==0,MX==0,MV==0
            break;
        case 3:
            width  = LCD_H;
            height = LCD_W;
            LCD_WriteReg(0x36, (1 << 3) | (1 << 7) | (1 << 5));  //BGR==1,MY==1,MX==0,MV==1
            break;
        default:
            break;
    }
}

/*********************************************************************
 * @fn      LCD_Clear
 * 
 * @brief   Full screen filled LCD screen
 * 
 * @param   Color -   Filled color
 * 
 * @return  none
 */
void LCD_Clear(uint16_t Color)
{
    unsigned int i, m;
    LCD_SetWindows(0, 0, width - 1, height - 1);
    LCD_CS_CLR;
    LCD_DC_SET;
    for (i = 0; i < height; i++)
    {
        for (m = 0; m < width; m++)
        {
            Lcd_WriteData_16Bit(Color);
        }
        
    }
    LCD_CS_SET;
}

/*********************************************************************
 * @fn      LCD_drawImageWithSize
 * 
 * @brief   read image from SPI Flash and display on the LCD screen
 * 
 * @param   x -  the x coordinate of the LCD screen
 *          y - the y coordinate of the LCD screen
 *          width -  the image width
 *          height - the image height
 *          addr - the image address in SPI flash
 * 
 * @return  none
 */
void LCD_drawImageWithSize(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t addr)
{
    uint32_t m  = 0;
    uint16_t x2 = (x + width) - 1;
    uint16_t y2 = (y + height) - 1;
    LCD_WR_REG(LCD_SET_X);
    LCD_WR_DATA(x >> 8);
    LCD_WR_DATA(0x00FF & x);
    LCD_WR_DATA(x2 >> 8);
    LCD_WR_DATA(0x00FF & x2);

    LCD_WR_REG(LCD_SET_Y);
    LCD_WR_DATA(y >> 8);
    LCD_WR_DATA(0x00FF & y);
    LCD_WR_DATA(y2 >> 8);
    LCD_WR_DATA(0x00FF & y2);

    LCD_WR_REG(LCD_MEM_WRITE);
    LCD_CS_CLR;
    LCD_DC_SET;

    uint32_t count = width * height;

    uint32_t fill_count = count / FILL_BUFFER_COUNT;
    uint32_t else_count = count % FILL_BUFFER_COUNT;
    SPI_Flash_Read_dma_start(addr);

    SPI1_Read_DMA((uint8_t *)color_buffer[m], FILL_BUFFER_SIZE);

    while (fill_count > 0)
    {
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
            ;
        fill_count--;
        SPI2_Write_DMA((uint8_t *)color_buffer[m], FILL_BUFFER_SIZE);
        m = !m;
        SPI1_Read_DMA((uint8_t *)color_buffer[m], FILL_BUFFER_SIZE);
    }
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
        ;
    SPI2_Write_DMA((uint8_t *)color_buffer[m], else_count * COLOR_BYTE);
    SPI_Flash_Read_dma_end();
}





// ���û�ͼ���� (x_start, y_start) �� (x_end, y_end)
void LCD_SetWindow(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end) {
    Write(Command, 0x2A); // �е�ַ��������
    Write(Parameter, x_start >> 8);   Write(Parameter, x_start & 0xFF);
    Write(Parameter, x_end >> 8);     Write(Parameter, x_end & 0xFF);

    Write(Command, 0x2B); // �е�ַ��������
    Write(Parameter, y_start >> 8);   Write(Parameter, y_start & 0xFF);
    Write(Parameter, y_end >> 8);     Write(Parameter, y_end & 0xFF);

    Write(Command, 0x2C); // ��ʼд���Դ�����
}

// ���Ƶ������ص�
void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if (x >= 320 || y >= 172) return;
    LCD_SetWindow(x, y, x, y);
    Write(Parameter, color >> 8);
    Write(Parameter, color & 0xFF);
}
// ��ָ������(x,y)����һ����
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color) {
    if (x >= 320 || y >= 172) return; // ȷ����������Ļ��Χ�� (172x320)
    LCD_SetWindow(x, y, x, y); // ����������Ϊ������
    // ����16λ��ɫֵ��RGB565��ʽ��
    Write(Parameter, color >> 8);   // ���͸�8λ
    Write(Parameter, color & 0xFF); // ���͵�8λ
}

// ��ָ����ɫ���һ����������
void LCD_Fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color) {
    uint32_t total_pixels = (x_end - x_start + 1) * (y_end - y_start + 1);
    LCD_SetWindow(x_start, y_start, x_end, y_end);
    
    // ����������ɫ���ݣ��������
    for (uint32_t i = 0; i < total_pixels; i++) {
        Write(Parameter, color >> 8);
        Write(Parameter, color & 0xFF);
    }
}

// ʹ��Bresenham�㷨����ֱ��
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    int16_t dx = abs(x2 - x1), dy = -abs(y2 - y1);
    int16_t sx = (x1 < x2) ? 1 : -1;
    int16_t sy = (y1 < y2) ? 1 : -1;
    int16_t err = dx + dy, e2; // �����

    while (1) {
        LCD_DrawPoint(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { // ˮƽ������
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) { // ��ֱ������
            err += dx;
            y1 += sy;
        }
    }
}


// ���ƾ��α߿�
void LCD_DrawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    LCD_DrawLine(x1, y1, x2, y1, color); // �ϱ�
    LCD_DrawLine(x1, y2, x2, y2, color); // �±�
    LCD_DrawLine(x1, y1, x1, y2, color); // ���
    LCD_DrawLine(x2, y1, x2, y2, color); // �ұ�
}

// ����ʵ�ľ���
void LCD_FillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    LCD_Fill(x1, y1, x2, y2, color);
}


// ʹ��Bresenham�㷨����Բ��
void LCD_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) {
    int16_t x = 0, y = r;
    int16_t d = 3 - 2 * r; // ���߲���

    while (x <= y) {
        // ����Բ�İ˶Գ��ԣ�һ�λ��˸���
        LCD_DrawPoint(x0 + x, y0 + y, color);
        LCD_DrawPoint(x0 - x, y0 + y, color);
        LCD_DrawPoint(x0 + x, y0 - y, color);
        LCD_DrawPoint(x0 - x, y0 - y, color);
        LCD_DrawPoint(x0 + y, y0 + x, color);
        LCD_DrawPoint(x0 - y, y0 + x, color);
        LCD_DrawPoint(x0 + y, y0 - x, color);
        LCD_DrawPoint(x0 - y, y0 - x, color);

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

// ���峣����ɫ��RGB565��ʽ��
#define COLOR_SKY_BLUE    0x067D  // ����ɫ
#define COLOR_GRASS_GREEN 0x07E0  // ����ɫ
#define COLOR_SUN_YELLOW  0xFFE0  // ����ɫ
#define COLOR_TRUNK_BROWN 0xBAAB  // ������ɫ
#define COLOR_LEAF_GREEN  0x3666  // ��Ҷ����

void DrawScenery() {
    // 1. ���Ʊ������ϰ벿����գ��°벿�ֲݵ�
    LCD_Fill(0, 0, 171, 159, COLOR_SKY_BLUE); // �ϰ������
    LCD_Fill(0, 160, 171, 319, COLOR_GRASS_GREEN); // �°����ݵ�

    // 2. ����̫�������Ͻǵ�һ��Բ��
    LCD_DrawCircle(140, 40, 20, COLOR_SUN_YELLOW);
    // Ϊ̫�����һЩ���ߣ��򵥵Ķ��ߣ�
    for (int i = 0; i < 12; i++) {
        uint16_t angle = i * 30;
        uint16_t x1 = 140 + 25 * cos(angle * 3.14159 / 180);
        uint16_t y1 = 40 + 25 * sin(angle * 3.14159 / 180);
        uint16_t x2 = 140 + 35 * cos(angle * 3.14159 / 180);
        uint16_t y2 = 40 + 35 * sin(angle * 3.14159 / 180);
        LCD_DrawLine(x1, y1, x2, y2, COLOR_SUN_YELLOW);
    }

    // 3. ����һ���������ɺ�����
    // ���ɣ�һ��ϸ���ľ��Σ�
    LCD_FillRect(80, 200, 84, 250, COLOR_TRUNK_BROWN);
    // ���ڣ�һ��ʵ��Բ��
    for (int r = 25; r > 0; r -= 2) {
        LCD_DrawCircle(82, 200, r, COLOR_LEAF_GREEN);
    }

    // 4. ����������һ���ƣ��ɼ����ص���Բ��ɣ�
    LCD_DrawCircle(50, 60, 15, 0xFFFF); // ��ɫ
    LCD_DrawCircle(70, 60, 15, 0xFFFF);
    LCD_DrawCircle(60, 50, 15, 0xFFFF);
    LCD_DrawCircle(60, 70, 15, 0xFFFF);

    // 5. ���һЩ���ֱ�ǩ
    // ע�⣺��Ҫ��ʵ���ַ���ʾ�����������ü���������
    LCD_DrawLine(20, 280, 50, 280, 0xFFFF); // �򵥵ĺ��ߣ���������
}



// �ܳ��������� (32x32���أ����԰������Ŵ�ȫ��)
const uint16_t sports_car_32x32[] = {
    // ������0x0000��ʾ͸��/������0xFFFF��ʾ��ɫ����0xF800��ʾ��ɫװ��
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    
    // ��������
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    
    // ������ϸ��
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    
    // �������������...
};

// ���򵥵��ܳ����� (16x8���أ��ʺϿ��ٲ���)
const uint16_t simple_car[] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
    0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};


// ����λͼ����
void LCD_DrawBitmap(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *bitmap) {
    for (uint16_t j = 0; j < height; j++) {
        for (uint16_t i = 0; i < width; i++) {
            uint16_t color = bitmap[j * width + i];
            if (color != 0x0000) { // ������͸������
                LCD_DrawPoint(x + i, y + j, color);
            }
        }
    }
}

// ȫ���Ŵ���ƺ���
void LCD_DrawBitmapScaled(uint16_t x, uint16_t y, uint16_t orig_width, uint16_t orig_height, 
                          uint16_t scale, const uint16_t *bitmap) {
    for (uint16_t j = 0; j < orig_height; j++) {
        for (uint16_t i = 0; i < orig_width; i++) {
            uint16_t color = bitmap[j * orig_width + i];
            if (color != 0x0000) {
                // �Ŵ����
                for (uint16_t sj = 0; sj < scale; sj++) {
                    for (uint16_t si = 0; si < scale; si++) {
                        LCD_DrawPoint(x + i * scale + si, y + j * scale + sj, color);
                    }
                }
            }
        }
    }
}




// ����˧���ܳ���ȫ���汾��
void DrawSportsCarFullScreen() {
    // ����Ϊ��ɫ����
    LCD_Fill(0, 0, 171, 319, 0x0000);
    
    // �������λ�� (32x32���طŴ�5����160x160)
    uint16_t car_width = 32;
    uint16_t car_height = 32;
    uint16_t scale = 5;
    uint16_t scaled_width = car_width * scale;
    uint16_t scaled_height = car_height * scale;
    
    uint16_t start_x = (172 - scaled_width) / 2;
    uint16_t start_y = (320 - scaled_height) / 2;
    
    // �����ܳ���ʹ�÷Ŵ��ܣ�
    LCD_DrawBitmapScaled(start_x, start_y, car_width, car_height, scale, sports_car_32x32);
    
    // ��Ӷ�̬Ч����β����˸
    for (int blink = 0; blink < 3; blink++) {
        // ���ƺ�ɫβ��
        LCD_FillRect(start_x + 5 * scale, start_y + 28 * scale, 
                    start_x + 8 * scale, start_y + 30 * scale, 0xF800);
        LCD_FillRect(start_x + 24 * scale, start_y + 28 * scale, 
                    start_x + 27 * scale, start_y + 30 * scale, 0xF800);
        
        // ����ʱ��ʵ��ʹ��ʱ��Ҫ����ȷ����ʱ��
        for (volatile int d = 0; d < 1000000; d++);
        
        // �ر�β��
        LCD_FillRect(start_x + 5 * scale, start_y + 28 * scale, 
                    start_x + 8 * scale, start_y + 30 * scale, 0x0000);
        LCD_FillRect(start_x + 24 * scale, start_y + 28 * scale, 
                    start_x + 27 * scale, start_y + 30 * scale, 0x0000);
        
        for (volatile int d = 0; d < 500000; d++);
    }
}


// ������ɫ��RGB565��ʽ��
#define COLOR_BLACK    0x0000
#define COLOR_WHITE    0xFFFF
#define COLOR_BLUE     0x001F
#define COLOR_DARK_BLUE 0x000F

#define STAR_COUNT 8       // ��������
#define MAX_BRIGHTNESS 255 // �������

// �������ݽṹ
typedef struct {
    uint16_t x, y;        // λ������
    uint8_t size;         // ���Ǵ�С
    uint16_t color;       // ������ɫ������ɫ��
    int brightness;       // ��ǰ���� (0-255)
    int brightness_delta; // ���ȱ仯����
} Star;

Star stars[STAR_COUNT];


// ��ʼ�����ǲ���
void Stars_Init(void) {
    for (int i = 0; i < STAR_COUNT; i++) {
        stars[i].x = rand() % 172;      // ���Xλ��
        stars[i].y = rand() % 320;      // ���Yλ��
        stars[i].size = 1 + rand() % 3; // �����С(1-3����)
        stars[i].color = COLOR_WHITE;   // ���ǻ�����ɫ
        
        // �����ʼ״̬����������
        stars[i].brightness = rand() % 256;
        stars[i].brightness_delta = (rand() % 5) + 1; // ����仯�ٶ�
    }
}

// ���ݻ�����ɫ�����ȼ���ʵ����ʾ��ɫ
uint16_t GetStarColor(uint16_t baseColor, int brightness) {
    // �򻯵����ȵ���������ɫ�����ȱ������
    uint8_t r = ((baseColor >> 11) & 0x1F) * brightness / 255;
    uint8_t g = ((baseColor >> 5) & 0x3F) * brightness / 255;
    uint8_t b = (baseColor & 0x1F) * brightness / 255;
    
    return (r << 11) | (g << 5) | b;
}

// ���Ƶ�������
void DrawStar(Star* star) {
    uint16_t drawColor = GetStarColor(star->color, star->brightness);
    uint8_t size = star->size;
    
    // �������Ǵ�С���Ʋ�ͬͼ��
    if (size == 1) {
        LCD_DrawPixel(star->x, star->y, drawColor);
    } else {
        // �����Դ�һ������ǣ�ʮ���Σ�
        for (int i = -size; i <= size; i++) {
            LCD_DrawPixel(star->x + i, star->y, drawColor);
            LCD_DrawPixel(star->x, star->y + i, drawColor);
        }
    }
}

// ������������״̬����˸Ч����
void Stars_Update(void) {
    for (int i = 0; i < STAR_COUNT; i++) {
        // ��������
        stars[i].brightness += stars[i].brightness_delta;
        
        // ���ȱ߽��飬ʵ�ֺ���Ч��
        if (stars[i].brightness <= 50) {
            stars[i].brightness = 50;
            stars[i].brightness_delta = -stars[i].brightness_delta; // ��ת����
        } else if (stars[i].brightness >= MAX_BRIGHTNESS) {
            stars[i].brightness = MAX_BRIGHTNESS;
            stars[i].brightness_delta = -stars[i].brightness_delta;
        }
    }
}

// ���ƽ��䱳��������ɫ���䣩
void DrawGradientBackground(void) {
    for (uint16_t y = 0; y < 320; y++) {
        // ���㵱ǰ�е���ɫ�����ϵ��½��䣩
        uint16_t color = COLOR_DARK_BLUE + (y / 320.0) * 0x0010;
        LCD_Fill(0, y, 171, y, color);
    }
}

// ���ǳ���Ⱦ������
void DrawStarfield(void) {
    // ��ʼ������
    Stars_Init();
    
    while (1) {
        // ���ƽ��䱳��
        DrawGradientBackground();
        
        // ������������
        for (int i = 0; i < STAR_COUNT; i++) {
            DrawStar(&stars[i]);
        }
        
        // ��������״̬��ʵ����˸��
        Stars_Update();
        
        // ���ƶ���֡��
        Delay_Ms(100); // 10FPS
    }
}

// ����������ɫ��ĳ��Ȩ��t�µĹ���ɫ (RGB565��ʽ)
uint16_t Color_Interpolate(uint16_t color1, uint16_t color2, float t) {
    // ��RGB565��ɫֵ����ȡR��G��B��������5-6-5λ��
    uint8_t r1 = (color1 >> 11) & 0x1F;
    uint8_t g1 = (color1 >> 5)  & 0x3F;
    uint8_t b1 = color1 & 0x1F;

    uint8_t r2 = (color2 >> 11) & 0x1F;
    uint8_t g2 = (color2 >> 5)  & 0x3F;
    uint8_t b2 = color2 & 0x1F;

    // �������Բ�ֵ���㣬��������ϲ���RGB565��ʽ
    uint8_t r = (uint8_t)(r1 * (1 - t) + r2 * t);
    uint8_t g = (uint8_t)(g1 * (1 - t) + g2 * t);
    uint8_t b = (uint8_t)(b1 * (1 - t) + b2 * t);

    return (r << 11) | (g << 5) | b;
}

#define COLOR_START 0x867F // һ����ɫ
#define COLOR_END   0xFC1F // һ�ַ�ɫ

void Draw_Gradient() {
    // ����������ĻΪ��ͼ����
    LCD_SetWindow(0, 0, 171, 319); // 172x320 ����Ļ��������0-171��0-319

    // ѭ��������Ļ�ϵ�ÿһ��
    for (uint16_t y = 0; y < 320; y++) {
        // ���㵱ǰ���ڽ����е�λ��Ȩ��t (0.0 �ڶ���, 1.0 �ڵײ�)
        float t = (float)y / 319.0;

        // ������һ��Ӧ����ʾ����ɫ
        uint16_t line_color = Color_Interpolate(COLOR_START, COLOR_END, t);

        // ��16λ����ɫ���Ϊ��8λ�͵�8λ
        uint8_t color_high = line_color >> 8;
        uint8_t color_low  = line_color & 0xFF;

        // ��һ����172�����أ�ѭ������һ��
        for (uint16_t x = 0; x < 172; x++) {
            Write_Data(color_high); // �ȷ�����ɫ���ֽ�
            Write_Data(color_low);  // �ٷ�����ɫ���ֽ�
        }
    }
}
// �򵥵�ȫ��������
void Test_SolidColor() {
    LCD_SetWindow(0, 0, 171, 319);
    for (uint32_t i = 0; i < (172 * 320); i++) {
        Write_Data(0xF8); // ��ɫ���ֽ� (RGB565: 0xF800)
        Write_Data(0x00); // ��ɫ���ֽ�
    }
}









//////////////////////////////////////////////////

LCD_1IN47_ATTRIBUTES LCD_1IN47;

/******************************************************************************
function :	Hardware reset
parameter:
******************************************************************************/
static void LCD_1IN47_Reset(void)
{
	LCD_1IN47_RST_1;
	DEV_Delay_ms(100);
	LCD_1IN47_RST_0;
	DEV_Delay_ms(100);
	LCD_1IN47_RST_1;
	DEV_Delay_ms(100);
}

/******************************************************************************
function :	send command
parameter:
	 Reg : Command register
******************************************************************************/
static void LCD_1IN47_SendCommand(UBYTE Reg)
{
	LCD_1IN47_DC_0;
	LCD_1IN47_CS_0;
	DEV_SPI_WRITE(Reg);
	//LCD_1IN47_CS_1;
}


/******************************************************************************
function :	send data
parameter:
	Data : Write data
******************************************************************************/
static void LCD_1IN47_SendData_8Bit(UBYTE Data)
{
	LCD_1IN47_DC_1;
	//LCD_1IN47_CS_0;
	DEV_SPI_WRITE(Data);
	//LCD_1IN47_CS_1;
}


/******************************************************************************
function :	send data
parameter:
	Data : Write data
******************************************************************************/
static void LCD_1IN47_SendData_16Bit(UWORD Data)
{
	LCD_1IN47_DC_1;
	LCD_1IN47_CS_0;
	DEV_SPI_WRITE((Data >> 8) & 0xFF);
    DEV_SPI_WRITE(Data & 0xFF);
	LCD_1IN47_CS_1;
}

/******************************************************************************
function :	Initialize the lcd register
parameter:
******************************************************************************/
static void LCD_1IN47_InitReg(void)
{


	LCD_1IN47_SendCommand(0x11);
	DEV_Delay_ms(120);

	LCD_1IN47_SendCommand(0x3A);
	LCD_1IN47_SendData_8Bit(0x05);

	LCD_1IN47_SendCommand(0xB2);
	LCD_1IN47_SendData_8Bit(0x0C);
	LCD_1IN47_SendData_8Bit(0x0C);
	LCD_1IN47_SendData_8Bit(0x00);
	LCD_1IN47_SendData_8Bit(0x33);
	LCD_1IN47_SendData_8Bit(0x33);

	LCD_1IN47_SendCommand(0xB7);
	LCD_1IN47_SendData_8Bit(0x35);

	LCD_1IN47_SendCommand(0xBB);
	LCD_1IN47_SendData_8Bit(0x35);

	LCD_1IN47_SendCommand(0xC0);
	LCD_1IN47_SendData_8Bit(0x2C);

	LCD_1IN47_SendCommand(0xC2);
	LCD_1IN47_SendData_8Bit(0x01);

	LCD_1IN47_SendCommand(0xC3);
	LCD_1IN47_SendData_8Bit(0x13);

	LCD_1IN47_SendCommand(0xC4);
	LCD_1IN47_SendData_8Bit(0x20);

	LCD_1IN47_SendCommand(0xC6);
	LCD_1IN47_SendData_8Bit(0x0F);

	LCD_1IN47_SendCommand(0xD0);
	LCD_1IN47_SendData_8Bit(0xA4);
	LCD_1IN47_SendData_8Bit(0xA1);

	LCD_1IN47_SendCommand(0xD6);
	LCD_1IN47_SendData_8Bit(0xA1);

	LCD_1IN47_SendCommand(0xE0);
	LCD_1IN47_SendData_8Bit(0xF0);
	LCD_1IN47_SendData_8Bit(0x00);
	LCD_1IN47_SendData_8Bit(0x04);
	LCD_1IN47_SendData_8Bit(0x04);
	LCD_1IN47_SendData_8Bit(0x04);
	LCD_1IN47_SendData_8Bit(0x05);
	LCD_1IN47_SendData_8Bit(0x29);
	LCD_1IN47_SendData_8Bit(0x33);
	LCD_1IN47_SendData_8Bit(0x3E);
	LCD_1IN47_SendData_8Bit(0x38);
	LCD_1IN47_SendData_8Bit(0x12);
	LCD_1IN47_SendData_8Bit(0x12);
	LCD_1IN47_SendData_8Bit(0x28);
	LCD_1IN47_SendData_8Bit(0x30);

	LCD_1IN47_SendCommand(0xE1);
	LCD_1IN47_SendData_8Bit(0xF0);
	LCD_1IN47_SendData_8Bit(0x07);
	LCD_1IN47_SendData_8Bit(0x0A);
	LCD_1IN47_SendData_8Bit(0x0D);
	LCD_1IN47_SendData_8Bit(0x0B);
	LCD_1IN47_SendData_8Bit(0x07);
	LCD_1IN47_SendData_8Bit(0x28);
	LCD_1IN47_SendData_8Bit(0x33);
	LCD_1IN47_SendData_8Bit(0x3E);
	LCD_1IN47_SendData_8Bit(0x36);
	LCD_1IN47_SendData_8Bit(0x14);
	LCD_1IN47_SendData_8Bit(0x14);
	LCD_1IN47_SendData_8Bit(0x29);
	LCD_1IN47_SendData_8Bit(0x32);

	LCD_1IN47_SendCommand(0x21);

	LCD_1IN47_SendCommand(0x11);
	DEV_Delay_ms(120);
	LCD_1IN47_SendCommand(0x29);
}




/********************************************************************************
function:	Set the resolution and scanning method of the screen
parameter:
		Scan_dir:   Scan direction
********************************************************************************/
static void LCD_1IN47_SetAttributes(UBYTE Scan_dir)
{
	// Get the screen scan direction
	LCD_1IN47.SCAN_DIR = Scan_dir;
	UBYTE MemoryAccessReg = 0x00;

	// Get GRAM and LCD width and height
	if (Scan_dir == HORIZONTAL)
	{
		LCD_1IN47.HEIGHT = LCD_1IN47_WIDTH;
		LCD_1IN47.WIDTH = LCD_1IN47_HEIGHT;
		MemoryAccessReg = 0X00;
	}
	else
	{
		LCD_1IN47.HEIGHT = LCD_1IN47_HEIGHT;
		LCD_1IN47.WIDTH = LCD_1IN47_WIDTH;		
		MemoryAccessReg = 0X70;
	}

	// Set the read / write scan direction of the frame memory
	LCD_1IN47_SendCommand(0x36); // MX, MY, RGB mode
    //LCD_1IN47_SendData_8Bit(0x08);	//0x08 set RGB
	LCD_1IN47_SendData_8Bit(MemoryAccessReg); // 0x08 set RGB
}



/********************************************************************************
function :	Initialize the lcd
parameter:
********************************************************************************/
void LCD_1IN47_Init(UBYTE Scan_dir)
{

    SPI1_Init();
    //SPI2_DMA_Init();
    LCD_GPIOInit();

    LCD_LED_SET;
    LCD_CS_SET;
    LCD_DC_SET;
    LCD_RST_SET;
	// Hardware reset
	LCD_1IN47_Reset();
    
	// Set the resolution and scanning method of the screen
	LCD_1IN47_SetAttributes(Scan_dir);

	// Set the initialization register
	LCD_1IN47_InitReg();
}

/********************************************************************************
function:	Sets the start position and size of the display area
parameter:
		Xstart 	:   X direction Start coordinates
		Ystart  :   Y direction Start coordinates
		Xend    :   X direction end coordinates
		Yend    :   Y direction end coordinates
********************************************************************************/
void LCD_1IN47_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
	if (LCD_1IN47.SCAN_DIR == HORIZONTAL)
	{ 
		
		// set the X coordinates
		LCD_1IN47_SendCommand(0x2A);
		LCD_1IN47_SendData_8Bit(0x00);
		LCD_1IN47_SendData_8Bit(Xstart + 0x22);
		LCD_1IN47_SendData_8Bit(((Xend + 0x22) - 1) >> 8);
		LCD_1IN47_SendData_8Bit((Xend + 0x22) - 1);

		// set the Y coordinates
		LCD_1IN47_SendCommand(0x2B);
		LCD_1IN47_SendData_8Bit(0x00);
		LCD_1IN47_SendData_8Bit(Ystart);
		LCD_1IN47_SendData_8Bit((Yend - 1) >> 8);
		LCD_1IN47_SendData_8Bit(Yend - 1);
	}
	else
	{ 
		// set the X coordinates
		LCD_1IN47_SendCommand(0x2A);
		LCD_1IN47_SendData_8Bit(Xstart >> 8);
		LCD_1IN47_SendData_8Bit(Xstart);
		LCD_1IN47_SendData_8Bit((Xend - 1) >> 8);
		LCD_1IN47_SendData_8Bit(Xend - 1);

		// set the Y coordinates
		LCD_1IN47_SendCommand(0x2B);
		LCD_1IN47_SendData_8Bit(Ystart >> 8);
		LCD_1IN47_SendData_8Bit(Ystart + 0x22);
		LCD_1IN47_SendData_8Bit((Yend - 1 + 0x22) >> 8);
		LCD_1IN47_SendData_8Bit(Yend - 1 + 0x22);
	}

	LCD_1IN47_SendCommand(0X2C);
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void LCD_1IN47_Clear(UWORD Color)
{
	UWORD i,j;
  LCD_1IN47_SetWindows(0, 0, LCD_1IN47.WIDTH, LCD_1IN47.HEIGHT);

	LCD_1IN47_DC_1;
	for(i = 0; i < LCD_1IN47_WIDTH; i++){
		for(j = 0; j < LCD_1IN47_HEIGHT; j++){
			DEV_SPI_WRITE((Color>>8)&0xff);
			DEV_SPI_WRITE(Color);
		}
	 }
}

/******************************************************************************
function :	Sends the image buffer in RAM to displays
parameter:
******************************************************************************/
void LCD_1IN47_Display(UWORD *Image)
{
		UWORD i,j;
    LCD_1IN47_SetWindows(0, 0, LCD_1IN47.WIDTH, LCD_1IN47.HEIGHT);
    LCD_1IN47_DC_1;
    for(i = 0; i < LCD_1IN47_WIDTH; i++){
      for(j = 0; j < LCD_1IN47_HEIGHT; j++){
        DEV_SPI_WRITE((*(Image+i*LCD_1IN47_HEIGHT+j)>>8)&0xff);
        DEV_SPI_WRITE(*(Image+i*LCD_1IN47_WIDTH+j));
      }
    }

}

void LCD_1IN47_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image)
{
	// display
    UDOUBLE Addr = 0;

    UWORD i,j;
    LCD_1IN47_SetWindows(Xstart, Ystart, Xend , Yend);
    LCD_1IN47_DC_1;
    for (i = Ystart; i < Yend - 1; i++) {
        Addr = Xstart + i * LCD_1IN47_WIDTH ;
        for(j=Xstart;j<Xend-1;j++){
          DEV_SPI_WRITE((*(Image+Addr+j)>>8)&0xff);
          DEV_SPI_WRITE(*(Image+Addr+j));
				}  
		}
}

void LCD_1IN47_DrawPoint(UWORD X, UWORD Y, UWORD Color)
{
	LCD_1IN47_SetWindows(X, Y, X, Y);
	LCD_1IN47_SendData_16Bit(Color);
}

void DrawHeart() {
    UWORD center_x = 86;
    UWORD center_y = 160;
    UWORD heart_color = 0xF800;
    
    // ���Ĵ�С�仯
    for(UWORD size = 30; size <= 60; size += 2) {
        LCD_1IN47_Clear(0x0000);
        
        for(float t = 0.0f; t <= 6.283f; t += 0.02f) {
            float x = 16 * pow(sin(t), 3);
            float y = 13 * cos(t) - 5 * cos(2*t) - 2 * cos(3*t) - cos(4*t);
            
            UWORD screen_x = center_x + (UWORD)(x * size / 16);
            UWORD screen_y = center_y - (UWORD)(y * size / 16);
            
            if(screen_x < LCD_1IN47.WIDTH && screen_y < LCD_1IN47.HEIGHT) {
                LCD_1IN47_DrawPoint(screen_x, screen_y, heart_color);
            }
        }
        Delay_Ms(1);
    }
}



void Draw_Solid_Heart() {
    int center_x = 160;
    int center_y = 85;
    int size = 40;

    // ���á�����������ɫ����
    uint16_t color_start = 0x2128;  // �����
    uint16_t color_end = 0x5D3C;    // ����ɫ
    uint16_t heart_color = 0xFD80;  // ���Ľ�

    LCD_1IN47_SetWindows(0, 0, 320, 172);

    for (int y = 0; y < 172; y++) {
        float t = (float)y / 171.0;
        uint16_t gradient_color = Color_Interpolate(color_start, color_end, t);
        
        for (int x = 0; x < 320; x++) {
            float dx = (x - center_x) / (float)size;
            float dy = (y - center_y) / (float)size;
            float heart_eq = my_pow_basic(dx*dx + dy*dy - 1, 3) - dx*dx * dy*dy*dy;

            if (heart_eq < 0.0) { 
                LCD_1IN47_SendData_16Bit(heart_color); // ʹ�ø߼��еİ�����ɫ
            } else {
                LCD_1IN47_SendData_16Bit(gradient_color);
            }
        }
    }
}
