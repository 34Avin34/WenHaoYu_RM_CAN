//
// Created by 27541 on 2026/10/25.
//
#include "Is_1ms_Elapsed.h"
#include "main.h"      // 获得HAL_GetTick()这个函数

uint8_t Is_1ms_Elapsed(uint32_t *last_ms)
{
    uint32_t now = HAL_GetTick();

    if (now == *last_ms)
    {
        return 0;      /* 这一毫秒已经放行过 */
    }

    *last_ms = now;
    return 1;
}