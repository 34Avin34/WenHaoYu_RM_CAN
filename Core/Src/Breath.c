//
// Created by 27541 on 2026/10/25.
//
#include "Breath.h"
#include <math.h>

#define BREATH_PERIOD_MS  2000U //宏定义，这样方便修改呼吸周期

float Breath(uint32_t now)
{
    const float phase = (float)(now % 2000) / 2000.0f;   // 计时，记录呼吸的进度，一个周期是4s
    //存在的问题：不断循环，计算新的phase,会占用一定的CPU资源
    float k = 0.5f - 0.5f * cosf(phase * 6.2831853f);        //生成0-1的数，画个函数图像
    k=k*k;//经过测试发现效果不明显，加个平方看看
    return k;
}