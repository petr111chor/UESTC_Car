/*
 * pid.c
 *
 *  Created on: 2025��3��10��
 *      Author: 15208
 */

#include "zf_common_headfile.h"

//λ��ʽPID  ʵ��ֵ Ŀ��ֵ       ͼ���Ǳ��Ǵ�����Ŀ������ֵ
float PID_Normal(PID*PID ,float now_data ,float target_data)
{
    PID->Error=target_data-now_data;
    PID->Out_D=(PID->Error-PID->Out_P)*PID->LowPass+PID->Out_D*(1-PID->LowPass);
    PID->Out_P=PID->Error;
    return (PID->Kp * PID->Out_P +PID->Kd * PID->Out_D);

}


//����ʽPID
float PID_Increase(PID*PID ,float now_data ,float target_data)
{
    PID->Error=target_data-now_data;
    PID->Out_P=(PID->Error - PID->LastError);
    PID->Out_I=PID->Error;
    PID->Out_D=(PID->Error -2*PID->LastError + PID->PrevError);
    PID->PrevError=0.9 * PID->LastError + 0.1 * PID->PrevError;
    PID->LastError=0.9 *PID->Error + 0.1 * PID->LastError;
    PID->Lastdata = now_data;

    return (PID->Kp*PID->Out_P+PID->Ki*PID->Out_I+PID->Kd*PID->Out_D);
}


int limit_int(int a, int b, int c)
{
    // �Զ�ȷ����С���ֵ���������˳�����
    int real_min = a < c ? a : c;
    int real_max = a < c ? c : a;

    return (b < real_min) ? real_min :
           (b > real_max) ? real_max : b;
}


