#include "OLED.h"
#include "stdlib.h"
#include "oledfont.h"  	 
#include "delay.h"
/*************************************************************************/
/*函数功能: GPIO模拟SPI端口初始化                                         */
/*************************************************************************/
void OLED_SPI_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);//使能PA端口时钟
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7; //端口配置
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;//IO口速度为50MHz
    GPIO_Init(GPIOA,&GPIO_InitStructure);//根据设定参数初始化GPIOA
}
 
/*************************************************************************/
/*函数功能: 通过SPIO软件模拟SPI通信协议,向模块(SSD1306)写入一个字节                                  */
/*入口参数：                                                                                                                          */ 
/*                      data：要写入的数据/命令                                                                       */             
/*                      cmd ：数据/命令标志 0,表示命令;1,表示数据;                   */
/*************************************************************************/
void SPI_WriteByte(unsigned char data,unsigned char cmd)
{
    unsigned char i=0;
    OLED_DC =cmd;
    OLED_CLK=0;
    for(i=0;i<8;i++)
    {
        OLED_CLK=0;
        if(data&0x80)OLED_MOSI=1; //从高位到低位
        else OLED_MOSI=0;
        OLED_CLK=1;
        data<<=1;
    }
    OLED_CLK=1;
    OLED_DC=1;
}
/*************************************************************************/
/*函数功能: 写命令                                                        */
/*************************************************************************/
void WriteCmd(unsigned char cmd)
{
    SPI_WriteByte(cmd,OLED_CMD);
}
/*************************************************************************/
/*函数功能: 写数据                                                        */
/*************************************************************************/
void WriteData(unsigned char data)
{
    SPI_WriteByte(data,OLED_DATA);
}
//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127    
//[1]0 1 2 3 ... 127    
//[2]0 1 2 3 ... 127    
//[3]0 1 2 3 ... 127    
//[4]0 1 2 3 ... 127    
//[5]0 1 2 3 ... 127    
//[6]0 1 2 3 ... 127    
//[7]0 1 2 3 ... 127           
u8 OLED_GRAM[128][8];   
 
/*************************************************************************/
/*函数功能: 软延时                                                        */
/*************************************************************************/
void OLED_DLY_ms(unsigned int ms)
{                         
  unsigned int a;
  while(ms)
  {
    a=1335;
    while(a--);
    ms--;
  }
}
/*************************************************************************/
/*函数功能: 初始化OLED模块                                                */
/*************************************************************************/
void OLED_Init(void)
{
    OLED_SPI_Init();
    OLED_CLK = 1;
    OLED_RST = 0;
    OLED_DLY_ms(100);
    OLED_RST = 1;
 
    //从上电到下面开始初始化要有足够的时间，即等待RC复位完毕
    WriteCmd(0xAE);         // Display Off (0x00)
    WriteCmd(0xD5);
    WriteCmd(0x80);         // Set Clock as 100 Frames/Sec
    WriteCmd(0xA8);
    WriteCmd(0x3F);         // 1/64 Duty (0x0F~0x3F)
    WriteCmd(0xD3);
    WriteCmd(0x00);         // Shift Mapping RAM Counter (0x00~0x3F)
    WriteCmd(0x40 | 0x00);  // Set Mapping RAM Display Start Line (0x00~0x3F)
    WriteCmd(0x8D);
    WriteCmd(0x10 | 0x04);  // Enable Embedded DC/DC Converter (0x00/0x04)
    WriteCmd(0x20);
    WriteCmd(0x02);         // Set Page Addressing Mode (0x00/0x01/0x02)
    WriteCmd(0xA0 | 0x01);  // Set SEG/Column Mapping    
    WriteCmd(0xC0);  // Set COM/x Scan Direction   
    WriteCmd(0xDA);
    WriteCmd(0x02 | 0x10);  // Set Sequential Configuration (0x00/0x10)
    WriteCmd(0x81);
    WriteCmd(0xCF);         // Set SEG Output Current
    WriteCmd(0xD9);
    WriteCmd(0xF1);         // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    WriteCmd(0xDB);
    WriteCmd(0x40);         // Set VCOM Deselect Level
    WriteCmd(0xA4 | 0x00);  // Disable Entire Display On (0x00/0x01)
    WriteCmd(0xA6 | 0x00);  // Disable Inverse Display On (0x00/0x01)
    WriteCmd(0xAE | 0x01);  // Display On (0x01)
 
    OLED_Clear();  //初始清屏   
}
/*************************************************************************/
/*函数功能: 将OLED从休眠中唤醒                                            */
/*************************************************************************/
void OLED_ON(void)
{
    WriteCmd(0X8D);  //设置电荷泵
    WriteCmd(0X14);  //开启电荷泵
    WriteCmd(0XAF);  //OLED唤醒
}
/*************************************************************************/
/*函数功能: 将OLED休眠 -- 休眠模式下,OLED功耗不到10uA                      */
/*************************************************************************/
void OLED_OFF(void)
{
    WriteCmd(0X8D);  //设置电荷泵
    WriteCmd(0X10);  //关闭电荷泵
    WriteCmd(0XAE);  //OLED休眠
}
 
/*************************************************************************/
/*函数功能: 更新显存到OLED                                                 */
/*************************************************************************/
void OLED_Refresh_Gram(void)
{
    u8 i,n;         
    for(i=0;i<8;i++)  
    {  
        WriteCmd(0xb0+i);   //设置页地址（0~7）
        WriteCmd(0x00);      //设置显示位置—列低地址
        WriteCmd(0x10);      //设置显示位置—列高地址   
        for(n=0;n<128;n++)WriteData(OLED_GRAM[n][i]); 
    }   
}
/*************************************************************************/
/*函数功能: 清屏                                                          */
/*************************************************************************/
void OLED_Clear(void)  
{  
    u8 i,n;  
    for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;  
    OLED_Refresh_Gram();//更新显示
}
/*************************************************************************/
/*函数功能: 画点                                                          */
/*入口参数：                                                             */
/*                      x：横坐标       0~127                               */
/*                      y：纵坐标               0~63                     */
/*                      dot:0,清空;1,填充                                                                                */             
/*************************************************************************/
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
    u8 pos,bx,temp=0;
    if(x>127||y>63)return;//超出范围了.
    pos=7-y/8;
    bx=y%8;
    temp=1<<(7-bx);
    if(t)OLED_GRAM[x][pos]|=temp;
    else OLED_GRAM[x][pos]&=~temp;     
}
/*************************************************************************/
/*函数功能: 填充                                                          */
/*入口参数：                                                                                                                          */
/*                      x1,y1,x2,y2 填充区域的对角坐标                              */
/*                      确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63                                         */
/*                      dot:0,清空;1,填充                                                                                */             
/*************************************************************************/
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
    u8 x,y;  
    for(x=x1;x<=x2;x++)
    {
        for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
    }                                                       
    OLED_Refresh_Gram();//更新显示
}
 
/*************************************************************************/
/*函数功能: 在指定位置显示一个字符,包括部分字符                             */
/*入口参数：                                                                                                                          */
/*                      x:0~12                                                     */
/*                      y:0~63                                                                      */
/*                      mode:0,反白显示;1,正常显示                                                       */ 
/*            size:选择字体 16/12                                        */
/*************************************************************************/
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{                   
    u8 temp,t,t1;
    u8 y0=y;
    u8 csize=(size/8+((size%8)?1:0))*(size/2);      //得到字体一个字符对应点阵集所占的字节数
    chr=chr-' ';//得到偏移后的值        
    for(t=0;t<csize;t++)
    {   
        if(size==12)temp=asc2_1206[chr][t];         //调用1206字体
        else if(size==16)temp=asc2_1608[chr][t];    //调用1608字体
        else if(size==24)temp=asc2_2412[chr][t];    //调用2412字体
        else return;                                //没有的字库
        for(t1=0;t1<8;t1++)
        {
            if(temp&0x80)OLED_DrawPoint(x,y,mode);
            else OLED_DrawPoint(x,y,!mode);
            temp<<=1;
            y++;
            if((y-y0)==size)
            {
                y=y0;
                x++;
                break;
            }
        }    
    }          
}
//m^n函数
u32 mypow(u8 m,u8 n)
{
    u32 result=1;    
    while(n--)result*=m;    
    return result;
}                 
/*************************************************************************/
/*函数功能: 显示2个数字                                                   */
/*入口参数：                                                             */
/*                      x,y :起点坐标                                     */
/*                      len :数字的位数                                   */
/*                      size:字体大小                                        */ 
/*            mode:模式   0,填充模式;1,叠加模式                             */
/*            num:数值(0~4294967295)                                     */
/*************************************************************************/       
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{           
    u8 t,temp;
    u8 enshow=0;                           
    for(t=0;t<len;t++)
    {
        temp=(num/mypow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
                continue;
            }else enshow=1; 
 
        }
        OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
    }
} 
/*************************************************************************/
/*函数功能: 显示字符串                                                                            */
/*入口参数：                                                                                                                          */
/*                      x,y:起点坐标                                                */
/*                      size:字体大小                                                          */
/*                      *p:字符串起始地址                                                               */ 
/*************************************************************************/
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size)
{   
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,1);    
        x+=size/2;
        p++;
    }  
}  

void OLED_ShowGBK(u8 x, u8 y,  u8 num, u8 size,u8 mode)
    {
        u8 temp,t,t1;
        u8 y0=y;
    	//u8 size = 16;
        u8 csize=(size/8 + ((size%8)?1:0)) * size;     //得到字体一个字符对应点阵集所占的字节数
                  
        for(t=0;t<csize;t++)
        {  
    		  //  我只定义了16，12号字体   没有声明其他字体
            if(size==12)      temp = gbk_1212[num][t];    //调用1212字体
            else if(size==16) temp = gbk_1616[num][t];    //调用1616字体
    //      else if(size==24)temp=asc2_2412[chr][t];    //调用2412字体
            else return;                                //没有的字库
    		for(t1=0;t1<8;t1++)
            {
                if(temp&0x80)OLED_DrawPoint(x,y,mode);
                else OLED_DrawPoint(x,y,!mode);
                temp<<=1;
                y++;
                if((y-y0)==size)
                {
                    y=y0;
                    x++;
                    break;
                }
            }    
        } 	
    }
		
		void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)OLED_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
				else OLED_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 



/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
	unsigned int j=0;
	unsigned char x,y;

	if( y1 % 8 == 0 ) 
		y = y1/8;      
	else 
		y = y1/8+1;
	for(y=y0; y<y1; y++)
	{
		OLED_Set_Pos(x0,y);
		for(x=x0; x<x1; x++)
		{      
			SPI_WriteByte(BMP[j++],OLED_DATA);	    	
		}
	}
} 

void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	SPI_WriteByte(0xb0+y,OLED_CMD);
	SPI_WriteByte(((x&0xf0)>>4)|0x10,OLED_CMD);
	SPI_WriteByte((x&0x0f)|0x01,OLED_CMD); 
}   	




//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 	
//u8 OLED_GRAM[128][8];	
//图片的OLED点阵，点阵库，是利用PCtoLCD2002生产的，
//软件设置的取模方式同OLED实验ASCII的取模方式一模一样，采用128*64大小，一个汉字点阵占用1KB。
/*
	* 1. 128*64点整OLED模块功能演示程序的字表，仅适用heltec.taobao.com所售产品;
    * 2. 字表由打包资料中的“取字软件”计算得出;
    * 3. 取字方式 -- 共阴、列行式、逆向输出
*/
//向SSD1106写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
//void OLED_WR_Byte(u8 dat,u8 cmd)
//{	
//	u8 i;			  
//	if(cmd)
//	  OLED_DC_Set();
//	else 
//	  OLED_DC_Clr();		  
//	OLED_CS_Clr();
//	for(i=0;i<8;i++)
//	{			  
//		OLED_SCLK_Clr();
//		if(dat&0x80)
//		   OLED_SDIN_Set();
//		else 
//		   OLED_SDIN_Clr();
//		OLED_SCLK_Set();
//		dat<<=1;   
//	}				 		  
//	OLED_CS_Set();
//	OLED_DC_Set();   	  
//} 

//void OLED_Set_Pos(unsigned char x, unsigned char y) 
//{ 
//	OLED_WR_Byte(0xb0+y,OLED_CMD);
//	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
//	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 
//}   	  
////开启OLED显示    
//void OLED_Display_On(void)
//{
//	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
//	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
//	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
//}
////关闭OLED显示     
//void OLED_Display_Off(void)
//{
//	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
//	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
//	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
//}		   			 
////清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
//void OLED_Clear(void)  
//{  
//	u8 i,n;		    
//	for(i=0;i<8;i++)  
//	{  
//		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
//		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
//		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
//		for(n=0;n<128;n++)
//			OLED_WR_Byte(0,OLED_DATA); 
//	} //更新显示
//}

////在指定位置显示一个字符,包括部分字符
////x:0~127
////y:0~63
////mode:0,反白显示;1,正常显示				 
////size:选择字体 16/12 
//void OLED_ShowChar(u8 x,u8 y,u8 chr)
//{      	
//	unsigned char c=0,i=0;	
//		c=chr-' ';//得到偏移后的值			
//		if(x>Max_Column-1){x=0;y=y+2;}
//		if(SIZE ==16)
//			{
//			OLED_Set_Pos(x,y);	
//			for(i=0;i<8;i++)
//			OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
//			OLED_Set_Pos(x,y+1);
//			for(i=0;i<8;i++)
//			OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
//			}
//			else {	
//				OLED_Set_Pos(x,y+1);
//				for(i=0;i<6;i++)
//				OLED_WR_Byte(F6x8[c][i],OLED_DATA);
//			}
//}
////m^n函数
//u32 oled_pow(u8 m,u8 n)
//{
//	u32 result=1;	 
//	while(n--)result*=m;    
//	return result;
//}				  
////显示2个数字
////x,y :起点坐标	 
////len :数字的位数
////size:字体大小
////mode:模式	0,填充模式;1,叠加模式
////num:数值(0~4294967295);	 		  
//void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
//{         	
//	u8 t,temp;
//	u8 enshow=0;						   
//	for(t=0;t<len;t++)
//	{
//		temp=(num/oled_pow(10,len-t-1))%10;
//		if(enshow==0&&t<(len-1))
//		{
//			if(temp==0)
//			{
//				OLED_ShowChar(x+(size/2)*t,y,' ');
//				continue;
//			}else enshow=1; 
//		 	 
//		}
//	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0'); 
//	}
//} 
////显示一个字符号串
//void OLED_ShowString(u8 x,u8 y,u8 *chr)
//{
//	unsigned char j=0;
//	while (chr[j]!='\0')
//	{		
//		OLED_ShowChar(x,y,chr[j]);
//		x+=8;
//		if(x>120){x=0;y+=2;}
//			j++;
//	}
//}
////显示汉字
//void OLED_ShowCHinese(u8 x,u8 y,u8 no)
//{      			    
//	u8 t,adder=0;
//	OLED_Set_Pos(x,y);	
//    for(t=0;t<16;t++)
//	{
//		OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
//		adder+=1;
//	}	
//	OLED_Set_Pos(x,y+1);	
//    for(t=0;t<16;t++)
//	{	
//		OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
//		adder+=1;
//	}					
//}
///***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
//void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
//{ 	
//	unsigned int j=0;
//	unsigned char x,y;

//	if( y1 % 8 == 0 ) 
//		y = y1/8;      
//	else 
//		y = y1/8+1;
//	for(y=y0; y<y1; y++)
//	{
//		OLED_Set_Pos(x0,y);
//		for(x=x0; x<x1; x++)
//		{      
//			OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
//		}
//	}
//} 


////初始化SSD1306					    
//void OLED_Init(void)
//{ 	
//	//IO口设置，此处需要修改
//	RCC->APB2ENR|=1<<4;   	//使能PORTC时钟	   	 
//	RCC->APB2ENR|=1<<2;   	//使能PORTA时钟	   
//	GPIOA->CRL&=0XFF0FF0FF;
//	GPIOA->CRL|=0X00300300;
//	GPIOC->CRH&=0X000FFFFF;
//	GPIOC->CRH|=0x22200000;  //频率最大不能超过50MHZ
//	PWR->CR|=1<<8;           //允许写入RTC和后备寄存器
//	RCC->BDCR&=0XFFFFFFFE;   //外部32KHZ振荡器关闭
//	BKP->CR&=0XFFFFFFFE;     //侵入检测TAMPER引脚作为通用IO口使用
//	PWR->CR&=0XFFFFFEFF;     //禁止写入RTC和后备寄存器
//	//修改截止
//	
//	//全部拉高
//	OLED_CS=1;
//	OLED_RST=1;
//	OLED_DC=1;
//	OLED_SCLK=1;
//	OLED_SDIN=1;

//	OLED_RST_Set();
//	delay_ms(100);
//	OLED_RST_Clr();
//	delay_ms(100);
//	OLED_RST_Set(); 
//					  
//	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
//	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
//	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
//	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
//	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
//	OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
//	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
//	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
//	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
//	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
//	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
//	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
//	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
//	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
//	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
//	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
//	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
//	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
//	OLED_WR_Byte(0x12,OLED_CMD);
//	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
//	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
//	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
//	OLED_WR_Byte(0x02,OLED_CMD);//
//	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
//	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
//	OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
//	OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
//	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
//	
//	OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 
//	OLED_Clear();
//	OLED_Set_Pos(0,0); 	
//	
//} 
//void HeadLine1(u8 y)		//主标题显示
//{
//	OLED_ShowCHinese(10,y,0);	//双线循迹停泊
//	OLED_ShowCHinese(28,y,1);	//
//	OLED_ShowCHinese(46,y,2);	//
//	OLED_ShowCHinese(64,y,3);	//
//	OLED_ShowCHinese(82,y,4);	//
//	OLED_ShowCHinese(100,y,5);	//
//} 
//void HeadLine2(u8 y)		//副标题显示
//{
////	
////	OLED_ShowCHinese(54,y,10);	//停
////	OLED_ShowCHinese(72,y,11);	//泊
////	OLED_ShowCHinese(72,y,12);	//系
////	OLED_ShowCHinese(54,y,13);	//统
//}

//void Start(u8 y)	//开始
//{
//	OLED_ShowCHinese(46,y,8);	//开
//	OLED_ShowCHinese(64,y,9);	//始
//}
//void About(u8 y)	//关于
//{
//	OLED_ShowCHinese(46,y,10);	
//	OLED_ShowCHinese(64,y,11);	
//}
//void Thank(u8 y)	//鸣谢
//{
//	OLED_ShowCHinese(46,y,12);	
//	OLED_ShowCHinese(64,y,13);	
//}
////void FuncName4(u8 y)
////{
////	OLED_ShowCHinese(54,y,8);	//子
////	OLED_ShowCHinese(72,y,9);	//科
////}
////void FuncName5(u8 y)
////{
////	OLED_ShowCHinese(54,y,10);	//园
////	OLED_ShowCHinese(72,y,11);	//电
////}
////void SettingName(u8 y)
////{
////	OLED_ShowCHinese(54,y,6);	//置
////	OLED_ShowCHinese(72,y,7);	//设
////}
//void HangClear(u8 y)	//清除第y行
//{
//	OLED_ShowString(0,y,"                ");
//}
//void OLED_Menu(void)
//{
//	pFunc[0](0);
//	pFunc[1](3);
//	pFunc[2](6);
//}
//void Menu_Init(void)
//{
//	pFunc[0] = HeadLine1;	//函数指针指向各功能，显示时通过交换函数指针实现功能显示
//	pFunc[1] = Start;
//	pFunc[2] = About;
//	pFunc[3] = Thank;
//}
