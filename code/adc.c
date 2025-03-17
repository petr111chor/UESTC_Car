/*
 * adc.c
 *
 *  Created on: 2023年5月11日
 *      Author: Kern
 */

#include "zf_common_headfile.h"
#define count_ave    (12)

//volatile adc_cbh adc = {0};  // 定义并初始化 adc

unsigned short func_left_limit_a(unsigned short value, unsigned short limit) {
    if (value < limit) {
        return limit;  // 如果值小于下限，则返回下限值
    }
    return value;  // 否则返回原始值
}

void calculate_s(L_State* L ,Err_Steering* Err_Ste )//根据电感状态计算偏差值，并更新偏差值结构体中last量，结果保存在舵机偏差值结构体中
{
    //求取12次采样平均值作为真实值
    L->L_real = adc_mean_filter_convert(L_ADC, count_ave);
    L->LM_real = adc_mean_filter_convert(LM_ADC, count_ave);
    L->R_real = adc_mean_filter_convert(R_ADC, count_ave);
    L->RM_real = adc_mean_filter_convert(RM_ADC, count_ave);


    //使采样最小值为1
    L->L_real = func_left_limit_a(L->L_real, 1);
    L->LM_real = func_left_limit_a(L->LM_real, 1);
    L->R_real = func_left_limit_a(L->R_real, 1);
    L->RM_real = func_left_limit_a(L->RM_real, 1);

    //第一次归一化，使得电感距离电感最大值处的吻合度用百分比表示，并开方使得偏差和小车偏离赛道的距离成单调关系（即消除极点），使得丢线更好判断
    L->L_once = (sqrt(((float)L->L_real)/(L->L_max)));
    L->LM_once = (sqrt(((float)L->LM_real)/(L->LM_max)));
    L->R_once = (sqrt(((float)L->R_real)/(L->R_max)));
    L->RM_once = (sqrt(((float)L->RM_real)/(L->RM_max)));

//
    //第二次归一化，求取横电感和斜电感偏差值占总的百分比
    L->L_twice=10.0*(L->L_once)/(L->L_once*L->L_once+L->LM_once*L->LM_once+L->R_once*L->R_once+L->RM_once*L->RM_once);
    L->LM_twice=10.0*(L->LM_once)/(L->L_once*L->L_once+L->LM_once*L->LM_once+L->R_once*L->R_once+L->RM_once*L->RM_once);
    L->R_twice=10.0*(L->R_once)/(L->L_once*L->L_once+L->LM_once*L->LM_once+L->R_once*L->R_once+L->RM_once*L->RM_once);
    L->RM_twice=10.0*(L->RM_once)/(L->L_once*L->L_once+L->LM_once*L->LM_once+L->R_once*L->R_once+L->RM_once*L->RM_once);

    Err_Ste->Err_x = L->L_once - L->R_once;
    Err_Ste->Err_h = L->LM_once - L->RM_once;
    Err_Ste->Err = (adc.A*Err_Ste->Err_x + adc.B*Err_Ste->Err_h) / (adc.A*(L->L_once + L->R_once) + adc.C*ABS(Err_Ste->Err_h));

////以上系数使用matlab拟合，使得偏差和小车偏离赛道的距离成线性关系，至此，偏差和小车偏离赛道的距离成单调线性关系
    Err_Ste->Errsum += Err_Ste->Err;
    Err_Ste->Errdif = Err_Ste->Err - Err_Ste->Err_last;
////位置式PID积分和差分运算
    Err_Ste->Err_x_last = Err_Ste->Err_x;
    Err_Ste->Err_h_last = Err_Ste->Err_h;
    Err_Ste->Err_last = Err_Ste->Err;
////偏差值更新
}





