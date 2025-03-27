/*
 * adc.c
 *
 *  Created on: 2023��5��11��
 *      Author: Kern
 */

#include "zf_common_headfile.h"
#define count_ave    (12)

//volatile adc_cbh adc = {0};  // ���岢��ʼ�� adc

unsigned short func_left_limit_a(unsigned short value, unsigned short limit) {
    if (value < limit) {
        return limit;  // ���ֵС�����ޣ��򷵻�����ֵ
    }
    return value;  // ���򷵻�ԭʼֵ
}

void calculate_s(L_State* L ,Err_Steering* Err_Ste )//���ݵ��״̬����ƫ��ֵ��������ƫ��ֵ�ṹ����last������������ڶ��ƫ��ֵ�ṹ����
{
    //��ȡ12�β���ƽ��ֵ��Ϊ��ʵֵ
    L->L_real = adc_mean_filter_convert(L_ADC, count_ave);
    L->LM_real = adc_mean_filter_convert(LM_ADC, count_ave);
    L->R_real = adc_mean_filter_convert(R_ADC, count_ave);
    L->RM_real = adc_mean_filter_convert(RM_ADC, count_ave);


    //ʹ������СֵΪ1
    L->L_real = func_left_limit_a(L->L_real, 1);
    L->LM_real = func_left_limit_a(L->LM_real, 1);
    L->R_real = func_left_limit_a(L->R_real, 1);
    L->RM_real = func_left_limit_a(L->RM_real, 1);

    //��һ�ι�һ����ʹ�õ�о��������ֵ�����Ǻ϶��ðٷֱȱ�ʾ��������ʹ��ƫ���С��ƫ�������ľ���ɵ�����ϵ�����������㣩��ʹ�ö��߸����ж�
    L->L_once = (sqrt(((float)L->L_real)/(L->L_max)));
    L->LM_once = (sqrt(((float)L->LM_real)/(L->LM_max)));
    L->R_once = (sqrt(((float)L->R_real)/(L->R_max)));
    L->RM_once = (sqrt(((float)L->RM_real)/(L->RM_max)));

//
    //�ڶ��ι�һ������ȡ���к�б���ƫ��ֵռ�ܵİٷֱ�
    L->L_twice=10.0*(L->L_once)/(L->L_once*L->L_once+L->LM_once*L->LM_once+L->R_once*L->R_once+L->RM_once*L->RM_once);
    L->LM_twice=10.0*(L->LM_once)/(L->L_once*L->L_once+L->LM_once*L->LM_once+L->R_once*L->R_once+L->RM_once*L->RM_once);
    L->R_twice=10.0*(L->R_once)/(L->L_once*L->L_once+L->LM_once*L->LM_once+L->R_once*L->R_once+L->RM_once*L->RM_once);
    L->RM_twice=10.0*(L->RM_once)/(L->L_once*L->L_once+L->LM_once*L->LM_once+L->R_once*L->R_once+L->RM_once*L->RM_once);

    Err_Ste->Err_x = L->L_once - L->R_once;
    Err_Ste->Err_h = L->LM_once - L->RM_once;
    Err_Ste->Err = (adc.A*Err_Ste->Err_x + adc.B*Err_Ste->Err_h) / (adc.A*(L->L_once + L->R_once) + adc.C*ABS(Err_Ste->Err_h));

////����ϵ��ʹ��matlab��ϣ�ʹ��ƫ���С��ƫ�������ľ�������Թ�ϵ�����ˣ�ƫ���С��ƫ�������ľ���ɵ������Թ�ϵ
    Err_Ste->Errsum += Err_Ste->Err;
    Err_Ste->Errdif = Err_Ste->Err - Err_Ste->Err_last;
////λ��ʽPID���ֺͲ������
    Err_Ste->Err_x_last = Err_Ste->Err_x;
    Err_Ste->Err_h_last = Err_Ste->Err_h;
    Err_Ste->Err_last = Err_Ste->Err;
////ƫ��ֵ����
}





