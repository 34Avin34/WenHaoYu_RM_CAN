//
// Created by 27541 on 2026/10/28.
//

#ifndef RM_CAN_TEST_01_BSP_CAN_H
#define RM_CAN_TEST_01_BSP_CAN_H

#include "main.h"

// 一台电机（C620）反馈的状态。
typedef struct
//C语言中使用结构体变量时通常要写 struct，
//但如果使用typedef关键字，就可以像Cpp中一样直接使用
{
    int16_t rotor_angle;
    int16_t rotor_speed;
    int16_t torque_current;
    int8_t  temp;
} DJI_motor_info;
//由于一般是C编译器编译这个DJI_motor_info,所以这里准确来说是结构体
//把 DJI_motor_info 及其变量称为“结构体”和“结构体变量”是最严谨、最不容易引起歧义的。

#ifdef __cplusplus
extern "C" {
#endif
//用来支持 C 和 C++ 混合编程。
//在头文件extern "C"声明是为了保证其他.c文件中能够"认识"这几个变量
//因为cpp和C的编译器不同，main.c中不能看到

extern DJI_motor_info motor_1;
extern DJI_motor_info motor_2;
extern DJI_motor_info motor_3;
extern DJI_motor_info motor_4;
//C语言编译器会“记下它们”，但“绝对不给它们分配内存”
//这几个变量是在bsp_can.cpp文件中定义的，得extern

HAL_StatusTypeDef CAN_SendMotorCommand(uint32_t std_id,int16_t motor1,
                                       int16_t motor2,int16_t motor3, int16_t motor4);
void BSP_CAN_Init(void);

#ifdef __cplusplus
}
#endif

#endif //RM_CAN_TEST_01_BSP_CAN_H
