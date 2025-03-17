#ifndef __SPEED_PROCESS_H__
#define __SPEED_PROCESS_H__


#define index_max   500

extern volatile float Purpost_Speed;           //Ŀ���ٶ�
extern volatile short Purpost_Left_Speed;      //Ŀ�������ٶ�
extern volatile short Purpost_Rigt_Speed;      //Ŀ�������ٶ�
extern float Enc_Total_Pos;     //·�̣����������٣�
extern short alpha ;//��ǰ�Ķ����ǵ�PWM��

extern volatile short purpost_1;
extern volatile short purpost_2;


extern int16_t Error1 ;
extern int16_t Error2 ;

extern int16_t ecpulse1;
extern int16_t ecpulse2;
extern bool start_sum_dis;
extern float ALL_dis_sum ;
extern int dis_error;
extern int speed_delta;
extern int begin_time;
extern float break_rate;
extern int16_t ecpulse3[index_max];
extern int16_t ecpulse4[index_max];
extern uint16 e_index;

void SetToFixedSpeed(short SetFixed_Speed);
void CancelSetToFixedSpeed(void);

float Get_Left_Uk(int16_t LeftEk);
float Get_Right_Uk(int16_t RightEk);
float Get_delta(int16_t dis_error);
void Update_Purpost_Speed(void);
void Update_Purpost_Speed_NEW(void);
void speed_Update(void);



#endif
