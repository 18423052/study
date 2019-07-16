#include "led.h"
#include "delay.h"
#include "sys.h"
#include "tim.h"
#include "OLED.h"
#include "adc.h"
#include "usart.h"

//ALIENTEK Mini STM32开发板范例代码8
//PWM输出实验   
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司

 int main(void)
 {	
 	u8 t;
	u8 len;	
	u16 times=0; 
 
	delay_init();	    	 //延时函数初始化	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	uart_init(9600);	 //串口初始化为9600
	LED_Init();		  	 //初始化与LED连接的硬件接口 
	GPIO_WriteBit(GPIOA,GPIO_Pin_11,(BitAction)(1));
	while(1)
	{
		
		if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			printf("\r\n您发送的消息为:\r\n");
			for(t=0;t<len;t++)
			{
				USART1->DR=USART_RX_BUF[t];
				while((USART1->SR&0X40)==0);//等待发送结束
			}
			printf("\r\n\r\n");//插入换行
			USART_RX_STA=0;
		}else
		{
			times++;
			if(times%5000==0)
			{
				printf("test\n");
			}
			if(times%200==0)printf("请输入数据,以回车键结束\r\n");  
			if(times%30==0)GPIO_WriteBit(GPIOC,GPIO_Pin_13,(BitAction)(1-GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_13))); //闪烁LED,提示系统正在运行.
			delay_ms(10);   
		}
	}	 
	
}






























// int main(void)
// {	   
//	delay_init();	    	 //延时函数初始化	  
//	LED_Init();		  	//初始化与LED连接的硬件接口
//	TIM3_PWM_Init(899,0);//不分频。PWM频率=72000/(899+1)=80Khz 
//   	while(1)
//	{			 
//		TIM_SetCompare2(TIM3,450);	   
//	} 
//}

