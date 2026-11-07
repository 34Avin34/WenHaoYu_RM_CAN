//
// Created by 27541 on 2026/10/28.
//

#include "bsp_can.h"
#include "can.h"

extern "C" {
DJI_motor_info motor_1;
DJI_motor_info motor_2;
DJI_motor_info motor_3;
DJI_motor_info motor_4;
}
//从 C++ 编译器和标准的角度：是绝对的“对象（Object）”
//从嵌入式工程和 C/C++ 混编的角度：叫“全局结构体变量”最稳妥

HAL_StatusTypeDef CAN_SendMotorCommand(uint32_t std_id,
                                       int16_t motor1, int16_t motor2,
                                       int16_t motor3, int16_t motor4)
{
    CAN_TxHeaderTypeDef TxHeader = {0};
    //初始化TxHeader（发送帧头）这个变量（名字可以自己起），本质上是一个结构体
    uint8_t  TxData[8];
    //和下面相对应（意为：发送的数据）
    uint32_t TxMailbox;

    TxHeader.StdId = std_id;//标识符：0x200 管 ID1~4，0x1FF 管 ID5~8
                            //标准标识符，标准帧，11位的ID；还有扩展帧（CAN_ID_EXT），是29位ID
                            //0x200是 0  1  0  0  0  0  0  0  0  0  0 一共11位
    TxHeader.IDE   = CAN_ID_STD;	//标志位
    TxHeader.RTR   = CAN_RTR_DATA;  //这是一帧数据帧，Remote Transmission Request（远程发送请求）
    TxHeader.DLC   = 8;  //Data Length Code 数据长度码
    //4 个电机 × 每个 int16_t 2 字节 = 8 字节
    //数据来源：RoboMaster C620无刷电机调速器使用说明（中英日）V1.01 P.32

    TxData[0] = (uint8_t)((uint16_t)motor1 >> 8);
    // “>>” 是整体右移，被"挤出去"的是右边（低位），留下来的是左边（高位）
    //所谓高位就是对整体数的大小影响大的位数，也就是前面的位数（个人理解）
    TxData[1] = (uint8_t)motor1;
    /*16位强转位8位，强转丢掉的是**前8位**（高位），
      而保留的恰好就是后 8 位（低位），
      所以它精确读到的正是后半部分，丢的是前半部分*/
    TxData[2] = (uint8_t)((uint16_t)motor2 >> 8);
    TxData[3] = (uint8_t)motor2;
    TxData[4] = (uint8_t)((uint16_t)motor3 >> 8);
    TxData[5] = (uint8_t)motor3;
    TxData[6] = (uint8_t)((uint16_t)motor4 >> 8);
    TxData[7] = (uint8_t)motor4;

    return HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);
    /*返回的是以下四种状态：
      HAL_OK      = 0x00U,    成功  注意：发进去邮箱不等于得到执行，仍然存在出错的可能性
                                        HAL 库提供了发送完成中断，可以用于查看是否真的执行了
                                        可以在 can.c 里打开 CAN 的 TX 中断，重写这个弱函数
      HAL_ERROR   = 0x01U,    错误
      HAL_BUSY    = 0x02U,    忙
      HAL_TIMEOUT = 0x03U，   超时  */
    //U 是 C/C++ 语言中的字面量后缀，代表 unsigned（无符号）。更加严谨，防止潜在的类型转换警告
}

/* 内部函数：只给 BSP_CAN_Init 调用。static 让它在别的文件里看不见，
   所以不需要（也不该）写进头文件。必须定义在调用它之前 */
static void BSP_CAN_FilterConfig(void)
//static关键字防止绕过统一入口（最主要），防止以后命名冲突，让头文件诚实地表达"接口有几个"
//我不知道为什么文档里面要用bsp_can::，并没有相关的上下文，我用static关键字替代
//这个工程里面可以删除
{
    CAN_FilterTypeDef filter = {0};

    filter.FilterActivation = ENABLE;
    filter.FilterMode = CAN_FILTERMODE_IDMASK;
    filter.FilterScale = CAN_FILTERSCALE_32BIT;
    filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;

    filter.FilterIdHigh = 0x0000;
    filter.FilterIdLow = 0x0000;
    filter.FilterMaskIdHigh = 0x0000;
    filter.FilterMaskIdLow = 0x0000;
    //如果没有配置过滤器，谁都不能进入FIFO
    //表示所有的都能通过，过滤的主要是前四个字段：
    //STID、EXID（扩展帧后18位，在这里是空的）、IDE、RTR

    filter.FilterBank = 0;
    filter.SlaveStartFilterBank = 14;
    //一共有14个过滤器，这里是第1个，即第0号

    HAL_CAN_ConfigFilter(&hcan1, &filter);
}

void BSP_CAN_Init(void)
{
    BSP_CAN_FilterConfig();                    // 1 先配过滤器：哪些帧能进 FIFO0

    if (HAL_CAN_Start(&hcan1) != HAL_OK)       // 2 再启动外设
    {
        Error_Handler();
    }

    if (HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING) !=HAL_OK)
    {
        Error_Handler();                       // 最后开"FIFO0 有新报文"的通知
        //把 CAN 外设里"FIFO0 有新报文"这个中断源打开
    }
}

extern "C" void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
//这个函数的调用者是HAL库，HAL库调用的符号名必须是C风格，所以需要extern "C"
//这原本是一个弱定义函数，但是被我重新定义了
//这个函数在FIFO非空的时候就会被触发。（难不成会一直中断吗？不是！）
//这个函数在运行时，不会自己中断自己！
{
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[8];

    if (HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&RxHeader,RxData) == HAL_OK)
        //获取FIFO0（这个项目中）中目前最先进入的数据帧，OK代表读取成功
    {
        switch (RxHeader.StdId)
        {
        case 0x201:
            {
                motor_1.rotor_angle = (int16_t)((RxData[0] << 8) | RxData[1]);
                motor_1.rotor_speed = (int16_t)((RxData[2] << 8) | RxData[3]);
                motor_1.torque_current = (int16_t)((RxData[4] << 8) | RxData[5]);
                motor_1.temp = (int8_t)RxData[6];
                break;
            }
        //把一大串数据分层整理
        case 0x202:
            {
                motor_2.rotor_angle = (int16_t)((RxData[0] << 8) | RxData[1]);
                motor_2.rotor_speed = (int16_t)((RxData[2] << 8) | RxData[3]);
                motor_2.torque_current = (int16_t)((RxData[4] << 8) | RxData[5]);
                motor_2.temp = (int8_t)RxData[6];
                break;
            }
        case 0x203:
            {
                motor_3.rotor_angle = (int16_t)((RxData[0] << 8) | RxData[1]);
                motor_3.rotor_speed = (int16_t)((RxData[2] << 8) | RxData[3]);
                motor_3.torque_current = (int16_t)((RxData[4] << 8) | RxData[5]);
                motor_3.temp = (int8_t)RxData[6];
                break;
            }
        case 0x204:
            {
                motor_4.rotor_angle = (int16_t)((RxData[0] << 8) | RxData[1]);
                motor_4.rotor_speed = (int16_t)((RxData[2] << 8) | RxData[3]);
                motor_4.torque_current = (int16_t)((RxData[4] << 8) | RxData[5]);
                motor_4.temp = (int8_t)RxData[6];
                break;
            }
        default:
            break;
        }
    }
}