//
// Created by 27541 on 2026/11/2.
//
#include "speed_pid.h"
#include <algorithm>

//构建有参构造函数
//初始化列表方式初始化
SpeedPID::SpeedPID(float kp, float ki, float kd,
                   float max_out, float max_iout)
    : Kp(kp),
      Ki(ki),
      Kd(kd),
      maxOutput(max_out),
      maxIntegral(max_iout),
      integral(0.0f),
      prevError(0.0f),//注意到这里是0误差
      firstUpdate(true)
{
}

void SpeedPID::Clear()//归零函数，用于切换模式
{
    integral    = 0.0f;
    prevError   = 0.0f;
    firstUpdate = true;
}

float SpeedPID::Calculate(float target_speed,
                          float actual_speed,
                          float dt)
//这个函数的作用是算一个合理的output值
{
    const float error = target_speed - actual_speed;
    const float p_out = Kp * error;

    float d_out = 0.0f;
    if (!firstUpdate)
    //一开始这里是假，因为prevError默认是0，Clear()之后也是0
    //代表上一次无误差，如果执行，会出现很大问题
    {
        d_out = Kd * (error - prevError) / dt;
        //prevError相当于上次的误差，在最下面赋值
        //整体算的是误差的变化速率，用的是割线逼近
    }

    const float candidate_i = std::clamp(
        //这里限制最大积分
        integral + Ki * error * dt,//这里相当于积分
        -maxIntegral,
        maxIntegral
    );
    /*std::clamp(),会将 value 限制在low和high之间。它是定义在<algorithm>中的。
      std::clamp 的返回引用可能指向：
      -maxIntegral（当计算结果太小时);
      maxIntegral（当计算结果太大时);
      integral + Ki*error*dt 这个临时结果（在范围内时）*/

    const float candidate_out = p_out + candidate_i + d_out;
    const float i_change      = candidate_i - integral;//这里看他整体的变化趋势

    const bool worsen_upper =
        (candidate_out >  maxOutput) && (i_change > 0.0f);
    //如果输出大于最大输出且积分在增大-->过载
    const bool worsen_lower =
        (candidate_out < -maxOutput) && (i_change < 0.0f);
    //如果反向输出大于最大反向输出且积分在反向增大-->过载
    if (!(worsen_upper || worsen_lower))
        //只要有一种过载情况就不要这个candidate_i，继续用上次的integral
    {
        integral = candidate_i;
    }

    const float output = std::clamp(
        //这里限制最大输出值
        p_out + integral + d_out,
        -maxOutput,
        maxOutput
    );

    prevError   = error;
    firstUpdate = false;

    return output;
}

static SpeedPID speed_pid(
    0.25f,
    0.15f,
    0.001f,
    1000.0f,   // 总输出限幅
    500.0f     // 积分项限幅
);

//下面是两个C链接函数，充当中转站
extern "C" void speed_pid_clear(void)
{
    speed_pid.Clear();
}

extern "C" int16_t speed_pid_calculate(float target_speed,
                                       float actual_speed,
                                       float dt)
{
    return static_cast<int16_t>(speed_pid.Calculate(target_speed, actual_speed, dt));
        //这里返回的是output，类型是float转int16_t;
}