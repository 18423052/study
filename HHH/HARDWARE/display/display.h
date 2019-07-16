#ifndef __DISPLAY_H
#define __DISPLAY_H
#include "sys.h"
#include "OLED.h"
typedef struct
{
		unsigned char current;
		unsigned chaup;//上键
		unsigned chadown;//下键
		unsigned chaenter;//确认键
		
   void (*current_operation)();
} key_table;

/*********************************************
 * 创建一个结构体
 * 存放界面标志位
*/
typedef struct
{
    u8 Interface_Mark;     //界面状态
    u8 Task_Mark;          //任务状态
    u8 Run_Task;           //开始运行任务
} Mark;
Mark Mark_Sign;//状态标志位

/*********************************************
 * 创建一个枚举
 * 存放界面变量
*/
enum
{
    Main_Interface = 0x10, /****主界面*****/
    Menu_Interface = 0x20, /****菜单界面***/
    Task_Interface = 0x30, /****任务界面***/
};
/*******************************************/

void Main_Interface_APP();//显示主界面
void Menu_Interface_APP();//显示菜单界面
void Function_Menu_APP();//显示功能界面

#endif