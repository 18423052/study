#ifndef __OLED_H
#define __OLED_H			  	 
#include "sys.h"
#include "stdlib.h"
#define OLED_CMD 0   //命令声明
#define OLED_DATA 1 //数据声明

#define OLED_CLK    PAout(5)    // CLK时钟    d0
#define OLED_MOSI   PAout(7)    // MOSI     d1
#define OLED_RST    PAout(6)    // RET复位    ret
#define OLED_DC     PAout(4)    // 命令|数据    dc  （0表传输命令1表传输数据）

void OLED_SPI_Init(void); //配置MCU的SPI
void SPI_WriteByte(uint8_t addr,uint8_t data); //向寄存器地址写一个byte的数据
void WriteCmd(unsigned char cmd); //写命令
void WriteDat(unsigned char data); //写数据

void OLED_Init(void);//初始化OLED
void OLED_ON(void);//唤醒OLED
void OLED_OFF(void);//OLED休眠
void OLED_Refresh_Gram(void);//更新显存到OLED
void OLED_Clear(void);//清屏
void OLED_DrawPoint(u8 x,u8 y,u8 t);//画点
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);//填充
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);//显示字符
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);//显示2个数字
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size);//显示字符串
void OLED_ShowGBK(u8 x, u8 y,  u8 num, u8 size,u8 mode);//显示汉字
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);
void OLED_Set_Pos(unsigned char x, unsigned char y);
#endif



