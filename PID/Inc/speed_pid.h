//
// Created by 27541 on 2026/11/2.
//

#ifndef RM_CAN_TEST_01_SPEED_PID_H
#define RM_CAN_TEST_01_SPEED_PID_H

#include <stdint.h>

#ifdef __cplusplus
// C++的类，C文件读取不了，不需要extern "C"
class SpeedPID //定义了这个类
{
public:
    SpeedPID(float kp, float ki, float kd,
             float max_out, float max_iout);
    //有参构造函数,初始化对象

    void Clear();
    //把PID的积累的参数清空

    float Calculate(float target_speed,
                     float actual_speed,
                    float dt);
    //用来计算最后的输出值
//这里的函数都是只声明
private:
    float Kp;
    float Ki;
    float Kd;
    float maxOutput;
    float maxIntegral;//积分如果一直累计，不好消除
    float integral;
    float prevError;//用来算微分
    bool  firstUpdate;//用于初始化prevError
};

extern "C" {
#endif

void    speed_pid_clear(void);
int16_t speed_pid_calculate(float target_speed,
                            float actual_speed,
                            float dt);

#ifdef __cplusplus
}

#endif
#endif //RM_CAN_TEST_01_SPEED_PID_H
