//
// Created by 27541 on 2026/10/25.
//
#include "MotorControl.h"
#include "main.h"                 /* BTN_Pin、HAL_GetTick */
#include "bsp_can.h"
#include "Breath.h"
#include "speed_pid.h"

#define MODE_COUNT 2U //宏定义，一共2种模式，方便更改
#define CONTROL_PERIOD_MS 10U //定义周期，这里是10ms
//电机的机械响应本来就慢，10ms可以节省运算能力

static volatile uint8_t mode = 0;   // 中断里改、主循环里读，必须
static volatile uint8_t pid_reset_request = 0; //和按钮配合，清空PID的累计

// 调试器里方便观察这四个变量
volatile float debug_target = 0.0f;
volatile float debug_actual = 0.0f;
volatile float debug_error  = 0.0f;
volatile float debug_output = 0.0f;

void MotorControl(uint32_t now_ms)
{
    static uint32_t last_control_ms = 0;
    static uint8_t  control_started = 0; //用来判断是不是第一次启动
    //加static关键字，这里初始化一次后，就不会在这里赋值成0

    //因为系统初始化会花费一些时间，启动往往不及时
    if (!control_started)
    {
        control_started = 1;
        last_control_ms = now_ms;
        speed_pid_clear();
        return;
    }

    uint32_t elapsed_ms = now_ms - last_control_ms;
    if (elapsed_ms < CONTROL_PERIOD_MS)
        //没有限制她是10
    {
        return;
    }
    last_control_ms = now_ms;
    //这个限制为10ms

    //PID公式里的Ki、Kd是按秒定义的
    //把单位ms换算成s
    const float dt = (float)elapsed_ms * 0.001f;

    //切换模式时，积分累计的量还在积累，这里把他清除！
    if (pid_reset_request)
    {
        pid_reset_request = 0U;
        speed_pid_clear();
    }

    float target;
    switch (mode)
    {
    case 0:
        target = 0.0f;
        break;
    case 1:
        target = 1000.0f;
        break;
    default:
        target = 0.0f;
        break;
    }

    const float actual = motor_1.rotor_speed;//这里有隐式转换
    const int16_t out  = speed_pid_calculate(target, actual, dt);

    debug_target = target;
    debug_actual = actual;
    debug_error  = target - actual;
    debug_output = out;

    CAN_SendMotorCommand(0x200, out, 0, 0, 0);
}

// HAL 的弱回调，在这里重定义
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == BTN_Pin)
    {
        mode = (uint8_t)((mode + 1U) % MODE_COUNT);//mode在0-1循环！
        pid_reset_request = 1U;
    }
}