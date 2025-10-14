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
    Write_Data(color >> 8);   // 发送颜色数据的高8位
    Write_Data(color & 0xFF); // 发送颜色数据的低8位
}
// 设置将要进行绘图的窗口区域 (x_start, y_start) 到 (x_end, y_end)
void Set_Draw_Window(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end) {
    Write_Command(0x2A); // 列地址设置命令
    Write_Data(x_start >> 8);   Write_Data(x_start & 0xFF); // 起始列高8位和低8位
    Write_Data(x_end >> 8);     Write_Data(x_end & 0xFF);   // 结束列高8位和低8位

    Write_Command(0x2B); // 行地址设置命令
    Write_Data(y_start >> 8);   Write_Data(y_start & 0xFF); // 起始行高8位和低8位
    Write_Data(y_end >> 8);     Write_Data(y_end & 0xFF);   // 结束行高8位和低8位

    Write_Command(0x2C); // 开始写入GRAM（显存）命令
    // 执行此命令后，后续连续写入的数据将直接填充到设定的窗口内
}

double my_pow_basic(double base, int exponent) {
    if (exponent == 0) return 1.0; // 任何数的0次幂都是1[1](@ref)
    
    // 处理负指数：a^(-b) = 1/(a^b)[1](@ref)
    if (exponent < 0) {
        base = 1 / base;
        exponent = -exponent;
    }
    
    double result = 1.0;
    for (int i = 0; i < exponent; i++) {
        result *= base; // 循环相乘[2](@ref)
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
    Write(Parameter , 0xA1);   //sleep in后，gate输出为GND

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

    Write(Command , 0x35);   //打开TE    
    Write(Parameter , 0x00);   

    Write(Command , 0x44);     //移动撕裂线
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





// 设置绘图窗口 (x_start, y_start) 到 (x_end, y_end)
void LCD_SetWindow(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end) {
    Write(Command, 0x2A); // 列地址设置命令
    Write(Parameter, x_start >> 8);   Write(Parameter, x_start & 0xFF);
    Write(Parameter, x_end >> 8);     Write(Parameter, x_end & 0xFF);

    Write(Command, 0x2B); // 行地址设置命令
    Write(Parameter, y_start >> 8);   Write(Parameter, y_start & 0xFF);
    Write(Parameter, y_end >> 8);     Write(Parameter, y_end & 0xFF);

    Write(Command, 0x2C); // 开始写入显存命令
}

// 绘制单个像素点
void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if (x >= 320 || y >= 172) return;
    LCD_SetWindow(x, y, x, y);
    Write(Parameter, color >> 8);
    Write(Parameter, color & 0xFF);
}
// 在指定坐标(x,y)绘制一个点
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color) {
    if (x >= 320 || y >= 172) return; // 确保坐标在屏幕范围内 (172x320)
    LCD_SetWindow(x, y, x, y); // 将窗口设置为单个点
    // 发送16位颜色值（RGB565格式）
    Write(Parameter, color >> 8);   // 发送高8位
    Write(Parameter, color & 0xFF); // 发送低8位
}

// 用指定颜色填充一个矩形区域
void LCD_Fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color) {
    uint32_t total_pixels = (x_end - x_start + 1) * (y_end - y_start + 1);
    LCD_SetWindow(x_start, y_start, x_end, y_end);
    
    // 连续发送颜色数据，快速填充
    for (uint32_t i = 0; i < total_pixels; i++) {
        Write(Parameter, color >> 8);
        Write(Parameter, color & 0xFF);
    }
}

// 使用Bresenham算法绘制直线
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    int16_t dx = abs(x2 - x1), dy = -abs(y2 - y1);
    int16_t sx = (x1 < x2) ? 1 : -1;
    int16_t sy = (y1 < y2) ? 1 : -1;
    int16_t err = dx + dy, e2; // 误差项

    while (1) {
        LCD_DrawPoint(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { // 水平误差更大
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) { // 垂直误差更大
            err += dx;
            y1 += sy;
        }
    }
}


// 绘制矩形边框
void LCD_DrawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    LCD_DrawLine(x1, y1, x2, y1, color); // 上边
    LCD_DrawLine(x1, y2, x2, y2, color); // 下边
    LCD_DrawLine(x1, y1, x1, y2, color); // 左边
    LCD_DrawLine(x2, y1, x2, y2, color); // 右边
}

// 绘制实心矩形
void LCD_FillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    LCD_Fill(x1, y1, x2, y2, color);
}


// 使用Bresenham算法绘制圆形
void LCD_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) {
    int16_t x = 0, y = r;
    int16_t d = 3 - 2 * r; // 决策参数

    while (x <= y) {
        // 利用圆的八对称性，一次画八个点
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

// 定义常用颜色（RGB565格式）
#define COLOR_SKY_BLUE    0x067D  // 天蓝色
#define COLOR_GRASS_GREEN 0x07E0  // 鲜绿色
#define COLOR_SUN_YELLOW  0xFFE0  // 明黄色
#define COLOR_TRUNK_BROWN 0xBAAB  // 树干棕色
#define COLOR_LEAF_GREEN  0x3666  // 树叶深绿

void DrawScenery() {
    // 1. 绘制背景：上半部分天空，下半部分草地
    LCD_Fill(0, 0, 171, 159, COLOR_SKY_BLUE); // 上半屏天空
    LCD_Fill(0, 160, 171, 319, COLOR_GRASS_GREEN); // 下半屏草地

    // 2. 绘制太阳（右上角的一个圆）
    LCD_DrawCircle(140, 40, 20, COLOR_SUN_YELLOW);
    // 为太阳添加一些光线（简单的短线）
    for (int i = 0; i < 12; i++) {
        uint16_t angle = i * 30;
        uint16_t x1 = 140 + 25 * cos(angle * 3.14159 / 180);
        uint16_t y1 = 40 + 25 * sin(angle * 3.14159 / 180);
        uint16_t x2 = 140 + 35 * cos(angle * 3.14159 / 180);
        uint16_t y2 = 40 + 35 * sin(angle * 3.14159 / 180);
        LCD_DrawLine(x1, y1, x2, y2, COLOR_SUN_YELLOW);
    }

    // 3. 绘制一棵树：树干和树冠
    // 树干（一个细长的矩形）
    LCD_FillRect(80, 200, 84, 250, COLOR_TRUNK_BROWN);
    // 树冠（一个实心圆）
    for (int r = 25; r > 0; r -= 2) {
        LCD_DrawCircle(82, 200, r, COLOR_LEAF_GREEN);
    }

    // 4. 在天空中添加一朵云（由几个重叠的圆组成）
    LCD_DrawCircle(50, 60, 15, 0xFFFF); // 白色
    LCD_DrawCircle(70, 60, 15, 0xFFFF);
    LCD_DrawCircle(60, 50, 15, 0xFFFF);
    LCD_DrawCircle(60, 70, 15, 0xFFFF);

    // 5. 添加一些文字标签
    // 注意：需要先实现字符显示函数，这里用简单线条代替
    LCD_DrawLine(20, 280, 50, 280, 0xFFFF); // 简单的横线，代表文字
}



// 跑车像素数据 (32x32像素，可以按比例放大到全屏)
const uint16_t sports_car_32x32[] = {
    // 这里用0x0000表示透明/背景，0xFFFF表示白色车身，0xF800表示红色装饰
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    
    // 车身主体
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    
    // 车窗和细节
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    
    // 继续定义更多行...
};

// 更简单的跑车轮廓 (16x8像素，适合快速测试)
const uint16_t simple_car[] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
    0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};


// 绘制位图函数
void LCD_DrawBitmap(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *bitmap) {
    for (uint16_t j = 0; j < height; j++) {
        for (uint16_t i = 0; i < width; i++) {
            uint16_t color = bitmap[j * width + i];
            if (color != 0x0000) { // 不绘制透明像素
                LCD_DrawPoint(x + i, y + j, color);
            }
        }
    }
}

// 全屏放大绘制函数
void LCD_DrawBitmapScaled(uint16_t x, uint16_t y, uint16_t orig_width, uint16_t orig_height, 
                          uint16_t scale, const uint16_t *bitmap) {
    for (uint16_t j = 0; j < orig_height; j++) {
        for (uint16_t i = 0; i < orig_width; i++) {
            uint16_t color = bitmap[j * orig_width + i];
            if (color != 0x0000) {
                // 放大绘制
                for (uint16_t sj = 0; sj < scale; sj++) {
                    for (uint16_t si = 0; si < scale; si++) {
                        LCD_DrawPoint(x + i * scale + si, y + j * scale + sj, color);
                    }
                }
            }
        }
    }
}




// 绘制帅气跑车（全屏版本）
void DrawSportsCarFullScreen() {
    // 清屏为黑色背景
    LCD_Fill(0, 0, 171, 319, 0x0000);
    
    // 计算居中位置 (32x32像素放大5倍到160x160)
    uint16_t car_width = 32;
    uint16_t car_height = 32;
    uint16_t scale = 5;
    uint16_t scaled_width = car_width * scale;
    uint16_t scaled_height = car_height * scale;
    
    uint16_t start_x = (172 - scaled_width) / 2;
    uint16_t start_y = (320 - scaled_height) / 2;
    
    // 绘制跑车（使用放大功能）
    LCD_DrawBitmapScaled(start_x, start_y, car_width, car_height, scale, sports_car_32x32);
    
    // 添加动态效果：尾灯闪烁
    for (int blink = 0; blink < 3; blink++) {
        // 绘制红色尾灯
        LCD_FillRect(start_x + 5 * scale, start_y + 28 * scale, 
                    start_x + 8 * scale, start_y + 30 * scale, 0xF800);
        LCD_FillRect(start_x + 24 * scale, start_y + 28 * scale, 
                    start_x + 27 * scale, start_y + 30 * scale, 0xF800);
        
        // 简单延时（实际使用时需要更精确的延时）
        for (volatile int d = 0; d < 1000000; d++);
        
        // 关闭尾灯
        LCD_FillRect(start_x + 5 * scale, start_y + 28 * scale, 
                    start_x + 8 * scale, start_y + 30 * scale, 0x0000);
        LCD_FillRect(start_x + 24 * scale, start_y + 28 * scale, 
                    start_x + 27 * scale, start_y + 30 * scale, 0x0000);
        
        for (volatile int d = 0; d < 500000; d++);
    }
}


// 定义颜色（RGB565格式）
#define COLOR_BLACK    0x0000
#define COLOR_WHITE    0xFFFF
#define COLOR_BLUE     0x001F
#define COLOR_DARK_BLUE 0x000F

#define STAR_COUNT 8       // 星星数量
#define MAX_BRIGHTNESS 255 // 最大亮度

// 星星数据结构
typedef struct {
    uint16_t x, y;        // 位置坐标
    uint8_t size;         // 星星大小
    uint16_t color;       // 星星颜色（基础色）
    int brightness;       // 当前亮度 (0-255)
    int brightness_delta; // 亮度变化步长
} Star;

Star stars[STAR_COUNT];


// 初始化星星参数
void Stars_Init(void) {
    for (int i = 0; i < STAR_COUNT; i++) {
        stars[i].x = rand() % 172;      // 随机X位置
        stars[i].y = rand() % 320;      // 随机Y位置
        stars[i].size = 1 + rand() % 3; // 随机大小(1-3像素)
        stars[i].color = COLOR_WHITE;   // 星星基础颜色
        
        // 随机初始状态，创造错落感
        stars[i].brightness = rand() % 256;
        stars[i].brightness_delta = (rand() % 5) + 1; // 随机变化速度
    }
}

// 根据基础颜色和亮度计算实际显示颜色
uint16_t GetStarColor(uint16_t baseColor, int brightness) {
    // 简化的亮度调整：将白色按亮度比例混合
    uint8_t r = ((baseColor >> 11) & 0x1F) * brightness / 255;
    uint8_t g = ((baseColor >> 5) & 0x3F) * brightness / 255;
    uint8_t b = (baseColor & 0x1F) * brightness / 255;
    
    return (r << 11) | (g << 5) | b;
}

// 绘制单个星星
void DrawStar(Star* star) {
    uint16_t drawColor = GetStarColor(star->color, star->brightness);
    uint8_t size = star->size;
    
    // 根据星星大小绘制不同图案
    if (size == 1) {
        LCD_DrawPixel(star->x, star->y, drawColor);
    } else {
        // 绘制稍大一点的星星（十字形）
        for (int i = -size; i <= size; i++) {
            LCD_DrawPixel(star->x + i, star->y, drawColor);
            LCD_DrawPixel(star->x, star->y + i, drawColor);
        }
    }
}

// 更新所有星星状态（闪烁效果）
void Stars_Update(void) {
    for (int i = 0; i < STAR_COUNT; i++) {
        // 更新亮度
        stars[i].brightness += stars[i].brightness_delta;
        
        // 亮度边界检查，实现呼吸效果
        if (stars[i].brightness <= 50) {
            stars[i].brightness = 50;
            stars[i].brightness_delta = -stars[i].brightness_delta; // 反转方向
        } else if (stars[i].brightness >= MAX_BRIGHTNESS) {
            stars[i].brightness = MAX_BRIGHTNESS;
            stars[i].brightness_delta = -stars[i].brightness_delta;
        }
    }
}

// 绘制渐变背景（深蓝色渐变）
void DrawGradientBackground(void) {
    for (uint16_t y = 0; y < 320; y++) {
        // 计算当前行的颜色（从上到下渐变）
        uint16_t color = COLOR_DARK_BLUE + (y / 320.0) * 0x0010;
        LCD_Fill(0, y, 171, y, color);
    }
}

// 星星场渲染主函数
void DrawStarfield(void) {
    // 初始化星星
    Stars_Init();
    
    while (1) {
        // 绘制渐变背景
        DrawGradientBackground();
        
        // 绘制所有星星
        for (int i = 0; i < STAR_COUNT; i++) {
            DrawStar(&stars[i]);
        }
        
        // 更新星星状态（实现闪烁）
        Stars_Update();
        
        // 控制动画帧率
        Delay_Ms(100); // 10FPS
    }
}

// 计算两种颜色在某个权重t下的过渡色 (RGB565格式)
uint16_t Color_Interpolate(uint16_t color1, uint16_t color2, float t) {
    // 从RGB565颜色值中提取R、G、B分量（各5-6-5位）
    uint8_t r1 = (color1 >> 11) & 0x1F;
    uint8_t g1 = (color1 >> 5)  & 0x3F;
    uint8_t b1 = color1 & 0x1F;

    uint8_t r2 = (color2 >> 11) & 0x1F;
    uint8_t g2 = (color2 >> 5)  & 0x3F;
    uint8_t b2 = color2 & 0x1F;

    // 进行线性插值计算，并将结果合并回RGB565格式
    uint8_t r = (uint8_t)(r1 * (1 - t) + r2 * t);
    uint8_t g = (uint8_t)(g1 * (1 - t) + g2 * t);
    uint8_t b = (uint8_t)(b1 * (1 - t) + b2 * t);

    return (r << 11) | (g << 5) | b;
}

#define COLOR_START 0x867F // 一种蓝色
#define COLOR_END   0xFC1F // 一种粉色

void Draw_Gradient() {
    // 设置整个屏幕为绘图区域
    LCD_SetWindow(0, 0, 171, 319); // 172x320 的屏幕，坐标是0-171和0-319

    // 循环遍历屏幕上的每一行
    for (uint16_t y = 0; y < 320; y++) {
        // 计算当前行在渐变中的位置权重t (0.0 在顶部, 1.0 在底部)
        float t = (float)y / 319.0;

        // 计算这一行应该显示的颜色
        uint16_t line_color = Color_Interpolate(COLOR_START, COLOR_END, t);

        // 将16位的颜色拆分为高8位和低8位
        uint8_t color_high = line_color >> 8;
        uint8_t color_low  = line_color & 0xFF;

        // 这一行有172个像素，循环绘制一行
        for (uint16_t x = 0; x < 172; x++) {
            Write_Data(color_high); // 先发送颜色高字节
            Write_Data(color_low);  // 再发送颜色低字节
        }
    }
}
// 简单的全屏填充测试
void Test_SolidColor() {
    LCD_SetWindow(0, 0, 171, 319);
    for (uint32_t i = 0; i < (172 * 320); i++) {
        Write_Data(0xF8); // 红色高字节 (RGB565: 0xF800)
        Write_Data(0x00); // 红色低字节
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
    
    // 爱心大小变化
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

    // 采用「深邃蓝金」配色方案
    uint16_t color_start = 0x2128;  // 深藏青
    uint16_t color_end = 0x5D3C;    // 灰蓝色
    uint16_t heart_color = 0xFD80;  // 香槟金

    LCD_1IN47_SetWindows(0, 0, 320, 172);

    for (int y = 0; y < 172; y++) {
        float t = (float)y / 171.0;
        uint16_t gradient_color = Color_Interpolate(color_start, color_end, t);
        
        for (int x = 0; x < 320; x++) {
            float dx = (x - center_x) / (float)size;
            float dy = (y - center_y) / (float)size;
            float heart_eq = my_pow_basic(dx*dx + dy*dy - 1, 3) - dx*dx * dy*dy*dy;

            if (heart_eq < 0.0) { 
                LCD_1IN47_SendData_16Bit(heart_color); // 使用高级感的爱心颜色
            } else {
                LCD_1IN47_SendData_16Bit(gradient_color);
            }
        }
    }
}
